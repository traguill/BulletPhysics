#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include "p2List.h"

class btRigidBody;
class btTransform;
class btVector3;
class Module;
// =================================================
struct PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	void Stop();
	void SetRotation(float x, float y, float z);

	btTransform GetRealTransform()const;
	void ApplyCentralForce(btVector3& force);

	void SetBounciness(const float restitution,const float friction);

private:
	btRigidBody* body = nullptr;

public:
	p2List<Module*> collision_listeners;
};

#endif // __PhysBody3D_H__