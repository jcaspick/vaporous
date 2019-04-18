#include "Car.h"
#include "MeshUtilities.h"

Car::Car(Context* context) :
	_context(context)
{}

void Car::init() {
	_mesh = MeshUtil::loadFromObj("resources/3d/car.obj", 0.05f);
	_mesh.bind();
}

void Car::draw() {
	_context->gl->setNumBuffers(2);
	_context->renderer->drawMesh(_mesh, getModelMatrix(),
		&_context->resourceMgr->getShader(Shaders::Reflective));
	_context->gl->setNumBuffers(1);
}