#include "Station.h"
#include "Texture.h"
#include <ext.hpp>
#include "PxPhysicsAPI.h"



Core::Station::Station(bool)
{
}

Core::Station::Station(glm::vec3 pos, Core::RenderContext& ctxt)
{
	position = pos;
	context = ctxt;
}


void Core::Station::render(GLuint program,Core::Camera cam,float time)
{
	this->updatePhysics(time);
	glm::mat4 shipModelMatrix =
		glm::translate(glm::vec3(actor->getGlobalPose().p.x, actor->getGlobalPose().p.y, actor->getGlobalPose().p.z)) * glm::rotate(glm::radians(time),glm::vec3(0.0f,1.0f,0.0f))
		* glm::scale(glm::vec3(scale));
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "objectColor"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cam.getPosition().x, cam.getPosition().y, cam.getPosition().z);
	glm::mat4 transformation = cam.getPerspective() * cam.getView() * shipModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&shipModelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	Core::DrawContext(context);
	glUseProgram(0);
}

void Core::Station::renderTexture(GLuint program,GLuint tex, Core::Camera cam, float time,float rotate)
{
	this->updatePhysics(time);
	glm::mat4 shipModelMatrix =
		glm::translate(glm::vec3(actor->getGlobalPose().p.x, actor->getGlobalPose().p.y, actor->getGlobalPose().p.z)) * glm::rotate(glm::radians(time), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::scale(glm::vec3(scale));
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cam.getPosition().x, cam.getPosition().y, cam.getPosition().z);
	glm::mat4 transformation = cam.getPerspective() * cam.getView() * shipModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&shipModelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	Core::SetActiveTexture(tex, "texCoord", program, 0);
	Core::DrawContext(context);
	glUseProgram(0);
}





glm::vec3 Core::Station::getPosition()
{
	return position;
}

void Core::Station::setPosition(glm::vec3 in)
{
	position = in;

}

physx::PxVec3 Core::Station::getPositionPx()
{
	return physx::PxVec3(position.x,position.y,position.z);
}

void Core::Station::setPosition(physx::PxVec3 in)
{
	position = glm::vec3(in.x, in.y, in.z);
}







void Core::Station::setActor(physx::PxRigidDynamic* act)
{
	actor = act;
	actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
	actor->setDominanceGroup(1);
	actor->setContactReportThreshold(600.0f);
}

physx::PxRigidDynamic* Core::Station::getActor()
{
	return actor;
}

void Core::Station::setTrigger(physx::PxRigidDynamic* act)
{
	trigger = act;
	trigger->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
	trigger->setContactReportThreshold(0.1f);
	trigger->setDominanceGroup(4);
}

physx::PxRigidDynamic* Core::Station::getTrigger()
{
	return trigger;
}

void Core::Station::updatePhysics(float time)
{
	
}

float Core::Station::getScale()
{
	return scale;
}

void Core::Station::setScale(float in)
{
	scale = in;
}
