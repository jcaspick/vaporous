#pragma once

#include <string>

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

class Shader {

public:
	Shader() {}

	Shader& use();
	bool loadFromFile(const std::string& vertFile, const std::string& fragFile,
		const std::string& geoFile = "");
	void compile(const GLchar* vertSource, const GLchar* fragSource, 
		const GLchar* geoSource = nullptr);

	void setFloat(const GLchar* name, GLfloat value, GLboolean useShader = false);
	void setInt(const GLchar* name, GLint value, GLboolean useShader = false);
	void setVec2(const GLchar* name, GLfloat x, GLfloat y, GLboolean useShader = false);
	void setVec2(const GLchar* name, const glm::vec2& value, GLboolean useShader = false);
	void setVec3(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
	void setVec3(const GLchar* name, const glm::vec3& value, GLboolean useShader = false);
	void setVec4(const GLchar* name, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLboolean useShader = false);
	void setVec4(const GLchar* name, const glm::vec4& value, GLboolean useShader = false);
	void setMat4(const GLchar* name, const glm::mat4& value, GLboolean useShader = false);

	GLuint id;

private:
	void checkCompileErrors(GLuint object, std::string type);
};