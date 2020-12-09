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
		Core::RenderContext context;
	public:
		
		Player(glm::vec3 pos = glm::vec3(0, 0, 0),glm::vec3 fr = glm::vec3(0, 0, 0),Core::RenderContext ctxt = Core::RenderContext(),int health = 100, int ammo = 30, int mags = 3);
		void render(GLuint program, glm::vec3 color, Core::Camera cam, float yaw, float pitch);
	};
}


