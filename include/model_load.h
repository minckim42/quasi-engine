#pragma once
#include <filesystem>
#include <functional>
#include <vector>
#include "mesh.h"
#include "model.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "definition.h"
#include "texture.h"
#include "material.h"
#include "parallel.h"

glm::vec3 to_glm(const aiColor3D& v);
glm::vec4 to_glm(const aiColor4D& v);
glm::vec2 to_glm(const aiVector2D& v);
glm::vec3 to_glm(const aiVector3D& v);
glm::mat3 to_glm(const aiMatrix3x3& m);
glm::mat4 to_glm(const aiMatrix4x4& m);
glm::quat to_glm(const aiQuaternion& q);

class AssimpLoader
{
public:
	struct Options
	{
		bool triangulate = true;
		bool flip_uv = false;
		bool calculate_tangent_space = true;
		bool generate_normal = true;
		bool populate_amature = true;
		bool limit_bone_weights = true;

		std::shared_ptr<Object> load(const std::filesystem::path& input_path);
	};

	std::shared_ptr<Object> load(
		const std::filesystem::path& input_path, 
		unsigned int flag = 
			aiProcess_Triangulate | 
			aiProcess_CalcTangentSpace |
			aiProcess_GenNormals
	);

private:
	const aiScene* scene;
	std::filesystem::path parent_directory;
	std::filesystem::path path;

	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Model>> nodes;
	std::map<std::string, std::shared_ptr<Model>> nodes_map;
	std::shared_ptr<Model> root_node;

	ParallelProccessor parallel = ParallelProccessor(16);

	void load_texture_from_scene();
	Texture* load_texture_from_material(
		const aiMaterial* ai_material, 
		const aiTextureType ai_texture_type
	);
	void load_material_from_scene();
	void load_mesh_from_scene();
	void mesh_set_face(std::shared_ptr<Mesh> mesh, const aiMesh* ai_mesh);
	std::shared_ptr<Mesh> load_mesh(const aiMesh* ai_mesh);
	std::shared_ptr<Model> load_node(const aiNode* ai_node);
	
	void load_bone();
	void mesh_set_bone(std::shared_ptr<Mesh> mesh, const aiMesh* ai_mesh);
	void load_animation_from_scene();
	void load_animation(Mesh* mesh, const aiAnimation* ai_animation);
	void cull_empty_node(std::shared_ptr<Model> node);
};



	// template <unsigned int Flag>
	// std::shared_ptr<Mesh> load_mesh(const aiMesh* ai_mesh)
	// {
	// 	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	// 	if constexpr(Flag & (1u << VERTEX_ATTRIB::POSITION))
	// 	{
	// 		mesh->position.resize(ai_mesh->mNumVertices);
	// 	}
	// 	if constexpr(Flag & (1u << VERTEX_ATTRIB::NORMAL))
	// 	{
	// 		mesh->normal.resize(ai_mesh->mNumVertices);
	// 	}
	// 	if constexpr(Flag & (1u << VERTEX_ATTRIB::TANGENT))
	// 	{
	// 		mesh->tangent.resize(ai_mesh->mNumVertices);
	// 	}
	// 	if constexpr(Flag & (1u << VERTEX_ATTRIB::BI_TANGENT))
	// 	{
	// 		mesh->bi_tangent.resize(ai_mesh->mNumVertices);
	// 	}
	// 	if constexpr(Flag & (1u << VERTEX_ATTRIB::TEX_COORD))
	// 	{
	// 		mesh->tex_coord.resize(ai_mesh->mNumVertices);
	// 	}
	// 	if constexpr(Flag & (1u << VERTEX_ATTRIB::COLOR))
	// 	{
	// 		mesh->color.resize(ai_mesh->mNumVertices);
	// 	}
	// 	parallel.work(ai_mesh, ai_mesh->mNumVertices, 8,
	// 		[this, &mesh](void* p, int i)
	// 		{
	// 			const aiMesh* ai_mesh = reinterpret_cast<const aiMesh*>(p);

	// 			if constexpr(Flag & (1u << VERTEX_ATTRIB::POSITION))
	// 			{
	// 				mesh->position[i] = to_glm(ai_mesh->mVertices[i]);
	// 			}
	// 			if constexpr(Flag & (1u << VERTEX_ATTRIB::NORMAL))
	// 			{
	// 				mesh->normal[i] = to_glm(ai_mesh->mNormals[i]);
	// 			}
	// 			if constexpr(Flag & (1u << VERTEX_ATTRIB::TANGENT))
	// 			{
	// 				mesh->tangent[i] = to_glm(ai_mesh->mTangents[i]);
	// 			}
	// 			if constexpr(Flag & (1u << VERTEX_ATTRIB::BI_TANGENT))
	// 			{
	// 				mesh->bi_tangent[i] = to_glm(ai_mesh->mBitangents[i]);
	// 			}
	// 			if constexpr(Flag & (1u << VERTEX_ATTRIB::TEX_COORD))
	// 			{
	// 				mesh->tex_coord[i] = to_glm(ai_mesh->mTextureCoords[0][i]);
	// 			}
	// 			if constexpr(Flag & (1u << VERTEX_ATTRIB::COLOR))
	// 			{
	// 				mesh->color[i] = to_glm(ai_mesh->mColors[0][i]);
	// 			}
	// 		}
	// 	);

	// 	std::string material_name = scene->mMaterials[ai_mesh->mMaterialIndex]->GetName().C_Str();
	// 	mesh->material = &Material::container[material_name];
	// 	return mesh;
	// }

	// void load_mesh_from_scene()
	// {
	// 	for (int i = 0; i < scene->mNumMeshes; ++i)
	// 	{
	// 		const aiMesh* ai_mesh = scene->mMeshes[i];
	// 		unsigned int flag = 0;
	// 		if (ai_mesh->HasPositions())
	// 		{
	// 			flag |= (1u << static_cast<unsigned int>(VERTEX_ATTRIB::POSITION));
	// 		}
	// 		if (ai_mesh->HasNormals())
	// 		{
	// 			flag |= (1u << static_cast<unsigned int>(VERTEX_ATTRIB::NORMAL));
	// 		}
	// 		if (ai_mesh->HasTangentsAndBitangents())
	// 		{
	// 			flag |= (1u << static_cast<unsigned int>(VERTEX_ATTRIB::TANGENT));
	// 			flag |= (1u << static_cast<unsigned int>(VERTEX_ATTRIB::BI_TANGENT));
	// 		}
	// 		if (ai_mesh->HasTextureCoords(0))
	// 		{
	// 			flag |= (1u << static_cast<unsigned int>(VERTEX_ATTRIB::TEX_COORD));
	// 		}
	// 		if (ai_mesh->HasVertexColors(0))
	// 		{
	// 			flag |= (1u << static_cast<unsigned int>(VERTEX_ATTRIB::COLOR));
	// 		}
	// 		meshes.emplace_back(load_mesh<flag>(ai_mesh));
	// 	}
	// }