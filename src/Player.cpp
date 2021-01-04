#include "Player.h"
#include <ext.hpp>
#include "PxPhysicsAPI.h"



Core::Player::Player()
{
}

Core::Player::Player(std::vector<float> vert,glm::vec3 pos,glm::vec3 fr,Core::RenderContext& ctxt,int hth, int amm, int mag)
{

	position = glm::vec3(pos + fr + glm::vec3(0.4, -0.1, 0));
	context = ctxt;
	health = hth;
	ammo = amm;
	mags = mag;
	score = 0;

	//calculating model dimensions
	float minx = vert[0];
	float maxx = vert[0];
	float miny = vert[1];
	float maxy = vert[1];
	float minz = vert[2];
	float maxz = vert[2];
	int counter = 1;
	for (int i = 0; i < vert.size(); i++) {
		switch (counter){
		case 1:
			if (minx > vert[i]) {
				minx = vert[i];
			}
			if (maxx < vert[i]) {
				maxx = vert[i];
			}
			counter++;
			continue;
		case 2:
			if (miny > vert[i]) {
				miny = vert[i];
			}
			if (maxy < vert[i]) {
				maxy = vert[i];
			}
			counter++;
			continue;
		case 3:
			if (minz > vert[i]) {
				minz = vert[i];
			}
			if (maxz < vert[i]) {
				maxz = vert[i];
			}
			counter = 1;
			continue;
		}
	}
	dimensions = glm::vec3(abs(maxx - minx), abs(maxy - miny), abs(maxz - minz));
}


void Core::Player::render(GLuint program,glm::vec3 color,Core::Camera cam,float yw,float ptch)
{
	updatePhysics();
	glm::mat4 shipModelMatrix =
		glm::translate(glm::vec3(position))
		* glm::rotate(glm::radians(-yw), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(-ptch), glm::vec3(1, 0, 0))
		* glm::scale(glm::vec3(0.25f));
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cam.getPosition().x, cam.getPosition().y, cam.getPosition().z);
	glm::mat4 transformation = cam.getPerspective() * cam.getView() * shipModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&shipModelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	Core::DrawContext(context);
	glUseProgram(0);

}

void Core::Player::render(GLuint program, glm::vec3 color, Core::Camera cam)
{
	updatePhysics();
	position = glm::vec3(actor->getGlobalPose().p.x, actor->getGlobalPose().p.y, actor->getGlobalPose().p.z);
	glm::mat4 shipModelMatrix =
		glm::translate(glm::vec3(position))
		* glm::toMat4(glm::quat(actor->getGlobalPose().q.w, actor->getGlobalPose().q.x, actor->getGlobalPose().q.y, actor->getGlobalPose().q.z))
		* glm::scale(glm::vec3(0.25f));
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cam.getPosition().x, cam.getPosition().y, cam.getPosition().z);
	glm::mat4 transformation = cam.getPerspective() * cam.getView() * shipModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&shipModelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	Core::DrawContext(context);
	glUseProgram(0);

}

void Core::Player::setHealth(int in)
{
	health = in;
}

int Core::Player::getHealth()
{
	return health;
}

void Core::Player::setScore(int in)
{
	score = in;
}

int Core::Player::getScore()
{
	return score;
}

float Core::Player::getScale()
{
	return scale;
}

void Core::Player::setAmmo(int in)
{
	ammo = in;
}

int Core::Player::getAmmo()
{
	return ammo;
}

bool Core::Player::shoot()
{
	if (ammo > 0) {
		ammo = ammo - 1;
		return true;
	}
	else {
		if (mags > 0) {
			ammo = 29;
			mags = mags - 1;
			return true;
		}
		else {
			return false;
		}
	}
}

bool Core::Player::getRefill()
{
	return isRefilling;
}

void Core::Player::setRefill(bool in)
{
	isRefilling = in;
}

void Core::Player::setMags(int in)
{
	mags = in;
}

float Core::Player::getRotation()
{
	return rotation;
}

int Core::Player::getMags()
{
	return mags;
}

void Core::Player::setDimensions(glm::vec3 pos)
{
	dimensions = pos;
}

glm::vec3 Core::Player::getDimensions()
{
	return dimensions;
}

void Core::Player::setDimensions(physx::PxVec3 pos)
{
	dimensions = glm::vec3(pos.x, pos.y, pos.z);
}

physx::PxVec3 Core::Player::getDimensionsPx()
{
	return physx::PxVec3(dimensions.x,dimensions.y,dimensions.z);
}

void Core::Player::setPosition(glm::vec3 pos)
{
	position = pos;
}

glm::vec3 Core::Player::getPosition()
{
	return position;
}

void Core::Player::setPosition(physx::PxVec3 pos)
{
	position = glm::vec3(pos.x, pos.y, pos.z);
}

physx::PxVec3 Core::Player::getPositionPx()
{
	return physx::PxVec3(position.x,position.y,position.z);
}

void Core::Player::setActor(physx::PxRigidDynamic* act)
{
	actor = act;
	actor->setLinearDamping(1.5f);
	actor->setAngularDamping(10.0f);
	actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
	actor->setDominanceGroup(0);
	actor->setContactReportThreshold(1000.0f);
}

physx::PxRigidDynamic* Core::Player::getActor()
{
	return actor;
}



void Core::Player::updatePhysics()
{
	
}
