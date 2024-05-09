
#include "glm/ext/matrix_clip_space.hpp"
#include <_gl.hpp>
#include <cstdio>
#include <log.hpp>
#include <SDL2/SDL.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <r3dgl.hpp>
#include <string>


Shader::Shader()
	: id(0) {
}
Shader::Shader(const char *vShaderPath, const char *fShaderPath)
	: id(0) {

	Load(vShaderPath, fShaderPath);
}
void Shader::Load(const char *vShaderPath, const char *fShaderPath){

	SDL_RWops* shaderFile = SDL_RWFromFile(vShaderPath, "r");
	if(!shaderFile){
		LOGERR("Error opening file : %s\n %s\n", vShaderPath, SDL_GetError());
	}

	int64_t size = SDL_RWsize(shaderFile);
	char *vShaderSrc = (char*)malloc(size+1);
	char *buf = vShaderSrc;
	int64_t read = 0, cur = 1;
	while (read < size && cur != 0){
		
		cur = SDL_RWread(shaderFile, buf, 1, size-read);
		read += cur;
		buf += cur;
	}
	buf[0] = '\0';
	SDL_RWclose(shaderFile);
	if(size != read){
		LOGERR("Error reading file : %s\n", vShaderPath);
	}

	shaderFile = SDL_RWFromFile(fShaderPath, "r");
	if(!shaderFile){
		LOGERR("Error opening file : %s\n %s\n", fShaderPath, SDL_GetError());
	}

	size = SDL_RWsize(shaderFile);
	char *fShaderSrc = (char*)malloc(size+1);
	buf = fShaderSrc;
	read = 0, cur = 1;
	while (read < size && cur != 0){
		
		cur = SDL_RWread(shaderFile, buf, 1, size-read);
		read += cur;
		buf += cur;
	}
	buf[0] = '\0';
	SDL_RWclose(shaderFile);
	if(size != read){
		LOGERR("Error reading file : %s\n", fShaderPath);
		// return;
	}

	unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vShaderSrc, nullptr);
	free(vShaderSrc);
	glCompileShader(vShader);

	int success;
	char infoLog[500];

	glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
	if(!success){

		glGetShaderInfoLog(vShader, 500, NULL, infoLog);
		LOGERR("Error Shader Compilation : %s\n %s\n", vShaderPath, infoLog);
		// return;
	}

	unsigned int fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fShaderSrc, nullptr);
	free(fShaderSrc);
	glCompileShader(fShader);

	glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
	if(!success){

		glGetShaderInfoLog(fShader, 500, NULL, infoLog);
		LOGERR("Error Shader Compilation : %s\n %s\n", fShaderPath, infoLog);
		// return;
	}

	if(id)
		glDeleteProgram(id);

	id = glCreateProgram();
	glAttachShader(id, vShader);
	glAttachShader(id, fShader);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if(!success){

		glGetProgramInfoLog(id, 500, NULL, infoLog);
		LOGERR("Error Linking Shader :\n %s\n", infoLog);
		// return;
	}

	glUseProgram(id);
	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

std::unordered_map<std::string, Texture2d> Texture2d::loaded = std::unordered_map<std::string, Texture2d>();

void Texture2d::New(GLenum internalformat, GLenum format, GLsizei width, GLsizei height, GLenum type, const void *data){

	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data);
}

void Texture2d::Bind(int slot){

	glActiveTexture(GL_TEXTURE0+slot);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture2d::Load(const char *filepath){

	if(loaded.find(filepath) != loaded.end()){
		// printf("skipping\n");
		id = loaded[filepath].id;
		return;
	}

	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	int w, h, nrChannels;
	unsigned char *data = stbi_load(filepath, &w, &h, &nrChannels, 0);

	if(!data){
		glDeleteTextures(1, &id);
		LOGERR("error : Texture Loading Failed: %s\n", filepath);
		return;
	}

	LOGERR("%s loading completed\n", filepath);

	int format;
	if(nrChannels == 1)
		format = GL_RED;
	else if(nrChannels == 2)
		format = GL_RG;
	else if(nrChannels == 3)
		format = GL_RGB;
	else if(nrChannels == 4)
		format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	loaded[filepath] = *this;
}

void Texture2d::Delete(){
	glDeleteTextures(1, &id);
}

RenderBuffer::RenderBuffer()
	: id(0){
}
void RenderBuffer::New(GLenum internalformat, GLsizei width, GLsizei height){

	glGenRenderbuffers(1, &id);
	glBindRenderbuffer(GL_RENDERBUFFER, id);
	glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
}
void RenderBuffer::Bind(){
	glBindRenderbuffer(GL_RENDERBUFFER, id);
}
void RenderBuffer::Delete(){
	glDeleteRenderbuffers(1, &id);
}

FrameBuffer::FrameBuffer()
	: id(0){
}
void FrameBuffer::New(){
	glGenFramebuffers(1, &id);
}
void FrameBuffer::Attach(Texture2d t, GLenum type){
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, type, GL_TEXTURE_2D, t.id, 0);
}
void FrameBuffer::Attach(RenderBuffer rb, GLenum type){
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, type, GL_RENDERBUFFER, rb.id);
}
void FrameBuffer::Bind(){
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}
void FrameBuffer::Unbind(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void FrameBuffer::Delete(){
	glDeleteFramebuffers(1, &id);
}

CubeMap::CubeMap()
	: id(0){
}
void CubeMap::New(GLenum internalformat, GLenum format, GLsizei width, GLsizei height, GLenum type, const void *data){

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalformat, width, height, 0, format, type, data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalformat, width, height, 0, format, type, data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalformat, width, height, 0, format, type, data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalformat, width, height, 0, format, type, data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalformat, width, height, 0, format, type, data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalformat, width, height, 0, format, type, data);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubeMap::Bind(int unit){

	glActiveTexture(GL_TEXTURE0+unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);	
}

void CubeMap::Delete(){
	glDeleteTextures(1, &id);
}