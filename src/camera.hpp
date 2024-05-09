#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "SDL2/SDL_events.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class CameraFly{

public:

	CameraFly();
	void ProcessInput(SDL_Window &window, SDL_Event &event, float delta);
	void ProcessFrame(SDL_Window &window, float delta);
	glm::mat4 LookAtMat();

	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 up;

	float speed;
	float yaw;
	float pitch;

private:

	bool keyL;
	bool keyR;
	bool keyU;
	bool keyD;
};

#endif