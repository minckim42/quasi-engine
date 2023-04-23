#include "model_load.h"
#include "assimp/anim.h"
#include <chrono>
#include <algorithm>
#include "warning.h"

#include "glm_to_string.h"

#include "clock.h"

constexpr int MULTI_THREAD_LOADING = 0;

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

std::shared_ptr<Object> AssimpLoader::Options::load(const std::filesystem::path& input_path)
{
	unsigned int flag = 0;
	if (triangulate) flag |= aiProcess_Triangulate;
	if (flip_uv) flag |= aiProcess_FlipUVs;
	if (calculate_tangent_space) flag |= aiProcess_CalcTangentSpace;
	if (generate_normal) flag |= aiProcess_GenNormals;
	if (populate_amature) flag |=  aiProcess_PopulateArmatureData;
	if (limit_bone_weights) flag |=  aiProcess_LimitBoneWeights;
	return AssimpLoader().load(input_path, flag);
}

std::shared_ptr<Object> AssimpLoader::load(
	const std::filesystem::path& input_path, 
	unsigned int flag)
{
	using namespace std::string_literals;
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
	std::cout << "load mesh: " << (t1 - t0).count() /1000 << std::endl;
	
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
		Texture::Container::iterator it = Texture::container.find(texture_path_string);
		if (it != Texture::container.end())
		{
			continue;
		}
		Texture::container[texture_path_string] = texture_path;
	}
}

Texture* AssimpLoader::load_texture_from_material(
	const aiMaterial* ai_material, 
	const aiTextureType ai_texture_type
)
{
	int texture_count = ai_material->GetTextureCount(ai_texture_type);
	if (!texture_count)
	{
		return nullptr;
	}
	aiString ai_texture_path;
	ai_material->GetTexture(ai_texture_type, 0, &ai_texture_path);
	
	if (ai_texture_path.length == 0)
	{
		return nullptr;
	}
	std::filesystem::path texture_path = parent_directory / ai_texture_path.C_Str();
	std::string texture_path_string = texture_path.string();
	Texture::Container::iterator it_texture = Texture::container.find(texture_path_string);
	if (it_texture == Texture::container.cend())
	{
		Texture::container[texture_path_string] = texture_path;
		return &Texture::container[texture_path_string];
	}
	return &it_texture->second;
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
		Material::Container::iterator it = Material::container.find(material_name);
		if (it != Material::container.end())
		{
			continue;
		}
		Material material = {
			load_texture_from_material(ai_material, aiTextureType_AMBIENT),
			load_texture_from_material(ai_material, aiTextureType_DIFFUSE),
			load_texture_from_material(ai_material, aiTextureType_SPECULAR),
			load_texture_from_material(ai_material, aiTextureType_NORMALS)
		};

		aiColor3D ai_color;
		ai_material->Get(AI_MATKEY_COLOR_AMBIENT, ai_color);
		material.ka = to_glm(ai_color);
		ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, ai_color);
		material.kd = to_glm(ai_color);
		ai_material->Get(AI_MATKEY_COLOR_SPECULAR, ai_color);
		material.ks = to_glm(ai_color);

		Material::container[material_name] = material;
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
		std::cout << "multy!\n";
		mesh->indices.resize(ai_mesh->mNumFaces * 3);
		parallel.work(ai_mesh->mFaces, ai_mesh->mNumFaces, 8,
			[&](void* p, int i)
			{
				const aiFace& ai_face = static_cast<const aiFace*>(p)[i];
				mesh->indices[i * 3 + 0] = ai_face.mIndices[0];
				mesh->indices[i * 3 + 1] = ai_face.mIndices[1];
				mesh->indices[i * 3 + 2] = ai_face.mIndices[2];
			}
		);
	}
	else
	{
		std::cout << "single!\n";
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
static void copy_from_ai(std::vector<Type>& dst, AiType* src, size_t size, ParallelProccessor* parallel = nullptr)
{

	if constexpr (MULTI_THREAD_LOADING)
	{
		std::cout << "multy!\n";
		dst.resize(size);
		parallel->work(src, size, 8, 
			[&dst](void* p, size_t i)
			{
				dst[i] = to_glm(reinterpret_cast<const AiType*>(p)[i]);
			}
		);
	}
	else
	{
		std::cout << "single!\n";
		dst.reserve(size);
		for (int i = 0; i < size; ++i)
		{
			dst.emplace_back(to_glm(src[i]));
		}
	}
}

std::shared_ptr<Mesh> AssimpLoader::load_mesh(const aiMesh* ai_mesh)
{
	std::shared_ptr<Mesh> mesh(new Mesh);
	if constexpr (MULTI_THREAD_LOADING)
	{
		if (ai_mesh->HasPositions())
		{
			copy_from_ai(mesh->position, ai_mesh->mVertices, ai_mesh->mNumVertices, &parallel);
		}
		if (ai_mesh->HasNormals())
		{
			copy_from_ai(mesh->normal, ai_mesh->mNormals, ai_mesh->mNumVertices, &parallel);
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
		if (ai_mesh->HasVertexColors(0))
		{
			copy_from_ai(mesh->color, ai_mesh->mColors[0], ai_mesh->mNumVertices, &parallel);
		}
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
		if (ai_mesh->HasVertexColors(0))
		{
			copy_from_ai(mesh->color, ai_mesh->mColors[0], ai_mesh->mNumVertices);
		}
	}
	if (ai_mesh->HasFaces())
	{
		mesh_set_face(mesh, ai_mesh);
	}
	
	std::string material_name = scene->mMaterials[ai_mesh->mMaterialIndex]->GetName().C_Str();
	mesh->material = &Material::container[material_name];
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
	mesh->transform.resize(nodes.size());

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
				Microseconds(static_cast<size_t>(ai_key.mTime * 100000)),
				to_glm(ai_key.mValue)
			);
		}
		animation.rotation_key.reserve(node->mNumRotationKeys);
		for (int k = 0; k < node->mNumRotationKeys; ++k)
		{
			const auto& ai_key = node->mRotationKeys[k];
			RotationKey& rotation_key = animation.rotation_key.emplace_back(
				Microseconds(static_cast<size_t>(ai_key.mTime * 100000)),
				to_glm(ai_key.mValue)
			);
		}
		animation.scaling_key.reserve(node->mNumScalingKeys);
		for (int k = 0; k < node->mNumScalingKeys; ++k)
		{
			const auto& ai_key = node->mScalingKeys[k];
			animation.scaling_key.emplace_back(
				Microseconds(static_cast<size_t>(ai_key.mTime * 100000)),
				to_glm(ai_key.mValue)
			);
		}
	}
}

void AssimpLoader::cull_empty_node(std::shared_ptr<Model> node)
{

}