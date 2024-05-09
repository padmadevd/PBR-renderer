#ifndef R3DGL_H
#define R3DGL_H

#include <_gl.hpp>
#include <unordered_map>
#include <string>

class VBO{

public:

	unsigned int id;
	
	inline VBO()
		: id(0){
	}
	inline void New(){
		glGenBuffers(1, &id);
	}
	inline void Bind(){
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}
	inline void Unbind(){
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	inline void Data(GLsizeiptr size, const void *data, GLenum usage){
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	}
	inline void SubData(GLintptr offset, GLsizeiptr size, const void *data){
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	}
	inline void Delete(){
		glDeleteBuffers(1, &id);
	}
};

class EBO{

public:

	unsigned int id;
	
	inline EBO()
		: id(0){
	}
	inline void New(){
		glGenBuffers(1, &id);
	}
	inline void Bind(){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}
	inline void Unbind(){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	inline void Data(GLsizeiptr size, const void *data, GLenum usage){
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
	}
	inline void SubData(GLintptr offset, GLsizeiptr size, const void *data){
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
	}
	inline void Delete(){
		glDeleteBuffers(1, &id);
	}
};

class VAO{

public:

	unsigned int id;

	inline VAO()
		: id(0){	
	}
	inline void New(){
		glGenVertexArrays(1, &id);
	}
	inline void Bind(){
		glBindVertexArray(id);
	}
	inline void Unbind(){
		glBindVertexArray(0);
	}
	inline void Delete(){
		glDeleteVertexArrays(1, &id);
	}
	inline void AttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *offset){
		glVertexAttribPointer(index, size, type, normalized, stride, offset);
	}
	inline void EnableAttrib(GLuint index){
		glEnableVertexAttribArray(index);
	}
	inline void DisableAttrib(GLuint index){
		glDisableVertexAttribArray(index);
	}
};


class Shader{

public:

	unsigned int id;

	Shader();
	Shader(const char *vShaderPath, const char *fShaderPath);
	void Load(const char *vShaderPath, const char *fShaderPath);

	inline void Use(){
		glUseProgram(id);
	}
	inline void Seti(const char *name, int i){
		glUniform1i(glGetUniformLocation(id, name), i);
	}
	inline void Setf(const char *name, float f){
		glUniform1f(glGetUniformLocation(id, name), f);
	}
	inline void Set3f(const char *name, float x, float y, float z){
		glUniform3f(glGetUniformLocation(id, name), x, y, z);
	}
	inline void SetMatrix4fv(const char *name, const float *mat){
		glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, mat);
	}
	inline void Delete(){
		glDeleteShader(id);
	}
};

class Texture2d{

public:

	unsigned int id;

	Texture2d()
		: id(0){
	}

	void New(GLenum internalformat, GLenum format, GLsizei width, GLsizei height, GLenum type = GL_UNSIGNED_BYTE, const void *data = nullptr);
	void Bind(int slot);
	void Load(const char *filepath);
	void Delete();

	static std::unordered_map<std::string, Texture2d> loaded;
};

class CubeMap{

public:

	unsigned int id;

	CubeMap();
	void New(GLenum internalformat, GLenum format, GLsizei width, GLsizei height, GLenum type = GL_UNSIGNED_BYTE, const void *data = 0);
	void Bind(int unit);
	void Delete();
};

class RenderBuffer{

public:

	unsigned int id;

	RenderBuffer();
	void New(GLenum internalformat, GLsizei width, GLsizei height);
	void Bind();
	void Delete();
};

class FrameBuffer{

public:

	unsigned int id;

	FrameBuffer();
	void New();
	void Attach(Texture2d t, GLenum type);
	void Attach(RenderBuffer rb, GLenum type);
	void Bind();
	void Unbind();
	void Delete();
};

#endif