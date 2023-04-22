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
};