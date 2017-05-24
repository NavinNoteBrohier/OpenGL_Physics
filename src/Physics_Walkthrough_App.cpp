#include "Physics_Walkthrough_App.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Camera.h"
#include "Physics\Physics\Physics.h"
#include <time.h>

using namespace glm;
using namespace std;
using namespace Physics;



Physics_Walkthrough_App::Physics_Walkthrough_App()
{
	
}

Physics_Walkthrough_App::~Physics_Walkthrough_App()
{

}

bool Physics_Walkthrough_App::startup()
{
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	SpaceX = 0.5f;
	SpaceY = 5;
	// initialise gizmo primitive counts
	aie::Gizmos::create(100000, 100000, 100000, 100000);

	// create simple camera transforms
	m_camera = new Camera();
	m_camera->SetProjection(glm::radians(45.0f), (float)getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.0f);
	m_camera->SetPosition(glm::vec3(10, 10, 10));
	m_camera->Lookat(glm::vec3(0, 0, 0));

	srand(time(NULL));


	// Init Physic's Scene
	PhysicCollection = new PhysicsScene();
	PhysicCollection->ApplyGlobalForce(vec3(0, -9.807, 0));

	// Create Renderer
	PhysicsRendering = new PhysicsRenderer();

	minmass = 1;
	maxmass = 20;
	minfriction = 1;
	maxfriction = 20;
	

	// Add objects to scene
	//Camera Sphere.
	PhysicsObjects *CameraObject = new PhysicsObjects(vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0), 1, 0);
	CameraObject->SetPosition(m_camera->GetPosition());
	CameraObject->AttatchVector(&m_camera->GetPosition());
	CameraObject->SetCollider(new SphereCollider(1.0f));
	CameraObject->SetToDraw(false);
	PhysicCollection->AttatchObject(CameraObject);
	char izard[] = "not as good as Venasaur";
	//Jello
	for (int x = 0; x < 20; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			for (int z = 0; z < 20; z++)
			{
				int tempmass = 1;
				int tempfriction = 1;
				PhysicsObjects *obj = new PhysicsObjects(vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0), tempmass, tempfriction);
				obj->SetPosition(vec3(x - 5.0f, y, z - 5.0f));
				obj->SetCollider(new SphereCollider(0.3f));
				PhysicCollection->AttatchObject(obj);
				PhysicsRendering->GetRenderInfo(obj)->color = vec4(glm::vec4(
					rand() % 255 / 255.0f,
					rand() % 255 / 255.0f,
					rand() % 255 / 255.0f,
					1));
			}
		}
	}
	
	//Falling Spheres
	for (int i = 0; i < 1; i++)
	{
		int tempmass = (rand() % 200 + 1);
		int tempfriction = (rand() % 2);
		PhysicsObjects* obj = new PhysicsObjects(vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0), tempmass, tempfriction);
		obj->SetPosition(vec3(i * SpaceX, i * SpaceY + SpaceY, 0));
		obj->SetCollider(new SphereCollider(0.8f));
		PhysicCollection->AttatchObject(obj);


		PhysicsRendering->GetRenderInfo(obj)->color = vec4(glm::vec4(
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			0.1 * tempmass));
	
	}


	return true;
}

void Physics_Walkthrough_App::shutdown()
{
	aie::Gizmos::destroy();
}

void Physics_Walkthrough_App::update(float deltaTime)
{

#pragma region //Input
	m_camera->Update(deltaTime);
	aie::Input* input = aie::Input::getInstance();

	float Force = 2;

	if (input->isKeyDown(aie::INPUT_KEY_HOME))
	{
		for (int i = 0; i < PhysicCollection->GetObjects().size(); i++)
		{
			PhysicCollection->GetObjects()[i]->SetPosition(vec3(SpaceX * i, i * SpaceY + SpaceY, 0));
		}
	}
	if (input->isKeyDown(aie::INPUT_KEY_END))
	{
		for (int i = 0; i < PhysicCollection->GetObjects().size(); i++)
		{
			float tempmass = (rand() % 10 + 1);
			PhysicCollection->GetObjects()[i]->SetPosition(vec3(SpaceX * i + (rand() % 5 + 1), i * SpaceY + SpaceY + (rand() % 5 + 1), 0)); // X Y Z 
			PhysicCollection->GetObjects()[i]->SetFriction((rand() % 2));
			PhysicCollection->GetObjects()[i]->SetMass(tempmass);
			PhysicsRendering->GetRenderInfo(PhysicCollection->GetObjects()[i])->color = vec4(glm::vec4(
				rand() % 255 / 255.0f,
				rand() % 255 / 255.0f,
				rand() % 255 / 255.0f,
				0.1 * tempmass));
		}
	}
	if (input->isKeyDown(aie::INPUT_KEY_SPACE) && !Pressed)
	{
		PhysicsObjects *obj = new PhysicsObjects(vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0), 1, 1);
		obj->SetPosition(m_camera->GetPosition());
		obj->SetVelocity(m_camera->GetFront() * 20.0f);
		obj->SetCollider(new SphereCollider(1.0f));
		obj->SetLifeTime(10.0f);
		PhysicsRendering->GetRenderInfo(obj)->color = vec4(0.1f, 0.1f, 0.1f, 1.0f);
		PhysicCollection->AttatchObject(obj);

		Pressed = true;
	}
	else if(input->isKeyUp(aie::INPUT_KEY_SPACE)) Pressed = false;
#pragma endregion


	PhysicCollection->Update(deltaTime);
	// quit if we press escape

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Physics_Walkthrough_App::draw()
{

	// wipe the screen to the background colour
	clearScreen();

	// wipe the gizmos clean for this frame
	aie::Gizmos::clear();
	RenderGizmosGrid();
	
	//
//	aie::Gizmos::addSphere(SphereObject->Getpos(), 1, 10, 10,vec4(1,0,0,1));

	//Create gizmos
	PhysicsRendering->RenderGizmosPhysics(PhysicCollection);
	//RenderGizmoObjects();
	

	aie::Gizmos::draw(m_camera->GetProjectionView());
}

void Physics_Walkthrough_App::RenderGizmosGrid()
{
	// draw a simple grid with gizmos
	glm::vec4 white(1);
	glm::vec4 black(0, 0, 0, 1);

	//for (int i = 0; i < 21; ++i)
	//{
	//	aie::Gizmos::addLine(glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), i == 10 ? white : black);
	//	aie::Gizmos::addLine(glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), i == 10 ? white : black);
	//}

	// add a transform so that we can see the axis
	aie::Gizmos::addTransform(glm::mat4());
}