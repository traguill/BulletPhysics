#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;
struct PhysBody3D;

#define MAX_ACCELERATION 3000.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 1000.0f

struct BALL
{
	Sphere sphere;
	PhysBody3D* body;
};

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

private:

	void InputPlayer1();
	void InputPlayer2();

	void Respawn();

public:

	//Players
	PhysVehicle3D* vehicle_red = NULL;
	PhysVehicle3D* vehicle_blue = NULL;

	float turn;
	float acceleration;
	float brake;

	//Ball
	BALL ball;
	
	//GoalS
	PhysBody3D*	goal_red = NULL;
	PhysBody3D*	goal_blue = NULL;

	//Game-Logic
	int score_red;
	int score_blue;

private:
	int joysticks_connected = 0;
};