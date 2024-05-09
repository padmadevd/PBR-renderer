#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include <model.hpp>
#include <r3dgl.hpp>
#include <camera.hpp>

class Renderer3d{

public:

	Renderer3d(int width, int height);

	void LoadHDRI(const char *filepath);

	void BeginDraw(CameraFly &camera);
	void Draw(Model &model);
	void EndDraw();

	~Renderer3d();

private:

	CubeMap hdriMap;
	CubeMap d_irMap;
	CubeMap s_irMap;
	Texture2d brdfLUT;

	Shader hdriToCubeShader;
	Shader s_irMapShader;
	Shader d_irMapShader;
	Shader brdfShader;
	Shader envShader;
	Shader pbrShader;

	VAO cubeVAO;
	VBO cubeVBO;

	bool brdfLUT_ready = false;

	glm::mat4 proj;
	int scr_w, scr_h;
};

#endif