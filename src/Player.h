#pragma once
#include "glm.hpp"
#include "objload.h"
#include "Render_Utils.h"
#include "Camera.h"
#include <PxPhysicsAPI.h>
#include <string>


namespace Core
{
	class Player
	{
	private:
		int health;
		int score;
		int ammo;
		int mags;
		float scale = 0.3f;
		glm::vec3 position;
		glm::vec3 dimensions;
		Core::RenderContext context;
		physx::PxRigidDynamic* actor;
		float rotation;
		float lastRotation; //Helper variable, not useful;
		bool isRefilling = false;
	public:
		Player();
		Player(std::vector<float> vert,glm::vec3 pos = glm::vec3(0, 0, 0),glm::vec3 fr = glm::vec3(0, 0, 0),Core::RenderContext& ctxt = Core::RenderContext(),int health = 100, int ammo = 30, int mags = 3);
		void render(GLuint program, glm::vec3 color, Core::Camera cam, float yaw, float pitch);
		void render(GLuint program, glm::vec3 color, Core::Camera cam);
		void setHealth(int in);
		int getHealth();
		void setScore(int in);
		int getScore();
		float getScale();
		void setAmmo(int in);
		int getAmmo();
		bool shoot();
		bool getRefill();
		void setRefill(bool in);
		void setMags(int in);
		float getRotation();
		int getMags();
		void setDimensions(glm::vec3 pos);
		glm::vec3 getDimensions();
		void setDimensions(physx::PxVec3 pos);
		physx::PxVec3 getDimensionsPx();
		void setPosition(glm::vec3 pos);
		glm::vec3 getPosition();
		void setPosition(physx::PxVec3 pos);
		physx::PxVec3 getPositionPx();
		void setActor(physx::PxRigidDynamic* act);
		physx::PxRigidDynamic* getActor();
		void updatePhysics();
	};
}


