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
	_context->renderer->drawMesh(_mesh, getModelMatrix(),
		&_context->resourceMgr->getShader(Shaders::BasicTextured));
}