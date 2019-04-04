#include "Shader.h"

#include <iostream>
#include <sstream>
#include <fstream>

Shader& Shader::use() {
	glUseProgram(id);
	return *this;
}

bool Shader::loadFromFile(const std::string& vertFile, 
	const std::string& fragFile, const std::string& geoFile) 
{
	std::string vertCode;
	std::string fragCode;
	std::string geoCode;
	try {
		std::ifstream vertFStream(vertFile);
		std::ifstream fragFStream(fragFile);

		if (!vertFStream.is_open()) {
			std::cout << "Error: failed to load vertex shader at path " 
				<< vertFile << std::endl;
		}
		if (!fragFStream.is_open()) {
			std::cout << "Error: failed to load fragment shader at path "
				<< vertFile << std::endl;
		}

		std::stringstream vertSStream, fragSStream;

		vertSStream << vertFStream.rdbuf();
		fragSStream << fragFStream.rdbuf();

		vertFStream.close();
		fragFStream.close();

		vertCode = vertSStream.str();
		fragCode = fragSStream.str();

		if (geoFile != "") {
			std::ifstream geoFStream(geoFile);

			if (!geoFStream.is_open()) {
				std::cout << "Error: failed to load geometry shader at path "
					<< vertFile << std::endl;
			}

			std::stringstream geoSStream;
			geoSStream << geoFStream.rdbuf();
			geoFStream.close();
			geoCode = geoSStream.str();
		}
	}
	catch (std::exception e) {
		std::cout << "Error: failed to load shader" << std::endl;
		return false;
	}

	compile(vertCode.c_str(), fragCode.c_str(), 
		geoFile != "" ? geoCode.c_str() : nullptr);
	return true;
}

void Shader::compile(const GLchar* vertSource, const GLchar* fragSource,
	const GLchar* geoSource)
{
	GLuint sVert, sFrag, sGeo;

	// create vertex shader
	sVert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVert, 1, &vertSource, NULL);
	glCompileShader(sVert);
	checkCompileErrors(sVert, "VERTEX");

	// create fragment shader
	sFrag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFrag, 1, &fragSource, NULL);
	glCompileShader(sFrag);
	checkCompileErrors(sFrag, "FRAGMENT");

	// create geometry shader
	if (geoSource != nullptr) {
		sGeo = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(sGeo, 1, &geoSource, NULL);
		glCompileShader(sGeo);
		checkCompileErrors(sGeo, "GEOMETRY");
	}

	// link shader program
	id = glCreateProgram();
	glAttachShader(id, sVert);
	glAttachShader(id, sFrag);
	if (geoSource != nullptr) glAttachShader(id, sGeo);
	glLinkProgram(id);
	checkCompileErrors(id, "PROGRAM");

	// discard shader objects after linking
	glDeleteShader(sVert);
	glDeleteShader(sFrag);
	if (geoSource != nullptr) glDeleteShader(sGeo);
}

void Shader::setFloat(const GLchar* name, GLfloat value, GLboolean useShader) {
	if (useShader) this->use();
	glUniform1f(glGetUniformLocation(id, name), value);
}

void Shader::setInt(const GLchar* name, GLint value, GLboolean useShader) {
	if (useShader) this->use();
	glUniform1i(glGetUniformLocation(id, name), value);
}

void Shader::setVec2(const GLchar* name, GLfloat x, GLfloat y, GLboolean useShader) {
	if (useShader) this->use();
	glUniform2f(glGetUniformLocation(id, name), x, y);
}

void Shader::setVec2(const GLchar* name, const glm::vec2& value, GLboolean useShader) {
	if (useShader) this->use();
	glUniform2f(glGetUniformLocation(id, name), value.x, value.y);
}

void Shader::setVec3(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader) {
	if (useShader) this->use();
	glUniform3f(glGetUniformLocation(id, name), x, y, z);
}

void Shader::setVec3(const GLchar* name, const glm::vec3& value, GLboolean useShader) {
	if (useShader) this->use();
	glUniform3f(glGetUniformLocation(id, name), value.x, value.y, value.z);
}

void Shader::setVec4(const GLchar* name, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLboolean useShader) {
	if (useShader) this->use();
	glUniform4f(glGetUniformLocation(id, name), r, g, b, a);
}

void Shader::setVec4(const GLchar* name, const glm::vec4& value, GLboolean useShader) {
	if (useShader) this->use();
	glUniform4f(glGetUniformLocation(id, name), value.r, value.g, value.b, value.a);
}

void Shader::setMat4(const GLchar* name, const glm::mat4& value, GLboolean useShader) {
	if (useShader) this->use();
	glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::checkCompileErrors(GLuint object, std::string type) {
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "Error: failed to compile shader " << type << "\n" << infoLog << std::endl;
		}
	}
	else {
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout << "Error: failed to link shader program\n" << infoLog << std::endl;
		}
	}
}