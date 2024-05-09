#ifndef MODEL_HPP
#define MODEL_HPP

#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <unordered_map>
#include <string>
#include <r3dgl.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex{

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 texCoord;
};

class Material{

public:

	Material(){}
	Material(const Material &mat);

	std::string name;

	Texture2d diffuse;
	//Texture2d specular;
	Texture2d metallic;
	glm::vec3 m_channel = glm::vec3(0.0, 0.0, 1.0);
	Texture2d roughness;
	glm::vec3 r_channel = glm::vec3(0.0, 1.0, 0.0);
	Texture2d normal;
	Texture2d ao;
	glm::vec3 ao_channel = glm::vec3(1.0, 0.0, 0.0);
};

class Mesh{

public:

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int mat_id;

	Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, unsigned int mat_id);

	VAO vao;
	VBO vbo;
	EBO ebo;

private:

	void setupMesh();
};

class Model{

public:

	Model(const char *path);
	std::vector<Mesh> meshes;
	std::vector<Material> materials;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

private:

	std::string dirPath;

	void LoadModel(std::string path);
	void ProcessNode(aiNode *node, const aiScene *scene);
	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
	void LoadMaterials(const aiScene *scene);
};

#endif