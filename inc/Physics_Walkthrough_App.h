#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>

class Camera;

namespace Physics
{
	class PhysicsObjects;
	class PhysicsScene;
	class PhysicsRenderer;
	class Collider;
	class SphereCollider;
}

class Physics_Walkthrough_App : public aie::Application {
public:

	Physics_Walkthrough_App();
	virtual ~Physics_Walkthrough_App();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void RenderGizmosGrid();
	

protected:



private:
	void RenderGizmoObjects();

	float SpaceX;
	float SpaceY;
	float SpaceZ;

	bool Pressed = false;

	float maxmass;
	float maxfriction;
	float minmass;
	float minfriction;

	Physics::PhysicsScene* PhysicCollection = nullptr;
	Physics::PhysicsRenderer* PhysicsRendering = nullptr;

	float Speed = 50.0f;

	// camera transforms
	Camera *m_camera = nullptr;

};