#include <chrono>
#include "model_load.hpp"
#include "assimp/anim.h"
#include "utilities/warning.hpp"
#include "utilities/clock.hpp"

#include "utilities/glm_to_string.hpp"

constexpr int MULTI_THREAD_LOADING = 1;

glm::vec3 to_glm(const aiColor3D& v)
{
	return {v[0], v[1], v[2]};
}

glm::vec4 to_glm(const aiColor4D& v)
{
	return {v[0], v[1], v[2], v[3]};
}

glm::vec2 to_glm(const aiVector2D& v)
{
	return {v[0], v[1]};
}

glm::vec3 to_glm(const aiVector3D& v)
{
	return {v[0], v[1], v[2]};
}

glm::mat3 to_glm(const aiMatrix3x3& m)
{
	return {
		m.a1, m.b1, m.c1,
		m.a2, m.b2, m.c2,
		m.a3, m.b3, m.c3,
	};
}

glm::mat4 to_glm(const aiMatrix4x4& m)
{
	return {
		m.a1, m.b1, m.c1, m.d1,
		m.a2, m.b2, m.c2, m.d2,
		m.a3, m.b3, m.c3, m.d3,
		m.a4, m.b4, m.c4, m.d4,
	};
}

glm::quat to_glm(const aiQuaternion& q)
{
	return {q.w, q.x, q.y, q.z};
}

AssimpLoader::AssimpLoader(const Options& options):
	options(options)
{}

std::shared_ptr<Model> AssimpLoader::load(const std::filesystem::path& input_path)
{
	using namespace std::string_literals;

	unsigned int flag = 0;
	if (options.triangulate) flag |= aiProcess_Triangulate;
	if (options.flip_uv) flag |= aiProcess_FlipUVs;
	if (options.calculate_tangent_space) flag |= aiProcess_CalcTangentSpace;
	if (options.generate_normal) flag |= aiProcess_GenNormals;
	if (options.populate_amature) flag |=  aiProcess_PopulateArmatureData;
	if (options.limit_bone_weights) flag |=  aiProcess_LimitBoneWeights;

	path = root_path / input_path;

	Assimp::Importer importer;
	scene = importer.ReadFile(path.string().data(), flag);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw std::runtime_error("[Error] Failed to open model: "s + path.string());
	}

	parent_directory = path.parent_path();

	load_texture_from_scene();
	load_material_from_scene();
	
	TimePoint t0 = Clock::now();
	load_mesh_from_scene();
	TimePoint t1 = Clock::now();
	
	root_node = load_node(scene->mRootNode);
	
	if (scene->HasAnimations())
	{
		load_bone();
		load_animation_from_scene();
	}
	for (auto mesh: meshes)
	{
		mesh->enable_attrib();
	}
	root_node->cull_empty();
	return root_node;
}

void AssimpLoader::load_texture_from_scene()
{
	for (int i = 0; i < scene->mNumTextures; ++i)
	{
		aiTexture* ai_texture = scene->mTextures[i];
		std::filesystem::path texture_path = parent_directory / ai_texture->mFilename.C_Str();
		std::string texture_path_string = texture_path.string();
		Texture::add_texture(texture_path_string);
	}
}

template <aiTextureType TextureType>
Texture::Ptr AssimpLoader::load_texture_from_material(const aiMaterial* ai_material)
{
	int texture_count = ai_material->GetTextureCount(TextureType);
	if (!texture_count)
	{
		if constexpr (TextureType == aiTextureType_AMBIENT) return Texture::default_texture();
		else if (TextureType == aiTextureType_DIFFUSE) return Texture::default_texture();
		else if (TextureType == aiTextureType_SPECULAR) return Texture::default_texture();
		else if (TextureType == aiTextureType_HEIGHT)
		{
			return load_texture_from_material<aiTextureType_NORMALS>(ai_material);
		}
		else if (TextureType == aiTextureType_NORMALS) return Texture::default_normal_map();
		return nullptr;
	}
	aiString ai_texture_path;
	ai_material->GetTexture(TextureType, 0, &ai_texture_path);

	if (ai_texture_path.length == 0)
	{
		WARNING(__func__, "Texture path is empty");
		return nullptr;
	}
	std::filesystem::path texture_path = parent_directory / ai_texture_path.C_Str();
	std::string texture_path_string = texture_path.string();
	return Texture::add_texture(texture_path_string);
}

void AssimpLoader::load_material_from_scene()
{
	for (int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* ai_material = scene->mMaterials[i];
		std::string material_name = ai_material->GetName().C_Str();
		if (material_name.empty())
		{
			continue;
		}
		Material::Ptr material = Material::get_material(material_name);
		if (material) 
		{
			return;
		}
		material = Material::add_empty_material(material_name);
		material->ambient = load_texture_from_material<aiTextureType_AMBIENT>(ai_material);
		material->diffuse = load_texture_from_material<aiTextureType_DIFFUSE>(ai_material);
		material->specular = load_texture_from_material<aiTextureType_SPECULAR>(ai_material);
		material->normal = load_texture_from_material<aiTextureType_HEIGHT>(ai_material);
		material->name = material_name;
		aiColor3D ai_color;
		ai_material->Get(AI_MATKEY_COLOR_AMBIENT, ai_color);
		material->ka = to_glm(ai_color);
		ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, ai_color);
		material->kd = to_glm(ai_color);
		ai_material->Get(AI_MATKEY_COLOR_SPECULAR, ai_color);
		material->ks = to_glm(ai_color);
	}
}

void AssimpLoader::load_mesh_from_scene()
{
	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		const aiMesh* ai_mesh = scene->mMeshes[i];
		meshes.emplace_back(load_mesh(ai_mesh));
	}
}

void AssimpLoader::mesh_set_face(std::shared_ptr<Mesh> mesh, const aiMesh* ai_mesh)
{
	if constexpr (MULTI_THREAD_LOADING)
	{
		if (ai_mesh->mNumFaces > 4096)
		{
			mesh->indices.resize(ai_mesh->mNumFaces * 3);
			parallel.work(ai_mesh->mFaces, ai_mesh->mNumFaces, 15,
				[&mesh](void* p, size_t i)
				{
					const unsigned int* indices = static_cast<const aiFace*>(p)[i].mIndices;
					mesh->indices[i * 3 + 0] = indices[0];
					mesh->indices[i * 3 + 1] = indices[1];
					mesh->indices[i * 3 + 2] = indices[2];
				}
			);
		}
		else
		{
			mesh->indices.reserve(ai_mesh->mNumFaces * 3);
			for (int i = 0; i < ai_mesh->mNumFaces; ++i)
			{
				const aiFace& ai_face = ai_mesh->mFaces[i];
				mesh->indices.emplace_back(ai_face.mIndices[0]);
				mesh->indices.emplace_back(ai_face.mIndices[1]);
				mesh->indices.emplace_back(ai_face.mIndices[2]);
			}
		}
	}
	else
	{
		mesh->indices.reserve(ai_mesh->mNumFaces * 3);
		for (int i = 0; i < ai_mesh->mNumFaces; ++i)
		{
			const aiFace& ai_face = ai_mesh->mFaces[i];
			mesh->indices.emplace_back(ai_face.mIndices[0]);
			mesh->indices.emplace_back(ai_face.mIndices[1]);
			mesh->indices.emplace_back(ai_face.mIndices[2]);
		}
	}
}

template <typename Type, typename AiType>
static void copy_from_ai(
	std::vector<Type>& dst, 
	AiType* src, 
	size_t size, 
	ParallelProccessor* parallel = nullptr
)
{
	if constexpr (MULTI_THREAD_LOADING)
	{
		if (size > 4096)
		{
			dst.resize(size);
			parallel->work(src, size, std::min(15llu, size / 256), 
				[&dst](void* p, size_t i)
				{
					dst[i] = to_glm(static_cast<const AiType*>(p)[i]);
				}
			);
		}
		else
		{
			dst.reserve(size);
			for (int i = 0; i < size; ++i)
			{
				dst.emplace_back(to_glm(src[i]));
			}
		}
	}
	else
	{
		dst.reserve(size);
		for (int i = 0; i < size; ++i)
		{
			dst.emplace_back(to_glm(src[i]));
		}
	}
}

std::shared_ptr<Mesh> AssimpLoader::load_mesh(const aiMesh* ai_mesh)
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	mesh->name = ai_mesh->mName.C_Str();
	unsigned int flag = 0;
	if constexpr (MULTI_THREAD_LOADING)
	{
		if (ai_mesh->HasPositions())
		{
			copy_from_ai(mesh->position, ai_mesh->mVertices, ai_mesh->mNumVertices, &parallel);
		}
		if (ai_mesh->HasNormals())
		{
			copy_from_ai(mesh->normal, ai_mesh->mNormals, ai_mesh->mNumVertices, &parallel);
			flag |= ShaderContainer::NORMAL;
		}
		if (ai_mesh->HasTangentsAndBitangents())
		{
			copy_from_ai(mesh->tangent, ai_mesh->mTangents, ai_mesh->mNumVertices, &parallel);
			copy_from_ai(mesh->bi_tangent, ai_mesh->mBitangents, ai_mesh->mNumVertices, &parallel);
		}
		if (ai_mesh->HasTextureCoords(0))
		{
			copy_from_ai(mesh->tex_coord, ai_mesh->mTextureCoords[0], ai_mesh->mNumVertices, &parallel);
		}
		// if (ai_mesh->HasVertexColors(0))
		// {
		// 	copy_from_ai(mesh->color, ai_mesh->mColors[0], ai_mesh->mNumVertices, &parallel);
		// }
	}
	else
	{
		if (ai_mesh->HasPositions())
		{
			copy_from_ai(mesh->position, ai_mesh->mVertices, ai_mesh->mNumVertices);
		}
		if (ai_mesh->HasNormals())
		{
			copy_from_ai(mesh->normal, ai_mesh->mNormals, ai_mesh->mNumVertices);
			flag |= ShaderContainer::NORMAL;
		}
		if (ai_mesh->HasTangentsAndBitangents())
		{
			copy_from_ai(mesh->tangent, ai_mesh->mTangents, ai_mesh->mNumVertices);
			copy_from_ai(mesh->bi_tangent, ai_mesh->mBitangents, ai_mesh->mNumVertices);
		}
		if (ai_mesh->HasTextureCoords(0))
		{
			copy_from_ai(mesh->tex_coord, ai_mesh->mTextureCoords[0], ai_mesh->mNumVertices);
		}
		// if (ai_mesh->HasVertexColors(0))
		// {
		// 	copy_from_ai(mesh->color, ai_mesh->mColors[0], ai_mesh->mNumVertices);
		// }
	}
	if (ai_mesh->HasFaces())
	{
		mesh_set_face(mesh, ai_mesh);
	}
	
	std::string material_name = scene->mMaterials[ai_mesh->mMaterialIndex]->GetName().C_Str();
	mesh->material = Material::get_material(material_name);
	if (mesh->material->normal)
	{
		flag |= ShaderContainer::NORMAL_MAP;
	}
	if (ai_mesh->HasBones())
	{
		flag |= ShaderContainer::RIGGING;
	}
	mesh->shader = ShaderContainer::default_shader(flag);
	mesh->shadow_shader = ShaderContainer::default_shadow_shader(flag);
	mesh->shadow_map_shader = ShaderContainer::default_shadow_map_shader(flag);

	return mesh;
}

static std::string indent(int i)
{
	std::string result;
	while (i--)
		result += " ";
	return result;
}

std::shared_ptr<Model> AssimpLoader::load_node(const aiNode* ai_node)
{
	static int depth = 0;

	std::shared_ptr<Model> model = nodes.emplace_back(std::make_shared<Model>());
	nodes_map.emplace(ai_node->mName.C_Str(), model);
	model->name = ai_node->mName.C_Str();
	model->transform = to_glm(ai_node->mTransformation);
	model->meshes.reserve(ai_node->mNumMeshes);
	for (int i = 0; i < ai_node->mNumMeshes; ++i)
	{
		model->meshes.emplace_back(meshes[ai_node->mMeshes[i]]);
	}
	model->meshes.reserve(ai_node->mNumChildren);
	for (int i = 0; i < ai_node->mNumChildren; ++i)
	{
		auto tmp = model->children.emplace_back(load_node(ai_node->mChildren[i]));
	}
	return model;
};


void AssimpLoader::load_bone()
{
	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		const aiMesh* ai_mesh = scene->mMeshes[i];
		if (!ai_mesh->HasBones()) 
			continue;
		mesh_set_bone(meshes[i], ai_mesh);
	}
}

static void set_weight(std::shared_ptr<Mesh> mesh, const aiBone* ai_bone, unsigned int idx_bone)
{
	for (int i = 0; i < ai_bone->mNumWeights; ++i)
	{
		const aiVertexWeight& ai_weight = ai_bone->mWeights[i];
		const int idx_position = ai_weight.mVertexId;
		unsigned int& indices_count = mesh->bone_indices_count[idx_position];
		if constexpr (WEIGHT_COUNT > 4)
		{
			if (indices_count >= WEIGHT_COUNT)
			{
				++indices_count;
				WARNING(__func__, "Too many weights: ", '[', idx_position , ']', indices_count);
				continue;
			}
		}
		mesh->bone_indices[idx_position][indices_count] = idx_bone;
		mesh->weights[idx_position][indices_count] = ai_weight.mWeight;
		++indices_count;
	}
}

static void init_bone_map(Bone& bone, std::map<std::string, Bone*>& bone_map)
{
	bone_map[bone.name] = &bone;
	for (Bone& child: bone.children)
	{
		init_bone_map(child, bone_map);
	}
}

void AssimpLoader::mesh_set_bone(std::shared_ptr<Mesh> mesh, const aiMesh* ai_mesh)
{
	mesh->has_bone = true;
	mesh->weights.resize(mesh->position.size(), {0.0f});
	mesh->bone_indices.resize(mesh->position.size(), {0u});
	mesh->bone_indices_count.resize(mesh->position.size(), 0);

	mesh->bone = std::move(Bone(root_node));
	std::map<std::string, Bone*> bone_map;
	init_bone_map(mesh->bone, bone_map);
	
	for (int idx_bone = 0; idx_bone < ai_mesh->mNumBones; ++idx_bone)
	{
		const aiBone* ai_bone = ai_mesh->mBones[idx_bone];
		auto it_bone = bone_map.find(ai_bone->mName.C_Str());
		if (it_bone == bone_map.end())
		{
			WARNING(__func__, "Bone not found: ", ai_bone->mName.C_Str());
			continue;
		}
		Bone& bone = *it_bone->second;
		
		bone.idx = idx_bone;
		set_weight(mesh, ai_bone, idx_bone);
		bone.offset = to_glm(ai_bone->mOffsetMatrix);
	}
}

void AssimpLoader::load_animation_from_scene()
{
	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		if (!scene->mMeshes[i]->HasBones())
			continue;
		Mesh* mesh = meshes[i].get();
		for (int j = 0; j < scene->mNumAnimations; ++j)
		{
			load_animation(mesh, scene->mAnimations[j]);
		}
	}
}

void AssimpLoader::load_animation(Mesh* mesh, const aiAnimation* ai_animation)
{
	std::map<std::string, Bone*> bone_map;
	init_bone_map(mesh->bone, bone_map);
	for (int i = 0; i < ai_animation->mNumChannels; ++i)
	{
		const aiNodeAnim* node = ai_animation->mChannels[i];

		auto it = bone_map.find(node->mNodeName.C_Str());
		if (it == bone_map.end())
		{
			WARNING(__func__, "Bone not found: ", node->mNodeName.C_Str());
			continue;
		}
		Bone* bone = it->second;

		Animation& animation = bone->animations.emplace_back();

		animation.position_key.reserve(node->mNumPositionKeys);
		for (int k = 0; k < node->mNumPositionKeys; ++k)
		{
			const auto& ai_key = node->mPositionKeys[k];
			PositionKey& position_key = animation.position_key.emplace_back(
				Microseconds(static_cast<size_t>(ai_key.mTime / ai_animation->mTicksPerSecond * 1000000)),
				to_glm(ai_key.mValue)
			);
		}
		animation.rotation_key.reserve(node->mNumRotationKeys);
		for (int k = 0; k < node->mNumRotationKeys; ++k)
		{
			const auto& ai_key = node->mRotationKeys[k];
			RotationKey& rotation_key = animation.rotation_key.emplace_back(
				Microseconds(static_cast<size_t>(ai_key.mTime / ai_animation->mTicksPerSecond * 1000000)),
				to_glm(ai_key.mValue)
			);
		}
		animation.scaling_key.reserve(node->mNumScalingKeys);
		for (int k = 0; k < node->mNumScalingKeys; ++k)
		{
			const auto& ai_key = node->mScalingKeys[k];
			animation.scaling_key.emplace_back(
				Microseconds(static_cast<size_t>(ai_key.mTime / ai_animation->mTicksPerSecond * 1000000)),
				to_glm(ai_key.mValue)
			);
		}
	}
}

void AssimpLoader::cull_empty_node(std::shared_ptr<Model> node)
{

}