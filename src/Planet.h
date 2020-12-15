#pragma once
#include "glm.hpp"
#include "objload.h"
#include "Render_Utils.h"
#include "Camera.h"


namespace Core
{
	class Planet
	{
	private:
		glm::vec3 position;
		glm::vec3 dimensions;
		glm::vec3 color;
		float distance;
		float moonDistance;
		float scale;
		Core::RenderContext context;
	public:

		Planet(glm::vec3 col = glm::vec3(0,0,0),glm::vec3 pos = glm::vec3(0, 0, 0), float distance = 0.0f,float sc = 1.0f,float md = 0.0f, Core::RenderContext& ctxt = Core::RenderContext());
		void render(GLuint program,Core::Camera cam,float time);
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
	};
}
