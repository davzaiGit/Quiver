#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <ctype.h>
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
#include "Controller.h"
#include "PxPhysicsAPI.h"
#include "Player.h"
#include "Planet.h"
#include "Box.cpp"

#define PVD_HOST "127.0.0.1"
#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

//OpenGL inits
GLuint program;
GLuint sunProgram;
GLuint skyboxProgram;


GLuint skyboxTex;
GLuint sunTex;


Core::Shader_Loader shaderLoader;
obj::Model sphereModel;
obj::Model shipModel;
obj::Model skyBoxModel;

Core::RenderContext sphereContext;
Core::RenderContext shipContext;
Core::RenderContext skyBoxContext;

//PhysX inits
static physx::PxDefaultErrorCallback gErrorCallback;
static physx::PxDefaultAllocator gAllocator;
physx::PxFoundation* gFoundation = NULL;
physx::PxPhysics* gPhysics = NULL;
physx::PxCooking* gCooking = NULL;
physx::PxPvd* gPvd = NULL;
physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
physx::PxScene* gScene = NULL;
physx::PxMaterial* gMaterial = NULL;
bool recordMemoryAllocations = true;
physx::PxRigidDynamic* shipActor;
physx::PxRigidStatic* sunActor;




//Entity intialization
Core::Camera cam;
Core::Player player;
Core::Planet skybox = Core::Planet(true);
Core::Planet planet1 = Core::Planet(true);
Core::Planet planet2 = Core::Planet(true);
Core::Planet planet3 = Core::Planet(true);
Core::Planet planet4 = Core::Planet(true);
Core::Planet planet5 = Core::Planet(true);
Core::Controller controller;


//light source position
glm::vec3 lightSource = glm::normalize(glm::vec3(0.0f, 0.0f, 3.0f));


//Keyboard input 
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w': shipActor->setLinearVelocity(shipActor->getLinearVelocity() + Core::GlmToPxVec3(cam.getFront()) * 0.6f); break;
	case 's': shipActor->setLinearVelocity(shipActor->getLinearVelocity() - Core::GlmToPxVec3(cam.getFront()) * 0.5f); break;
	case 'd': shipActor->setLinearVelocity(shipActor->getLinearVelocity() + Core::GlmToPxVec3(glm::cross(cam.getFront(), glm::vec3(0.0f, 1.0f, 0.0f))) * 0.6f); break;
	case 'a': shipActor->setLinearVelocity(shipActor->getLinearVelocity() - Core::GlmToPxVec3(glm::cross(cam.getFront(), glm::vec3(0.0f, 1.0f, 0.0f))) * 0.6f); break;
	case 'e': shipActor->setLinearVelocity(shipActor->getLinearVelocity() - Core::GlmToPxVec3(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), cam.getFront())) * 0.3f);; break;
	case 'q': shipActor->setLinearVelocity(shipActor->getLinearVelocity() + Core::GlmToPxVec3(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), cam.getFront())) * 0.3f);; break;
	}
}



//TODO:Pointer should go to the center of the window after leaving said window.


void mouse_callback(int x, int y)
{

	if (controller.getFirstMouse())
	{
		controller.setLastX(x);
		controller.setLastY(y);
		controller.setFirstMouse(false);
	}
	float xoffset = x - controller.getLastX();
	float yoffset = controller.getLastY() - y;
	controller.setLastX(x);
	controller.setLastY(y);
	float sensitivity = 0.9f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	controller.setYaw(controller.getYaw() + xoffset);
	controller.setPitch(controller.getPitch() + yoffset);
	std::cout << controller.getPitch();
	std::cout << "\n";
	if (controller.getPitch() > 89.0f)
		controller.setPitch(89.0f);
	if (controller.getPitch() < -89.0f)
		controller.setPitch(-89.0f);
	controller.setDirection(glm::vec3(
		cos(glm::radians(controller.getYaw())) * cos(glm::radians(controller.getPitch())),
		sin(glm::radians(controller.getPitch())),
		sin(glm::radians(controller.getYaw())) * cos(glm::radians(controller.getPitch())))
	);
	cam.setFront(glm::normalize(controller.getDirection()));
}


void initPhysics(bool interactive)
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, physx::PxCookingParams(physx::PxTolerancesScale()));

	
	physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, 0.0f);
	gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);
	physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

}

void stepPhysics(bool )
{
	gScene->simulate(1.0f / 60.0f);
	gScene->fetchResults(true);
	physx::PxQuat kekw = physx::PxQuat(glm::radians(-controller.getYaw()) + glm::radians(90.f), physx::PxVec3(0.0f, 1.0f, 0.0f)) * physx::PxQuat(glm::radians(-controller.getPitch()), physx::PxVec3(1.0f, 0.0f, 0.0f));
	cam.setPosition(player.getPosition() + controller.getDirection() * -1.f + glm::vec3(0, 0.25f, 0));
	player.setPosition(shipActor->getGlobalPose().p);
	planet1.setPosition(sunActor->getGlobalPose().p);
	skybox.setPosition(player.getPosition());
}





void cleanupPhysics(bool)
{
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		physx::PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
}




void renderScene()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	stepPhysics(true);
	//Camera update
	cam.update();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glUseProgram(program); 
	glUniform3f(glGetUniformLocation(program, "lightPos"),2,0,2);


	//Player drawing
	player.render(program, glm::vec3(0.6f), cam, controller.getYaw(), controller.getPitch());

	//Planet drawing
	skybox.renderTexture(skyboxProgram,skyboxTex, cam, time,0.0f);
	planet1.renderTexture(skyboxProgram,sunTex, cam, time,55.0f);
	planet2.render(program, cam, time);
	planet3.render(program, cam, time);
	planet4.render(program, cam, time);
	planet5.render(program, cam, time);
	
	glutSwapBuffers();
}

physx::PxRigidDynamic* createDynamic(const physx::PxTransform& t,obj::Model mod, const physx::PxVec3& velocity = physx::PxVec3(0))
{

	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count = mod.vertex.size() / 3;
	convexDesc.points.stride = 3 * sizeof(float);
	convexDesc.points.data = &mod.vertex[0];
	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
	gCooking->cookConvexMesh(convexDesc, buf, &result);
	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	physx::PxConvexMesh* convexMesh = gPhysics->createConvexMesh(input);


	physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(*gPhysics, t, physx::PxConvexMeshGeometry(convexMesh) , *gMaterial,1.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}

physx::PxRigidStatic* createStatic(const physx::PxTransform& t, obj::Model mod) {
	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count = mod.vertex.size() / 3;
	convexDesc.points.stride = 3 * sizeof(float);
	convexDesc.points.data = &mod.vertex[0];
	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
	gCooking->cookConvexMesh(convexDesc, buf, &result);
	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	physx::PxConvexMesh* convexMesh = gPhysics->createConvexMesh(input);

	physx::PxRigidStatic* stat = physx::PxCreateStatic(*gPhysics, t, physx::PxConvexMeshGeometry(convexMesh),*gMaterial);
	gScene->addActor(*stat);
	return stat;

}


void init()
{
	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader.vert", "shaders/shader.frag");
	//sunProgram = shaderLoader.CreateProgram("shaders/shader.vert", "shaders/shader.frag");
	skyboxProgram = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");
	//Camera object loading
	cam = Core::Camera(glm::vec3(-5, 0, 0), glm::vec3(0, 0, 0));
	controller = Core::Controller();

	//Asset loading
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	skyBoxModel = obj::loadModelFromFile("models/skyBoxCube.obj");
	
	skyboxTex = Core::LoadTexture("textures/Cube2.png");
	sunTex = Core::LoadTexture("textures/papa.png");

	sphereContext.initFromOBJ(sphereModel);
	skyBoxContext.initFromOBJ(skyBoxModel);
	shipContext.initFromOBJ(shipModel);


	//Object instantialization
	player = Core::Player(shipModel.vertex, cam.getPosition(), cam.getFront(), shipContext, 90, 30, 3);
	skybox = Core::Planet(player.getPosition(), glm::vec3(1.0f, 0.0f, 0.0f), 10000.0f, skyBoxContext);
	planet1 = Core::Planet(glm::vec3(1.0f, 0.5f, 0.2f), glm::vec3(2, 0, 2),0.0f, 1.0f, 0.0f, 30.0f, 0.0f, sphereContext);
	planet2 = Core::Planet(glm::vec3(0.0f, 0.6f, 0.9f), glm::vec3(2, 0, 2),0.3f, 0.01f, 123.0f, 15.0f, 0.0f, sphereContext);
	planet3 = Core::Planet(glm::vec3(0.0f, 0.5f, 0.1f), glm::vec3(2, 0, 2),1.1f, 0.01f,203.0f, 10.0f, 0.0f, sphereContext);
	planet4 = Core::Planet(glm::vec3(0.5f, 0.1f, 0.3f), glm::vec3(2, 0, 2),3.2f, 0.01f,293.0f, 17.0f, 0.0f, sphereContext);
	planet5 = Core::Planet(glm::vec3(0.9f, 0.9f, 0.7f), glm::vec3(2, 0, 2),5.1f, 0.01f, 373.0f, 24.0f, 0.0f, sphereContext);


	//Physics inits
	initPhysics(true);

	shipActor = createDynamic(physx::PxTransform(player.getPositionPx()),shipModel);
	shipActor->setLinearDamping(0.9f);
	shipActor->setMaxLinearVelocity(3.5f);
	sunActor = createStatic(physx::PxTransform(planet1.getPositionPx()),sphereModel);

	
}




void shutdown()
{
	shaderLoader.DeleteProgram(program);
	cleanupPhysics(true);
}

void idle()
{
	glutPostRedisplay();
}


int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Quiver");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutMotionFunc(mouse_callback);
	glutPassiveMotionFunc(mouse_callback);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);
	glutMainLoop();
	shutdown();

	return 0;
}
