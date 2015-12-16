#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{}

ParticleSystem::~ParticleSystem()
{
	p2List_item<Particle*>* item = particles.getFirst();
	while (item)
	{
		delete item->data;
		item = item->next;
	}
	particles.clear();
}

ParticleSystem* ParticleSystem::CreateParticleSystem(PhysBody3D* ref, vec3 _offset, TYPE _type, bool active)
{
	reference = ref;
	offset = _offset;
	type = _type;
	on = active;
	timer.Start();

	return this;
}

void ParticleSystem::Update(float dt)
{
	if (on)
	{
		if (timer.ReadSec() > SPAWN_TIME)
		{
			vec3 pos = reference->GetPosition();
			pos += offset;
			Particle* p = new Particle();
			p->CreateParticle(pos);
			particles.add(p);

			timer.Start();
		}
	}

	p2List_item<Particle*>* item = particles.getFirst();
	while (item)
	{
		item->data->life -= dt;
		if (item->data->life <= 0)
		{
			p2List_item<Particle*>*tmp = item->next;
			particles.del(item);
			item = tmp;
		}
		else
		{
			item->data->shape.Render();
			item = item->next;
		}
			
	}
}

//----------------------------------------------------------------------------------------------------------------------------
void Particle::CreateParticle(vec3 pos)
{
	life = PARTICLE_LIFE;
	shape.size = vec3(1, 1, 1);
	shape.SetPos(pos.x, pos.y, pos.z);
	shape.color = Orange;
}