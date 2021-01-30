#pragma once
#include "glm.hpp"
#include "objload.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
#include "PxPhysicsAPI.h"


namespace Core
{
	class Planet
	{
	private:
		glm::vec3 origin;
		glm::vec3 position;
		glm::vec3 dimensions;
		glm::vec3 color;
		
		float positionOffsetAngle;
		float distance;
		float moonDistance;
		float scale;
		float speed;
		Core::RenderContext context;
		physx::PxRigidDynamic* actor;
	public:
		std::string type;
		Planet(bool);
		Planet(glm::vec3 pos = glm::vec3(0, 0, 0),glm::vec3 col = glm::vec3(0, 0, 0),float sc = 1.0f,Core::RenderContext& ctxt = Core::RenderContext());
		Planet(glm::vec3 col = glm::vec3(0, 0, 0), glm::vec3 pos = glm::vec3(0, 0, 0),float poa = 0.0f, float sp = 1.0f, float distance = 0.0f, float sc = 1.0f, float md = 0.0f, Core::RenderContext& ctxt = Core::RenderContext());
		void render(GLuint program,Core::Camera cam,float time);
		void renderTexture(GLuint program,GLuint tex, Core::Camera cam, float time,float rotate);
		void renderTextureMoon(GLuint program, GLuint tex, Core::Camera cam, float time, float rotate);
		void renderSkybox(GLuint program, GLuint tex, Core::Camera cam, float time, float rotate);
		glm::vec3 getPosition();
		void setPosition(glm::vec3 in);
		physx::PxVec3 getPositionPx();
		void setPosition(physx::PxVec3 in);
		float getDistance();
		void setDistance(float in);
		float getMoonDistance();
		void setMoonDistance(float in);
		float getScale();
		void setScale(float in);
		void setActor(physx::PxRigidDynamic* act);
		physx::PxRigidDynamic* getActor();
		void updatePhysics(float time);
	};
}
