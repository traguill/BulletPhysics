#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

struct Camera
{
public:
	vec3 X, Y, Z, Position, Reference;
private:
	mat4x4 ViewMatrix, ViewMatrixInverse;

public:

	Camera();
	~Camera();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();

private:

	void CalculateViewMatrix();

};

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	Camera* GetCam(int index);

private:
	Camera* cam1;
	Camera* cam2;
};