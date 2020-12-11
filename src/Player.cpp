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

	position = glm::vec3(cam.getPosition() + cam.getFront() + glm::vec3(0.4, -0.1, 0));
	glm::mat4 shipModelMatrix =
		glm::translate(glm::vec3(position))
		* glm::rotate((glm::radians(-yw) + glm::radians(90.f)), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(-ptch), glm::vec3(1, 0, 0))
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

void Core::Player::setAmmo(int in)
{
	ammo = in;
}

int Core::Player::getAmmo()
{
	return ammo;
}

void Core::Player::setMags(int in)
{
	mags = in;
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
