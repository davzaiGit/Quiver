#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "PxPhysicsAPI.h"


#include "Box.cpp"

#define PVD_HOST "127.0.0.1"


GLuint program;
GLuint sunProgram;
Core::Shader_Loader shaderLoader;


obj::Model shipModel;
obj::Model sphereModel;
Core::RenderContext shipContext;
Core::RenderContext sphereContext;


static physx::PxDefaultErrorCallback gDefaultErrorCallback;
static physx::PxDefaultAllocator gDefaultAllocatorCallback;

physx::PxFoundation* gFoundation = NULL;
physx::PxPhysics* gPhysics = NULL;
physx::PxCooking* gCooking = NULL;

physx::PxPvd* gPvd = NULL;

physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
physx::PxScene* gScene = NULL;

bool recordMemoryAllocations = true;
	
float yaw = 0.0f;
float pitch = 0.0f;
float cameraAngle = 0;
int lastX = 640;
int lastY = 360;
glm::vec3 cameraPos = glm::vec3(-5, 0, 0);
glm::vec3 cameraDir;
glm::vec3 cameraFront;
glm::vec3 direction;
glm::mat4 cameraMatrix, perspectiveMatrix;

Core::Camera cam;

glm::vec3 lightSource = glm::normalize(glm::vec3(0.0f, 0.0f, 3.0f));


bool firstMouse = true;

void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch (key)
	{
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'w': cam.setPosition(cam.getPosition() + direction * moveSpeed); break;
	case 's': cam.setPosition(cam.getPosition() - direction * moveSpeed); break;
	case 'd': cam.setPosition(cam.getPosition() + (glm::cross(cam.getForward(), glm::vec3(0, 1, 0)) * moveSpeed)); break;
	case 'a': cam.setPosition(cam.getPosition() - (glm::cross(cam.getForward(), glm::vec3(0, 1, 0)) * moveSpeed)); break;
	case 'e': cam.setPosition(cam.getPosition() + (glm::cross(cam.getForward(), glm::vec3(1, 0, 0)) * moveSpeed)); break;
	case 'q': cam.setPosition(cam.getPosition() - (glm::cross(cam.getForward(), glm::vec3(1, 0, 0)) * moveSpeed)); break;
	}
}

void mouse_callback(int x, int y)
{
	if (firstMouse)
	{
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	float xoffset = x - lastX;
	float yoffset = lastY - y;
	lastX = x;
	lastY = y;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cam.setFront(glm::normalize(direction));
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
	cam.update();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glm::mat4 shipModelMatrix =
		glm::translate(glm::vec3(cam.getPosition() + cam.getFront() + glm::vec3(0.4,-0.1,0)))
        * glm::rotate((glm::radians(-yaw) + glm::radians(90.f)), glm::vec3(0,1,0)) * glm::rotate(glm::radians(-pitch), glm::vec3(1, 0, 0))
        * glm::scale(glm::vec3(0.25f));
	glUseProgram(program); 
	glUniform3f(glGetUniformLocation(program, "lightPos"),2,0,2);
	drawObject(program,shipContext, shipModelMatrix, glm::vec3(0.6f));
	//Sun init
	drawObject(program,sphereContext, glm::translate(glm::vec3(2,0,2)), glm::vec3(1.0f, 0.5f, 0.2f));
	//Planet inits
	drawObject(program,sphereContext, glm::translate(glm::vec3( 2 + 3 * sinf(time),0, 2 + 3 * cosf(time))), glm::vec3(0.0f, 0.6f, 0.9f));
	drawObject(program,sphereContext, glm::translate(glm::vec3(2 + 5 * cosf(time), 0, 2 + 5 * sinf(time))) * glm::scale(glm::vec3(0.5,0.5,0.5)), glm::vec3(0.0f, 0.5f, 0.1f));
	//Moon inits
	drawObject(program, sphereContext, glm::translate(glm::vec3(2 + 3 * sinf(time) + 1.5 * sinf(2 * time), 0, 2 + 3 * cosf(time) + 1.5 * cosf(2 * time))) * glm::scale(glm::vec3(0.2, 0.2, 0.2)), glm::vec3(0.5f, 0.1f, 0.3f));
	drawObject(program, sphereContext, glm::translate(glm::vec3(2 + 5 * cosf(time) + sinf(2 * time), 0, 2 + 5 * sinf(time) +cosf(2 * time))) * glm::scale(glm::vec3(0.2, 0.2, 0.2)), glm::vec3(0.9f, 0.9f, 0.7f));
	glutSwapBuffers();
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader.vert", "shaders/shader.frag");
	cam = Core::Camera(glm::vec3(-5, 0, 0), glm::vec3(0, 0, 0) );
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	shipContext.initFromOBJ(shipModel);
	sphereContext.initFromOBJ(sphereModel);
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
