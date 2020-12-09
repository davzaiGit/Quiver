#pragma once
#include "glm.hpp"
#include "Camera.h"

namespace Core {

	class Controller

	{

		private:

			float angleSpeed;
			float moveSpeed;
			float yaw;
			float pitch;
			int lastX;
			int lastY;
			bool firstMouse;
			glm::vec3 direction;

		public:

			Controller(float angSp = 0.1f, float mvSp = 0.1f, float yw = 0.0f, float ptch = 0.0f, int lasX = 640, int lasY = 360, bool firstM = true, glm::vec3 dir = glm::vec3(0, 0, 0));

			float getYaw();
			void setYaw(float yw);

			float getPitch();
			void setPitch(float ptch);

			float getAngleSpeed();
			void setAngleSpeed(float angSp);

			float getMoveSpeed();
			void setMoveSpeed(float mvSp);

			int getLastX();
			void setLastX(int lasX);

			int getLastY();
			void setLastY(int lasY);

			bool getFirstMouse();
			void setFirstMouse(bool firstM);

			glm::vec3 getDirection();
			void setDirection(glm::vec3 dir);

	};

}

