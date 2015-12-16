#include "Globals.h"
#include "Application.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"

#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Debug camera mode: Disabled for the final game (but better keep the code)

	/*vec3 newPos(0,0,0);
	float speed = 3.0f * dt;
	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 8.0f * dt;
	
	if(App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += speed;
	if(App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.y -= speed;

	if(App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


	if(App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
	if(App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;
	
	Position += newPos;
	Reference += newPos;

	// Mouse motion ----------------

	if(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		Position -= Reference;

		if(dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if(dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);

			if(Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}
		}

		Position = Reference + Z * length(Position);
	}

	// Recalculate matrix -------------
	CalculateViewMatrix();*/

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}
// ------------------------------------------------------------------
void ModuleCamera3D::Move(Direction d, float speed)
{
	vec3 newPos(0, 0, 0);
	switch (d)
	{
	case GO_RIGHT:
		newPos += X * speed;
		break;
	case GO_LEFT:
		newPos -= X * speed;
		break;
	case GO_UP:
		newPos.y += speed;
		break;
	case GO_DOWN:
		newPos.y -= speed;
		break;
	default:
		break;
	}

	Position += newPos;
	Reference += newPos;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

void ModuleCamera3D::Rotate(float x, float y)
{
	int dx = -x;
	int dy = -y;

	float Sensitivity = 0.25f;

	Position -= Reference;

	if (dx != 0)
	{
		float DeltaX = (float)dx * Sensitivity;

		X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * Sensitivity;

		Y = rotate(Y, DeltaY, X);
		Z = rotate(Z, DeltaY, X);

		if (Y.y < 0.0f)
		{
			Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = cross(Z, X);
		}
	}

	Position = Reference + Z * length(Position);

	CalculateViewMatrix();
}
// -----------------------------------------------------------------

void ModuleCamera3D::From3Dto2D(vec3 point, int& x, int& y)
{
	mat4x4 projection;
	projection = perspective(60.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.125f, 512.0f);

	vec3 screen = multiply(point, ViewMatrix);
	screen = multiply(screen, projection);

	screen.x /= screen.z;
	screen.y /= screen.z;

	x = (screen.x +1) * (SCREEN_WIDTH /2);
	y = (screen.y + 1) * (SCREEN_HEIGHT /2);
}

// -----------------------------------------------------------------

void ModuleCamera3D::FollowMultipleTargets(const p2List<p2Point<int>>* targets)
{
	//If there aren't any targets do nothing
	if (targets->getFirst() != NULL)
	{
		//Get the max and min X and Y
		int maxX = GetMaxX(targets);
		int maxY = GetMaxY(targets);
		int minX = GetMinX(targets);
		int minY = GetMinY(targets);

		//Calculate the central point of the rectangle created by all the targets
		int dstX = (maxX + minX)/2;
		int dstY = (maxY + minY)/2;

		//Calculate the amount of PIXELS the camera need to move to reach the center
		int moveX = dstX - CENTER_SCREEN_X;
		int moveY = dstY - CENTER_SCREEN_Y;

		//Now move the camera transforming PIXELS TO METERS
		Move(vec3(PIXEL_TO_METERS(-moveX), PIXEL_TO_METERS(moveY), 0));

		//Calculate the zoom in/out

		float Distance = (120.0f / SCREEN_WIDTH)*(maxX - minX);

		if (Distance > 120.0f)
			Distance = 120.0f;
		if (Distance < 20.0f)
			Distance = 20.0f;

		App->renderer3D->OnResize(SCREEN_WIDTH, SCREEN_HEIGHT, Distance);

	}
}

//Utilities for the previous function ----------------------------------------------------------------------------------------------------------------------

int ModuleCamera3D::GetMaxX(const p2List<p2Point<int>>* list)const
{
	int ret = -1;
	p2List_item<p2Point<int>>* item = list->getFirst();
	if (item != NULL)
		ret = item->data.x;
	while (item)
	{
		if (item->data.x > ret)
			ret = item->data.x;
		item = item->next;
	}

	return ret;
}

int ModuleCamera3D::GetMinX(const p2List<p2Point<int>>* list)const
{
	int ret = -1;
	p2List_item<p2Point<int>>* item = list->getFirst();
	if (item != NULL)
		ret = item->data.x;
	while (item)
	{
		if (item->data.x < ret)
			ret = item->data.x;
		item = item->next;
	}

	return ret;
}

int ModuleCamera3D::GetMaxY(const p2List<p2Point<int>>* list)const
{
	int ret = -1;
	p2List_item<p2Point<int>>* item = list->getFirst();
	if (item != NULL)
		ret = item->data.y;
	while (item)
	{
		if (item->data.y > ret)
			ret = item->data.y;
		item = item->next;
	}

	return ret;
}

int ModuleCamera3D::GetMinY(const p2List<p2Point<int>>* list)const
{
	int ret = -1;
	p2List_item<p2Point<int>>* item = list->getFirst();
	if (item != NULL)
		ret = item->data.y;
	while (item)
	{
		if (item->data.y < ret)
			ret = item->data.y;
		item = item->next;
	}

	return ret;
}