#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(20.0f, 50.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	//Create floor
	floor = new Cube(106, 1, 68);
	floor->color = Green;
	App->physics->AddBody(*floor, 0);

	//Create walls
	
	wall_front.size.x = 106;
	wall_front.size.y = 60;
	wall_front.size.z = 1;
	wall_front.SetPos(0, 30, -34);
	App->physics->AddBody(wall_front, 0);

	wall_back.size.x = 106;
	wall_back.size.y = 60;
	wall_back.size.z = 1;
	wall_back.SetPos(0, 30, 34);
	App->physics->AddBody(wall_back, 0);

	wall_right.size.x = 1;
	wall_right.size.y = 60;
	wall_right.size.z = 68;
	wall_right.SetPos(63, 30, 0);
	App->physics->AddBody(wall_right, 0);

	Cube wr1(10, 12, 16.5f);
	wr1.SetPos(58, 6, 25.75f);
	App->physics->AddBody(wr1, 0);
	wr1.SetPos(58, 6, -25.75f);
	App->physics->AddBody(wr1, 0);
	Cube wr2(10, 48, 68);
	wr2.SetPos(58, 36, 0);
	App->physics->AddBody(wr2, 0);

	wall_left.size.x = 1;
	wall_left.size.y = 60;
	wall_left.size.z = 68;
	wall_left.SetPos(-53, 30, 0);
	App->physics->AddBody(wall_left, 0);

	//Create Goals
	goal_right.post_r.Scale(1, 12, 1);
	goal_right.post_l.Scale(1, 12, 1);
	goal_right.post_u.Scale(1, 1, 37);

	goal_right.post_r.SetPos(53, 6, 17.5f);
	goal_right.post_l.SetPos(53, 6, -17.5f);
	goal_right.post_u.SetPos(53, 12, 0);

	goal_right.post_l.color = goal_right.post_r.color = goal_right.post_u.color = White;

	App->physics->AddBody(goal_right.post_r, 0);
	App->physics->AddBody(goal_right.post_l, 0);
	App->physics->AddBody(goal_right.post_u, 0);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;

	floor->Render();

	//Goals
	goal_right.post_l.Render();
	goal_right.post_r.Render();
	goal_right.post_u.Render();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

