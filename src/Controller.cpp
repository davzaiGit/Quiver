#include "Controller.h"
#include "Camera.h"
#include <ext.hpp>

Core::Controller::Controller(float angSp,float mvSp,float yw, float ptch, int lasX, int lasY, bool firstM, glm::vec3 dir)
{
	angleSpeed = angSp;
	moveSpeed = mvSp;
	yaw = yw;
	pitch = ptch;
	lastX = lasX;
	lastY = lasY;
	firstMouse = firstM;
	direction = dir;

}


float Core::Controller::getYaw()
{
	return yaw;
}

void Core::Controller::setYaw(float yw)
{
	yaw = yw;
}

float Core::Controller::getPitch()
{
	return pitch;
}

void Core::Controller::setPitch(float ptch)
{
	pitch = ptch;
}

float Core::Controller::getAngleSpeed()
{
	return angleSpeed;
}

void Core::Controller::setAngleSpeed(float angSp)
{
	angleSpeed = angSp;
}

float Core::Controller::getMoveSpeed()
{
	return moveSpeed;
}

void Core::Controller::setMoveSpeed(float mvSp)
{
	moveSpeed = mvSp;
}

int Core::Controller::getLastX()
{
	return lastX;
}

void Core::Controller::setLastX(int lasX)
{
	lastX = lasX;
}

int Core::Controller::getLastY()
{
	return lastY;
}

void Core::Controller::setLastY(int lasY)
{
	lastY = lasY;
}

bool Core::Controller::getFirstMouse()
{
	return firstMouse;
}

void Core::Controller::setFirstMouse(bool firstM)
{
	firstMouse = firstM;
}

glm::vec3 Core::Controller::getDirection()
{
	return direction;
}

void Core::Controller::setDirection(glm::vec3 dir)
{
	direction = dir;
}
