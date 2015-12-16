#ifndef __ParticleSystem_H__
#define __ParticleSystem_H__

#include "PhysBody3D.h"
#include "glmath.h"
#include "Color.h"
#include "Primitive.h"
#include "p2List.h"	
#include "Timer.h"

#define SPAWN_TIME 0.05f
#define PARTICLE_LIFE 0.5f

struct Particle
{
	float life;
	Cube shape;

	void CreateParticle(vec3 pos);
};

enum TYPE
{
	Fire
	//Maybe we will have different types?
};

struct ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	ParticleSystem* CreateParticleSystem(PhysBody3D* ref, vec3 offset, TYPE type, bool active = true);

	void Update(float dt);

public:

	bool on;

private:
	PhysBody3D* reference;
	vec3		offset;
	TYPE		type;
	p2List<Particle*> particles;
	Timer		timer;
};

#endif 