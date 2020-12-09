#include "Player.h"




Core::Player::Player(std::string path,int hth, int amm, int mag)
{
	model = obj::loadModelFromFile(path);
	context.initFromOBJ(model);
	health = hth;
	ammo = amm;
	mags = mag;
}

void Core::Player::render(GLuint program,glm::vec3 color,Core::Camera cam,float yw,float ptch) {
	glm::mat4 shipModelMatrix =
		glm::translate(glm::vec3(cam.getPosition() + cam.getFront() + glm::vec3(0.4, -0.1, 0)))
		* glm::rotate((glm::radians(-yw) + glm::radians(90.f)), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(-ptch), glm::vec3(1, 0, 0))
		* glm::scale(glm::vec3(0.25f));
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cam.getPosition().x, cam.getPosition().y, cam.getPosition().z);
	glm::mat4 transformation = cam.getPerspective() * cam.getView() * shipModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&shipModelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	Core::DrawContext(context);
	glUseProgram(0);

}