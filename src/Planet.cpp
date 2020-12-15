#include "Planet.h"
#include <ext.hpp>
#include "PxPhysicsAPI.h"



Core::Planet::Planet(glm::vec3 col,glm::vec3 pos, float dist, float sc, float md, Core::RenderContext& ctxt)
{
	color = col;
	moonDistance = md;
	distance = dist;
	scale = sc;
	position = pos;
	context = ctxt;
}

void Core::Planet::render(GLuint program,Core::Camera cam,float time)
{
	glm::mat4 shipModelMatrix =
		glm::translate(glm::vec3(position.x + distance * sinf(time) + moonDistance * sinf(time),position.y,position.z + distance * cosf(time) + moonDistance * cosf(time)))
		* glm::scale(glm::vec3(scale));
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cam.getPosition().x, cam.getPosition().y, cam.getPosition().z);
	glm::mat4 transformation = cam.getPerspective() * cam.getView() * shipModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&shipModelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	Core::DrawContext(context);
	glUseProgram(0);
}

glm::vec3 Core::Planet::getPosition()
{
	return position;
}

void Core::Planet::setPosition(glm::vec3 in)
{
	position = in;

}

physx::PxVec3 Core::Planet::getPositionPx()
{
	return physx::PxVec3(position.x,position.y,position.z);
}

void Core::Planet::setPosition(physx::PxVec3 in)
{
	position = glm::vec3(in.x, in.y, in.z);
}

float Core::Planet::getDistance()
{
	return distance;
}

void Core::Planet::setDistance(float in)
{
	distance = in;
}

float Core::Planet::getMoonDistance()
{
	return moonDistance;
}

void Core::Planet::setMoonDistance(float in)
{
	moonDistance = in;
}

float Core::Planet::getScale()
{
	return scale;
}

void Core::Planet::setScale(float in)
{
	scale = in;
}
