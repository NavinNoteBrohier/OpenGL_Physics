#include "Physics_Walkthrough_App.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Camera.h"
#include "Physics\Physics\Physics.h"
#include <time.h>
#include <imgui.h>

using namespace glm;
using namespace std;
using namespace Physics;

vec3* FixedPosition;

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

	///
	char izard[] = "not as good as Venasaur";///
	///



	//Jello
	const int maxX = 10;
	const int maxY = 10;
	const int maxZ = 10;
	int NumberofFollowers = 500;

	//Fixed object.
	FixedPosition = new vec3(30, 0, 5);
	PhysicsObjects *FixedObject = new PhysicsObjects(vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0), 100 * NumberofFollowers, 0);
	if (FixedPosition != nullptr)
	{

		FixedObject->SetPosition(*FixedPosition);
		FixedObject->AttatchVector(FixedPosition);
		FixedObject->SetCollider(new SphereCollider(1.0f));
		PhysicCollection->AttatchObject(FixedObject);
		PhysicsRendering->GetRenderInfo(FixedObject)->color = vec4(glm::vec4(
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			1));
	};

	for (int i = 0; i < NumberofFollowers; i++)
	{
		PhysicsObjects *left = new PhysicsObjects();
		left->SetPosition(vec3(FixedObject->Getpos().x + (i *1), 1, FixedObject->Getpos().z + (i * 1)));
		left->SetCollider(new SphereCollider(0.1f));
		left->SetMass(10);
		left->SetFriction(5);
		PhysicCollection->AttatchObject(left);
		PhysicsRendering->GetRenderInfo(left)->color = vec4(glm::vec4(0, 1, 0, 1));

		//PhysicsObjects *right = new PhysicsObjects();
		//right->SetPosition(vec3(0, 1, 10 + (i * 2)));
		//right->SetCollider(new SphereCollider(0.5f));
		//right->SetMass(1);
		//right->SetFriction(1);
		//PhysicCollection->AttatchObject(right);
		//PhysicsRendering->GetRenderInfo(right)->color = vec4(glm::vec4(0, 1, 0, 1));

		//Spring *spring = new Spring(left, right, 2, 300, 0.5f);
		//PhysicCollection->AttatchConstraint(spring);
		
		if (FixedPosition != nullptr)
		{
			Spring *springS = new Spring(left, PhysicCollection->GetObjects()[i], 0.1, 1000, 1.0f);
			PhysicCollection->AttatchConstraint(springS);
		//	springS = new Spring(right, FixedObject, 5, 300, 0.5f);
		//	PhysicCollection->AttatchConstraint(springS);
		}
	}
	// 
	for (int x = 0; x < maxX; x++)
	{
		for (int y = 0; y < maxY; y++)
		{
			for (int z = 0; z < maxZ; z++)
			{
			// In here we add constraints from each node to each other 
			// Around it we (above, below, left, right, forwards, bacwkwards and diagonals)

			// Most likely this will be about creating a list of connections between objects
			// Then creating a bunch of constraints using those connections.
			
			// Try starting out with a sheet of physics objects that are connected only verically and horizontally.

			// Then add some diagonals in the 2 dimensions

			// Then expand to three dimensions.

			// at every step, try throwing projectiles at your creation and see how it reacts.
			}
		}
	}




	// Add objects to scene
	//Camera Sphere.
	PhysicsObjects *CameraObject = new PhysicsObjects(vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0), 1, 0);
	CameraObject->SetPosition(m_camera->GetPosition());
	CameraObject->AttatchVector(&m_camera->GetPosition());
	CameraObject->SetCollider(new SphereCollider(1.0f));
	CameraObject->SetToDraw(false);
	PhysicCollection->AttatchObject(CameraObject);
	//Spring *springS = new Spring(left, CameraObject, 5, 300, 0.5f);
	//PhysicCollection->AttatchConstraint(springS);
	//springS = new Spring(right, CameraObject, 5, 300, 0.5f);
	//PhysicCollection->AttatchConstraint(springS);

	// Sphere Grid
	for (int x = 0; x < 20; x++)
	{
		for (int y = 0; y <3; y++)
		{
			for (int z = 0; z < 20; z++)
			{
				int tempmass = 1;
				int tempfriction = 1;
				PhysicsObjects *obj = new PhysicsObjects(vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0), tempmass, tempfriction);
				obj->SetPosition(vec3(x - 5.0f, y, z - 5.0f));
				obj->SetCollider(new SphereCollider(0.2f));
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
	for (int i = 0; i < 5; i++)
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
	//Debug keys
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
		PhysicsObjects *obj = new PhysicsObjects(vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0), 5, 1);
		obj->SetPosition(m_camera->GetPosition());
		obj->SetVelocity(m_camera->GetFront() * 20.0f);
		obj->SetCollider(new SphereCollider(1.0f));
		obj->SetLifeTime(10.0f);
		PhysicsRendering->GetRenderInfo(obj)->color = vec4(0.1f, 0.1f, 0.1f, 1.0f);
		PhysicCollection->AttatchObject(obj);

		Pressed = true;
	}
	else if(input->isKeyUp(aie::INPUT_KEY_SPACE)) Pressed = false;

	// Fixed Pos
	if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
	{
		FixedPosition->x += Speed * deltaTime;
	}
	if (input->isKeyDown(aie::INPUT_KEY_LEFT))
	{
		FixedPosition->x-= Speed * deltaTime;
	}
	if (input->isKeyDown(aie::INPUT_KEY_DOWN))
	{
		FixedPosition->z+= Speed * deltaTime;
	}
	if (input->isKeyDown(aie::INPUT_KEY_UP))
	{
		FixedPosition->z-= Speed * deltaTime;
	}

#pragma endregion

#pragma region //UI
	ImGui::Begin("Fixed object");
	ImGui::SliderFloat("Fixed Object X", &FixedPosition->x, -50, 50);
	ImGui::SliderFloat("Fixed Object Y", &FixedPosition->y, -50, 50);
	ImGui::SliderFloat("Fixed Object Z", &FixedPosition->z, -50, 50);
	ImGui::SliderFloat("Speed", &Speed, 0.5f, 50);
	ImGui::End();
#pragma endregion

	// Update Physics.
	PhysicCollection->Update(deltaTime);

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))// quit if we press escape
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
	PhysicsRendering->Draw(PhysicCollection);
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