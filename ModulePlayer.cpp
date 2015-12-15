#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"


ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
	turn = acceleration = brake = 0.0f;
	min = 0;
}

ModulePlayer::~ModulePlayer()
{}

bool ModulePlayer::Start()
{
	LOG("Loading player");

	score_blue = score_red = 0;

	CreateObjects();

	Respawn();

	match_time.Start();

	joysticks_connected = App->input->GetNumberJoysticks();

	App->camera->Move(vec3(0, 50, -150));
	App->camera->LookAt(vec3(0, 0, 0));

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

update_status ModulePlayer::Update(float dt)
{	

	//Get input
	if (joysticks_connected > 0)
		InputPlayer1();
	else
		KeyInputPlayer1();
	if (joysticks_connected > 1)
		InputPlayer2();
	else
		KeyInputPlayer2();

	//Render
	vehicle_red->Render();
	vehicle_blue->Render();

	ball.body->GetTransform(ball.sphere.transform.M);
	ball.sphere.Render();
	ball.shadow.radius = (ball.sphere.GetPos().y > 5) ? (ball.sphere.GetPos().y > 14) ? 0.5f : 3/(ball.sphere.GetPos().y-4) : 3;
	ball.shadow.SetPos(ball.sphere.GetPos().x, 1, ball.sphere.GetPos().z);
	ball.shadow.Render();

	p2List_item<POWERUP>* item = power_ups.getFirst();
	while (item)
	{
		item->data.cube.Render();
		item = item->next;
	}

	if ((int)match_time.ReadSec() == 60)
	{
		min++;
		match_time.Start();
	}

	if (min == 3)
		Restart();

	UpdateCamera();

	char title[80];
	sprintf_s(title, "Rocket League Chinese version:   Blue %d - %d Red          Time: %i : %i", score_blue, score_red, min, (int)match_time.ReadSec());
	App->window->SetTitle(title);
	
	return UPDATE_CONTINUE;
}

void ModulePlayer::UpdateCamera()const
{
	//Get the position of each object on the screen
	p2Point<int> red;
	btVector3 bt_red = vehicle_red->vehicle->getChassisWorldTransform().getOrigin();
	vec3 red_3d(bt_red.getX(), bt_red.getY(), bt_red.getZ());
	App->camera->From3Dto2D(red_3d, red.x, red.y);

	p2Point<int> blue;
	btVector3 bt_blue = vehicle_blue->vehicle->getChassisWorldTransform().getOrigin();
	vec3 blue_3d(bt_blue.getX(), bt_blue.getY(), bt_blue.getZ());
	App->camera->From3Dto2D(blue_3d, blue.x, blue.y);

	p2Point<int> ball_p;
	float m_ball[16];
	ball.body->GetTransform(m_ball);
	vec3 ball_3d(m_ball[12], m_ball[13], m_ball[14]);
	App->camera->From3Dto2D(ball_3d, ball_p.x, ball_p.y);

	p2List<p2Point<int>> objects;
	objects.add(red);
	objects.add(blue);
	objects.add(ball_p);

	//Call the camera method to follow multiple objects
	App->camera->FollowMultipleTargets(&objects);
}

void ModulePlayer::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

	//Goals
	if (body1 == ball.body && body2 == goal_red)
	{	
		Respawn();
		score_blue += 1;
	}

	if (body1 == ball.body && body2 == goal_blue)
	{
		Respawn();
		score_red += 1;
	}
	
	//Power ups
	if (body1 == speed_A.body || body1 == speed_B.body)
	{
		if (body2 != ball.body)
			Turbo(body2);
	}

	if (body1 == brake_A.body || body1 == brake_B.body)
	{
		if (body2 != ball.body)
			Turbo(body2, true);
	}
}

void ModulePlayer::Respawn()
{
	vehicle_red->Stop();
	vehicle_red->SetPos(10, 3, 0);
	vehicle_red->SetRotation(0, -3.14f / 2, 0);
	

	vehicle_blue->Stop();
	vehicle_blue->SetPos(-10, 3, 0);
	vehicle_blue->SetRotation(0, 3.14f / 2, 0);
	

	ball.body->SetPos(0, 2, 0);
	ball.body->Stop();
}

void ModulePlayer::Restart()
{ 
	score_blue = score_red = min= 0;
	Respawn();
	match_time.Start();
}

void ModulePlayer::CreateObjects()
{
	//Ball
	ball.sphere.radius = 4;
	ball.sphere.color = White;
	ball.body = App->physics->AddBody(ball.sphere, 0.01f);
	ball.body->SetPos(0, 2, 0);
	ball.shadow.radius = 3;
	ball.shadow.height = 0.5f;
	ball.shadow.color = Black;
	ball.shadow.SetRotation(90, vec3(0, 0, 1));
	ball.shadow.SetPos(ball.sphere.GetPos().x, 1, ball.sphere.GetPos().z);
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

	//Power ups
	Cube speed_up_c(30, 1, 10);
	speed_up_c.color = Yellow;
	speed_up_c.SetPos(20, 1, 92);

	speed_A.cube = speed_up_c;
	speed_A.body = App->physics->AddBody(speed_up_c, 0, true);
	speed_A.body->collision_listeners.add(this);
	power_ups.add(speed_A);

	speed_up_c.SetPos(20, 1, -92);
	speed_B.cube = speed_up_c;
	speed_B.body = App->physics->AddBody(speed_up_c, 0, true);
	speed_B.body->collision_listeners.add(this);
	power_ups.add(speed_B);

	Cube brake_c(20, 1, 20);
	brake_c.color = Pink;
	brake_c.SetPos(82, 1, -51);

	brake_A.cube = brake_c;
	brake_A.body = App->physics->AddBody(brake_c, 0, true);
	brake_A.body->collision_listeners.add(this);
	power_ups.add(brake_A);

	brake_c.SetPos(-82, 1, 51);
	brake_B.cube = brake_c;
	brake_B.body = App->physics->AddBody(brake_c, 0, true);
	brake_B.body->collision_listeners.add(this);
	power_ups.add(brake_B);

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
	car.frictionSlip = 5.5;
	car.maxSuspensionForce = 10000.0f;

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


	vehicle_red = App->physics->AddVehicle(car);
	vehicle_red->color = Red;
	vehicle_red->collision_listeners.add(this);

	vehicle_blue = App->physics->AddVehicle(car);
	vehicle_blue->color = Blue;
	vehicle_blue->collision_listeners.add(this);
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
	if (App->input->GetJoystickButton(0, A) == KEY_REPEAT)
	{
		Turbo(vehicle_red);
	}

	//FrontFlip
	if (App->input->GetJoystickButton(0, B) == KEY_DOWN)
	{
		btVector3 relativeForce = btVector3(0, 485 * vehicle_red->info.mass,0);
		btVector3 relativePosition = btVector3(0, 0, -vehicle_red->info.chassis_size.z / 2);

		btTransform boxTrans;
		vehicle_red->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);
		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

		vehicle_red->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}

	//Roll (right)
	if (App->input->GetJoystickButton(0, RB) == KEY_DOWN)
	{
		btTransform boxTrans;
		vehicle_red->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);

		btVector3 relativeForce = btVector3(0, 400 * vehicle_red->info.mass, 0);
		btVector3 relativePosition = btVector3(vehicle_red->info.chassis_size.x / 2, 0, 0);
	
		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

		vehicle_red->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}

	//Roll (left)
	if (App->input->GetJoystickButton(0, LB) == KEY_DOWN)
	{
		btTransform boxTrans;
		vehicle_red->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);

		btVector3 relativeForce = btVector3(0, 400 * vehicle_red->info.mass, 0);
		btVector3 relativePosition = btVector3(vehicle_red->info.chassis_size.x / -2, 0, 0);

		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

		vehicle_red->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}

	vehicle_red->ApplyEngineForce(acceleration);
	vehicle_red->Turn(turn);
	vehicle_red->Brake(brake);
}

void ModulePlayer::InputPlayer2()
{

	turn = acceleration = brake = 0.0f;

	//Break
	if (App->input->GetJoystickButton(1, X) == KEY_REPEAT)
	{
		brake = BRAKE_POWER;
	}

	//Direction
	if (App->input->GetJoystickAxis(1, LEFT_STICK_X) != 0)
	{
		turn = -TURN_DEGREES* App->input->GetJoystickAxis(1, LEFT_STICK_X);
	}

	//Go forward
	if (App->input->GetJoystickAxis(1, RIGHT_TRIGGER) > 0)
	{
		acceleration = MAX_ACCELERATION * App->input->GetJoystickAxis(1, RIGHT_TRIGGER);
	}

	//Go backward
	if (App->input->GetJoystickAxis(1, LEFT_TRIGGER) > 0)
	{
		acceleration = -MAX_ACCELERATION * App->input->GetJoystickAxis(1, LEFT_TRIGGER);
	}


	//Turbo
	if (App->input->GetJoystickButton(1, A) == KEY_REPEAT)
	{
		Turbo(vehicle_blue);
	}

	//FrontFlip
	if (App->input->GetJoystickButton(1, B) == KEY_DOWN)
	{
		btVector3 relativeForce = btVector3(0, 485 * vehicle_blue->info.mass, 0);
		btVector3 relativePosition = btVector3(0, 0, -vehicle_blue->info.chassis_size.z / 2);

		btTransform boxTrans;
		vehicle_blue->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);
		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

		vehicle_blue->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}

	//Roll (right)
	if (App->input->GetJoystickButton(1, RB) == KEY_DOWN)
	{
		btTransform boxTrans;
		vehicle_blue->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);

		btVector3 relativeForce = btVector3(0, 400 * vehicle_blue->info.mass, 0);
		btVector3 relativePosition = btVector3(vehicle_blue->info.chassis_size.x / 2, 0, 0);

		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

		vehicle_blue->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}

	//Roll (left)
	if (App->input->GetJoystickButton(1, LB) == KEY_DOWN)
	{
		btTransform boxTrans;
		vehicle_blue->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);

		btVector3 relativeForce = btVector3(0, 400 * vehicle_blue->info.mass, 0);
		btVector3 relativePosition = btVector3(vehicle_blue->info.chassis_size.x / -2, 0, 0);

		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

		vehicle_blue->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}

	vehicle_blue->ApplyEngineForce(acceleration);
	vehicle_blue->Turn(turn);
	vehicle_blue->Brake(brake);
}

void ModulePlayer::KeyInputPlayer1()
{
	turn = acceleration = brake = 0.0f;

	//Break
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
	{
		brake = BRAKE_POWER;
	}

	//Direction
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		turn = +TURN_DEGREES;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		turn = -TURN_DEGREES;
	}

	//Go forward
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		acceleration = MAX_ACCELERATION;
	}

	//Go backward
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		acceleration = -MAX_ACCELERATION;
	}


	//Turbo
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
	{
		Turbo(vehicle_red);
	}
	
	//FrontFlip
	if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
	{
		btVector3 relativeForce = btVector3(0, 485 * vehicle_red->info.mass, 0);
		btVector3 relativePosition = btVector3(0, 0, -vehicle_red->info.chassis_size.z / 2);

		btTransform boxTrans;
		vehicle_red->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);
		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

		vehicle_red->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}

	//Roll (right)
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
	{
		btTransform boxTrans;
		vehicle_red->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);

		btVector3 relativeForce = btVector3(0, 400 * vehicle_red->info.mass, 0);
		btVector3 relativePosition = btVector3(vehicle_red->info.chassis_size.x / 2, 0, 0);

		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

		vehicle_red->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}

	//Roll (left)
	if (App->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
	{
		btTransform boxTrans;
		vehicle_red->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);

		btVector3 relativeForce = btVector3(0, 400 * vehicle_red->info.mass, 0);
		btVector3 relativePosition = btVector3(vehicle_red->info.chassis_size.x / -2, 0, 0);

		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

		vehicle_red->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}

	vehicle_red->ApplyEngineForce(acceleration);
	vehicle_red->Turn(turn);
	vehicle_red->Brake(brake);
}
void ModulePlayer::KeyInputPlayer2()
{
	turn = acceleration = brake = 0.0f;

	//Break
	if (App->input->GetKey(SDL_SCANCODE_KP_0) == KEY_REPEAT)
	{
		brake = BRAKE_POWER;
	}

	//Direction
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		turn = +TURN_DEGREES;
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		turn = -TURN_DEGREES;
	}


	//Go forward
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		acceleration = MAX_ACCELERATION;
	}

	//Go backward
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		acceleration = -MAX_ACCELERATION;
	}

	//Turbo
	if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_REPEAT)
	{
		Turbo(vehicle_blue);
	}
	
	//FrontFlip
	if (App->input->GetKey(SDL_SCANCODE_KP_5) == KEY_DOWN)
	{
		btVector3 relativeForce = btVector3(0, 485 * vehicle_blue->info.mass, 0);
		btVector3 relativePosition = btVector3(0, 0, -vehicle_blue->info.chassis_size.z / 2);

	btTransform boxTrans;
	vehicle_blue->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);
	btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
	btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

	vehicle_blue->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}

	//Roll (right)
	if (App->input->GetKey(SDL_SCANCODE_KP_6) == KEY_DOWN)
	{
	btTransform boxTrans;
	vehicle_blue->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);

	btVector3 relativeForce = btVector3(0, 400 * vehicle_blue->info.mass, 0);
	btVector3 relativePosition = btVector3(vehicle_blue->info.chassis_size.x / 2, 0, 0);

	btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
	btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

	vehicle_blue->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}

	//Roll (left)
	if (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_DOWN)
	{
	btTransform boxTrans;
	vehicle_blue->vehicle->getRigidBody()->getMotionState()->getWorldTransform(boxTrans);

	btVector3 relativeForce = btVector3(0, 400 * vehicle_blue->info.mass, 0);
	btVector3 relativePosition = btVector3(vehicle_blue->info.chassis_size.x / -2, 0, 0);

	btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
	btVector3 correctedPosition = (boxTrans * relativePosition) - boxTrans.getOrigin();

	vehicle_blue->vehicle->getRigidBody()->applyForce(correctedForce, correctedPosition);
	}

	vehicle_blue->ApplyEngineForce(acceleration);
	vehicle_blue->Turn(turn);
	vehicle_blue->Brake(brake);

}



//Utilities ---------------------------------------------------------------------------------------------------------------------------------------------------

bool ModulePlayer::PointInRect(const int x, const int y, const SDL_Rect rect)const
{
	bool ret = false;
	if (x > rect.x && y > rect.y && x < rect.x + rect.w &&  y < rect.y + rect.h)
		ret = true;
	return ret;
}

int ModulePlayer::InsideRect(p2List<p2Point<int>>* list, p2List<p2Point<int>>* result, SDL_Rect rect, bool inside)const
{
	p2List_item<p2Point<int>>*	item = list->getFirst();
	while (item)
	{
		if (PointInRect(item->data.x, item->data.y, rect) == inside)
		{
			p2Point<int> p(item->data);
			result->add(p);
		}

		item = item->next;
	}

	return result->count();
}

void ModulePlayer::Turbo(PhysBody3D* body, bool brake)const
{
	btVector3 relativeForce;
	if (!brake)
		relativeForce = btVector3(0, 0, POWER_SPEED);
	else
		relativeForce = btVector3(0, 0, -POWER_SPEED);
	btTransform boxTrans = body->GetRealTransform();
	btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
	body->ApplyCentralForce(correctedForce);
}


