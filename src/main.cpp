#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Controller.h"
#include "PxPhysicsAPI.h"
#include "Player.h"
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
static physx::PxDefaultErrorCallback gDefaultErrorCallback;
static physx::PxDefaultAllocator gDefaultAllocatorCallback;
physx::PxFoundation* gFoundation = NULL;
physx::PxPhysics* gPhysics = NULL;
physx::PxCooking* gCooking = NULL;
physx::PxPvd* gPvd = NULL;
physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
physx::PxScene* gScene = NULL;
bool recordMemoryAllocations = true;

//Entity intialization
Core::Camera cam;
Core::Player player;
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


void drawObject(GLuint program,Core::RenderContext context, glm::mat4 modelMatrix, glm::vec3 color)
{
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cam.getPosition().x, cam.getPosition().y, cam.getPosition().z);
	glm::mat4 transformation = cam.getPerspective() * cam.getView() * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	Core::DrawContext(context);
	glUseProgram(0);
}

void renderScene()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	//Camera update
	cam.update();


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glUseProgram(program); 
	glUniform3f(glGetUniformLocation(program, "lightPos"),2,0,2);


	//Player drawing
	player.render(program, glm::vec3(0.6f), cam, controller.getYaw(), controller.getPitch());

	//Sun drawing
	drawObject(program,sphereContext, glm::translate(glm::vec3(2,0,2)), glm::vec3(1.0f, 0.5f, 0.2f));

	//Planet drawing
	drawObject(program,sphereContext, glm::translate(glm::vec3( 2 + 3 * sinf(time),0, 2 + 3 * cosf(time))), glm::vec3(0.0f, 0.6f, 0.9f));
	drawObject(program,sphereContext, glm::translate(glm::vec3(2 + 5 * cosf(time), 0, 2 + 5 * sinf(time))) * glm::scale(glm::vec3(0.5,0.5,0.5)), glm::vec3(0.0f, 0.5f, 0.1f));

	//Moon drawing
	drawObject(program, sphereContext, glm::translate(glm::vec3(2 + 3 * sinf(time) + 1.5 * sinf(2 * time), 0, 2 + 3 * cosf(time) + 1.5 * cosf(2 * time))) * glm::scale(glm::vec3(0.2, 0.2, 0.2)), glm::vec3(0.5f, 0.1f, 0.3f));
	drawObject(program, sphereContext, glm::translate(glm::vec3(2 + 5 * cosf(time) + sinf(2 * time), 0, 2 + 5 * sinf(time) +cosf(2 * time))) * glm::scale(glm::vec3(0.2, 0.2, 0.2)), glm::vec3(0.9f, 0.9f, 0.7f));

	glutSwapBuffers();
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader.vert", "shaders/shader.frag");

	//Camera object loading
	cam = Core::Camera(glm::vec3(-5, 0, 0), glm::vec3(0, 0, 0));
	controller = Core::Controller();

	//Asset loading
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	sphereContext.initFromOBJ(sphereModel);
	shipContext.initFromOBJ(shipModel);

	//Entity object inits
	player = Core::Player(cam.getPosition(),cam.getFront(),shipContext, 100, 30, 3);

	//Physics inits
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);
	gPvd = PxCreatePvd(*gFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation,
		physx::PxTolerancesScale::PxTolerancesScale(), recordMemoryAllocations, gPvd);
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
