#include "GL.h";

GL::GL() :
	_shader(-1),
	_texture0(-1),
	_texture1(-1),
	_vao(-1),
	_fbo(0),
	_isLineMode(false)
{}

void GL::useShader(GLuint id) {
	if (_shader == id) return;

	glUseProgram(id);
	_shader = id;
}

void GL::bindTexture0(GLuint id) {
	if (_texture0 == id) return;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);
	_texture0 = id;
}

void GL::bindTexture1(GLuint id) {
	if (_texture1 == id) return;

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, id);
	_texture1 = id;
}

void GL::bindTexture2(GLuint id) {
	if (_texture2 == id) return;

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, id);
	_texture2 = id;
}

void GL::bindTexture3(GLuint id) {
	if (_texture3 == id) return;

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, id);
	_texture3 = id;
}

void GL::bindCubemap4(GLuint id) {
	if (_texture4 == id) return;

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	_texture4 = id;
}

void GL::bindVAO(GLuint id) {
	if (_vao == id) return;

	glBindVertexArray(id);
	_vao = id;
}

void GL::bindFBO(GLuint id) {
	if (_fbo == id) return;

	glBindFramebuffer(GL_FRAMEBUFFER, id);
	_fbo = id;
}

void GL::setLineMode(bool lineMode) {
	if (_isLineMode == lineMode) return;

	glPolygonMode(GL_FRONT_AND_BACK,
		lineMode ? GL_LINE : GL_FILL);
	_isLineMode = lineMode;
}

void GL::setViewportSize(GLsizei width, GLsizei height) {
	if (width == _viewportWidth && height == _viewportHeight) return;

	glViewport(0, 0, width, height);
	_viewportWidth = width;
	_viewportHeight = height;
}

void GL::setNumBuffers(GLuint n) {
	if (_numBuffers == n) return;

	if (n == 1) {
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
	}
	else {
		GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, 
			GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(n, attachments);
	}

	_numBuffers = n;
}