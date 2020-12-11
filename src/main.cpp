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
#include "Controller.h"
#include "PxPhysicsAPI.h"
#include "Player.h"
#include "Planet.h"
#include "Box.cpp"

#define PVD_HOST "127.0.0.1"

//OpenGL inits
GLuint program;
GLuint sunProgram;
Core::Shader_Loader shaderLoader;
obj::Model sphereModel;
obj::Model shipModel;
Core::RenderContext sphereContext;
Core::RenderContext shipContext;
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

physx::PxRigidDynamic* ball;


#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

//Entity intialization
Core::Camera cam;
Core::Player player;
Core::Planet planet1, planet2, planet3, planet4, planet5;
Core::Controller controller;


//light source position
glm::vec3 lightSource = glm::normalize(glm::vec3(0.0f, 0.0f, 3.0f));


//Keyboard input 
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		//case 'z': cameraAngle -= angleSpeed; break;
		//case 'x': cameraAngle += angleSpeed; break;
	case 'w': cam.setPosition(cam.getPosition() + controller.getDirection() * controller.getMoveSpeed()); break;
	case 's': cam.setPosition(cam.getPosition() - controller.getDirection() * controller.getMoveSpeed()); break;
	case 'd': cam.setPosition(cam.getPosition() + (glm::cross(cam.getForward(), glm::vec3(0, 1, 0)) * controller.getMoveSpeed())); break;
	case 'a': cam.setPosition(cam.getPosition() - (glm::cross(cam.getForward(), glm::vec3(0, 1, 0)) * controller.getMoveSpeed())); break;
	case 'e': cam.setPosition(cam.getPosition() + (glm::cross(cam.getForward(), glm::vec3(1, 0, 0)) * controller.getMoveSpeed())); break;
	case 'q': cam.setPosition(cam.getPosition() - (glm::cross(cam.getForward(), glm::vec3(1, 0, 0)) * controller.getMoveSpeed())); break;
	}
}

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
	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	controller.setYaw(controller.getYaw() + xoffset);
	controller.setPitch(controller.getPitch() + yoffset);
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

	physx::PxRigidStatic* gPlane = physx::PxCreatePlane(*gPhysics, physx::PxPlane(0, 1, 0, 0), *gMaterial);
	
	gScene->addActor(*gPlane);
}

void stepPhysics(bool /*interactive*/)
{
	gScene->simulate(1.0f / 60.0f);
	physx::PxQuat kekw = physx::PxQuat(glm::radians(-controller.getYaw()) + glm::radians(90.f), physx::PxVec3(0.0f, 1.0f, 0.0f)) * physx::PxQuat(glm::radians(-controller.getPitch()), physx::PxVec3(1.0f, 0.0f, 0.0f));
	ball->setGlobalPose(physx::PxTransform(player.getPositionPx(), kekw));
	gScene->fetchResults(true);
}





void cleanupPhysics(bool /*interactive*/)
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
	physx::PxVec3 kek = physx::PxVec3(cam.getPosition().x, cam.getPosition().y, cam.getPosition().z);
	//ball->setGlobalPose(physx::PxTransform(kek));
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glUseProgram(program); 
	glUniform3f(glGetUniformLocation(program, "lightPos"),2,0,2);


	//Player drawing
	player.render(program, glm::vec3(0.6f), cam, controller.getYaw(), controller.getPitch());

	//Planet drawing
	planet1.render(program, cam, time);
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

	//Entity object inits

	physx::PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, physx::PxConvexMeshGeometry(convexMesh) , *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}


void init()
{
	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader.vert", "shaders/shader.frag");

	//Camera object loading
	cam = Core::Camera(glm::vec3(-5, 0, 0), glm::vec3(0, 0, 0));
	controller = Core::Controller();

	//Physics init
	initPhysics(true);

	//Asset loading
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	sphereContext.initFromOBJ(sphereModel);
	shipContext.initFromOBJ(shipModel);

	
	player = Core::Player(shipModel.vertex, cam.getPosition(), cam.getFront(), shipContext, 100, 30, 3);
	planet1 = Core::Planet(glm::vec3(1.0f, 0.5f, 0.2f), glm::vec3(2, 0, 2), 0.0f, 1.0f, 0.0f, sphereContext);
	planet2 = Core::Planet(glm::vec3(0.0f, 0.6f, 0.9f), glm::vec3(2, 0, 2), 3.0f, 1.0f, 0.0f, sphereContext);
	planet3 = Core::Planet(glm::vec3(0.0f, 0.5f, 0.1f), glm::vec3(2, 0, 2), 5.0f, 0.5f, 0.0f, sphereContext);
	planet4 = Core::Planet(glm::vec3(0.5f, 0.1f, 0.3f), glm::vec3(2, 0, 2), 3.0f, 0.2f, 1.5f, sphereContext);
	planet5 = Core::Planet(glm::vec3(0.9f, 0.9f, 0.7f), glm::vec3(2, 0, 2), 5.0f, 0.2f, 2.0f, sphereContext);
	physx::PxVec3 kek = physx::PxVec3(cam.getPosition().x, cam.getPosition().y, cam.getPosition().z);

	//createDynamic(physx::PxTransform(kek), physx::PxBoxGeometry(physx::PxVec3(player.getDimensions().x/2,player.getDimensions().y/2,player.getDimensions().z/2)));
	ball = createDynamic(physx::PxTransform(kek), shipModel);

	
}




void shutdown()
{
	shaderLoader.DeleteProgram(program);
	gPhysics->release();
	gFoundation->release();
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
