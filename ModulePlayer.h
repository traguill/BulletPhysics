#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;
struct PhysBody3D;

#define MAX_ACCELERATION 3000.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 1000.0f

#define CAM_SPEED 5	

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
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

	p2List<p2Point<int>*>* InsideRect(p2List<p2Point<int>*> list, SDL_Rect rect);

private:

	void InputPlayer1();
	void InputPlayer2();

	void Respawn();

	//Moves the cam to always look at the two cars and the ball
	void CameraFollow(float dt)const;

	//Utilities---------------------------------------------------------------------------------------------
	bool PointInRect(const int x,const int y,const SDL_Rect rect)const;

	//From a list of points fill another with those that are inside the rect and returns the number
	

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

	//Cam utilities
	SDL_Rect center_rec;
	SDL_Rect center_right;
	SDL_Rect center_left;

private:
	int joysticks_connected = 0;
};