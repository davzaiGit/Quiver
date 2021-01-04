#pragma once
#include "glm.hpp"
#include "objload.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
#include "PxPhysicsAPI.h"


namespace Core
{
	class Station
	{
	private:
		glm::vec3 position;
		float scale = 1.0f;
		Core::RenderContext context;
		physx::PxRigidDynamic* actor;
		physx::PxRigidDynamic* trigger;
	public:
		Station(bool);
		Station(glm::vec3 pos = glm::vec3(0, 0, 0),Core::RenderContext& ctxt = Core::RenderContext());
		void render(GLuint program,Core::Camera cam,float time);
		void renderTexture(GLuint program,GLuint tex, Core::Camera cam, float time,float rotate);
		glm::vec3 getPosition();
		void setPosition(glm::vec3 in);
		physx::PxVec3 getPositionPx();
		void setPosition(physx::PxVec3 in);
		float getScale();
		void setScale(float in);
		void setActor(physx::PxRigidDynamic* act);
		physx::PxRigidDynamic* getActor();
		void setTrigger(physx::PxRigidDynamic* act);
		physx::PxRigidDynamic* getTrigger();
		void updatePhysics(float time);
	};
}
