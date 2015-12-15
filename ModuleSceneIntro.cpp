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

	//Create floor
	floor = new Cube(318, 1, 204);
	floor->color = Green;
	App->physics->AddBody(*floor, 0);

	//Create roof
	Cube roof(318, 1, 204);
	roof.SetPos(0, 60, 0);
	App->physics->AddBody(roof, 0);

	//Create walls
	
	wall_front.size.x = 318;
	wall_front.size.y = 60;
	wall_front.size.z = 1;
	wall_front.SetPos(0, 30, -102);
	App->physics->AddBody(wall_front, 0);

	wall_back.size.x = 318;
	wall_back.size.y = 60;
	wall_back.size.z = 1;
	wall_back.SetPos(0, 30, 102);
	App->physics->AddBody(wall_back, 0);

	wall_right.size.x = 1;
	wall_right.size.y = 60;
	wall_right.size.z = 204;
	wall_right.SetPos(169, 30, 0);
	App->physics->AddBody(wall_right, 0);

	Cube wr1(10, 12, 84);
	wr1.SetPos(164, 6, 60.0f);
	App->physics->AddBody(wr1, 0);
	wr1.SetPos(164, 6, -60.0f);
	App->physics->AddBody(wr1, 0);
	Cube wr2(10, 48, 204);
	wr2.SetPos(164, 36, 0);
	App->physics->AddBody(wr2, 0);

	wall_left.size.x = 1;
	wall_left.size.y = 60;
	wall_left.size.z = 204;
	wall_left.SetPos(-169, 30, 0);
	App->physics->AddBody(wall_left, 0);

	Cube wl1(10, 12, 84);
	wl1.SetPos(-164, 6, 60.0f);
	App->physics->AddBody(wl1, 0);
	wl1.SetPos(-164, 6, -60.0f);
	App->physics->AddBody(wl1, 0);
	Cube wl2(10, 48, 204);
	wl2.SetPos(-164, 36, 0);
	App->physics->AddBody(wl2, 0);

	//Create Goals
	goal_right.post_r.Scale(1, 12, 1);
	goal_right.post_l.Scale(1, 12, 1);
	goal_right.post_u.Scale(1, 1, 37);

	goal_right.post_r.SetPos(159, 6, 17.5f);
	goal_right.post_l.SetPos(159, 6, -17.5f);
	goal_right.post_u.SetPos(159, 12, 0);

	goal_right.post_l.color = goal_right.post_r.color = goal_right.post_u.color = Red;

	App->physics->AddBody(goal_right.post_r, 0);
	App->physics->AddBody(goal_right.post_l, 0);
	App->physics->AddBody(goal_right.post_u, 0);

	goal_left.post_r.Scale(1, 12, 1);
	goal_left.post_l.Scale(1, 12, 1);
	goal_left.post_u.Scale(1, 1, 37);

	goal_left.post_r.SetPos(-159, 6, 17.5f);
	goal_left.post_l.SetPos(-159, 6, -17.5f);
	goal_left.post_u.SetPos(-159, 12, 0);

	goal_left.post_l.color = goal_left.post_r.color = goal_left.post_u.color = Blue;

	App->physics->AddBody(goal_left.post_r, 0);
	App->physics->AddBody(goal_left.post_l, 0);
	App->physics->AddBody(goal_left.post_u, 0);

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

	goal_left.post_l.Render();
	goal_left.post_r.Render();
	goal_left.post_u.Render();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

