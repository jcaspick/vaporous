#pragma once
#include <glad\glad.h>

class GL {
public:
	GL();
	void useShader(GLuint id);
	void bindTexture0(GLuint id);
	void bindTexture1(GLuint id);
	void bindTexture2(GLuint id);
	void bindTexture3(GLuint id);
	void bindCubemap4(GLuint id);
	void bindVAO(GLuint id);
	void bindFBO(GLuint id);
	void setLineMode(bool lineMode);
	void setViewportSize(GLsizei width, GLsizei height);

	GLuint getShader();
	GLuint getTexture0();
	GLuint getTexture1();
	GLuint getVAO();

private:
	GLuint _shader;
	GLuint _texture0;
	GLuint _texture1;
	GLuint _texture2;
	GLuint _texture3;
	GLuint _texture4;
	GLuint _vao;
	GLuint _fbo;
	bool _isLineMode;
	GLsizei _viewportWidth;
	GLsizei _viewportHeight;
};