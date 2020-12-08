#pragma once

#include "glm.hpp"

namespace Core
{
	class Camera 
	{
		private:
			glm::vec3 position;
			glm::vec3 forward;
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
			glm::mat4 perspective;
			glm::mat4 view;
			glm::vec3 front;
		public:
			Camera(glm::vec3 pos = glm::vec3(0,0,0), glm::vec3 front = glm::vec3(0,0,0));
			void update();
			// position - pozycja kamery
			// forward - wektor "do przodu" kamery (jednostkowy)
			// up - wektor "w gore" kamery (jednostkowy)
			// up i forward musza byc ortogonalne!
			void createViewMatrix();
			glm::vec3 getPosition();
			void setPosition(glm::vec3 pos);
			glm::vec3 getForward();
			void setForward(glm::vec3 fwd);
			glm::vec3 getFront();
			void setFront(glm::vec3 fr);
			glm::mat4 getPerspective();
			void setPerspective(glm::mat4 per);
			glm::mat4 getView();
			void setView(glm::mat4 vw);
	};
}