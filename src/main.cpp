#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <ctype.h>
#include <math.h>
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
#include "Asteroid.h"
#include "Bullet.h"
#include "Controller.h"
#include "PxPhysicsAPI.h"
#include "Player.h"
#include "Planet.h"
#include "Sun.h"
#include "Station.h"
#include <ft2build.h>
#include <list>
#include FT_FREETYPE_H

#define PVD_HOST "127.0.0.1"
#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}


//OpenGL inits

GLuint program;
GLuint sunProgram;
GLuint skyboxProgram;
GLuint textProgram;
GLuint programTex;
GLuint programTex2;
GLuint screenProgram;
GLuint blurProgram;
GLuint bloomFinalProgram;
GLuint lightProgram;
unsigned int quadVAO = 0;
unsigned int quadVBO;
unsigned int textureColorbuffer;
unsigned int FBO;
unsigned int rbo;
unsigned int colorBuffers[2];
unsigned int pingpongFBO[2];
unsigned int pingpongColorbuffers[2];
unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
GLuint skyboxTex;
GLuint sunTex;
GLuint sunTexHdr;
GLuint hudTex;
GLuint healthTex;
GLuint ammoTex;
GLuint spaceShipTex;

GLuint planet2Tex;
GLuint planet3Tex;
GLuint planet4Tex;
GLuint planet5Tex;
GLuint planet6Tex;
GLuint planet7Tex;
GLuint asteroidTex;
GLuint asteroidTex_normal;

// GLuint planet8Tex;

GLuint spaceStationTex;

Core::Shader_Loader shaderLoader;
obj::Model sphereModel;
obj::Model shipModel;
obj::Model skyBoxModel;
obj::Model asteroidModel;
obj::Model stationModel;

physx::PxConvexMesh* sphereMesh;
physx::PxConvexMesh* shipMesh;
physx::PxConvexMesh* asteroidMesh;
physx::PxConvexMesh* stationMesh;


Core::RenderContext sphereContext;
Core::RenderContext shipContext;
Core::RenderContext skyBoxContext;
Core::RenderContext asteroidContext;
Core::RenderContext stationContext;
Core::RenderContext sunContext;
Core::TextContext textContext;

int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

//Entity intialization
Core::Camera cam;
Core::Player player;
Core::Station station;
Core::Planet skybox = Core::Planet(true);
Core::Sun sun = Core::Sun(true);
Core::Planet planet2 = Core::Planet(true);
Core::Planet planet3 = Core::Planet(true);
Core::Planet planet4 = Core::Planet(true);
Core::Planet planet5 = Core::Planet(true);
Core::Planet planet6 = Core::Planet(true);
Core::Planet planet7 = Core::Planet(true);
Core::Planet planet8 = Core::Planet(true);
Core::Controller controller;

std::list<Core::Asteroid> asteroids;
std::list<Core::Bullet> bullets;


//PhysX inits
static physx::PxDefaultErrorCallback gErrorCallback;
static physx::PxDefaultAllocator gAllocator;
physx::PxFoundation* gFoundation = NULL;
physx::PxPhysics* gPhysics = NULL;
physx::PxCooking* gCooking = NULL;
physx::PxPvd* gPvd = NULL;
physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
physx::PxScene* gScene = NULL;
physx::PxMaterial* gMaterial = NULL;
bool recordMemoryAllocations = true;
physx::PxRigidDynamic* shipActor;
physx::PxRigidDynamic* sunActor;

physx::PxFilterFlags contactReportFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags & pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	if ((filterData0.word0 == 2 && filterData1.word0 == 1)||(filterData0.word0 == 1 && filterData1.word0 == 2)) {

		return physx::PxFilterFlag::eKILL;
	}
	if ((physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1)) && (filterData0.word0 == 2 || filterData1.word0 == 2))
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
		return physx::PxFilterFlag::eDEFAULT;
	}
	pairFlags = physx::PxPairFlag::eSOLVE_CONTACT | physx::PxPairFlag::eDETECT_DISCRETE_CONTACT
		| physx::PxPairFlag::eNOTIFY_THRESHOLD_FORCE_FOUND
		| physx::PxPairFlag::eNOTIFY_THRESHOLD_FORCE_PERSISTS
		| physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
	return physx::PxFilterFlag::eDEFAULT;
}

class ContactReportCallback : public physx::PxSimulationEventCallback
{
	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
	void onWake(physx::PxActor** actors, physx::PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onSleep(physx::PxActor** actors, physx::PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) { 
		if (pairs->status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
			player.setRefill(true);
		}
		else if (pairs->status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
			player.setRefill(false);
		}
	}
	void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) {}
	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
	{
		
		if (pairHeader.actors[0]->getDominanceGroup() == 3 || pairHeader.actors[1]->getDominanceGroup() == 3) {
			physx::PxActor* firstActor = (pairHeader.actors[0]->getDominanceGroup() == 3) ?
				pairHeader.actors[0] : pairHeader.actors[1];
			physx::PxActor* otherActor = (pairHeader.actors[0]->getDominanceGroup() == 3) ?
				pairHeader.actors[1] : pairHeader.actors[0];
			if (otherActor->getDominanceGroup() == 2) {
				player.setScore(player.getScore() + 300);
				for (std::list<Core::Asteroid>::iterator iter = asteroids.begin(); iter != asteroids.end(); iter++) {
					if (iter->getActor() == otherActor) {
						iter->destroyed = true;
						break;
					}
				}
			}
			for (std::list<Core::Bullet>::iterator iter = bullets.begin(); iter != bullets.end(); iter++) {
				if (iter->getActor() == firstActor) {
					iter->destroyed = true;
					break;
				}
			}
		}
		else if (pairHeader.actors[0]->getDominanceGroup() == 0 || pairHeader.actors[1]->getDominanceGroup() == 0) {
			physx::PxActor* otherActor = (pairHeader.actors[0]->getDominanceGroup() == 0) ?
				pairHeader.actors[1] : pairHeader.actors[0];
			physx::PxActor* firstActor = (pairHeader.actors[0]->getDominanceGroup() == 0) ?
				pairHeader.actors[0] : pairHeader.actors[1];
			switch (otherActor->getDominanceGroup())
			{
			case 2:
				player.setHealth(player.getHealth() - 0.01 * abs(glm::dot(Core::PxVec3ToGlm(reinterpret_cast<physx::PxRigidDynamic*>(otherActor)->getLinearVelocity()), Core::PxVec3ToGlm(player.getActor()->getLinearVelocity())))); break;
			default:
				player.setHealth(player.getHealth() - 0.3 * sqrt(pow(player.getActor()->getLinearVelocity().x, 2) + pow(player.getActor()->getLinearVelocity().y, 2) + pow(player.getActor()->getLinearVelocity().z, 2))); break;
			}
		}
		
	}
};

ContactReportCallback gContactReportCallback;

//light source position
glm::vec3 lightSource = glm::normalize(glm::vec3(0.0f, 0.0f, 3.0f));

void keyboard(unsigned char key, int x, int y)
{
	int mod = glutGetModifiers();
	if (mod == 1) {
		switch (key){
			case 'W': player.getActor()->setLinearVelocity(player.getActor()->getLinearVelocity() + physx::PxVec3(0, 1, 0) * 2.0); break;
			case 'S': player.getActor()->setLinearVelocity(player.getActor()->getLinearVelocity() - physx::PxVec3(0, 1, 0) * 2.0); break;
		
		}
		
	}
	switch (key)
	{
	case 'w': player.getActor()->setLinearVelocity(player.getActor()->getLinearVelocity() + Core::GlmToPxVec3(cam.getFront()) * 2.0f);break;
	case 's': player.getActor()->setLinearVelocity(player.getActor()->getLinearVelocity() - Core::GlmToPxVec3(cam.getFront()) * 2.0f); break;
	case 'd': player.getActor()->setLinearVelocity(player.getActor()->getLinearVelocity() + Core::GlmToPxVec3(glm::cross(cam.getFront(), glm::vec3(0.0f, 1.0f, 0.0f))) * 2.0f); break;
	case 'a': player.getActor()->setLinearVelocity(player.getActor()->getLinearVelocity() - Core::GlmToPxVec3(glm::cross(cam.getFront(), glm::vec3(0.0f, 1.0f, 0.0f))) * 2.0f); break;
	case 'q': player.getActor()->setAngularVelocity(physx::PxVec3(0,1,0) * 3.0); break;
	case 'e': player.getActor()->setAngularVelocity(physx::PxVec3(0, -1, 0) * 3.0); break;
	}
}


void mouse_callback(int x, int y)
{
	if (controller.getFirstMouse())
	{
		controller.setLastX(x);
		controller.setLastY(y);
		controller.setFirstMouse(false);
	}
	float xoffset = x - controller.getLastX();
	float yoffset = controller.getLastY() - y;
	controller.setLastX(x);
	controller.setLastY(y);
	float sensitivity = 0.9f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	controller.setYaw(controller.getYaw() + xoffset);
	controller.setPitch(controller.getPitch() + yoffset);
	if (x <= 50 || y <= 50 || x >= 1230 || y >= 670) {
		controller.setFirstMouse(true);
		glutWarpPointer(640, 360);
	}
	if (controller.getPitch() > 89.0f)
		controller.setPitch(89.0f);
	if (controller.getPitch() < -89.0f)
		controller.setPitch(-89.0f);
	controller.setDirection(glm::vec3(
		cos(glm::radians(controller.getYaw())) * cos(glm::radians(controller.getPitch())),
		sin(glm::radians(controller.getPitch())),
		sin(glm::radians(controller.getYaw())) * cos(glm::radians(controller.getPitch())))
	);
	cam.setFront(glm::normalize(controller.getDirection()));
}


//PhysX related functions

void initPhysics(bool interactive)
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, physx::PxCookingParams(physx::PxTolerancesScale()));
	physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, 0.0f);
	gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = &gContactReportCallback;
	gScene = gPhysics->createScene(sceneDesc);
	gScene->setDominanceGroupPair(0, 1, physx::PxDominanceGroupPair(1, 0));
	gScene->setDominanceGroupPair(0, 2, physx::PxDominanceGroupPair(0, 1));
	gScene->setDominanceGroupPair(1, 2, physx::PxDominanceGroupPair(0, 1));
	gScene->setDominanceGroupPair(2, 3, physx::PxDominanceGroupPair(1, 1));
	gScene->setDominanceGroupPair(3, 1, physx::PxDominanceGroupPair(1, 0));
	gScene->setDominanceGroupPair(3, 0, physx::PxDominanceGroupPair(1, 0));
	gScene->setDominanceGroupPair(4, 0, physx::PxDominanceGroupPair(1, 1));
	physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

}
physx::PxConvexMesh* cookMesh(obj::Model mod) {
	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count = mod.vertex.size() / 3;
	convexDesc.points.stride = 3 * sizeof(float);
	convexDesc.points.data = &mod.vertex[0];
	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
	gCooking->cookConvexMesh(convexDesc, buf, &result);
	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	physx::PxConvexMesh* convexMesh = gPhysics->createConvexMesh(input);
	return convexMesh;

}

physx::PxRigidDynamic* createDynamic(const physx::PxTransform& t, physx::PxConvexMesh* convexMesh, physx::PxReal scale = 1.0f, physx::PxU32 flag = 0, bool trigger = false, const physx::PxVec3& velocity = physx::PxVec3(0))
{
	physx::PxRigidDynamic* dynamic = gPhysics->createRigidDynamic(t);
	physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*dynamic,physx::PxConvexMeshGeometry(convexMesh, physx::PxMeshScale(scale)), *gMaterial);
	shape->setSimulationFilterData(physx::PxFilterData(flag, 0, 0, 0));
	if (trigger) {
		shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
	}
	shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, trigger);
	
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}

int lastRefill = 0;

void stepPhysics(bool,float time)
{
	gScene->simulate(1.0f / 60.0f);
	gScene->fetchResults(true);
	player.setPosition(player.getActor()->getGlobalPose().p);
	if (player.getRefill() && lastRefill != (int)floor(time)) {
		if (player.getHealth() < 100) {
			player.setHealth(player.getHealth() + 1);
		}
		if (player.getMags() < 3) {
			player.setMags(player.getMags() + 1);
		}
		lastRefill = (int)floor(time);
	}
	for (std::list<Core::Bullet>::iterator iter = bullets.begin(); iter != bullets.end(); iter++) {
		if (iter->destroyed) {
			iter->releaseActor();
			if (iter == bullets.end()) {
				bullets.pop_back();
				break;
			}
			else {
				iter = bullets.erase(iter);
				break;
			}
		}

	}
	for (std::list<Core::Asteroid>::iterator iter = asteroids.begin(); iter != asteroids.end(); iter++) {
		if (iter->destroyed) {
			iter->releaseActor();
			if (iter == asteroids.end()) {
				bullets.pop_back();
				break;
			}
			else {
				iter = asteroids.erase(iter);
				break;
			}
		}
	}
	while (asteroids.size() > 150) {
		asteroids.back().releaseActor();
		asteroids.pop_back();
	}
	while (bullets.size() > 10) {
		bullets.back().releaseActor();
		bullets.pop_back();
	}
	cam.setPosition(player.getPosition() + controller.getDirection() * -1.f + glm::vec3(0, 0.25f, 0));
	skybox.setPosition(player.getPosition());
	cam.update();
}

void mouse_clicks(int button, int state, int x, int y) 
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && player.shoot())
	{
		Core::Bullet bul = Core::Bullet(player.getPosition(), glm::vec3(1, 1, 1), 0.30f, sphereContext);
		bul.setActor(createDynamic(physx::PxTransform(bul.getPositionPx()), sphereMesh, bul.getScale(), physx::PxU32(1), false, Core::GlmToPxVec3(controller.getDirection() * 800.0f)));
		bullets.push_front(bul);
	}
}

void cleanupPhysics(bool)
{
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		physx::PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
}

int lastTime = 0;

void spawnAsteroids(float time) {
	if ((int)floor(time) % 5 == 0 && lastTime != (int)floor(time)) {
		lastTime = (int)floor(time);
		for (int i = 0; i < 20; i++) {
			float randVal1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float randVal2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float randVal3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float delta = acos(2 * randVal1 - 1) - (3.141593 / 2);
			float phi = 2 * 3.141593 * randVal2;
			glm::vec3 pos = player.getPosition() + glm::vec3(5000 * cosf(delta) * cosf(phi), 5000 * cosf(delta) * sinf(phi), 5000 * sinf(delta));
			Core::Asteroid ast = Core::Asteroid(pos, glm::vec3(1, 1, 1), 0.005f, asteroidContext);
			ast.setActor(createDynamic(physx::PxTransform(ast.getPositionPx()), asteroidMesh, ast.getScale(), 0, false,(player.getPositionPx() - ast.getPositionPx() + physx::PxVec3(randVal1, randVal2, randVal3) * randVal1 * 400.f) * (0.15 + randVal3 / 4)));
			asteroids.push_front(ast);
		}
	}
}

void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}



void renderScene()
{

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	stepPhysics(true,time);
	spawnAsteroids(time);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glUseProgram(program); 
	glUniform3f(glGetUniformLocation(program, "lightPos"),2,0,2);
	glUniform3f(glGetUniformLocation(program, "lightColor"), 0.0f, 0.0f, 15.0f);
	//Player drawing
	player.render(programTex, spaceShipTex, glm::vec3(0.6f), cam);
	
	//Planet drawing
	skybox.renderSkybox(skyboxProgram, skyboxTex, cam, time,0.0f);

	sun.renderTexture(sunProgram, sunTex, sunTexHdr, cam, time,55.0f);
	
	// Planets
	planet2.renderTexture(programTex, planet2Tex, cam, time, 0.0f);
	planet3.renderTexture(programTex, planet3Tex, cam, time, 0.0f);
	planet4.renderTexture(programTex, planet4Tex, cam, time, 0.0f);
	planet5.renderTexture(programTex, planet5Tex, cam, time, 0.0f);
	planet6.renderTexture(programTex, planet6Tex, cam, time, 0.0f);
	planet7.renderTexture(programTex, planet7Tex, cam, time, 0.0f);
	// planet8.renderTextureMoon(programTex, planet7Tex, cam, time, 0.0f);

	station.renderTexture(programTex, spaceStationTex, cam, time, 0.0f);
	
for (std::list<Core::Asteroid>::iterator iter = asteroids.begin(); iter != asteroids.end(); iter++) {
		iter->renderTexture(programTex2, asteroidTex, asteroidTex_normal, cam, time, 0.0f);
	}

	for (std::list<Core::Bullet>::iterator iter = bullets.begin(); iter != bullets.end(); iter++) {
		iter->render(program, cam, time);
	}

	//HUD elements rendering
	Core::RenderText(textContext,textProgram, std::string(std::to_string(player.getHealth())), 75.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
	Core::RenderText(textContext, textProgram, "Score: " + std::string(std::to_string(player.getScore())), 20.0f, 670.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
	Core::RenderText(textContext, textProgram, std::string(std::to_string(player.getAmmo())) + " / "  + std::string(std::to_string(player.getMags() * 30)), 1130.0f,25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
	Core::RenderHud(textContext, textProgram, hudTex, 620, 340, 40.0f,glm::vec3(1.f,1.f,1.f));
	Core::RenderHud(textContext, textProgram, healthTex, 25, 22, 35.0f, glm::vec3(0.5, 0.8f, 0.2f));
	Core::RenderHud(textContext, textProgram, ammoTex, 1080, 22, 35.0f, glm::vec3(0.5, 0.8f, 0.2f));

	//bloom light render
	glUseProgram(lightProgram);
	glm::mat4 projection = cam.getPerspective();
	glm::mat4 view = cam.getView();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(lightSource));
	model = glm::scale(model, glm::vec3(0.25f));
	glUniformMatrix4fv(glGetUniformLocation(lightProgram, "projection"), sizeof(projection), GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(lightProgram, "view"), sizeof(view), GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(lightProgram, "model"), sizeof(lightSource), GL_FALSE, &model[0][0]);

	//TODO: fix model uniforms to program
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), sizeof(projection), GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), sizeof(view), GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), sizeof(lightSource), GL_FALSE, &model[0][0]);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	bool horizontal = true;
	bool first_iteration = true;
	unsigned int amount = 10;
	glUseProgram(blurProgram);
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		glUniform1i(glGetUniformLocation(blurProgram, "horizontal"), horizontal);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
		renderQuad();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(bloomFinalProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
	glUniform1i(glGetUniformLocation(bloomFinalProgram, "bloom"), 1);
	glUniform1f(glGetUniformLocation(bloomFinalProgram, "exposure"), 1);
	
	renderQuad();
	glutSwapBuffers();
}


void init()
{
	glEnable(GL_DEPTH_TEST);

	blurProgram = shaderLoader.CreateProgram("shaders/blur_shader.vert", "shaders/blur_shader.frag");
	program = shaderLoader.CreateProgram("shaders/main_shader.vert", "shaders/main_shader.frag");
	lightProgram = shaderLoader.CreateProgram("shaders/shader_sun.vert", "shaders/lightBox_shader.frag");
	sunProgram = shaderLoader.CreateProgram("shaders/shader_sun.vert", "shaders/shader_sun.frag");
	skyboxProgram = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");
	textProgram = shaderLoader.CreateProgram("shaders/shader_text.vert", "shaders/shader_text.frag");
	bloomFinalProgram = shaderLoader.CreateProgram("shaders/shader_bloom.vert", "shaders/shader_bloom.frag");
	programTex = shaderLoader.CreateProgram("shaders/shader_texture.vert", "shaders/shader_texture.frag");
	programTex2 = shaderLoader.CreateProgram("shaders/shader_tex2.vert", "shaders/shader_tex2.frag");


	//testy program
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "diffuseTexture"), 0);


	glUseProgram(blurProgram);
	glUniform1i(glGetUniformLocation(blurProgram, "image"), 0);

	glUseProgram(bloomFinalProgram);
	glUniform1i(glGetUniformLocation(bloomFinalProgram, "scene"), 0);
	glUniform1i(glGetUniformLocation(bloomFinalProgram, "bloomBlur"), 1);

	//creating framebuffer
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}

	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	glDrawBuffers(2, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping-pong-framebuffer for blurring
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);




	//Camera object loading
	cam = Core::Camera(glm::vec3(-103, 0, 0), glm::vec3(0, 0, 0));
	controller = Core::Controller();
	textContext.initText();


	//Asset loading

	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	asteroidModel = obj::loadModelFromFile("models/asteroid.obj");
	skyBoxModel = obj::loadModelFromFile("models/skyBoxCube.obj");
	stationModel = obj::loadModelFromFile("models/spacestation.obj");

	skyboxTex = Core::LoadTextureToFramebuffer("textures/Cube2.png");
	sunTex = Core::LoadTextureToFramebuffer("textures/papa.png");
	hudTex = Core::LoadTextureToFramebuffer("textures/kek.png");
	healthTex = Core::LoadTextureToFramebuffer("textures/health.png");
	ammoTex = Core::LoadTextureToFramebuffer("textures/ammo.png");
	spaceShipTex = Core::LoadTextureToFramebuffer("textures/spaceship.png");
	asteroidTex = Core::LoadTextureToFramebuffer("textures/asteroid.png");
	asteroidTex_normal = Core::LoadTextureToFramebuffer("textures/asteroid_normals.png");
	planet2Tex = Core::LoadTextureToFramebuffer("textures/mars8k.png");
	planet3Tex = Core::LoadTextureToFramebuffer("textures/haumea.png");
	planet4Tex = Core::LoadTextureToFramebuffer("textures/jupiter8k.png");
	planet5Tex = Core::LoadTextureToFramebuffer("textures/karsaw8k.png");
	planet6Tex = Core::LoadTextureToFramebuffer("textures/neptune2k.png");
	planet7Tex = Core::LoadTextureToFramebuffer("textures/saturn8k.png");
	spaceStationTex = Core::LoadTextureToFramebuffer("textures/spaceStation.png");
	
	sunTexHdr = Core::LoadTextureToFramebuffer("textures/papa.png");

	sphereContext.initFromOBJ(sphereModel);
	skyBoxContext.initFromOBJ(skyBoxModel);
	shipContext.initFromOBJ(shipModel);
	asteroidContext.initFromOBJ(asteroidModel);
	stationContext.initFromOBJ(stationModel);
	sunContext.initFromOBJ(sphereModel);


	//Object instantialization
	player = Core::Player(shipModel.vertex, cam.getPosition(), cam.getFront(), shipContext, 100, 30, 3);
	skybox = Core::Planet(player.getPosition(), glm::vec3(1.0f, 0.0f, 0.0f), 10000.0f, skyBoxContext);
	station = Core::Station(glm::vec3(80, 0, -80), stationContext);
//Suninit
	sun = Core::Sun(glm::vec3(1.0f, 0.5f, 0.2f), glm::vec3(2, 0, 2),0.0f, 1.0f, 0.0f, 30.0f, 0.0f, sunContext);
	planet2 = Core::Planet(glm::vec3(0.0f, 0.6f, 0.9f), glm::vec3(2, 0, 2),0.3f, 0.01f, 123.0f, 15.0f, 13.0f, sphereContext);
	planet3 = Core::Planet(glm::vec3(0.0f, 0.5f, 0.1f), glm::vec3(2, 0, 2),1.1f, 0.01f,203.0f, 10.0f, 0.0f, sphereContext);
	planet4 = Core::Planet(glm::vec3(0.5f, 0.1f, 0.3f), glm::vec3(2, 0, 2),3.2f, 0.01f,293.0f, 17.0f, 0.0f, sphereContext);
	planet5 = Core::Planet(glm::vec3(0.9f, 0.9f, 0.7f), glm::vec3(2, 0, 2),5.1f, 0.01f, 373.0f, 24.0f, 0.0f, sphereContext);
	planet6 = Core::Planet(glm::vec3(0.4f, 0.2f, 0.3f), glm::vec3(2, 0, 2), 4.0f, 0.01f, 421.0f, 18.0f, 0.0f, sphereContext);
	planet7 = Core::Planet(glm::vec3(0.1f, 0.0f, 0.5f), glm::vec3(2, 0, 2), 1.7f, 0.01f, 507.0f, 25.0f, 0.0f, sphereContext);
// planet8 = Core::Planet(glm::vec3(0.1f, 0.0f, 0.5f), glm::vec3(2, 0, 2), 0.3f, 0.5f, 110.0f, 3.0f, 0.0f, sphereContext);

	//Physics inits
	initPhysics(true);
	shipMesh = cookMesh(shipModel);
	sphereMesh = cookMesh(sphereModel);
	asteroidMesh = cookMesh(asteroidModel);
	stationMesh = cookMesh(stationModel);

	station.setActor(createDynamic(physx::PxTransform(station.getPositionPx()), stationMesh, station.getScale()));
	station.setTrigger(createDynamic(physx::PxTransform(station.getPositionPx()), sphereMesh,15.0f,physx::PxU32(3),true));
	player.setActor(createDynamic(physx::PxTransform(player.getPositionPx()), shipMesh,player.getScale(), physx::PxU32(2)));
	sun.setActor(createDynamic(physx::PxTransform(sun.getPositionPx()), sphereMesh, sun.getScale()));
	planet2.setActor(createDynamic(physx::PxTransform(planet2.getPositionPx()), sphereMesh, planet2.getScale()));
	planet3.setActor(createDynamic(physx::PxTransform(planet3.getPositionPx()), sphereMesh, planet3.getScale()));
	planet4.setActor(createDynamic(physx::PxTransform(planet4.getPositionPx()), sphereMesh, planet4.getScale()));
	planet5.setActor(createDynamic(physx::PxTransform(planet5.getPositionPx()), sphereMesh, planet5.getScale()));
	planet6.setActor(createDynamic(physx::PxTransform(planet6.getPositionPx()), sphereMesh, planet6.getScale()));
	planet7.setActor(createDynamic(physx::PxTransform(planet7.getPositionPx()), sphereMesh, planet7.getScale()));
// planet8.setActor(createDynamic(physx::PxTransform(planet8.getPositionPx()), sphereMesh, planet8.getScale()));
	skybox.setActor(createDynamic(physx::PxTransform(skybox.getPositionPx()), cookMesh(skyBoxModel), skybox.getScale(), physx::PxU32(4), true));


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glutSetCursor(GLUT_CURSOR_NONE);
}



void shutdown()
{
	
	shaderLoader.DeleteProgram(program);
	cleanupPhysics(true);
}

void idle()
{
	glutPostRedisplay();
}


int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Quiver");
	glewInit();
	init();
	glutKeyboardFunc(keyboard);
	glutMotionFunc(mouse_callback);
	glutMouseFunc(mouse_clicks);
	glutPassiveMotionFunc(mouse_callback);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);
	glutMainLoop();
	shutdown();

	return 0;
}
