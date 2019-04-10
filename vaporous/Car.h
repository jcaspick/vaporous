#pragma once
#include "Context.h"
#include "Mesh.h"
#include "Transformable.h"

class Car : public Transformable {
public:
	Car(Context* context);
	void init();
	void draw();

private:
	Context* _context;
	Mesh _mesh;
};