#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "p2List.h"
#include "p2Point.h"

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum Direction
{
	GO_RIGHT,
	GO_LEFT,
	GO_UP,
	GO_DOWN
};

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	void Move(Direction d, float speed);
	float* GetViewMatrix();

	void Rotate(float x, float y);

	void FollowMultipleTargets(const p2List<p2Point<int>>* targets);

	//Transform a 3D point to a point of the screen
	void From3Dto2D(vec3 point, int& x, int& y); 

private:

	void CalculateViewMatrix();

	//Math utilities--------------------------------------------------------------------------
	int GetMaxX(const p2List<p2Point<int>>* list)const;
	int GetMaxY(const p2List<p2Point<int>>* list)const;
	int GetMinX(const p2List<p2Point<int>>* list)const;
	int GetMinY(const p2List<p2Point<int>>* list)const;

public:
	
	vec3 X, Y, Z, Position, Reference;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};