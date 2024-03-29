//
// sueda - geometry edits Z. Wood
// 3/16
//

#include <iostream>
#include "Particle.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"


float randFloat(float l, float h)
{
	float r = rand() / (float) RAND_MAX;
	return (1.0f - r) * l + r * h;
}

Particle::Particle(vec3 start) :
	charge(1.0f),
	m(1.0f),
	d(0.0f),
	x(start),
	v(0.0f, 0.0f, 0.0f),
	lifespan(5.0f),
	tEnd(0.0f),
	scale(10.0f),
	color(0.2f, 0.2f, 1.0f, 5.0f)
{
}

Particle::~Particle()
{
}

void Particle::load(vec3 start)
{
	// Random initialization
	rebirth(0.0f, start);
}

/* all particles born at the origin */
void Particle::rebirth(float t, vec3 start) {
    charge = randFloat(0.0f, 1.0f) < 0.5 ? -1.0f : 1.0f;
    m = 1.0f;
    d = randFloat(0.0f, 0.02f);
    x = start;
    v.x = randFloat(-2.5f, 2.35);
    v.y = randFloat(-4.0f, -2.0f);
    v.z = randFloat(-2.3f, 2.27f);
    lifespan = randFloat(100.0f, 200.0f);
    tEnd = t + lifespan;
    scale = randFloat(10.0f, 15.0f);
    color.r = randFloat(1.0f, 1.5f);
    color.g = randFloat(0.1f, 0.2f);
    color.b = randFloat(0.5f, 0.7f);
    color.a = 0.1f;
}


void Particle::update(float t, float h, const vec3 &g, const vec3 start)
{
	if(t > (tEnd/2)) {
		;
	}
	if(t > tEnd) {
		rebirth(t, start);
	}

	//very simple update
	x += 0.6f*h*v;
	//To do - how do you want to update the forces?
	//ACCELERATION
	color.a = 1.5f*(tEnd-t)/lifespan;// * 5;
	//color.a = color.a;
}
