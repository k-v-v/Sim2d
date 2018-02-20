//
// Created by konstantin on 21/06/17.
//
#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "ThirdParty/glad.h"

#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <iostream>
//UniformProxy: A quick and dirty hack to allow assigning to program uniforms like
//program["lightPos1"]=vec3(1.0f, 1.0f, 1.0f);

class UniformProxy {
private:
	GLuint programId;
	GLint uniformLocation;

public:
	UniformProxy(GLuint programId, GLint uniformLocation);
	template<class T>
	void operator=(const T & value);
};

class ShaderException: public std::runtime_error {
public:
	ShaderException(char const * const message) throw ();
};

/*
 * A generic shader class
 * It assumes all the shaders are in a subdirectory called 'shaders'
 * */
class Shader {
public:
	Shader(const char *vertexPath, const char *fragmentPath);
	~Shader();

	inline bool isCurrent() {
		return currentProgramID == programID;
	}

	GLuint getID();
	void use();
	GLuint getUnirformLocation(std::string name);
	UniformProxy operator[](std::string varName) {
		if (!isCurrent()) {
			throw ShaderException("Attempt to set uniform for non-active program");
		}
		std::unordered_map<std::string, GLuint>::iterator it = ActiveUniforms.find(varName);
		if (it != ActiveUniforms.end()) {
			return UniformProxy(programID, it->second);
		} else
			throw ShaderException( (std::string(varName + " is not an active uniform in shader.") ).c_str() );
	}
private:
	static GLuint currentProgramID;
	GLuint programID;

	//A map used to avoid fetching the uniform location every time
	//we assign a value to it
	std::unordered_map<std::string, GLuint> ActiveUniforms;
};

template<>
void UniformProxy::operator=(const float & value);
template<>
void UniformProxy::operator=(const int & value);
template<>
void UniformProxy::operator=(const bool & value);

