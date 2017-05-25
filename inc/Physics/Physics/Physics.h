#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <Gizmos.h>

using namespace std;
using namespace glm;

namespace Physics
{
	class PhysicsObjects;
	class PhysicsScene;
	class PhysicsRenderer;
	class Collider;
	class SphereCollider;
	class Constraint;
	class Spring;

#pragma region //Structs

#pragma region Intersect

	struct IntersectData
	{
		vec3 CollisionVector;
	};

#pragma endregion

#pragma endregion

#pragma region //Classes

#pragma region // Physics Objects
	class PhysicsObjects
	{
	public:
		PhysicsObjects();
		PhysicsObjects(vec3 a_pos, vec3 a_velocity, vec3 a_acceleration, float a_mass, float a_friction);
		virtual ~PhysicsObjects();

		void ApplyForce(const vec3 a_Force);
		void UpdateForce(float a_deltaTime);

		const vec3 & Getpos() const { return m_Position; };
		const vec3 & GetVelocity() const { return m_Velocity; };
		const vec3 & GetAcceleration() const { return m_Acceleration; };
		const float GetMass() const { return m_Mass; };
		const float GetFriction() const { return m_Friction; };
		Collider* GetCollider() const;
		const float GetLifeTime() const { return LifeTime; };
		const bool GetTODRAW() const { return To_Draw; };

		void SetPosition(const glm::vec3 & pos) { m_Position = pos; };
		void SetVelocity(const glm::vec3 & vel) { m_Velocity = vel; };
		void SetAcceleration(const glm::vec3 & acel) { m_Acceleration = acel; };
		void SetMass(float mass) { m_Mass = mass; };
		void SetFriction(float Friction) { m_Friction = Friction; };
		void SetCollider(Collider* a_collider);
		void SetLifeTime(float a_lifetime) { LifeTime = a_lifetime; };
		void SetToDraw(bool a_bool) { To_Draw = a_bool; };
		void AttatchVector(vec3* a_vec) { LockPosition = a_vec; };

	protected:
		vec3	m_Position;
		vec3	m_Velocity;
		vec3	m_Acceleration;
		float	m_Mass;
		float	m_Friction;

		float LifeTime;
		bool To_Draw;

		vec3 *LockPosition = nullptr;

		void CalculateLifeTime(float deltatime);

		Collider* m_collider = nullptr;

	private:
	};
#pragma endregion

#pragma region // Physics Scene
	class PhysicsScene
	{
	public:
		
		struct CollisionInfo
		{
			PhysicsObjects * objA;
			PhysicsObjects * objB;
		
			IntersectData intersect;
			// More data will be put in here for collision resolution
		};
		
		PhysicsScene();
		virtual	~PhysicsScene();

		void Update(float a_deltatime);

		void ApplyGlobalForce(const vec3 & a_force);

		// Attatch a physics object to the scene.
		void AttatchObject(PhysicsObjects * obj);

		// Remove a Physics object to the scene.
		void RemoveObject(PhysicsObjects * obj);

		const vector<PhysicsObjects*> & GetObjects() const { return m_objects; };

		bool isObjetColliding(PhysicsObjects* obj);

		void ResolveCollisions();

		//Constraints
		void AttatchConstraint(Constraint* con);
		void RemoveConstraint(Constraint* con);
		const vector<Constraint*> & GetConstraints() const;

	protected:
		vector<PhysicsObjects*> m_objects;
		vector<Constraint*> m_constraints;
		vec3 m_GlobalForce;

		void DetectCollisions();
		

	private:
		
		vector<CollisionInfo> m_collisions;
	};
#pragma endregion

#pragma region //Physics renderer

	class PhysicsRenderer
	{
	public:
		PhysicsRenderer();
		virtual ~PhysicsRenderer();

		struct RenderInfo
		{
			vec4 color = vec4(1, 1, 1, 1);
		};

		void Draw(PhysicsScene *a_Scene);

		RenderInfo* GetRenderInfo(PhysicsObjects* obj);
	protected:
		map<PhysicsObjects*, RenderInfo> m_renderInfo;
		void RenderGizmosPhysics(PhysicsScene* a_scene);
		void RenderGizmosConstraints(PhysicsScene* a_scene);

	private:

	};

#pragma endregion

#pragma region //Physics Colliders

	class Collider
	{
	public:
		enum Type
		{
			NONE,
			SPHERE,
			AABB,
			OBB
		};
		//
		Collider(Type type);
		virtual ~Collider();
		//
		static Collider* GetNullInstance();
		//
		Type GetType() const { return m_Type; };
		//
		virtual void Transform(PhysicsObjects* obj) {};
		virtual bool Intersects(Collider* other, IntersectData* intersect)const { return false; };
		//
		static bool SphereToSphereIntersect(const SphereCollider* objA, SphereCollider* objB, IntersectData* Info);



	protected:
		Type m_Type;

	private:
	};

#pragma region //Sphere Collider

	class SphereCollider : public Collider
	{
	public:
		SphereCollider();
		SphereCollider(float radius);
		virtual ~SphereCollider();

		// Getters
		const vec3& GetPos() const { return m_position; };
		float GetRadius() const { return m_radius; };

		// Setters
		void Setpos(const vec3& a_vec) { m_position = a_vec; };
		void SetRadius(float a_radius) { m_radius = a_radius; };

		bool Intersects(Collider* other,IntersectData* intersect) const;

		virtual void Transform(PhysicsObjects * obj);

	//	static Collider * GetNullInstance();

		

	protected:
		vec3 m_position;
		float m_radius;



	private:

	};

#pragma endregion

#pragma endregion

#pragma region //Physics Constraint

	class Constraint
	{
	public:

		enum class Type
		{
			SPRING,JOINT
		};

		Constraint(std::vector<PhysicsObjects*> objects, Type type);

		Constraint();
		virtual ~Constraint();

		virtual void Update(float deltatime) = 0;// {/* Intentionally left empty. */};

		const vector<PhysicsObjects*> & GetObjects() { return m_Objects; };
		Type GetType() const { return m_type; };



	protected:
		vector<PhysicsObjects*> m_Objects;
		Type m_type;
	private:
	};

#pragma region //Spring
	class Spring : public Constraint
	{
	public:
		Spring(PhysicsObjects *objA, PhysicsObjects *objB);
		Spring(PhysicsObjects *objA, PhysicsObjects *objB, float Length, float Stiff, float Friction);
		virtual ~Spring();

		void Update(float deltatime);

	protected:
		float m_length;
		float m_stiffness;
		float m_friction;
	private:
	};


#pragma endregion

#pragma endregion

#pragma endregion

};