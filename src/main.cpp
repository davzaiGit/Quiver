#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"

#include "Box.cpp"

GLuint program;
GLuint sunProgram;
Core::Shader_Loader shaderLoader;

obj::Model shipModel;
obj::Model sphereModel;
Core::RenderContext shipContext;
Core::RenderContext sphereContext;


float cameraAngle = 0;
glm::vec3 cameraPos = glm::vec3(-5, 0, 0);
glm::vec3 cameraDir;

glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightSource = glm::normalize(glm::vec3(0.0f, 0.0f, 3.0f));

void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch(key)
	{
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case 'e': cameraPos += glm::cross(cameraDir, glm::vec3(1, 0, 0)) * moveSpeed; break;
	case 'q': cameraPos -= glm::cross(cameraDir, glm::vec3(1, 0, 0)) * moveSpeed; break;
	}
}

glm::mat4 createCameraMatrix()
{
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0,1,0);

	return Core::createViewMatrix(cameraPos, cameraDir, up);
}

void drawObject(GLuint program,Core::RenderContext context, glm::mat4 modelMatrix, glm::vec3 color)
{
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);

	Core::DrawContext(context);
	glUseProgram(0);
}

void renderScene()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);


	glm::mat4 shipModelMatrix =
        glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0,-0.25f,0))
        * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0,1,0))
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
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	shipContext.initFromOBJ(shipModel);
	sphereContext.initFromOBJ(sphereModel);
}

void shutdown()
{
	shaderLoader.DeleteProgram(program);
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
	glutInitWindowSize(600, 600);
	glutCreateWindow("OpenGL Pierwszy Program");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
