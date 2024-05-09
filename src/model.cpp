#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "glm/fwd.hpp"
#include <cstdio>
#include <log.hpp>
#include <string>
#include <vector>
#include <r3dgl.hpp>

#include <model.hpp>

Material::Material(const Material &mat)
	: name(mat.name), diffuse(mat.diffuse), metallic(mat.metallic), roughness(mat.roughness), ao(mat.ao),
	normal(mat.normal), r_channel(mat.r_channel), m_channel(mat.m_channel), ao_channel(mat.ao_channel){
}

Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, unsigned int _mat_id)
	: vertices(_vertices), indices(_indices), mat_id(_mat_id){

	setupMesh();
}

void Mesh::setupMesh(){

	vao.New();
	vbo.New();
	ebo.New();

	vao.Bind();

	vbo.Bind();
	vbo.Data(vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	ebo.Bind();
	ebo.Data(indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	vao.EnableAttrib(0);
	vao.AttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));
	vao.EnableAttrib(1);
	vao.AttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	vao.EnableAttrib(2);
	vao.AttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	vao.EnableAttrib(3);
	vao.AttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	vao.Unbind();
}



Model::Model(const char *path)
	: position(0.0f), rotation(0.0f), scale(1.0f){

	LoadModel(path);
}

void Model::LoadModel(std::string path){

	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate  | aiProcess_CalcTangentSpace);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		LOGERR("error : Model Loading Failed : %s: %s\n", path.c_str(), importer.GetErrorString());
		return;
	}

	LOGERR("loading model ... %s\n", path.c_str());

	dirPath = path.substr(0, path.find_last_of("/"));
	LoadMaterials(scene);
	ProcessNode(scene->mRootNode, scene);
}

void Model::LoadMaterials(const aiScene *scene){

	for(int i = 0; i < scene->mNumMaterials; i++){

		Material mat;
		aiMaterial *ai_mat = scene->mMaterials[i];
		mat.name = ai_mat->GetName().C_Str();

		aiString file;
		std::string path;

		ai_mat->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		if(std::string(file.C_Str()) != ""){
			path = dirPath+"/"+file.C_Str();
			mat.diffuse.Load(path.c_str());
		}
		LOGERR("material %d %s\n", i, file.C_Str());

		// ai_mat->GetTexture(aiTextureType_SPECULAR, 0, &file);
		// if(std::string(file.C_Str()) != ""){
		// 	path = dirPath+"/"+file.C_Str();
		// 	mat.specular.Load(path.c_str());
		// }

		ai_mat->GetTexture(aiTextureType_METALNESS, 0, &file);
		if(std::string(file.C_Str()) != ""){
			path = dirPath+"/"+file.C_Str();
			mat.metallic.Load(path.c_str());
		}

		ai_mat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &file);
		if(std::string(file.C_Str()) != ""){
			path = dirPath+"/"+file.C_Str();
			mat.roughness.Load(path.c_str());
		}

		ai_mat->GetTexture(aiTextureType_NORMALS, 0, &file);
		if(std::string(file.C_Str()) != ""){
			path = dirPath+"/"+file.C_Str();
			mat.normal.Load(path.c_str());
		}

		ai_mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &file);
		if(std::string(file.C_Str()) != ""){
			path = dirPath+"/"+file.C_Str();
			mat.ao.Load(path.c_str());
		}

		materials.push_back(mat);
	}
}

void Model::ProcessNode(aiNode *node, const aiScene *scene){

	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	for(unsigned int i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene);

	// printf("processing model completed\n");
	// fflush(stdout);
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene){

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int mat_id = -1;

	for(unsigned int i = 0; i < mesh->mNumVertices; i++){

		Vertex v;

		glm::vec3 position;
		position.x = mesh->mVertices[i].x;
		position.y = mesh->mVertices[i].y;
		position.z = mesh->mVertices[i].z;
		v.position = position;

		glm::vec3 normal;
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		v.normal = normal;

		glm::vec3 tangent;
		tangent.x = mesh->mTangents[i].x;
		tangent.y = mesh->mTangents[i].y;
		tangent.z = mesh->mTangents[i].z;
		v.tangent = tangent;

		if(mesh->mTextureCoords[0]){

			glm::vec2 texCoord;
			texCoord.x = mesh->mTextureCoords[0][i].x;
			texCoord.y = mesh->mTextureCoords[0][i].y;
			v.texCoord = texCoord;
		}else
			v.texCoord = glm::vec2(0.0f, 0.f);

		vertices.push_back(v);
	}

	for(unsigned int i = 0; i < mesh->mNumFaces; i++){

		aiFace face = mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if(mesh->mMaterialIndex >= 0){

		mat_id = mesh->mMaterialIndex;
		LOGERR("material index %d\n", mesh->mMaterialIndex);
	}

	return Mesh(vertices, indices, mat_id);
}