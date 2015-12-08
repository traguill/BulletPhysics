#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	score_blue = score_red = 0;

	//Ball
	ball.sphere.radius = 4;
	ball.sphere.color = White;
	ball.body = App->physics->AddBody(ball.sphere, 0.1f);
	ball.body->SetPos(0, 2, 0);
	ball.body->collision_listeners.add(this);

	//Goals
	Cube g_red(10, 11, 35);
	g_red.SetPos(169, 5.5f, 0);
	goal_red = App->physics->AddBody(g_red, 0, true);
	goal_red->collision_listeners.add(this);

	Cube b_red(10, 11, 35);
	b_red.SetPos(-169, 5.5f, 0);
	goal_blue = App->physics->AddBody(b_red, 0, true);
	goal_blue->collision_listeners.add(this);

	//Cars
	
	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(3, 2, 4);
	car.chassis_offset.Set(0, 1.5, 0);
	car.nose_size.Set(3, 1, 1.5);
	car.nose_offset.Set(0, 1, 2.75);
	car.mass = 500.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 500.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 1.0f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;

	vec3 direction(0, -1, 0);
	vec3 axis(-1, 0, 0);

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
	vehicle->SetPos(10, 3, 0);


	joysticks_connected = App->input->GetNumberJoysticks();
	
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

	//Get input
	if (joysticks_connected > 0)
		InputPlayer1();

	//Render
	vehicle->Render();

	ball.body->GetTransform(ball.sphere.transform.M);
	ball.sphere.Render();

	char title[80];
	sprintf_s(title, "Rocket League Chinese version:   Blue %d - %d Red", score_blue, score_red);
	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}

update_status ModulePlayer::PostUpdate(float dt)
{
	vec3 pos;
	pos.x = vehicle->vehicle->getChassisWorldTransform().getOrigin().getX();
	pos.y = vehicle->vehicle->getChassisWorldTransform().getOrigin().getY();
	pos.z = vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ();
	App->camera->LookAt(pos);
	pos.y += 10;
	pos.z -= 38;
	App->camera->Position = pos;

	return UPDATE_CONTINUE;
}

void ModulePlayer::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body1 == ball.body && body2 == goal_red)
	{
		vehicle->SetPos(0, 3, -10);
		vehicle->Stop();
		ball.body->SetPos(0, 2, 0);
		ball.body->Stop();

		score_blue += 1;
	}

	if (body1 == ball.body && body2 == goal_blue)
	{
		vehicle->SetPos(0, 3, -10);
		vehicle->Stop();
		ball.body->SetPos(0, 2, 0);
		ball.body->Stop();

		score_red += 1;
	}
}


void ModulePlayer::InputPlayer1()
{

	turn = acceleration = brake = 0.0f;

	//Break
	if (App->input->GetJoystickButton(0, X) == KEY_REPEAT)
	{
		brake = BRAKE_POWER;
	}

	//Direction
	if (App->input->GetJoystickAxis(0, LEFT_STICK_X) != 0)
	{
		turn = -TURN_DEGREES* App->input->GetJoystickAxis(0, LEFT_STICK_X);
	}

	if (App->input->GetJoystickAxis(0, RIGHT_STICK_X) != 0 || App->input->GetJoystickAxis(0, RIGHT_STICK_Y) != 0)
	{
		float dx = App->input->GetJoystickAxis(0, RIGHT_STICK_X);
		float dy = App->input->GetJoystickAxis(0, RIGHT_STICK_Y);
		App->camera->Rotate(dx*10, dy*10);
	}

	//Go forward
	if (App->input->GetJoystickAxis(0, RIGHT_TRIGGER) > 0)
	{
		acceleration = MAX_ACCELERATION * App->input->GetJoystickAxis(0, RIGHT_TRIGGER);
	}

	//Go backward
	if (App->input->GetJoystickAxis(0, LEFT_TRIGGER) > 0)
	{
		acceleration = -MAX_ACCELERATION * App->input->GetJoystickAxis(0, LEFT_TRIGGER);
	}


	//Turbo
	if (App->input->GetJoystickButton(0, A) == KEY_DOWN)
	{
		btVector3 relativeForce = btVector3(0, 0, 1000 * vehicle->info.mass);
		btTransform boxTrans;
		vehicle->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);
		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		vehicle->vehicle->getRigidBody()->applyCentralForce(correctedForce);
	}


	//FrontFlip
	if (App->input->GetJoystickButton(0, B) == KEY_DOWN)
	{
		btVector3 relativeForce = btVector3(0, 485 * vehicle->info.mass,0);
		btVector3 relativePosition = btVector3(0, 0, -vehicle->info.chassis_size.z / 2);

		btTransform boxTrans;
		vehicle->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);
		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

		vehicle->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}

	//Roll (right)
	if (App->input->GetJoystickButton(0, RB) == KEY_DOWN)
	{
		btTransform boxTrans;
		vehicle->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);

		btVector3 relativeForce = btVector3(0, 400 * vehicle->info.mass, 0);
		btVector3 relativePosition = btVector3(vehicle->info.chassis_size.x / 2, 0, 0);
	
		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

		vehicle->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}

	//Roll (left)
	if (App->input->GetJoystickButton(0, LB) == KEY_DOWN)
	{
		btTransform boxTrans;
		vehicle->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);

		btVector3 relativeForce = btVector3(0, 400 * vehicle->info.mass, 0);
		btVector3 relativePosition = btVector3(vehicle->info.chassis_size.x / -2, 0, 0);

		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

		vehicle->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}


	if (App->input->GetJoystickButton(0, DPAD_RIGHT) == KEY_DOWN)
	{
		btVector3 gravity(10.0f, 0, 0);
		vehicle->vehicle->getRigidBody()->setGravity(gravity);
	}

	if (App->input->GetJoystickButton(0, DPAD_LEFT) == KEY_DOWN)
	{
		btVector3 gravity(-10.0f, 0, 0);
		vehicle->vehicle->getRigidBody()->setGravity(gravity);
	}

	if (App->input->GetJoystickButton(0, DPAD_UP) == KEY_DOWN)
	{
		btVector3 gravity(0, 10.0f, 0);
		vehicle->vehicle->getRigidBody()->setGravity(gravity);
	}

	if (App->input->GetJoystickButton(0, DPAD_DOWN) == KEY_DOWN)
	{
		btVector3 gravity(0, -10.0f, 0);
		vehicle->vehicle->getRigidBody()->setGravity(gravity);
	}


	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);
}

