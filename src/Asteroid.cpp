#include "Asteroid.h"
#include "Texture.h"
#include <ext.hpp>
#include "PxPhysicsAPI.h"



Core::Asteroid::Asteroid(bool)
{
}


Core::Asteroid::Asteroid(glm::vec3 pos, glm::vec3 col, float sc, Core::RenderContext& ctxt)
{
	color = col;
	scale = sc;
	speed = 1.0f;
	position = pos;
	context = ctxt;
}


void Core::Asteroid::render(GLuint program,Core::Camera cam,float time)
{
	glm::mat4 shipModelMatrix =
		glm::translate(glm::vec3(actor->getGlobalPose().p.x, actor->getGlobalPose().p.y, actor->getGlobalPose().p.z)) * glm::toMat4(glm::quat(actor->getGlobalPose().q.w, actor->getGlobalPose().q.x, actor->getGlobalPose().q.y, actor->getGlobalPose().q.z))
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

void Core::Asteroid::renderTexture(GLuint program,GLuint tex, Core::Camera cam, float time,float rotate)
{
	glm::mat4 shipModelMatrix =
		glm::translate(glm::vec3(actor->getGlobalPose().p.x, actor->getGlobalPose().p.y, actor->getGlobalPose().p.z)) * glm::toMat4(glm::quat(actor->getGlobalPose().q.w, actor->getGlobalPose().q.x, actor->getGlobalPose().q.y, actor->getGlobalPose().q.z))
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





glm::vec3 Core::Asteroid::getPosition()
{
	return position;
}

void Core::Asteroid::setPosition(glm::vec3 in)
{
	position = in;

}

physx::PxVec3 Core::Asteroid::getPositionPx()
{
	return physx::PxVec3(position.x,position.y,position.z);
}

void Core::Asteroid::setPosition(physx::PxVec3 in)
{
	position = glm::vec3(in.x, in.y, in.z);
}


float Core::Asteroid::getScale()
{
	return scale;
}

void Core::Asteroid::setScale(float in)
{
	scale = in;
}

void Core::Asteroid::releaseActor()
{
	actor->release();
}

void Core::Asteroid::setActor(physx::PxRigidDynamic* act)
{
	actor = act;
	actor->setDominanceGroup(2);
	actor->setLinearDamping(0.3f);
}


physx::PxRigidDynamic* Core::Asteroid::getActor()
{
	return actor;
}

