#pragma once
#include "glm.hpp"
#include "objload.h"
#include "Render_Utils.h"
#include "Camera.h"


namespace Core
{
	class Player
	{
	private:
		int health;
		int ammo;
		int mags;
		glm::vec3 position;
		glm::vec3 dimensions;
		Core::RenderContext context;
	public:
		
		Player();
		Player(std::vector<float> vert,glm::vec3 pos = glm::vec3(0, 0, 0),glm::vec3 fr = glm::vec3(0, 0, 0),Core::RenderContext& ctxt = Core::RenderContext(),int health = 100, int ammo = 30, int mags = 3);
		void render(GLuint program, glm::vec3 color, Core::Camera cam, float yaw, float pitch);
		void setHealth(int in);
		int getHealth();
		void setAmmo(int in);
		int getAmmo();
		void setMags(int in);
		int getMags();
		void setDimensions(glm::vec3 pos);
		glm::vec3 getDimensions();
		void setDimensions(physx::PxVec3 pos);
		physx::PxVec3 getDimensionsPx();
		void setPosition(glm::vec3 pos);
		glm::vec3 getPosition();
		void setPosition(physx::PxVec3 pos);
		physx::PxVec3 getPositionPx();
	};
}


