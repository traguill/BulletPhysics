#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;
struct PhysBody3D;
struct SDL_Rect;

#define MAX_ACCELERATION 3000.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 1000.0f


#define CAM_SPEED 3

#define POWER_SPEED 10000


struct BALL
{
	Sphere sphere;
	PhysBody3D* body;
};

struct POWERUP
{
	Cube cube;
	PhysBody3D* body;
};

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

private:

	void InputPlayer1();
	void InputPlayer2();

	void Respawn();
	
	void Turbo(PhysBody3D* body, bool brake = false)const;

	//Moves the cam to always look at the two cars and the ball
	void CameraFollow(float dt)const;
	void CameraFollow2(float dt)const;

	//Utilities---------------------------------------------------------------------------------------------
	bool PointInRect(const int x,const int y,const SDL_Rect rect)const;

	//From a list of points fill another with those that are inside the rect and returns the number
	int InsideRect(p2List<p2Point<int>>* list, p2List<p2Point<int>>* result, SDL_Rect rect, bool inside = true)const;

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

	//PowerUps
	p2List<POWERUP> power_ups; //List of power ups just for render

	POWERUP speed_A;
	POWERUP speed_B;
	POWERUP brake_A;
	POWERUP brake_B;

	//Game-Logic
	int score_red;
	int score_blue;

	//Cam utilities
	SDL_Rect center_focus;
	SDL_Rect center_rec;
	SDL_Rect center_right;
	SDL_Rect center_left;

private:
	int joysticks_connected = 0;

	//Utilities
	p2Point<int> center_screen;

	int area_focus_screen;
};