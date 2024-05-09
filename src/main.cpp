#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keycode.h"
#include "glm/fwd.hpp"
#include "r3dgl.hpp"
#include <cmath>
#include <cstdint>
#include <stdio.h>

#include <stb/stb_image_write.h>

#include <SDL2/SDL.h>

#include <log.hpp>
#include <camera.hpp>
#include <model.hpp>
#include <renderer.hpp>
#include <string>
#include <vector>
#include <unordered_map>

const char *hdris[] = {

	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/WinterForest.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/AlexsApt.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/cobblestone_street_night_1k.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/cyclorama_hard_light_1k.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/etzwihl_1k.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/fireplace_1k.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/FrozenWaterfall.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/industrial_sunset_02_puresky_1k.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/kloppenheim_04_1k.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/Kloppenheim.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/Milkyway.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/MonValley.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/moonless_golf_1k.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/moonlit_golf_1k.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/newport_loft.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/steinbach_field_1k.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/studio_small_04_1k.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/thatch_chapel_1k.hdr",
	"/home/dev/Documents/projects/cpp/renderer3d/assets/hdri/UenoShrine.hdr"
};

SDL_Window *window;

int main(int argv, char** args){

	if(SDL_Init(SDL_INIT_EVERYTHING)){
		LOGERR("error SDL init\n");
		return 0;
	}

	int width = 0, height = 0;

	window = SDL_CreateWindow("SDL_LearnOpengl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
	if(!window){

		LOGERR("window creation failed\n");
		return 0;
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_GetWindowSize(window, &width, &height);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_GLContext GLctx = SDL_GL_CreateContext(window);
	if(!GLctx){

		LOGERR("GL context creation failed\n");
		return 0;
	}
	SDL_GL_MakeCurrent(window, GLctx);

	if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
		LOGERR("failed to initialize glad\n");
		return 0;
	}

	CameraFly camera;
	camera.pos = glm::vec3(0.0, 0.0, 3.0);
	camera.dir = glm::vec3(0.0, 0.0, -1.0);

	float mspf = 1000.0f/60;
	uint64_t lastTime = SDL_GetTicks();
	SDL_Event event;
	bool close = false;

	Renderer3d renderer(width, height);
	renderer.LoadHDRI(hdris[0]);
	int hdri = 1;

	std::vector<Model*> models;
	int model = 0;

	// Model model1("assets/models/football/football_2k.gltf");
	// model1.scale *= 4;
	// {
	// 	Material mat;
	// 	mat.diffuse.Load("assets/models/football/textures/football_diff_2k.jpg");
	// 	mat.metallic.Load("assets/models/football/textures/football_arm_2k.jpg");
	// 	mat.roughness.Load("assets/models/football/textures/football_arm_2k.jpg");
	// 	mat.normal.Load("assets/models/football/textures/football_nor_gl_2k.jpg");
	// 	mat.ao.Load("assets/models/football/textures/football_arm_2k.jpg");

	// 	for (int i = 0; i < model1.materials.size(); ++i)
	// 		model1.materials[i] = mat;
	// }
	// models.push_back(&model1);

	Model model2("assets/models/cat_statue/concrete_cat_statue_1k.gltf");
	model2.scale *= 4;
	{
		Material mat;
		mat.diffuse.Load("assets/models/cat_statue/textures/concrete_cat_statue_diff_1k.jpg");
		mat.metallic.Load("assets/models/cat_statue/textures/concrete_cat_statue_arm_1k.jpg");
		mat.roughness.Load("assets/models/cat_statue/textures/concrete_cat_statue_arm_1k.jpg");
		mat.normal.Load("assets/models/cat_statue/textures/concrete_cat_statue_nor_gl_1k.jpg");
		mat.ao.Load("assets/models/cat_statue/textures/concrete_cat_statue_arm_1k.jpg");

		for (int i = 0; i < model2.materials.size(); ++i)
			model2.materials[i] = mat;
	}
	models.push_back(&model2);

	Model model3("assets/models/sofa/sofa_03_2k.gltf");
	{
		Material mat;
		mat.diffuse.Load("assets/models/sofa/textures/sofa_03_diff_2k.jpg");
		// mat.metallic.Load("assets/models/sofa/textures/sofa_03_arm_2k.jpg");
		mat.roughness.Load("assets/models/sofa/textures/sofa_03_rough_2k.jpg");
		mat.r_channel = glm::vec3(1.0, 0.0, 0.0);
        // LOGERR("r_channel %f %f %f\n", mat.r_channel.x, mat.r_channel.y, mat.r_channel.z);
		mat.normal.Load("assets/models/sofa/textures/sofa_03_nor_gl_2k.jpg");
		// mat.ao.Load("assets/models/sofa/textures/sofa_03_arm_2k.jpg");

		for (int i = 0; i < model3.materials.size(); ++i)
			model3.materials[i] = mat;
	}
	models.push_back(&model3);

	Model model4("assets/models/gun/Cerberus_LP.FBX");
	model4.scale = glm::vec3(0.025);
	model4.rotation.x = -90;
	model4.position.x = 2;
	model4.rotation.y = -90;
	{
		Material mat;
		mat.diffuse.Load("assets/models/gun/Textures/Cerberus_A.jpg");
		mat.metallic.Load("assets/models/gun/Textures/Cerberus_M.jpg");
		mat.roughness.Load("assets/models/gun/Textures/Cerberus_R.jpg");
		mat.normal.Load("assets/models/gun/Textures/Cerberus_N.jpg");
		mat.ao.Load("assets/models/gun/Textures/Raw/Cerberus_AO.jpg");

		for (int i = 0; i < model4.materials.size(); ++i)
			model4.materials[i] = mat;
	}
	models.push_back(&model4);

	Model model5("assets/models/chest/chest.obj");
	model5.scale *= 2;
	{
		Material mat;
		mat.diffuse.Load("assets/models/chest/textures/treasure_chest_diff_2k.jpg");
		mat.metallic.Load("assets/models/chest/textures/treasure_chest_arm_2k.jpg");
		mat.roughness.Load("assets/models/chest/textures/treasure_chest_arm_2k.jpg");
		mat.normal.Load("assets/models/chest/textures/treasure_chest_nor_gl_2k.jpg");
		// printf("loading ao\n");
		mat.ao.Load("assets/models/chest/textures/treasure_chest_arm_2k.jpg");

		for (int i = 0; i < model5.materials.size(); ++i)
			model5.materials[i] = mat;
	}
	models.push_back(&model5);

	Model model6("assets/models/coffeecart/coffeecart.obj");
	{
		Material mat1;
		mat1.diffuse.Load("assets/models/coffeecart/textures/CoffeeCart_01_cart_diff_4k.jpg");
		mat1.metallic.Load("assets/models/coffeecart/textures/CoffeeCart_01_cart_arm_4k.jpg");
		mat1.roughness.Load("assets/models/coffeecart/textures/CoffeeCart_01_cart_arm_4k.jpg");
		mat1.normal.Load("assets/models/coffeecart/textures/CoffeeCart_01_cart_nor_gl_4k.jpg");
		mat1.ao.Load("assets/models/coffeecart/textures/CoffeeCart_01_cart_arm_4k.jpg");

		Material mat2;
		mat2.diffuse.Load("assets/models/coffeecart/textures/CoffeeCart_01_props_diff_4k.jpg");
		mat2.metallic.Load("assets/models/coffeecart/textures/CoffeeCart_01_props_arm_4k.jpg");
		mat2.roughness.Load("assets/models/coffeecart/textures/CoffeeCart_01_props_arm_4k.jpg");
		mat2.normal.Load("assets/models/coffeecart/textures/CoffeeCart_01_props_nor_gl_4k.jpg");
		mat2.ao.Load("assets/models/coffeecart/textures/CoffeeCart_01_props_arm_4k.jpg");

		model6.materials[1] = mat1;
		model6.materials[3] = mat2;
	}
	models.push_back(&model6);

	float delta;

	bool ctrlDown = false;
	bool shiftDown = false;

	while(!close){

		delta = (SDL_GetTicks() - lastTime)/1000.0f;
		lastTime = SDL_GetTicks();

		while (SDL_PollEvent(&event)) {

			if(event.type == SDL_QUIT)
				close = true;

			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RCTRL){
				if(!ctrlDown)
					ctrlDown = true;
			}
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RSHIFT){
				if(!shiftDown)
					shiftDown = true;
			}
			if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_RCTRL){
				if(ctrlDown){
					ctrlDown = false;
					renderer.LoadHDRI(hdris[hdri]);
					hdri++;
					if(hdri == 19)
						hdri = 0;
				}
			}
			if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_RSHIFT){
				if(shiftDown){
					shiftDown = false;
					model++;
					if(model == 5)
						model = 0;
				}
			}
			if(event.type == SDL_KEYUP || event.type == SDL_KEYDOWN || event.type == SDL_MOUSEMOTION)
				camera.ProcessInput(*window, event, delta);
		}

		camera.ProcessFrame(*window, delta);

		renderer.BeginDraw(camera);
		renderer.Draw(*models[model]);
		renderer.EndDraw();

		SDL_GL_SwapWindow(window);

		uint64_t delay = SDL_GetTicks() - lastTime;
		if(delay < mspf)
			SDL_Delay(mspf - delay);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
