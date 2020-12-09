#include "Controller.h"
#include "src/Camera.h"
#include <ext.hpp>

void Core::Controller::keyboard(Coreunsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch (key)
	{
		//case 'z': cameraAngle -= angleSpeed; break;
		//case 'x': cameraAngle += angleSpeed; break;
	case 'w': cam.setPosition(cam.getPosition() + direction * moveSpeed); break;
	case 's': cam.setPosition(cam.getPosition() - direction * moveSpeed); break;
	case 'd': cam.setPosition(cam.getPosition() + (glm::cross(cam.getForward(), glm::vec3(0, 1, 0)) * moveSpeed)); break;
	case 'a': cam.setPosition(cam.getPosition() - (glm::cross(cam.getForward(), glm::vec3(0, 1, 0)) * moveSpeed)); break;
	case 'e': cam.setPosition(cam.getPosition() + (glm::cross(cam.getForward(), glm::vec3(1, 0, 0)) * moveSpeed)); break;
	case 'q': cam.setPosition(cam.getPosition() - (glm::cross(cam.getForward(), glm::vec3(1, 0, 0)) * moveSpeed)); break;
	}
}