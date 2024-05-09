#include <camera.hpp>

CameraFly::CameraFly()
	: pos(0.0f, 0.0f, 3.0f), dir(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f), speed(5), yaw(0), pitch(0),
	  keyL(false), keyU(false), keyD(false), keyR(false) {
}

void CameraFly::ProcessInput(SDL_Window &window, SDL_Event &event, float delta){

	if(event.type == SDL_MOUSEMOTION){

		yaw += event.motion.xrel*0.1f;
		pitch += event.motion.yrel*0.1f;

		if(pitch > 80.0f)
			pitch = 80.0f;
		if(pitch < -80.0f)
			pitch = -80.0f;

		glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(-yaw), up);
		dir = trans*glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);

		glm::vec3 right = glm::normalize(glm::cross(up, dir));
		trans = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), right);
		dir = trans*glm::vec4(dir, 1.0f);

		dir = glm::normalize(dir);
	}

	if(event.type == SDL_KEYDOWN){

		if(event.key.keysym.sym == SDLK_LEFT){
			keyL = true;
		}
		if(event.key.keysym.sym == SDLK_RIGHT){
			keyR = true;
		}
		if(event.key.keysym.sym == SDLK_UP){
			keyU = true;
		}
		if(event.key.keysym.sym == SDLK_DOWN){
			keyD = true;
		}
	}

	if(event.type == SDL_KEYUP){

		if(event.key.keysym.sym == SDLK_LEFT){
			keyL = false;
		}
		if(event.key.keysym.sym == SDLK_RIGHT){
			keyR = false;
		}
		if(event.key.keysym.sym == SDLK_UP){
			keyU = false;
		}
		if(event.key.keysym.sym == SDLK_DOWN){
			keyD = false;
		}
	}

}

void CameraFly::ProcessFrame(SDL_Window &window, float delta){

	if(keyL){
		pos -= glm::normalize(glm::cross(dir, up))*speed*delta;
	}
	if(keyR){
		pos += glm::normalize(glm::cross(dir, up))*speed*delta;
	}
	if(keyU){
		pos += dir*speed*delta;
	}
	if(keyD){
		pos -= dir*speed*delta;
	}
}

glm::mat4 CameraFly::LookAtMat(){

	return glm::lookAt(pos, pos+dir, up);
}