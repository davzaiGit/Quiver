#pragma once
#include "glm.hpp"
#include "objload.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
#include "PxPhysicsAPI.h"


namespace Core
{
	class Asteroid
	{
	private:
		glm::vec3 position;
		glm::vec3 dimensions;
		glm::vec3 color;
		float scale;
		float speed;
		Core::RenderContext context;
		physx::PxRigidDynamic* actor;
	public:
		bool destroyed = false;
		Asteroid(bool);
		Asteroid(glm::vec3 pos = glm::vec3(0, 0, 0),glm::vec3 col = glm::vec3(0, 0, 0),float sc = 1.0f,Core::RenderContext& ctxt = Core::RenderContext());
		void render(GLuint program,Core::Camera cam,float time);
		void renderTexture(GLuint program, GLuint tex, GLuint normalmapId, Core::Camera cam, float time, float rotate);
		glm::vec3 getPosition();
		void setPosition(glm::vec3 in);
		physx::PxVec3 getPositionPx();
		void setPosition(physx::PxVec3 in);
		float getScale();
		void setScale(float in);
		void releaseActor();
		void setActor(physx::PxRigidDynamic* act);
		physx::PxRigidDynamic* getActor();
	};
}
