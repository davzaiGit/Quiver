#include "Sun.h"
#include "Texture.h"
#include <ext.hpp>
#include "PxPhysicsAPI.h"



Core::Sun::Sun(bool)
{
}

Core::Sun::Sun(glm::vec3 pos, glm::vec3 col, float sc, Core::RenderContext& ctxt)
{
	type = "planet";
	color = col;
	moonDistance = 0.0f;
	distance = 0.0f;
	scale = sc;
	speed = 1.0f;
	origin = pos;
	positionOffsetAngle = 0.0f;
	context = ctxt;
}



Core::Sun::Sun(glm::vec3 col, glm::vec3 pos, float poa, float sp, float dist, float sc, float md, Core::RenderContext& ctxt)
{
	type = "planet";
	color = col;
	moonDistance = md;
	distance = dist;
	scale = sc;
	speed = sp;
	origin = pos;
	positionOffsetAngle = poa;
	position = glm::vec3(pos.x + dist * sinf(poa) + md * sinf(poa), pos.y, pos.z + dist * cosf(poa) + md * cosf(poa));
	context = ctxt;
}

void Core::Sun::render(GLuint program, Core::Camera cam, float time)
{
	this->updatePhysics(time);
	glm::mat4 shipModelMatrix =
		glm::translate(glm::vec3(actor->getGlobalPose().p.x, actor->getGlobalPose().p.y, actor->getGlobalPose().p.z)) * glm::rotate(glm::radians(time), glm::vec3(0.0f, 1.0f, 0.0f))
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

void Core::Sun::renderTexture(GLuint program, GLuint tex, GLuint texHdr, Core::Camera cam, float time, float rotate)
{
	glm::mat4 shipModelMatrix =
		glm::translate(glm::vec3(origin.x + distance * sinf(time) + moonDistance * sinf(time), origin.y, origin.z + distance * cosf(time) + moonDistance * cosf(time))) * glm::rotate(glm::radians(rotate * time), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::scale(glm::vec3(scale));
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cam.getPosition().x, cam.getPosition().y, cam.getPosition().z);
	glm::mat4 transformation = cam.getPerspective() * cam.getView() * shipModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&shipModelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	Core::SetSunTexture(tex, "texCoord", program, 0);
	Core::DrawContext(context);
	glUseProgram(0);
}





glm::vec3 Core::Sun::getPosition()
{
	return position;
}

void Core::Sun::setPosition(glm::vec3 in)
{
	position = in;

}

physx::PxVec3 Core::Sun::getPositionPx()
{
	return physx::PxVec3(position.x, position.y, position.z);
}

void Core::Sun::setPosition(physx::PxVec3 in)
{
	position = glm::vec3(in.x, in.y, in.z);
}

float Core::Sun::getDistance()
{
	return distance;
}

void Core::Sun::setDistance(float in)
{
	distance = in;
}

float Core::Sun::getMoonDistance()
{
	return moonDistance;
}

void Core::Sun::setMoonDistance(float in)
{
	moonDistance = in;
}

float Core::Sun::getScale()
{
	return scale;
}

void Core::Sun::setScale(float in)
{
	scale = in;
}

void Core::Sun::setActor(physx::PxRigidDynamic* act)
{
	actor = act;
	actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
	actor->setDominanceGroup(1);
	actor->userData = &this->type;
	actor->setContactReportThreshold(1000.0f);
}

physx::PxRigidDynamic* Core::Sun::getActor()
{
	return actor;
}

void Core::Sun::updatePhysics(float time)
{
	actor->setKinematicTarget(physx::PxTransform(origin.x + distance * sinf(positionOffsetAngle + time * speed) + moonDistance * sinf(positionOffsetAngle + time * speed), origin.y, origin.z + distance * cosf(positionOffsetAngle + time * speed) + moonDistance * cosf(positionOffsetAngle + time * speed)));
}
