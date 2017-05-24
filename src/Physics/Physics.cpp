#include "Physics\Physics\Physics.h"
using namespace Physics;

#pragma region // Physics Objects

PhysicsObjects::PhysicsObjects()
{
	m_Position = vec3(0, 0, 0);
	m_Velocity = vec3(0, 0, 0);
	m_Acceleration = vec3(0, 0, 0);
	m_Mass = 0.0f;
	m_Friction =0.0f;
	LifeTime = -10;
	To_Draw = true;
}

PhysicsObjects::PhysicsObjects(vec3 a_pos, vec3 a_velocity, vec3 a_acceleration, float a_mass, float a_friction)
{
	m_Position = a_pos;
	m_Velocity - a_velocity;
	m_Acceleration = a_acceleration;
	m_Mass = a_mass;
	m_Friction = a_friction;
	LifeTime = -10;
	To_Draw = true;
}

PhysicsObjects::~PhysicsObjects()
{
	delete m_collider;
}

void PhysicsObjects::ApplyForce(const vec3 a_Force)
{
	m_Acceleration += a_Force / m_Mass;
}

void PhysicsObjects::UpdateForce(float a_deltatime)
{
    ApplyForce(-m_Velocity * m_Friction);
	m_Velocity += m_Acceleration * a_deltatime;
	if (LockPosition == nullptr) { m_Position += m_Velocity * a_deltatime; }
	else { m_Position = *LockPosition; };
	m_Acceleration = glm::vec3();
	m_collider->Transform(this);
	CalculateLifeTime(a_deltatime);
}

Collider * Physics::PhysicsObjects::GetCollider() const
{
	if (m_collider == nullptr)
	{
		return Collider::GetNullInstance();
	};
	return m_collider;
}



void Physics::PhysicsObjects::SetCollider(Collider * a_collider)
{
	 delete m_collider;
	 m_collider = a_collider;
	 m_collider->Transform(this);
}

void Physics::PhysicsObjects::CalculateLifeTime(float deltatime)
{
	if (LifeTime == -10) return;
	else LifeTime -= deltatime * 1;
}

#pragma endregion

#pragma region // Physics Scene
// Physics scene

Physics::PhysicsScene::PhysicsScene()
{

}

Physics::PhysicsScene::~PhysicsScene()
{
	for (auto iter = m_objects.begin(); iter != m_objects.end(); iter++)
	{
		delete *iter;
	}

	m_objects.clear();

}

void Physics::PhysicsScene::Update(float a_deltatime)
{
	vector<int> LifetimeNumbers;
	for (auto iter = m_objects.begin(); iter != m_objects.end(); iter++)
	{
		PhysicsObjects *obj = *iter;

		obj->ApplyForce(m_GlobalForce);

		obj->UpdateForce(a_deltatime);
		
		// super dodgy ground collision bouncing.
		// This whill be replaced by peoper collision detection and reaction later.

		glm::vec3 pos = obj->Getpos();
		glm::vec3 vel = obj->GetVelocity();

		if (pos.y < 0.0f)
		{
			obj->SetPosition(vec3(pos.x,0,pos.z));
			obj->SetVelocity(vec3(vel.x, -vel.y, vel.z));
		}

	}

	DetectCollisions();
	ResolveCollisions();
}

void Physics::PhysicsScene::ApplyGlobalForce(const vec3 & a_force)
{
	m_GlobalForce = a_force;
}

void Physics::PhysicsScene::AttatchObject(PhysicsObjects * obj)
{
	auto iter = find(m_objects.begin(), m_objects.end(),obj);

	if (iter == m_objects.end()) // Object pointer is not already in our vector
	{
		m_objects.push_back(obj);
	}


}

void Physics::PhysicsScene::RemoveObject(PhysicsObjects * obj)
{
	auto iter = find(m_objects.begin(), m_objects.end(), obj);
	if (iter != m_objects.end()) // Object is already in our vector.
	{
		m_objects.erase(iter); //Maybe this is needed, maybe we give the object back to another class.
	//	delete *iter;
	}
}
bool Physics::PhysicsScene::isObjetColliding(PhysicsObjects * obj)
{
	// Super inefficient searching . . . going to need to change this
	for (auto iter = m_collisions.begin(); iter != m_collisions.end(); iter++)
	{
		if ((*iter).objA == obj || (*iter).objB == obj) return true;
	}
	
	return false;
}

void Physics::PhysicsScene::ResolveCollisions()
{
	// Loop through all collision pairs.
	for (auto iter = m_collisions.begin(); iter != m_collisions.end(); iter++)
	{
		// Get data from the collision 
		// Collision normal ( Direction of collision and overlap)
		vec3 CollisionNormal = iter->intersect.CollisionVector;
		// Mass of both objects.
		float massA = iter->objA->GetMass();
		float massB = iter->objB->GetMass();
		// Velocity of both objects (We might use the relative velocity).
		vec3 velA = iter->objA->GetVelocity();
		vec3 velB = iter->objB->GetVelocity();
		
		// Relative velocity.
		vec3 RelativeVelocity = velA - velB;

		// Find out how much velocity each object had in the collision normal direction
		// In fact, since we have the relative velocity we can just find out once how much total velocity
		// There is in the collision normal direction.
		vec3 colVector = CollisionNormal * (dot(RelativeVelocity,CollisionNormal));

		// Calculate the impulse force (Vector of force and direction)
		// TODO: This feels a bit strange that the masses are affecting this here and 
		// maybe they should be affecting the relative momentums earlier
		vec3 Impulse = colVector / (1.0f / massA + 1.0f + massB);


		// Apply that force to both objects ( each one in an opposite direction)
		iter->objA->SetVelocity(velA - Impulse * (1.0f / massB));
		iter->objB->SetVelocity(velB + Impulse * (1.0f / massA));

		// Move the spheres so that they're not overlapping.
		vec3 seperate = iter->intersect.CollisionVector * 0.5f;
		iter->objA->SetPosition(iter->objA->Getpos() - seperate);
		iter->objB->SetPosition(iter->objB->Getpos() + seperate);
	}
}

void Physics::PhysicsScene::DetectCollisions()
{
	m_collisions.clear(); // Remove old collisions from previous frame.

	// Super inefficient brute force collision detection

	for (auto iterA = m_objects.begin(); iterA != m_objects.end(); iterA++)
	{
		PhysicsObjects* ObjA = *iterA;

		// iterB = iterA + 1 stops double checking
		// Also stops it from checking itself.

		for (auto iterB = iterA + 1; iterB != m_objects.end(); iterB++)
		{
			PhysicsObjects* ObjB = *iterB; 
			CollisionInfo info;
			//Do the Colliders of the two objects overlap.
			if ((ObjA->GetCollider())->Intersects(ObjB->GetCollider(),&info.intersect))
			{
				info.objA = ObjA; 
				info.objB = ObjB;
				m_collisions.push_back(info);
			};

	
		}
	}
}
#pragma endregion

#pragma region // Physics Renderer
// Renderer
Physics::PhysicsRenderer::PhysicsRenderer()
{

}

Physics::PhysicsRenderer::~PhysicsRenderer()
{

}

void Physics::PhysicsRenderer::Draw(PhysicsScene * a_Scene)
{

}

void Physics::PhysicsRenderer::RenderGizmosPhysics(PhysicsScene * a_scene)
{	
		const vector<PhysicsObjects*> & objects = a_scene->GetObjects();

		for (auto iter = objects.begin(); iter != objects.end(); iter++)
		{
			PhysicsObjects* obj = *iter;
			RenderInfo *info = GetRenderInfo(obj);

			vec4 RenderColor = info->color;

			if (a_scene->isObjetColliding(obj))
			{
				RenderColor = vec4(1, 0, 0, info->color.z);
			}

			switch (obj->GetCollider()->GetType())
			{
			case Collider::SPHERE: // Draw the colliders actual size sphere.
				if(obj->GetTODRAW())aie::Gizmos::addSphere(obj->Getpos(), ((SphereCollider*)obj->GetCollider())->GetRadius(),4,4, RenderColor);
				break;
			case Collider::NONE: // No Collider, just show a small sphere.
				if (obj->GetTODRAW())aie::Gizmos::addSphere(obj->Getpos(), 0.05f, 2,2, info->color);
			}

		}
}

PhysicsRenderer::RenderInfo * Physics::PhysicsRenderer::GetRenderInfo(PhysicsObjects * obj)
{
	return &m_renderInfo[obj];
}

#pragma endregion

#pragma region // Physics Collider

#pragma region // Default Collider
Physics::Collider::Collider(Type type) : m_Type(type)
{

}

Physics::Collider::~Collider()
{

}

Collider * Physics::Collider::GetNullInstance()
{
	//// Static object will be created once, then every time
	//// you try to create it again, it will simply refrence
	// the one that was created.
	static Collider RCollider(Type::NONE);
	return &RCollider;

}

bool Physics::Collider::SphereToSphereIntersect(const SphereCollider* objA, SphereCollider* objB, IntersectData* Info)
{
	vec3 CollisionVector = objB->GetPos() - objA->GetPos();
	//vec3 vSpheres 
	
	// Create the collision vector which points from A to B and is the length of the overlap
	float distance = length(CollisionVector);
	float colDistance = objA->GetRadius() + objB->GetRadius();

	//Calculate the distance and total of both radii so that we can tell if we intersect.
	CollisionVector = normalize(CollisionVector) * (colDistance - distance);
	Info->CollisionVector = CollisionVector;



	return distance < colDistance;
}

#pragma endregion

#pragma region // Sphere Collider

Physics::SphereCollider::SphereCollider() : m_radius(1.0f), Collider(Type::SPHERE)
{
	
}

Physics::SphereCollider::SphereCollider(float radius) : m_radius(radius), Collider(Type::SPHERE)
{
	
}

Physics::SphereCollider::~SphereCollider()
{

}
bool Physics::SphereCollider::Intersects(Collider * other, IntersectData* Intersect) const
{
	switch (other->GetType())
	{
	case Collider::SPHERE :
		return SphereToSphereIntersect((SphereCollider*)this, (SphereCollider*)other, Intersect);
	break;

	default:
	break;

	}
	return false;
}

void Physics::SphereCollider::Transform(PhysicsObjects * obj)
{
	Setpos(obj->Getpos());
}

#pragma endregion

#pragma endregion