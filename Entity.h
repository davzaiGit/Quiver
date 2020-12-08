#pragma once

#include "glm.hpp"
#include "src/objload.h"
#include <string>

class Entity
{
	private:
		glm::vec3 position;
		std::string modelPath;
		std::string texturePath;
		obj::Model model;

	public:
		void updatePhysics();
		void render();

};

