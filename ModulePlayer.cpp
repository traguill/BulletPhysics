#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	ball.sphere.radius = 2;
	ball.sphere.color = White;
	ball.body = App->physics->AddBody(ball.sphere, 0.1f);
	ball.body->SetPos(0, 2, 0);

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(2, 0.5, 4);
	car.chassis_offset.Set(0, 1.5, 0);
	car.mass = 350.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;


	vehicle = App->physics->AddVehicle(car);
	vehicle->color = Red;
	vehicle->SetPos(0, 3, -10);
	
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = brake = 0.0f;


	//PC MASTER RACE
	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		acceleration = MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if(turn < TURN_DEGREES)
			turn +=  TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if(turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		acceleration = -MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_REPEAT || App->input->GetJoystickButton(0, X) == KEY_REPEAT)
	{
		brake = BRAKE_POWER;
	}

	//CONSOLE FOR THE WIN
	if (App->input->GetJoystickAxis(0, LEFT_STICK_X) != 0)
	{
		turn = -TURN_DEGREES* App->input->GetJoystickAxis(0, LEFT_STICK_X);
	}

	if (App->input->GetJoystickAxis(0, RIGHT_TRIGGER) > 0)
	{
		acceleration = MAX_ACCELERATION * App->input->GetJoystickAxis(0, RIGHT_TRIGGER);
	}

	if (App->input->GetJoystickAxis(0, LEFT_TRIGGER) > 0)
	{
		acceleration = -MAX_ACCELERATION * App->input->GetJoystickAxis(0, LEFT_TRIGGER);
	}

	//MIX

	if (App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_DOWN || App->input->GetJoystickButton(0, A) == KEY_DOWN)
	{
		//Turbo
		btVector3 relativeForce = btVector3(0, 0, 1000 * vehicle->info.mass);
		btTransform boxTrans;
		vehicle->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);
		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		vehicle->vehicle->getRigidBody()->applyCentralForce(correctedForce);
	}

	

	if (App->input->GetKey(SDL_SCANCODE_KP_3) == KEY_DOWN || App->input->GetJoystickButton(0, B) == KEY_DOWN)
	{
		btVector3 relativeForce = btVector3(0, 1000 * vehicle->info.mass, 0);
		btTransform boxTrans;
		vehicle->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);
		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		vehicle->vehicle->getRigidBody()->applyCentralForce(correctedForce);
	}


	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	vehicle->Render();

	ball.body->GetTransform(ball.sphere.transform.M);
	ball.sphere.Render();

	char title[80];
	sprintf_s(title, "Rocket League Chinese version");
	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}



