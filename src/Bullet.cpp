#include "Bullet.h"
#include "Texture.h"
#include <ext.hpp>
#include "PxPhysicsAPI.h"



Core::Bullet::Bullet(bool)
{
}

Core::Bullet::Bullet(glm::vec3 pos, glm::vec3 col, float sc, Core::RenderContext& ctxt)
{
	color = col;
	scale = sc;
	speed = 1.0f;
	position = pos;
	context = ctxt;
}


void Core::Bullet::render(GLuint program,Core::Camera cam,float time)
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

void Core::Bullet::renderTexture(GLuint program,GLuint tex, Core::Camera cam, float time,float rotate)
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





glm::vec3 Core::Bullet::getPosition()
{
	return position;
}

void Core::Bullet::setPosition(glm::vec3 in)
{
	position = in;

}

physx::PxVec3 Core::Bullet::getPositionPx()
{
	return physx::PxVec3(position.x,position.y,position.z);
}

void Core::Bullet::setPosition(physx::PxVec3 in)
{
	position = glm::vec3(in.x, in.y, in.z);
}


float Core::Bullet::getScale()
{
	return scale;
}

void Core::Bullet::setScale(float in)
{
	scale = in;
}

void Core::Bullet::setActor(physx::PxRigidDynamic* act)
{
	actor = act;
	actor->setDominanceGroup(3);
	actor->setLinearDamping(0.f);
	actor->setContactReportThreshold(0.01f);
}

void Core::Bullet::releaseActor()
{
	actor->release();
}

physx::PxRigidDynamic* Core::Bullet::getActor()
{
	return actor;
}

