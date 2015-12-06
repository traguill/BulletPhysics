#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

struct PhysBody3D;
struct Cube;

struct GOAL
{
	Cube post_r;
	Cube post_l;
	Cube post_u;
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:
	Cube* floor = NULL;

	Cube wall_front;
	Cube wall_right;
	Cube wall_left;
	Cube wall_back;

	GOAL goal_right;

};
