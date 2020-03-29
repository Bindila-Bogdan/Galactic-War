//
//  main.cpp
//  OpenGL Shadows
//
//  Created by CGIS on 05/12/16.
//  Copyright � 2016 CGIS. All rights reserved.
//
#include "stdfax.h"
#define GLEW_STATIC

#include "SkyBox.hpp"
#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"

int glWindowWidth = 1920;
int glWindowHeight = 1000;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const GLuint SHADOW_WIDTH = 16384, SHADOW_HEIGHT = 16384;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

glm::vec3 cameraPosition = glm::vec3(25.0f, 25.0f, 30.0f);
glm::vec3 cameraTarget = glm::vec3(25.0f, 12.5f, -15.0f);
gps::Camera myCamera(cameraPosition, cameraTarget);
GLfloat cameraSpeed = 5.0f;

bool freeCamera = false;

float lastX = (int)(glWindowWidth / 2.0f);
float lastY = (int)(glWindowHeight / 2.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float yaw = -90.0f;
float pitch = 0.0f;
float skyBoxAngle = 0.0f;

bool firstAnimation = true;

glm::vec3 edge1 = glm::vec3(25.2354f, 12.7461f, 23.5195f); 
glm::vec3 edge2 = glm::vec3(25.8679f, 10.6197f, -78.9536f);
glm::vec3 edge3 = glm::vec3(-11.3739f, 7.54454f, -47.6592f);
glm::vec3 edge4 = glm::vec3(59.4581f, 8.75493f, -27.6318f);

void determineBoudingBox() {
	if (!firstAnimation && !freeCamera) {
		if (myCamera.getCameraPosition().z > 23.5195f) {
			float x = myCamera.getCameraPosition().x;
			float y = myCamera.getCameraPosition().y;
			float z = 23.5194f;
			myCamera.setPosition(x, y, z);
		}
		else if (myCamera.getCameraPosition().z < -78.0f)
		{
			float x = myCamera.getCameraPosition().x;
			float y = myCamera.getCameraPosition().y;
			float z = -77.995f;
			myCamera.setPosition(x, y, z);
		}
		else if (myCamera.getCameraPosition().x < -10.0f)
		{
			float x = -9.995f;
			float y = myCamera.getCameraPosition().y;
			float z = myCamera.getCameraPosition().z;
			myCamera.setPosition(x, y, z);
		}
		else if (myCamera.getCameraPosition().x > 58.0f)
		{
			float x = 57.995;
			float y = myCamera.getCameraPosition().y;
			float z = myCamera.getCameraPosition().z;
			myCamera.setPosition(x, y, z);
		}
		else if (myCamera.getCameraPosition().y < 1.0f)
		{
			float x = myCamera.getCameraPosition().x;
			float y = 1.005;
			float z = myCamera.getCameraPosition().z;
			myCamera.setPosition(x, y, z);
		}
		else if (myCamera.getCameraPosition().y > 8.0f)
		{
			float x = myCamera.getCameraPosition().x;
			float y = 7.995;
			float z = myCamera.getCameraPosition().z;
			myCamera.setPosition(x, y, z);
		}
		else if (myCamera.getCameraPosition().x < 7.0f && myCamera.getCameraPosition().z > 10.0f) {
			glfwSetWindowShouldClose(glWindow, GL_TRUE);

		}
	}
}

bool generateFog = true;
bool generateShadows = true;
bool turnOnFlashLight = false;

bool pressedKeys[1024];
GLfloat angle;
GLfloat lightAngle;

gps::Model3D building;
gps::Model3D ground;
gps::Model3D ufoLight;
gps::Model3D ufo;
gps::Model3D centaur;
gps::Model3D robot;
gps::Model3D mars;
gps::Model3D spaceship;
gps::Model3D wall;
gps::Model3D flashLight;
gps::Model3D oil_pump;
gps::Model3D electric_building;
gps::Model3D portal;
gps::Model3D galaxy;
gps::Model3D oil_tank;
gps::Model3D electric_generator;
gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;

float zTranslateSpaceship = 0.0f;
float incrementZSpaceShip = -0.004f;
float xTranslateRobot = 0.0f;
float yTranslateRobot = 0.0;
float incrementTranslationRobot = 0.1f;
float flashLightAngle = 0.0f;
float incrementFlashLightAngle = 0.4f;
GLfloat galaxyRotationAngle = 0.0f;

std::vector<const GLchar*> faces{
	"textures/skybox/right.png",
	"textures/skybox/left.png",
	"textures/skybox/top.png",
	"textures/skybox/bottom.png",
	"textures/skybox/back.png",
	"textures/skybox/front.png", };

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	lightShader.useShaderProgram();
	
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}

	if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
		generateFog = !generateFog;

	}

	if (key == GLFW_KEY_K && action == GLFW_RELEASE) {
		generateShadows = !generateShadows;
	}

	if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
		turnOnFlashLight = !turnOnFlashLight;
	}

	if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
		freeCamera = !freeCamera;
	}

	if (key == GLFW_KEY_X && action == GLFW_RELEASE) {
		std::cout << myCamera.getCameraPosition().x << "f, ";
		std::cout << myCamera.getCameraPosition().y << "f, ";
		std::cout << myCamera.getCameraPosition().z << "f" << std::endl;

		std::cout << myCamera.getCameraTarget().x << "f, ";
		std::cout << myCamera.getCameraTarget().y << "f, ";
		std::cout << myCamera.getCameraTarget().z << "f";
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	float sensitivity = 0.5f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	if(!firstAnimation)
	myCamera.rotate(pitch, yaw);
}

void processMovement()
{

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_UP]) {
		xTranslateRobot += incrementTranslationRobot;
	}

	if (pressedKeys[GLFW_KEY_DOWN]) {
		xTranslateRobot -= incrementTranslationRobot;
	}

	if (pressedKeys[GLFW_KEY_LEFT]) {
		yTranslateRobot += incrementTranslationRobot;
	}

	if (pressedKeys[GLFW_KEY_RIGHT]) {
		yTranslateRobot -= incrementTranslationRobot;
	}

	if (pressedKeys[GLFW_KEY_CAPS_LOCK]) {
		incrementTranslationRobot = 0.3f;
	}
	else {
		incrementTranslationRobot = 0.1f;
	}

	if (pressedKeys[GLFW_KEY_I]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_O]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_P]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "Seek and Destroy", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initFBOs()
{
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	const GLfloat near_plane = 7.4f, far_plane = 75.0f;
	glm::mat4 lightProjection = glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, near_plane, far_plane);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(myCamera.getCameraTarget() + 1.0f * lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void initModels()
{
	mySkyBox.Load(faces);
	building = gps::Model3D("objects/building/building.obj", "objects/building/");
	oil_pump = gps::Model3D("objects/oil_pump/oil_pump.obj", "objects/oil_pump/");
	oil_tank = gps::Model3D("objects/tank/tank.obj", "objects/tank/");
	electric_building = gps::Model3D("objects/electrical_building/electric.obj", "objects/electrical_building/");
	electric_generator = gps::Model3D("objects/generator/generator.obj", "objects/generator/");
	ground = gps::Model3D("objects/ground/ground.obj", "objects/ground/");
	ufoLight = gps::Model3D("objects/cube/light.obj", "objects/cube/");
	ufo = gps::Model3D("objects/ufo/ufo.obj", "objects/ufo/");
	robot = gps::Model3D("objects/robot/robot.obj", "objects/robot/");
	mars = gps::Model3D("objects/mars/mars.obj", "objects/mars/");
	spaceship = gps::Model3D("objects/spaceship/spaceship.obj", "objects/spaceship/");
	wall = gps::Model3D("objects/wall/wall.obj", "objects/wall/");
	portal = gps::Model3D("objects/portal/portal.obj", "objects/portal/");
	galaxy = gps::Model3D("objects/galaxy/galaxy.obj", "objects/galaxy/");
	centaur = gps::Model3D("objects/centaur/centaur.obj", "objects/centaur/");
	flashLight = gps::Model3D("objects/flashlight/flashlight.obj", "objects/flashlight/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
}

void initUniforms()
{
	myCustomShader.useShaderProgram();

	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	
	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(+10.8f, 22.5f, -19.9f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	float lightCutOff = glm::cos(glm::radians(10.5f));
	float ligthOuterCutOff = glm::cos(glm::radians(12.5f));

	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "spotLight"), 1, glm::value_ptr(glm::vec3(lightCutOff, ligthOuterCutOff, 0.0f)));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
	skyboxShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));

}

void spaceShipComputeMove() {
	if (zTranslateSpaceship > 0.5f)
		incrementZSpaceShip *= -1.0f;
	else if (zTranslateSpaceship < 0.0f)
		incrementZSpaceShip *= -1.0f;
	zTranslateSpaceship += incrementZSpaceShip;
}

void ufoComputeMove() {
	lightAngle += 0.3f;
	if (lightAngle > 360.0f)
		lightAngle -= 360.0f;
	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	myCustomShader.useShaderProgram();
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
}

void flashLightComputeMove() {
	if (flashLightAngle > 15.0f)
		incrementFlashLightAngle *= -1.0f;
	else if (flashLightAngle < -15.0f)
		incrementFlashLightAngle *= -1.0f;
	flashLightAngle += incrementFlashLightAngle;
}

void computeGalaxyRotationAngle() {
	galaxyRotationAngle += 500000.0f;
	if (galaxyRotationAngle > 360.0f)
		galaxyRotationAngle -= 360.0f;

}

std::vector<glm::vec3> vecOfCameraPosition;
std::vector<glm::vec3> vecOfCameraTarget;
int frame = 0;

// Positions of camera for animations
void secondAnimation() {
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-2.0f, 5.0f, 30.0f));
	vecOfCameraPosition.push_back(glm::vec3(-1.99123f, 5.0f, 29.8246f));
	vecOfCameraPosition.push_back(glm::vec3(-1.98318f, 5.0f, 29.6636f));
	vecOfCameraPosition.push_back(glm::vec3(-1.97315f, 5.0f, 29.463f));
	vecOfCameraPosition.push_back(glm::vec3(-1.96677f, 5.0f, 29.3353f));
	vecOfCameraPosition.push_back(glm::vec3(-1.95774f, 5.0f, 29.1547f));
	vecOfCameraPosition.push_back(glm::vec3(-1.94955f, 5.0f, 28.9911f));
	vecOfCameraPosition.push_back(glm::vec3(-1.94067f, 5.0f, 28.8133f));
	vecOfCameraPosition.push_back(glm::vec3(-1.9325f, 5.0f, 28.65f));
	vecOfCameraPosition.push_back(glm::vec3(-1.92445f, 5.0f, 28.489f));
	vecOfCameraPosition.push_back(glm::vec3(-1.9164f, 5.0f, 28.328f));
	vecOfCameraPosition.push_back(glm::vec3(-1.90831f, 5.0f, 28.1662f));
	vecOfCameraPosition.push_back(glm::vec3(-1.89983f, 5.0f, 27.9966f));
	vecOfCameraPosition.push_back(glm::vec3(-1.89132f, 5.0f, 27.8264f));
	vecOfCameraPosition.push_back(glm::vec3(-1.88333f, 5.0f, 27.6665f));
	vecOfCameraPosition.push_back(glm::vec3(-1.87436f, 5.0f, 27.4872f));
	vecOfCameraPosition.push_back(glm::vec3(-1.86642f, 5.0f, 27.3284f));
	vecOfCameraPosition.push_back(glm::vec3(-1.85797f, 5.0f, 27.1595f));
	vecOfCameraPosition.push_back(glm::vec3(-1.84944f, 5.0f, 26.9889f));
	vecOfCameraPosition.push_back(glm::vec3(-1.84055f, 5.0f, 26.8111f));
	vecOfCameraPosition.push_back(glm::vec3(-1.83269f, 5.0f, 26.6538f));
	vecOfCameraPosition.push_back(glm::vec3(-1.82402f, 5.0f, 26.4804f));
	vecOfCameraPosition.push_back(glm::vec3(-1.81585f, 5.0f, 26.317f));
	vecOfCameraPosition.push_back(glm::vec3(-1.63749f, 5.0f, 26.1556f));
	vecOfCameraPosition.push_back(glm::vec3(-1.47571f, 5.0f, 26.0093f));
	vecOfCameraPosition.push_back(glm::vec3(-1.28919f, 5.0f, 25.8405f));
	vecOfCameraPosition.push_back(glm::vec3(-1.11539f, 5.0f, 25.6833f));
	vecOfCameraPosition.push_back(glm::vec3(-0.945596f, 5.0f, 25.5296f));
	vecOfCameraPosition.push_back(glm::vec3(-0.757515f, 5.0f, 25.3595f));
	vecOfCameraPosition.push_back(glm::vec3(-0.585537f, 5.0f, 25.2039f));
	vecOfCameraPosition.push_back(glm::vec3(-0.409778f, 5.0f, 25.0448f));
	vecOfCameraPosition.push_back(glm::vec3(-0.237819f, 5.0f, 24.8893f));
	vecOfCameraPosition.push_back(glm::vec3(-0.080882f, 5.0f, 24.7473f));
	vecOfCameraPosition.push_back(glm::vec3(0.107719f, 5.0f, 24.5766f));
	vecOfCameraPosition.push_back(glm::vec3(0.270556f, 5.0f, 24.4293f));
	vecOfCameraPosition.push_back(glm::vec3(0.458197f, 5.0f, 24.2595f));
	vecOfCameraPosition.push_back(glm::vec3(0.620594f, 5.0f, 24.1126f));
	vecOfCameraPosition.push_back(glm::vec3(0.809675f, 5.0f, 23.9415f));
	vecOfCameraPosition.push_back(glm::vec3(0.987694f, 5.0f, 23.7805f));
	vecOfCameraPosition.push_back(glm::vec3(1.16105f, 5.0f, 23.6236f));
	vecOfCameraPosition.push_back(glm::vec3(1.32437f, 5.0f, 23.4759f));
	vecOfCameraPosition.push_back(glm::vec3(1.50793f, 5.0f, 23.3098f));
	vecOfCameraPosition.push_back(glm::vec3(1.67037f, 5.0f, 23.1628f));
	vecOfCameraPosition.push_back(glm::vec3(1.86259f, 5.0f, 22.9889f));
	vecOfCameraPosition.push_back(glm::vec3(2.01215f, 5.0f, 22.8536f));
	vecOfCameraPosition.push_back(glm::vec3(2.19581f, 5.0f, 22.6874f));
	vecOfCameraPosition.push_back(glm::vec3(2.36268f, 5.0f, 22.5364f));
	vecOfCameraPosition.push_back(glm::vec3(2.54322f, 5.0f, 22.3731f));
	vecOfCameraPosition.push_back(glm::vec3(2.7211f, 5.0f, 22.2121f));
	vecOfCameraPosition.push_back(glm::vec3(2.89796f, 5.0f, 22.0521f));
	vecOfCameraPosition.push_back(glm::vec3(3.06188f, 5.0f, 21.9038f));
	vecOfCameraPosition.push_back(glm::vec3(3.24034f, 5.0f, 21.7424f));
	vecOfCameraPosition.push_back(glm::vec3(3.4242f, 5.0f, 21.576f));
	vecOfCameraPosition.push_back(glm::vec3(3.6041f, 5.0f, 21.4132f));
	vecOfCameraPosition.push_back(glm::vec3(3.76554f, 5.0f, 21.2672f));
	vecOfCameraPosition.push_back(glm::vec3(3.96254f, 5.0f, 21.0889f));
	vecOfCameraPosition.push_back(glm::vec3(4.12243f, 5.0f, 20.9443f));
	vecOfCameraPosition.push_back(glm::vec3(4.30693f, 5.0f, 20.7773f));
	vecOfCameraPosition.push_back(glm::vec3(4.47719f, 5.0f, 20.6233f));
	vecOfCameraPosition.push_back(glm::vec3(4.64499f, 5.0f, 20.4715f));
	vecOfCameraPosition.push_back(glm::vec3(4.80873f, 5.0f, 20.3233f));
	vecOfCameraPosition.push_back(glm::vec3(5.00587f, 5.0f, 20.145f));
	vecOfCameraPosition.push_back(glm::vec3(5.16331f, 5.0f, 20.0025f));
	vecOfCameraPosition.push_back(glm::vec3(5.33485f, 5.0f, 19.8473f));
	vecOfCameraPosition.push_back(glm::vec3(5.52893f, 5.0f, 19.6717f));
	vecOfCameraPosition.push_back(glm::vec3(5.72187f, 5.0f, 19.4972f));
	vecOfCameraPosition.push_back(glm::vec3(5.88283f, 5.0f, 19.3515f));
	vecOfCameraPosition.push_back(glm::vec3(6.05438f, 5.0f, 19.1963f));
	vecOfCameraPosition.push_back(glm::vec3(6.21794f, 5.0f, 19.0483f));
	vecOfCameraPosition.push_back(glm::vec3(6.39124f, 5.0f, 18.8915f));
	vecOfCameraPosition.push_back(glm::vec3(6.56904f, 5.0f, 18.7307f));
	vecOfCameraPosition.push_back(glm::vec3(6.74642f, 5.0f, 18.5702f));
	vecOfCameraPosition.push_back(glm::vec3(6.91042f, 5.0f, 18.4218f));
	vecOfCameraPosition.push_back(glm::vec3(7.1037f, 5.0f, 18.2469f));
	vecOfCameraPosition.push_back(glm::vec3(7.26128f, 5.0f, 18.1044f));
	vecOfCameraPosition.push_back(glm::vec3(7.45854f, 5.0f, 17.9259f));
	vecOfCameraPosition.push_back(glm::vec3(7.60701f, 5.0f, 17.7915f));
	vecOfCameraPosition.push_back(glm::vec3(7.80278f, 5.0f, 17.6144f));
	vecOfCameraPosition.push_back(glm::vec3(7.94403f, 5.0f, 17.4866f));
	vecOfCameraPosition.push_back(glm::vec3(8.18196f, 5.0f, 17.2714f));
	vecOfCameraPosition.push_back(glm::vec3(8.30389f, 5.0f, 17.161f));
	vecOfCameraPosition.push_back(glm::vec3(8.51403f, 5.0f, 16.9709f));
	vecOfCameraPosition.push_back(glm::vec3(8.66607f, 5.0f, 16.8334f));
	vecOfCameraPosition.push_back(glm::vec3(8.86455f, 5.0f, 16.6538f));
	vecOfCameraPosition.push_back(glm::vec3(9.01193f, 5.0f, 16.5204f));
	vecOfCameraPosition.push_back(glm::vec3(9.19651f, 5.0f, 16.3534f));
	vecOfCameraPosition.push_back(glm::vec3(9.38263f, 5.0f, 16.185f));
	vecOfCameraPosition.push_back(glm::vec3(9.53728f, 5.0f, 16.0451f));
	vecOfCameraPosition.push_back(glm::vec3(9.7055f, 5.0f, 15.8929f));
	vecOfCameraPosition.push_back(glm::vec3(9.87954f, 5.0f, 15.7354f));
	vecOfCameraPosition.push_back(glm::vec3(10.0522f, 5.0f, 15.5792f));
	vecOfCameraPosition.push_back(glm::vec3(10.2474f, 5.0f, 15.4026f));
	vecOfCameraPosition.push_back(glm::vec3(10.3998f, 5.0f, 15.2648f));
	vecOfCameraPosition.push_back(glm::vec3(10.596f, 5.0f, 15.0872f));
	vecOfCameraPosition.push_back(glm::vec3(10.7714f, 5.0f, 14.9285f));
	vecOfCameraPosition.push_back(glm::vec3(10.9589f, 5.0f, 14.7589f));
	vecOfCameraPosition.push_back(glm::vec3(11.1026f, 5.0f, 14.6288f));
	vecOfCameraPosition.push_back(glm::vec3(11.2737f, 5.0f, 14.4741f));
	vecOfCameraPosition.push_back(glm::vec3(11.461f, 5.0f, 14.3046f));
	vecOfCameraPosition.push_back(glm::vec3(11.6362f, 5.0f, 14.1461f));
	vecOfCameraPosition.push_back(glm::vec3(11.799f, 5.0f, 13.9988f));
	vecOfCameraPosition.push_back(glm::vec3(11.9848f, 5.0f, 13.8307f));
	vecOfCameraPosition.push_back(glm::vec3(12.1647f, 5.0f, 13.6679f));
	vecOfCameraPosition.push_back(glm::vec3(12.3365f, 5.0f, 13.5125f));
	vecOfCameraPosition.push_back(glm::vec3(12.4972f, 5.0f, 13.367f));
	vecOfCameraPosition.push_back(glm::vec3(12.7072f, 5.0f, 13.177f));
	vecOfCameraPosition.push_back(glm::vec3(12.8433f, 5.0f, 13.054f));
	vecOfCameraPosition.push_back(glm::vec3(13.0453f, 5.0f, 12.8711f));
	vecOfCameraPosition.push_back(glm::vec3(13.1933f, 5.0f, 12.7373f));
	vecOfCameraPosition.push_back(glm::vec3(13.3738f, 5.0f, 12.574f));
	vecOfCameraPosition.push_back(glm::vec3(13.5466f, 5.0f, 12.4176f));
	vecOfCameraPosition.push_back(glm::vec3(13.746f, 5.0f, 12.2372f));
	vecOfCameraPosition.push_back(glm::vec3(13.8925f, 5.0f, 12.1046f));
	vecOfCameraPosition.push_back(glm::vec3(14.0971f, 5.0f, 11.9195f));
	vecOfCameraPosition.push_back(glm::vec3(14.2428f, 5.0f, 11.7877f));
	vecOfCameraPosition.push_back(glm::vec3(14.4347f, 5.0f, 11.6141f));
	vecOfCameraPosition.push_back(glm::vec3(14.6234f, 5.0f, 11.4434f));
	vecOfCameraPosition.push_back(glm::vec3(14.7769f, 5.0f, 11.3045f));
	vecOfCameraPosition.push_back(glm::vec3(14.9488f, 5.0f, 11.1489f));
	vecOfCameraPosition.push_back(glm::vec3(15.1403f, 5.0f, 10.9757f));
	vecOfCameraPosition.push_back(glm::vec3(15.3037f, 5.0f, 10.8279f));
	vecOfCameraPosition.push_back(glm::vec3(15.4793f, 5.0f, 10.669f));
	vecOfCameraPosition.push_back(glm::vec3(15.641f, 5.0f, 10.5227f));
	vecOfCameraPosition.push_back(glm::vec3(15.8405f, 5.0f, 10.3422f));
	vecOfCameraPosition.push_back(glm::vec3(15.9988f, 5.0f, 10.1989f));
	vecOfCameraPosition.push_back(glm::vec3(16.0079f, 5.0f, 10.0178f));
	vecOfCameraPosition.push_back(glm::vec3(16.0151f, 5.0f, 9.8741f));
	vecOfCameraPosition.push_back(glm::vec3(16.0237f, 5.0f, 9.70094f));
	vecOfCameraPosition.push_back(glm::vec3(16.0326f, 5.0f, 9.52396f));
	vecOfCameraPosition.push_back(glm::vec3(16.0404f, 5.0f, 9.3669f));
	vecOfCameraPosition.push_back(glm::vec3(16.0484f, 5.0f, 9.20694f));
	vecOfCameraPosition.push_back(glm::vec3(16.0585f, 5.0f, 9.00637f));
	vecOfCameraPosition.push_back(glm::vec3(16.0658f, 5.0f, 8.8606f));
	vecOfCameraPosition.push_back(glm::vec3(16.0741f, 5.0f, 8.69279f));
	vecOfCameraPosition.push_back(glm::vec3(16.0819f, 5.0f, 8.53776f));
	vecOfCameraPosition.push_back(glm::vec3(16.0912f, 5.0f, 8.35266f));
	vecOfCameraPosition.push_back(glm::vec3(16.0983f, 5.0f, 8.20976f));
	vecOfCameraPosition.push_back(glm::vec3(16.1071f, 5.0f, 8.03298f));
	vecOfCameraPosition.push_back(glm::vec3(16.1146f, 5.0f, 7.88459f));
	vecOfCameraPosition.push_back(glm::vec3(16.1235f, 5.0f, 7.70661f));
	vecOfCameraPosition.push_back(glm::vec3(16.1316f, 5.0f, 7.54333f));
	vecOfCameraPosition.push_back(glm::vec3(16.1403f, 5.0f, 7.36986f));
	vecOfCameraPosition.push_back(glm::vec3(16.1483f, 5.0f, 7.20889f));
	vecOfCameraPosition.push_back(glm::vec3(16.1584f, 5.0f, 7.00767f));
	vecOfCameraPosition.push_back(glm::vec3(16.1655f, 5.0f, 6.8651f));
	vecOfCameraPosition.push_back(glm::vec3(16.1747f, 5.0f, 6.6819f));
	vecOfCameraPosition.push_back(glm::vec3(16.1825f, 5.0f, 6.52492f));
	vecOfCameraPosition.push_back(glm::vec3(16.1903f, 5.0f, 6.37044f));
	vecOfCameraPosition.push_back(glm::vec3(16.198f, 5.0f, 6.21494f));
	vecOfCameraPosition.push_back(glm::vec3(16.2068f, 5.0f, 6.03995f));
	vecOfCameraPosition.push_back(glm::vec3(16.2145f, 5.0f, 5.8849f));
	vecOfCameraPosition.push_back(glm::vec3(16.2236f, 5.0f, 5.70321f));
	vecOfCameraPosition.push_back(glm::vec3(16.2318f, 5.0f, 5.5397f));
	vecOfCameraPosition.push_back(glm::vec3(16.2408f, 5.0f, 5.35898f));
	vecOfCameraPosition.push_back(glm::vec3(16.2495f, 5.0f, 5.1865f));
	vecOfCameraPosition.push_back(glm::vec3(16.2592f, 5.0f, 4.99187f));
	vecOfCameraPosition.push_back(glm::vec3(16.266f, 5.0f, 4.85479f));
	vecOfCameraPosition.push_back(glm::vec3(16.2741f, 5.0f, 4.69277f));
	vecOfCameraPosition.push_back(glm::vec3(16.2813f, 5.0f, 4.54975f));
	vecOfCameraPosition.push_back(glm::vec3(16.2902f, 5.0f, 4.37154f));
	vecOfCameraPosition.push_back(glm::vec3(16.2978f, 5.0f, 4.22004f));
	vecOfCameraPosition.push_back(glm::vec3(16.3074f, 5.0f, 4.02832f));
	vecOfCameraPosition.push_back(glm::vec3(16.3144f, 5.0f, 3.88708f));
	vecOfCameraPosition.push_back(glm::vec3(16.324f, 5.0f, 3.6964f));
	vecOfCameraPosition.push_back(glm::vec3(16.3324f, 5.0f, 3.52716f));
	vecOfCameraPosition.push_back(glm::vec3(16.3417f, 5.0f, 3.3411f));
	vecOfCameraPosition.push_back(glm::vec3(16.3477f, 5.0f, 3.22176f));
	vecOfCameraPosition.push_back(glm::vec3(16.3568f, 5.0f, 3.03928f));
	vecOfCameraPosition.push_back(glm::vec3(16.3647f, 5.0f, 2.88069f));
	vecOfCameraPosition.push_back(glm::vec3(16.3742f, 5.0f, 2.69073f));
	vecOfCameraPosition.push_back(glm::vec3(16.3817f, 5.0f, 2.5413f));
	vecOfCameraPosition.push_back(glm::vec3(16.39f, 5.0f, 2.37622f));
	vecOfCameraPosition.push_back(glm::vec3(16.3977f, 5.0f, 2.22151f));
	vecOfCameraPosition.push_back(glm::vec3(16.4068f, 5.0f, 2.04043f));
	vecOfCameraPosition.push_back(glm::vec3(16.4157f, 5.0f, 1.86239f));
	vecOfCameraPosition.push_back(glm::vec3(16.4237f, 5.0f, 1.70142f));
	vecOfCameraPosition.push_back(glm::vec3(16.4318f, 5.0f, 1.53873f));
	vecOfCameraPosition.push_back(glm::vec3(16.4401f, 5.0f, 1.37287f));
	vecOfCameraPosition.push_back(glm::vec3(16.4476f, 5.0f, 1.22459f));
	vecOfCameraPosition.push_back(glm::vec3(16.4563f, 5.0f, 1.05005f));
	vecOfCameraPosition.push_back(glm::vec3(16.4654f, 5.0f, 0.867005f));
	vecOfCameraPosition.push_back(glm::vec3(16.4746f, 5.0f, 0.683175f));
	vecOfCameraPosition.push_back(glm::vec3(16.4823f, 5.0f, 0.529292f));
	vecOfCameraPosition.push_back(glm::vec3(16.4907f, 5.0f, 0.361331f));
	vecOfCameraPosition.push_back(glm::vec3(16.4984f, 5.0f, 0.207276f));
	vecOfCameraPosition.push_back(glm::vec3(16.5075f, 5.0f, 0.0259607f));
	vecOfCameraPosition.push_back(glm::vec3(16.5141f, 5.0f, -0.106892f));
	vecOfCameraPosition.push_back(glm::vec3(16.5235f, 5.0f, -0.293979f));
	vecOfCameraPosition.push_back(glm::vec3(16.5312f, 5.0f, -0.448319f));
	vecOfCameraPosition.push_back(glm::vec3(16.5397f, 5.0f, -0.618433f));
	vecOfCameraPosition.push_back(glm::vec3(16.5476f, 5.0f, -0.775364f));
	vecOfCameraPosition.push_back(glm::vec3(16.5574f, 5.0f, -0.972738f));
	vecOfCameraPosition.push_back(glm::vec3(16.5655f, 5.0f, -1.13514f));
	vecOfCameraPosition.push_back(glm::vec3(16.5743f, 5.0f, -1.31001f));
	vecOfCameraPosition.push_back(glm::vec3(16.5821f, 5.0f, -1.46592f));
	vecOfCameraPosition.push_back(glm::vec3(16.5895f, 5.0f, -1.61384f));
	vecOfCameraPosition.push_back(glm::vec3(16.5976f, 5.0f, -1.77595f));
	vecOfCameraPosition.push_back(glm::vec3(16.6061f, 5.0f, -1.94721f));
	vecOfCameraPosition.push_back(glm::vec3(16.6144f, 5.0f, -2.11195f));
	vecOfCameraPosition.push_back(glm::vec3(16.6226f, 5.0f, -2.27536f));
	vecOfCameraPosition.push_back(glm::vec3(16.6306f, 5.0f, -2.4369f));
	vecOfCameraPosition.push_back(glm::vec3(16.6397f, 5.0f, -2.61876f));
	vecOfCameraPosition.push_back(glm::vec3(16.8171f, 5.0f, -2.77923f));
	vecOfCameraPosition.push_back(glm::vec3(17.0084f, 5.0f, -2.95233f));
	vecOfCameraPosition.push_back(glm::vec3(17.1536f, 5.0f, -3.08368f));
	vecOfCameraPosition.push_back(glm::vec3(17.3518f, 5.0f, -3.26303f));
	vecOfCameraPosition.push_back(glm::vec3(17.4971f, 5.0f, -3.39447f));
	vecOfCameraPosition.push_back(glm::vec3(17.6926f, 5.0f, -3.57138f));
	vecOfCameraPosition.push_back(glm::vec3(17.8518f, 5.0f, -3.71536f));
	vecOfCameraPosition.push_back(glm::vec3(18.0338f, 5.0f, -3.88003f));
	vecOfCameraPosition.push_back(glm::vec3(18.2038f, 5.0f, -4.03389f));
	vecOfCameraPosition.push_back(glm::vec3(18.3925f, 5.0f, -4.20462f));
	vecOfCameraPosition.push_back(glm::vec3(18.5764f, 5.0f, -4.37095f));
	vecOfCameraPosition.push_back(glm::vec3(18.7387f, 5.0f, -4.51787f));
	vecOfCameraPosition.push_back(glm::vec3(18.9105f, 5.0f, -4.67325f));
	vecOfCameraPosition.push_back(glm::vec3(19.0934f, 5.0f, -4.83877f));
	vecOfCameraPosition.push_back(glm::vec3(19.2443f, 5.0f, -4.97526f));
	vecOfCameraPosition.push_back(glm::vec3(19.4402f, 5.0f, -5.1525f));
	vecOfCameraPosition.push_back(glm::vec3(19.6211f, 5.0f, -5.31623f));
	vecOfCameraPosition.push_back(glm::vec3(19.7892f, 5.0f, -5.46833f));
	vecOfCameraPosition.push_back(glm::vec3(19.9625f, 5.0f, -5.62511f));
	vecOfCameraPosition.push_back(glm::vec3(20.1552f, 5.0f, -5.79946f));
	vecOfCameraPosition.push_back(glm::vec3(20.3187f, 5.0f, -5.94737f));
	vecOfCameraPosition.push_back(glm::vec3(20.4753f, 5.0f, -6.08907f));
	vecOfCameraPosition.push_back(glm::vec3(20.6471f, 5.0f, -6.2445f));
	vecOfCameraPosition.push_back(glm::vec3(20.8289f, 5.0f, -6.40895f));
	vecOfCameraPosition.push_back(glm::vec3(20.9908f, 5.0f, -6.55541f));
	vecOfCameraPosition.push_back(glm::vec3(21.1818f, 5.0f, -6.72826f));
	vecOfCameraPosition.push_back(glm::vec3(21.3427f, 5.0f, -6.87383f));
	vecOfCameraPosition.push_back(glm::vec3(21.5522f, 5.0f, -7.06338f));
	vecOfCameraPosition.push_back(glm::vec3(21.7306f, 5.0f, -7.22481f));
	vecOfCameraPosition.push_back(glm::vec3(21.8941f, 5.0f, -7.37274f));
	vecOfCameraPosition.push_back(glm::vec3(22.0522f, 5.0f, -7.51572f));
	vecOfCameraPosition.push_back(glm::vec3(22.2391f, 5.0f, -7.68486f));
	vecOfCameraPosition.push_back(glm::vec3(22.3937f, 5.0f, -7.82473f));
	vecOfCameraPosition.push_back(glm::vec3(22.5708f, 5.0f, -7.98497f));
	vecOfCameraPosition.push_back(glm::vec3(22.7573f, 5.0f, -8.15376f));
	vecOfCameraPosition.push_back(glm::vec3(22.9383f, 5.0f, -8.31745f));
	vecOfCameraPosition.push_back(glm::vec3(23.0958f, 5.0f, -8.46002f));
	vecOfCameraPosition.push_back(glm::vec3(23.2738f, 5.0f, -8.62103f));
	vecOfCameraPosition.push_back(glm::vec3(23.4588f, 5.0f, -8.78839f));
	vecOfCameraPosition.push_back(glm::vec3(23.6154f, 5.0f, -8.93009f));
	vecOfCameraPosition.push_back(glm::vec3(23.7943f, 5.0f, -9.09199f));
	vecOfCameraPosition.push_back(glm::vec3(23.9918f, 5.0f, -9.27066f));
	vecOfCameraPosition.push_back(glm::vec3(24.1326f, 5.0f, -9.39803f));
	vecOfCameraPosition.push_back(glm::vec3(24.3373f, 5.0f, -9.58328f));
	vecOfCameraPosition.push_back(glm::vec3(24.4959f, 5.0f, -9.7267f));
	vecOfCameraPosition.push_back(glm::vec3(24.6912f, 5.0f, -9.9034f));
	vecOfCameraPosition.push_back(glm::vec3(24.8466f, 5.0f, -10.044f));
	vecOfCameraPosition.push_back(glm::vec3(25.0301f, 5.0f, -10.21f));
	vecOfCameraPosition.push_back(glm::vec3(25.1882f, 5.0f, -10.3531f));
	vecOfCameraPosition.push_back(glm::vec3(25.368f, 5.0f, -10.5158f));
	vecOfCameraPosition.push_back(glm::vec3(25.5284f, 5.0f, -10.6609f));
	vecOfCameraPosition.push_back(glm::vec3(25.7224f, 5.0f, -10.8364f));
	vecOfCameraPosition.push_back(glm::vec3(25.9097f, 5.0f, -11.0058f));
	vecOfCameraPosition.push_back(glm::vec3(26.0925f, 5.0f, -11.1713f));
	vecOfCameraPosition.push_back(glm::vec3(26.2394f, 5.0f, -11.3042f));
	vecOfCameraPosition.push_back(glm::vec3(26.4205f, 5.0f, -11.468f));
	vecOfCameraPosition.push_back(glm::vec3(26.5839f, 5.0f, -11.6159f));
	vecOfCameraPosition.push_back(glm::vec3(26.7606f, 5.0f, -11.7757f));
	vecOfCameraPosition.push_back(glm::vec3(26.9349f, 5.0f, -11.9335f));
	vecOfCameraPosition.push_back(glm::vec3(27.1131f, 5.0f, -12.0946f));
	vecOfCameraPosition.push_back(glm::vec3(27.289f, 5.0f, -12.2539f));
	vecOfCameraPosition.push_back(glm::vec3(27.4733f, 5.0f, -12.4206f));
	vecOfCameraPosition.push_back(glm::vec3(27.6502f, 5.0f, -12.5806f));
	vecOfCameraPosition.push_back(glm::vec3(27.8412f, 5.0f, -12.7534f));
	vecOfCameraPosition.push_back(glm::vec3(27.9849f, 5.0f, -12.8834f));
	vecOfCameraPosition.push_back(glm::vec3(28.19f, 5.0f, -13.069f));
	vecOfCameraPosition.push_back(glm::vec3(28.3656f, 5.0f, -13.2279f));
	vecOfCameraPosition.push_back(glm::vec3(28.5326f, 5.0f, -13.379f));
	vecOfCameraPosition.push_back(glm::vec3(28.6889f, 5.0f, -13.5204f));
	vecOfCameraPosition.push_back(glm::vec3(28.8606f, 5.0f, -13.6757f));
	vecOfCameraPosition.push_back(glm::vec3(29.0292f, 5.0f, -13.8283f));
	vecOfCameraPosition.push_back(glm::vec3(29.206f, 5.0f, -13.9883f));
	vecOfCameraPosition.push_back(glm::vec3(29.3884f, 5.0f, -14.1533f));
	vecOfCameraPosition.push_back(glm::vec3(29.5803f, 5.0f, -14.327f));
	vecOfCameraPosition.push_back(glm::vec3(29.7391f, 5.0f, -14.4706f));
	vecOfCameraPosition.push_back(glm::vec3(29.9258f, 5.0f, -14.6395f));
	vecOfCameraPosition.push_back(glm::vec3(30.0805f, 5.0f, -14.7795f));
	vecOfCameraPosition.push_back(glm::vec3(30.277f, 5.0f, -14.9573f));
	vecOfCameraPosition.push_back(glm::vec3(30.4528f, 5.0f, -15.1163f));
	vecOfCameraPosition.push_back(glm::vec3(30.6504f, 5.0f, -15.2951f));
	vecOfCameraPosition.push_back(glm::vec3(30.787f, 5.0f, -15.4187f));
	vecOfCameraPosition.push_back(glm::vec3(30.9792f, 5.0f, -15.5926f));
	vecOfCameraPosition.push_back(glm::vec3(31.1343f, 5.0f, -15.7329f));
	vecOfCameraPosition.push_back(glm::vec3(31.3088f, 5.0f, -15.8908f));
	vecOfCameraPosition.push_back(glm::vec3(31.4779f, 5.0f, -16.0438f));
	vecOfCameraPosition.push_back(glm::vec3(31.6742f, 5.0f, -16.2214f));
	vecOfCameraPosition.push_back(glm::vec3(31.8258f, 5.0f, -16.3585f));
	vecOfCameraPosition.push_back(glm::vec3(32.0265f, 5.0f, -16.5401f));
	vecOfCameraPosition.push_back(glm::vec3(32.2138f, 5.0f, -16.7096f));
	vecOfCameraPosition.push_back(glm::vec3(32.3529f, 5.0f, -16.8354f));
	vecOfCameraPosition.push_back(glm::vec3(32.5414f, 5.0f, -17.006f));
	vecOfCameraPosition.push_back(glm::vec3(32.721f, 5.0f, -17.1685f));
	vecOfCameraPosition.push_back(glm::vec3(32.8712f, 5.0f, -17.3044f));
	vecOfCameraPosition.push_back(glm::vec3(33.0645f, 5.0f, -17.4793f));
	vecOfCameraPosition.push_back(glm::vec3(33.2197f, 5.0f, -17.6197f));
	vecOfCameraPosition.push_back(glm::vec3(33.4151f, 5.0f, -17.7965f));
	vecOfCameraPosition.push_back(glm::vec3(33.5772f, 5.0f, -17.9431f));
	vecOfCameraPosition.push_back(glm::vec3(33.7568f, 5.0f, -18.1056f));
	vecOfCameraPosition.push_back(glm::vec3(33.9199f, 5.0f, -18.2532f));
	vecOfCameraPosition.push_back(glm::vec3(34.1168f, 5.0f, -18.4314f));
	vecOfCameraPosition.push_back(glm::vec3(34.2801f, 5.0f, -18.5791f));
	vecOfCameraPosition.push_back(glm::vec3(34.4736f, 5.0f, -18.7542f));
	vecOfCameraPosition.push_back(glm::vec3(34.6286f, 5.0f, -18.8944f));
	vecOfCameraPosition.push_back(glm::vec3(34.8092f, 5.0f, -19.0578f));
	vecOfCameraPosition.push_back(glm::vec3(34.8169f, 5.0f, -19.2115f));
	vecOfCameraPosition.push_back(glm::vec3(34.8265f, 5.0f, -19.4037f));
	vecOfCameraPosition.push_back(glm::vec3(34.834f, 5.0f, -19.5548f));
	vecOfCameraPosition.push_back(glm::vec3(34.8424f, 5.0f, -19.7218f));
	vecOfCameraPosition.push_back(glm::vec3(34.8512f, 5.0f, -19.8981f));
	vecOfCameraPosition.push_back(glm::vec3(34.8589f, 5.0f, -20.0518f));
	vecOfCameraPosition.push_back(glm::vec3(34.8674f, 5.0f, -20.2226f));
	vecOfCameraPosition.push_back(glm::vec3(34.8762f, 5.0f, -20.399f));
	vecOfCameraPosition.push_back(glm::vec3(34.8835f, 5.0f, -20.545f));
	vecOfCameraPosition.push_back(glm::vec3(34.8926f, 5.0f, -20.7264f));
	vecOfCameraPosition.push_back(glm::vec3(34.9004f, 5.0f, -20.8823f));
	vecOfCameraPosition.push_back(glm::vec3(34.9102f, 5.0f, -21.0777f));
	vecOfCameraPosition.push_back(glm::vec3(34.9172f, 5.0f, -21.2171f));
	vecOfCameraPosition.push_back(glm::vec3(34.9261f, 5.0f, -21.3952f));
	vecOfCameraPosition.push_back(glm::vec3(34.9337f, 5.0f, -21.5486f));
	vecOfCameraPosition.push_back(glm::vec3(34.9428f, 5.0f, -21.7296f));
	vecOfCameraPosition.push_back(glm::vec3(34.9499f, 5.0f, -21.8716f));
	vecOfCameraPosition.push_back(glm::vec3(34.9594f, 5.0f, -22.0617f));
	vecOfCameraPosition.push_back(glm::vec3(34.9668f, 5.0f, -22.2095f));
	vecOfCameraPosition.push_back(glm::vec3(34.9763f, 5.0f, -22.4f));
	vecOfCameraPosition.push_back(glm::vec3(34.9833f, 5.0f, -22.5396f));
	vecOfCameraPosition.push_back(glm::vec3(34.9924f, 5.0f, -22.7225f));
	vecOfCameraPosition.push_back(glm::vec3(35.001f, 5.0f, -22.8933f));
	vecOfCameraPosition.push_back(glm::vec3(35.0089f, 5.0f, -23.0513f));
	vecOfCameraPosition.push_back(glm::vec3(35.0175f, 5.0f, -23.2248f));
	vecOfCameraPosition.push_back(glm::vec3(35.0263f, 5.0f, -23.3992f));
	vecOfCameraPosition.push_back(glm::vec3(35.0334f, 5.0f, -23.5425f));
	vecOfCameraPosition.push_back(glm::vec3(35.0426f, 5.0f, -23.7252f));
	vecOfCameraPosition.push_back(glm::vec3(35.0498f, 5.0f, -23.8697f));
	vecOfCameraPosition.push_back(glm::vec3(35.0586f, 5.0f, -24.0464f));
	vecOfCameraPosition.push_back(glm::vec3(35.067f, 5.0f, -24.2151f));
	vecOfCameraPosition.push_back(glm::vec3(35.0759f, 5.0f, -24.3914f));
	vecOfCameraPosition.push_back(glm::vec3(35.0835f, 5.0f, -24.5442f));
	vecOfCameraPosition.push_back(glm::vec3(35.0923f, 5.0f, -24.7206f));
	vecOfCameraPosition.push_back(glm::vec3(35.1004f, 5.0f, -24.8817f));
	vecOfCameraPosition.push_back(glm::vec3(35.1094f, 5.0f, -25.0629f));
	vecOfCameraPosition.push_back(glm::vec3(35.1166f, 5.0f, -25.2052f));
	vecOfCameraPosition.push_back(glm::vec3(35.1255f, 5.0f, -25.3843f));
	vecOfCameraPosition.push_back(glm::vec3(35.1333f, 5.0f, -25.5391f));
	vecOfCameraPosition.push_back(glm::vec3(35.1414f, 5.0f, -25.702f));
	vecOfCameraPosition.push_back(glm::vec3(34.9893f, 5.0f, -25.8702f));
	vecOfCameraPosition.push_back(glm::vec3(34.8151f, 5.0f, -26.0626f));
	vecOfCameraPosition.push_back(glm::vec3(34.6672f, 5.0f, -26.2262f));
	vecOfCameraPosition.push_back(glm::vec3(34.4992f, 5.0f, -26.4119f));
	vecOfCameraPosition.push_back(glm::vec3(34.3502f, 5.0f, -26.5765f));
	vecOfCameraPosition.push_back(glm::vec3(34.175f, 5.0f, -26.7702f));
	vecOfCameraPosition.push_back(glm::vec3(34.0284f, 5.0f, -26.9322f));
	vecOfCameraPosition.push_back(glm::vec3(33.8424f, 5.0f, -27.1378f));
	vecOfCameraPosition.push_back(glm::vec3(33.7201f, 5.0f, -27.2729f));
	vecOfCameraPosition.push_back(glm::vec3(33.537f, 5.0f, -27.4753f));
	vecOfCameraPosition.push_back(glm::vec3(33.4033f, 5.0f, -27.6231f));
	vecOfCameraPosition.push_back(glm::vec3(33.2326f, 5.0f, -27.8117f));
	vecOfCameraPosition.push_back(glm::vec3(33.0896f, 5.0f, -27.9698f));
	vecOfCameraPosition.push_back(glm::vec3(32.912f, 5.0f, -28.1661f));
	vecOfCameraPosition.push_back(glm::vec3(32.7725f, 5.0f, -28.3202f));
	vecOfCameraPosition.push_back(glm::vec3(32.6037f, 5.0f, -28.5068f));
	vecOfCameraPosition.push_back(glm::vec3(32.457f, 5.0f, -28.6689f));
	vecOfCameraPosition.push_back(glm::vec3(32.2888f, 5.0f, -28.8549f));
	vecOfCameraPosition.push_back(glm::vec3(32.1367f, 5.0f, -29.023f));
	vecOfCameraPosition.push_back(glm::vec3(31.9632f, 5.0f, -29.2148f));
	vecOfCameraPosition.push_back(glm::vec3(31.8213f, 5.0f, -29.3716f));
	vecOfCameraPosition.push_back(glm::vec3(31.6494f, 5.0f, -29.5616f));
	vecOfCameraPosition.push_back(glm::vec3(31.5059f, 5.0f, -29.7202f));
	vecOfCameraPosition.push_back(glm::vec3(31.3366f, 5.0f, -29.9073f));
	vecOfCameraPosition.push_back(glm::vec3(31.1671f, 5.0f, -30.0947f));
	vecOfCameraPosition.push_back(glm::vec3(31.0208f, 5.0f, -30.2564f));
	vecOfCameraPosition.push_back(glm::vec3(30.8511f, 5.0f, -30.4439f));
	vecOfCameraPosition.push_back(glm::vec3(30.7008f, 5.0f, -30.6101f));
	vecOfCameraPosition.push_back(glm::vec3(30.5458f, 5.0f, -30.7813f));
	vecOfCameraPosition.push_back(glm::vec3(30.3732f, 5.0f, -30.9721f));
	vecOfCameraPosition.push_back(glm::vec3(30.2299f, 5.0f, -31.1305f));
	vecOfCameraPosition.push_back(glm::vec3(30.0684f, 5.0f, -31.3091f));
	vecOfCameraPosition.push_back(glm::vec3(29.9078f, 5.0f, -31.4865f));
	vecOfCameraPosition.push_back(glm::vec3(29.7417f, 5.0f, -31.6701f));
	vecOfCameraPosition.push_back(glm::vec3(29.5947f, 5.0f, -31.8326f));
	vecOfCameraPosition.push_back(glm::vec3(29.4323f, 5.0f, -32.0121f));
	vecOfCameraPosition.push_back(glm::vec3(29.2756f, 5.0f, -32.1853f));
	vecOfCameraPosition.push_back(glm::vec3(29.1193f, 5.0f, -32.3581f));
	vecOfCameraPosition.push_back(glm::vec3(28.9558f, 5.0f, -32.5388f));
	vecOfCameraPosition.push_back(glm::vec3(28.7979f, 5.0f, -32.7133f));
	vecOfCameraPosition.push_back(glm::vec3(28.6541f, 5.0f, -32.8722f));
	vecOfCameraPosition.push_back(glm::vec3(28.4852f, 5.0f, -33.0589f));
	vecOfCameraPosition.push_back(glm::vec3(28.3264f, 5.0f, -33.2344f));
	vecOfCameraPosition.push_back(glm::vec3(28.177f, 5.0f, -33.3995f));
	vecOfCameraPosition.push_back(glm::vec3(28.0134f, 5.0f, -33.5803f));
	vecOfCameraPosition.push_back(glm::vec3(27.8401f, 5.0f, -33.7719f));
	vecOfCameraPosition.push_back(glm::vec3(27.6976f, 5.0f, -33.9294f));
	vecOfCameraPosition.push_back(glm::vec3(27.5311f, 5.0f, -34.1134f));
	vecOfCameraPosition.push_back(glm::vec3(27.3875f, 5.0f, -34.2722f));
	vecOfCameraPosition.push_back(glm::vec3(27.2303f, 5.0f, -34.4459f));
	vecOfCameraPosition.push_back(glm::vec3(27.0766f, 5.0f, -34.6158f));
	vecOfCameraPosition.push_back(glm::vec3(26.9144f, 5.0f, -34.7951f));
	vecOfCameraPosition.push_back(glm::vec3(26.7537f, 5.0f, -34.9727f));
	vecOfCameraPosition.push_back(glm::vec3(26.5883f, 5.0f, -35.1554f));
	vecOfCameraPosition.push_back(glm::vec3(26.4396f, 5.0f, -35.3198f));
	vecOfCameraPosition.push_back(glm::vec3(26.2701f, 5.0f, -35.5071f));
	vecOfCameraPosition.push_back(glm::vec3(26.1073f, 5.0f, -35.6871f));
	vecOfCameraPosition.push_back(glm::vec3(25.9484f, 5.0f, -35.8628f));
	vecOfCameraPosition.push_back(glm::vec3(25.8096f, 5.0f, -36.0161f));
	vecOfCameraPosition.push_back(glm::vec3(25.6377f, 5.0f, -36.2061f));
	vecOfCameraPosition.push_back(glm::vec3(25.4801f, 5.0f, -36.3803f));
	vecOfCameraPosition.push_back(glm::vec3(25.3319f, 5.0f, -36.5441f));
	vecOfCameraPosition.push_back(glm::vec3(25.1693f, 5.0f, -36.7238f));
	vecOfCameraPosition.push_back(glm::vec3(25.0124f, 5.0f, -36.8972f));
	vecOfCameraPosition.push_back(glm::vec3(24.85f, 5.0f, -37.0767f));
	vecOfCameraPosition.push_back(glm::vec3(24.6934f, 5.0f, -37.2498f));
	vecOfCameraPosition.push_back(glm::vec3(24.5365f, 5.0f, -37.4233f));
	vecOfCameraPosition.push_back(glm::vec3(24.3774f, 5.0f, -37.599f));
	vecOfCameraPosition.push_back(glm::vec3(24.2282f, 5.0f, -37.764f));
	vecOfCameraPosition.push_back(glm::vec3(24.0558f, 5.0f, -37.9546f));
	vecOfCameraPosition.push_back(glm::vec3(23.9182f, 5.0f, -38.1066f));
	vecOfCameraPosition.push_back(glm::vec3(23.7423f, 5.0f, -38.3011f));
	vecOfCameraPosition.push_back(glm::vec3(23.5855f, 5.0f, -38.4743f));
	vecOfCameraPosition.push_back(glm::vec3(23.4307f, 5.0f, -38.6455f));
	vecOfCameraPosition.push_back(glm::vec3(23.2763f, 5.0f, -38.8161f));
	vecOfCameraPosition.push_back(glm::vec3(23.1101f, 5.0f, -38.9998f));
	vecOfCameraPosition.push_back(glm::vec3(22.9526f, 5.0f, -39.1739f));
	vecOfCameraPosition.push_back(glm::vec3(22.7987f, 5.0f, -39.344f));
	vecOfCameraPosition.push_back(glm::vec3(22.6407f, 5.0f, -39.5186f));
	vecOfCameraPosition.push_back(glm::vec3(22.4826f, 5.0f, -39.6934f));
	vecOfCameraPosition.push_back(glm::vec3(22.3378f, 5.0f, -39.8534f));
	vecOfCameraPosition.push_back(glm::vec3(22.1711f, 5.0f, -40.0376f));
	vecOfCameraPosition.push_back(glm::vec3(22.0004f, 5.0f, -40.2264f));
	vecOfCameraPosition.push_back(glm::vec3(21.8576f, 5.0f, -40.3842f));
	vecOfCameraPosition.push_back(glm::vec3(21.7009f, 5.0f, -40.5573f));
	vecOfCameraPosition.push_back(glm::vec3(21.5284f, 5.0f, -40.748f));
	vecOfCameraPosition.push_back(glm::vec3(21.3873f, 5.0f, -40.904f));
	vecOfCameraPosition.push_back(glm::vec3(21.2066f, 5.0f, -41.1037f));
	vecOfCameraPosition.push_back(glm::vec3(21.0706f, 5.0f, -41.254f));
	vecOfCameraPosition.push_back(glm::vec3(20.9065f, 5.0f, -41.4354f));
	vecOfCameraPosition.push_back(glm::vec3(20.7515f, 5.0f, -41.6067f));
	vecOfCameraPosition.push_back(glm::vec3(20.592f, 5.0f, -41.783f));
	vecOfCameraPosition.push_back(glm::vec3(20.4393f, 5.0f, -41.9518f));
	vecOfCameraPosition.push_back(glm::vec3(20.2675f, 5.0f, -42.1417f));
	vecOfCameraPosition.push_back(glm::vec3(20.1006f, 5.0f, -42.3262f));
	vecOfCameraPosition.push_back(glm::vec3(19.939f, 5.0f, -42.5047f));
	vecOfCameraPosition.push_back(glm::vec3(19.7989f, 5.0f, -42.6596f));
	vecOfCameraPosition.push_back(glm::vec3(19.6327f, 5.0f, -42.8433f));
	vecOfCameraPosition.push_back(glm::vec3(19.4803f, 5.0f, -43.0117f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
	vecOfCameraPosition.push_back(glm::vec3(19.3225f, 5.0f, -43.1862f));
}

void initialAnimation() {
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.8764f, -6.3707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.9247f, -6.22589f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 12.9794f, -6.06197f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.0359f, -5.89237f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.0932f, -5.72054f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.1363f, -5.59126f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.1941f, -5.4178f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.2401f, -5.27959f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.2945f, -5.11665f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.3456f, -4.96312f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.401f, -4.79704f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.4507f, -4.64787f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.5075f, -4.4776f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.5579f, -4.32629f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.6105f, -4.16842f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.6658f, -4.00251f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.7149f, -3.85523f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.7714f, -3.68585f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.8302f, -3.50935f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.8728f, -3.38166f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.9272f, -3.2183f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 13.9788f, -3.06368f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.0347f, -2.896f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.0838f, -2.74862f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.1427f, -2.57199f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.1902f, -2.42933f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.2449f, -2.2654f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.3005f, -2.09856f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.3512f, -1.9464f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.4066f, -1.7802f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.4586f, -1.62421f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.5086f, -1.4741f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.5644f, -1.3069f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.6138f, -1.15867f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.6697f, -0.991077f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.7195f, -0.841615f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.7739f, -0.678365f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.8213f, -0.536213f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.8812f, -0.35655f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.9314f, -0.205984f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 14.9885f, -0.0346095f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.0383f, 0.114889f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.0881f, 0.264206f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.1371f, 0.411208f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.1947f, 0.584121f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.2436f, 0.730814f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.2992f, 0.897702f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.349f, 1.04682f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.4019f, 1.20578f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.453f, 1.35894f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.5087f, 1.52604f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.5594f, 1.67822f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.6233f, 1.86988f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.6657f, 1.99719f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.7207f, 2.1621f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.7693f, 2.30786f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.8239f, 2.47176f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.8751f, 2.62535f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.9308f, 2.79234f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 15.9827f, 2.94817f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.034f, 3.10209f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.0889f, 3.26661f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.1398f, 3.41945f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.193f, 3.57899f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.2441f, 3.73216f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.2963f, 3.88892f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.3551f, 4.06532f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.4032f, 4.20956f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.4575f, 4.37248f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.5104f, 4.53123f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.565f, 4.69496f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.6137f, 4.84097f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.6729f, 5.01862f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.7216f, 5.1647f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.7771f, 5.33135f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.8236f, 5.47086f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.8236f, 5.47086f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.8236f, 5.47086f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.8236f, 5.47086f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.8236f, 5.47086f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.8236f, 5.47086f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.87f, 5.61003f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.9252f, 5.77563f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 16.9784f, 5.93508f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.0273f, 6.0818f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.0806f, 6.24161f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.1361f, 6.40813f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.1891f, 6.56711f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.2407f, 6.72206f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.2935f, 6.8805f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.3471f, 7.04132f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.395f, 7.18499f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.4552f, 7.36545f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.5003f, 7.50093f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.5565f, 7.66944f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.6113f, 7.83389f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.6652f, 7.99555f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.712f, 8.13582f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.7685f, 8.30545f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.8154f, 8.44625f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.8746f, 8.62385f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.9261f, 8.77816f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 17.9808f, 8.94242f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.0297f, 9.08894f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.0916f, 9.27486f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.1358f, 9.40744f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.1908f, 9.57246f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.242f, 9.72589f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.2939f, 9.88166f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.346f, 10.0381f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.4011f, 10.2032f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.4502f, 10.3506f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.5066f, 10.5199f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.5555f, 10.6665f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.6108f, 10.8324f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.6654f, 10.9962f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.7197f, 11.1591f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.7693f, 11.3079f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.8182f, 11.4545f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.8703f, 11.6108f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.9282f, 11.7847f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 18.9752f, 11.9256f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.0309f, 12.0926f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.0798f, 12.2392f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.1351f, 12.4053f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.1845f, 12.5535f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.2394f, 12.718f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.2893f, 12.8679f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.352f, 13.056f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.3976f, 13.1927f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.4531f, 13.3593f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.5069f, 13.5206f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.5616f, 13.6848f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.608f, 13.8241f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.6633f, 13.9899f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.7116f, 14.1348f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.7651f, 14.2951f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.8155f, 14.4465f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.8761f, 14.6283f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.9282f, 14.7847f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 19.9794f, 14.9382f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.0327f, 15.098f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.0888f, 15.2663f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.1387f, 15.4161f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.1926f, 15.5777f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.2465f, 15.7395f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.2971f, 15.8911f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.3505f, 16.0514f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.403f, 16.209f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.454f, 16.362f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.5114f, 16.5341f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.5603f, 16.6809f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.6134f, 16.8403f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.6609f, 16.9826f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.7206f, 17.1617f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.7721f, 17.3163f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.8221f, 17.4663f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.8758f, 17.6273f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.931f, 17.7928f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 20.9766f, 17.9299f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.0352f, 18.1056f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.089f, 18.2669f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.1381f, 18.4144f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.193f, 18.5791f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.244f, 18.7318f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.2965f, 18.8896f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.3496f, 19.0489f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.4067f, 19.22f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.454f, 19.3619f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.5086f, 19.5257f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.5608f, 19.6823f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.6088f, 19.8265f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.6655f, 19.9965f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.7146f, 20.1437f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.7743f, 20.3228f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.8252f, 20.4756f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.8756f, 20.6267f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.928f, 20.784f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 21.9825f, 20.9473f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.0318f, 21.0953f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.0929f, 21.2785f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.1403f, 21.4209f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.1931f, 21.5792f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.241f, 21.723f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.2937f, 21.881f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.3466f, 22.0398f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.4058f, 22.2175f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.4518f, 22.3554f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.5092f, 22.5275f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.5569f, 22.6707f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.6166f, 22.8497f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.6684f, 23.0053f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.729f, 23.187f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.7688f, 23.3063f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.8264f, 23.4792f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.8784f, 23.6352f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.9317f, 23.795f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 22.9818f, 23.9453f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.0367f, 24.1102f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.0895f, 24.2685f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.1386f, 24.4157f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.1906f, 24.5718f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.2478f, 24.7434f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.3028f, 24.9083f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.3535f, 25.0605f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.4042f, 25.2125f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.4609f, 25.3825f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.5114f, 25.534f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.5672f, 25.7015f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.6106f, 25.8317f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.6696f, 26.0088f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.7172f, 26.1515f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.7752f, 26.3256f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.8325f, 26.4976f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.8802f, 26.6405f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.9319f, 26.7955f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 23.9871f, 26.9612f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.0338f, 27.1013f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.0912f, 27.2734f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.1474f, 27.4421f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.2006f, 27.6016f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.2513f, 27.7538f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.3146f, 27.9438f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.3533f, 28.0598f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.4023f, 28.2069f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.4536f, 28.3609f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.5169f, 28.5506f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.558f, 28.6739f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.6193f, 28.8579f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.6735f, 29.0206f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.7227f, 29.168f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.7713f, 29.3138f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.8318f, 29.4953f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.8757f, 29.6269f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.9329f, 29.7988f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 24.9846f, 29.9538f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.0407f, 30.1219f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.0857f, 30.2571f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.1426f, 30.4277f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.1971f, 30.5914f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.2476f, 30.7427f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.3039f, 30.9117f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.3577f, 31.073f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.4018f, 31.2053f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.4579f, 31.3737f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.5132f, 31.5396f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.5621f, 31.6863f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.6169f, 31.8508f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.6704f, 32.0111f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.7179f, 32.1537f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.7728f, 32.3184f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.8272f, 32.4815f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.885f, 32.6551f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.9341f, 32.8021f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 25.9929f, 32.9786f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.0357f, 33.107f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.0881f, 33.2644f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.1417f, 33.425f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.2007f, 33.6019f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.2447f, 33.7342f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.3087f, 33.926f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.3509f, 34.0527f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.404f, 34.2119f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.4578f, 34.3735f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.5092f, 34.5276f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.5691f, 34.7072f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.6272f, 34.8816f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.6695f, 35.0085f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.7295f, 35.1886f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.7827f, 35.3479f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.8298f, 35.4893f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.8827f, 35.648f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.9378f, 35.8134f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 26.9833f, 35.9497f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.0401f, 36.1203f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.0887f, 36.266f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.1475f, 36.4424f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.2035f, 36.6103f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.2588f, 36.7763f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.3076f, 36.9226f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.3574f, 37.072f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.4038f, 37.2114f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.4637f, 37.3909f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.5091f, 37.5272f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.5636f, 37.6908f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.6196f, 37.8588f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.6722f, 38.0167f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.7226f, 38.1677f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.7821f, 38.3463f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.8303f, 38.4909f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.8862f, 38.6586f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.9316f, 38.7946f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 27.9895f, 38.9684f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.0375f, 39.1124f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.0943f, 39.2828f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.1417f, 39.425f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.2017f, 39.605f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.2485f, 39.7455f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.3111f, 39.9334f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.3528f, 40.0583f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.4089f, 40.2267f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.4577f, 40.373f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.5139f, 40.5417f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.5709f, 40.7128f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.6208f, 40.8623f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.6786f, 41.0356f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.7297f, 41.1891f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.7764f, 41.3291f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.8287f, 41.4859f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.88f, 41.6399f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.9372f, 41.8115f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 28.9907f, 41.972f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.0385f, 42.1154f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.0923f, 42.2767f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.1495f, 42.4486f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.1943f, 42.5829f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.2528f, 42.7583f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.3051f, 42.9154f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.3643f, 43.0929f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.4117f, 43.2351f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.4735f, 43.4206f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.5134f, 43.5402f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.5744f, 43.7231f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.6205f, 43.8613f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.6757f, 44.027f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.7243f, 44.1729f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.7824f, 44.3471f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.834f, 44.5018f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.8857f, 44.6572f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.9434f, 44.8301f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 29.9953f, 44.9859f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.0426f, 45.1278f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.096f, 45.2881f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.1443f, 45.4327f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.1977f, 45.593f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.2513f, 45.7538f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.3087f, 45.9259f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.3571f, 46.0713f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.4121f, 46.2363f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.4744f, 46.423f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.515f, 46.545f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.5683f, 46.7049f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.6263f, 46.8787f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.6727f, 47.0181f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.7289f, 47.1865f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.779f, 47.337f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.8359f, 47.5077f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.8914f, 47.6743f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.9366f, 47.8098f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 30.9875f, 47.9625f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.0409f, 48.1225f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.0916f, 48.2748f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.1457f, 48.437f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.2077f, 48.6232f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.2633f, 48.7898f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.3136f, 48.9406f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.3664f, 49.0993f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.415f, 49.2451f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.4773f, 49.4319f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.5239f, 49.5717f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.5707f, 49.712f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.6195f, 49.8584f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.6757f, 50.027f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.724f, 50.1721f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.7808f, 50.3424f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.8325f, 50.4974f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.8908f, 50.6722f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.9429f, 50.8288f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 31.9991f, 50.9973f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.0428f, 51.1284f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.0959f, 51.2876f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.1478f, 51.4432f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.2011f, 51.6032f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.2527f, 51.7582f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.3121f, 51.9362f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.3599f, 52.0795f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.4132f, 52.2396f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.4724f, 52.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.5174f, 52.5522f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.5678f, 52.7034f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.6265f, 52.8794f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.6812f, 53.0435f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.7389f, 53.2165f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.783f, 53.3491f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.8392f, 53.5174f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.8867f, 53.6599f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.9477f, 53.843f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 32.9891f, 53.9673f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.0468f, 54.1404f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.0933f, 54.2798f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.1474f, 54.4422f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.1983f, 54.5949f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.2593f, 54.7777f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.3054f, 54.9163f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.3673f, 55.1018f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.4115f, 55.2346f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.4655f, 55.3965f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.5157f, 55.5469f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.5744f, 55.7231f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.6231f, 55.8691f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.6769f, 56.0305f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.7276f, 56.1828f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.7838f, 56.3515f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.8365f, 56.5093f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.8912f, 56.6736f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.9405f, 56.8214f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 33.9978f, 56.9933f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.0459f, 57.1377f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.1009f, 57.3026f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.1536f, 57.4608f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.2054f, 57.616f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.2542f, 57.7626f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.3082f, 57.9245f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.3609f, 58.0827f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.4176f, 58.2527f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.4711f, 58.4133f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.5242f, 58.5724f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.5693f, 58.7078f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.6323f, 58.8967f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.6787f, 59.0361f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.7326f, 59.1978f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.7803f, 59.3409f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.8397f, 59.5189f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.8881f, 59.6641f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.9451f, 59.8352f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 34.9922f, 59.9765f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.0474f, 60.142f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.1018f, 60.3052f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.1511f, 60.4532f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.2025f, 60.6073f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.2592f, 60.7775f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.3115f, 60.9343f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.3677f, 61.1031f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.4131f, 61.2393f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.4715f, 61.4145f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.5184f, 61.5551f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.5768f, 61.7303f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.6294f, 61.888f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.677f, 62.0308f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.7305f, 62.1913f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.7877f, 62.3629f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.8362f, 62.5084f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.8958f, 62.6872f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 35.9462f, 62.8385f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.0013f, 63.0039f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.0505f, 63.1514f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.1043f, 63.3127f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.1501f, 63.4502f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.2068f, 63.6203f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.2566f, 63.7699f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.3115f, 63.9344f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.365f, 64.095f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.4183f, 64.2548f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.4707f, 64.4119f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.5258f, 64.5773f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.5731f, 64.7192f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.63f, 64.8899f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.6818f, 65.0454f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.7335f, 65.2004f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.7828f, 65.3482f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.8407f, 65.522f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.8887f, 65.666f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.944f, 65.832f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 36.9943f, 65.9829f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.0477f, 66.1431f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.1009f, 66.3025f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.1547f, 66.4641f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.2044f, 66.6132f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.2619f, 66.7855f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.3194f, 66.958f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.3667f, 67.1001f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.4172f, 67.2515f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.4747f, 67.4241f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.5203f, 67.5609f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.5773f, 67.7318f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.6262f, 67.8784f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.6838f, 68.0514f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.7303f, 68.1908f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.7898f, 68.3694f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.8358f, 68.5074f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.8983f, 68.6947f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.9456f, 68.8366f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 37.9961f, 68.9881f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.0464f, 69.1392f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.1021f, 69.3062f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.1526f, 69.4576f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.2098f, 69.6293f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.2584f, 69.775f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.3116f, 69.9348f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.3617f, 70.0849f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.4193f, 70.2579f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.4679f, 70.4036f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.5296f, 70.5887f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.5735f, 70.7205f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.6327f, 70.898f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.6809f, 71.0427f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.7365f, 71.2095f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.7902f, 71.3705f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.8387f, 71.516f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.8897f, 71.669f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.9496f, 71.8487f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 38.9975f, 71.9923f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.054f, 72.1618f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.1075f, 72.3222f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.1562f, 72.4683f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.2097f, 72.629f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.2644f, 72.7932f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.3124f, 72.937f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.3679f, 73.1036f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4172f, 73.2515f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
	vecOfCameraPosition.push_back(glm::vec3(25.0f, 39.4724f, 73.4171f));
}

void recordAnimation() {
	GLfloat x = myCamera.getCameraPosition().x;
	if (x == (int)x)
		std::cout << "vecOfCameraPosition.push_back(glm::vec3(" << x << ".0f, ";
	else
		std::cout << "vecOfCameraPosition.push_back(glm::vec3(" << x << "f, ";

	GLfloat y = myCamera.getCameraPosition().y;
	if (y == (int)y)
		std::cout << y << ".0f, ";
	else
		std::cout << y << "f, ";

	GLfloat z = myCamera.getCameraPosition().z;
	if (z == (int)z)
		std::cout << z << ".0f));" << std::endl;
	else
		std::cout << z << "f));" << std::endl;

	if (false) {
		GLfloat x2 = myCamera.getCameraTarget().x;
		if (x2 == (int)x2)
			std::cout << "vecOfCameraTarget.push_back(glm::vec3(" << x2 << ".0f, ";
		else
			std::cout << "vecOfCameraTarget.push_back(glm::vec3(" << x2 << "f, ";

		GLfloat y2 = myCamera.getCameraTarget().y;
		if (y2 == (int)y2)
			std::cout << y2 << ".0f, ";
		else
			std::cout << y2 << "f, ";

		GLfloat z2 = myCamera.getCameraTarget().z;
		if (z2 == (int)z2)
			std::cout << z2 << ".0f));" << std::endl;
		else
			std::cout << z2 << "f));" << std::endl;
	}
}


bool unique = true;

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//recordAnimation();
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	cameraSpeed = 10.0f * deltaTime;
	if (myCamera.getCameraPosition().x == 19.3225f && myCamera.getCameraPosition().y == 5.0f && myCamera.getCameraPosition().z == -43.1862f  && unique == true) {
		myCamera.setPosition(19.3225f, 5.0f, -43.1862f);
		myCamera.setTarget(+0.0f, 2.0f, -10.0f);
		firstAnimation = false;
		unique = false;
	}
	else if (frame % 1 == 0 && unique == true){
			myCamera.setPosition(vecOfCameraPosition[frame].x, vecOfCameraPosition[frame].y, vecOfCameraPosition[frame].z);
	}

	frame += 1;
	determineBoudingBox();

	computeGalaxyRotationAngle();
	processMovement();
	spaceShipComputeMove();
	ufoComputeMove();
	flashLightComputeMove();

	//render the scene to the depth buffer (first pass)

	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

		
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	if (generateShadows) {
		////////////////////////////////////////////
		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -0.5f, -1.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		building.Draw(depthMapShader);

		//electric_building
		model = glm::translate(glm::mat4(1.0f), glm::vec3(47.0f, -0.8f, -59.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		electric_building.Draw(depthMapShader);

		//electric_generator
		model = glm::translate(glm::mat4(1.0f), glm::vec3(48.5f, 3.0f, 8.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		electric_generator.Draw(depthMapShader);

		//oil_pump
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.2f, -45.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		oil_pump.Draw(depthMapShader);

		//oil_tank
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -4.2f, -73.0f));
		model = glm::rotate(model, glm::radians(225.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(3.5f, 3.5f, 3.5f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		oil_tank.Draw(depthMapShader);

		if (!turnOnFlashLight) {
			//robot
			model = glm::translate(glm::mat4(1.0f), glm::vec3(+20.0f + yTranslateRobot, 3.0f, -60.0f + xTranslateRobot));
			model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f));
			glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
			robot.Draw(depthMapShader);

			//flashlight
			model = glm::translate(glm::mat4(1.0f), glm::vec3(21.85f + yTranslateRobot, 1.97f, -56.9f + xTranslateRobot));
			model = glm::rotate(model, glm::radians(-180.0f + flashLightAngle), glm::vec3(0, 1, 0));
			model = glm::scale(model, glm::vec3(0.16f, 0.16f, 0.16f));
			glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
			flashLight.Draw(depthMapShader);
		}

		//spaceship
		model = glm::translate(glm::mat4(1.0f), glm::vec3(+25.0f, 7.0f + zTranslateSpaceship, -25.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		spaceship.Draw(depthMapShader);

		//portal
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 5.0f, 27.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		portal.Draw(depthMapShader);

		//galaxy
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 5.0f, 27.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(galaxyRotationAngle), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		galaxy.Draw(depthMapShader);

		//first_wall
		model = glm::translate(glm::mat4(1.0f), glm::vec3(+22.4f, -1.5f, -80.0f));
		model = glm::scale(model, glm::vec3(147.0f, 200.0f, 147.0f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		wall.Draw(depthMapShader);

		//second_wall
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-12.1f, -2.6f, -25.4f));
		model = glm::rotate(model, glm::radians(90.f), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(214.0f, 222.5f, 194.0f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		wall.Draw(depthMapShader);

		//third_wall
		model = glm::translate(glm::mat4(1.0f), glm::vec3(+60.0f, -0.5f, -30.0f));
		model = glm::rotate(model, glm::radians(-90.f), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(214.0f, 180.0f, 147.0f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		wall.Draw(depthMapShader);

		//centaur
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -0.45f, 19.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		centaur.Draw(depthMapShader);
	}
	
	////////////////////////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//render the scene (second pass)
	myCustomShader.useShaderProgram();

	//send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	//send view matrix to shader
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(view));	

	//compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	//send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightPos"), 1,
		glm::value_ptr(glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle),
			glm::vec3(0.0f, 1.0f, 0.0f))* glm::vec4(lightDir, 1.0f))));

	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "cameraPostion"), 1,
		glm::value_ptr(myCamera.getCameraPosition()));

	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "cameraTarget"), 1,
		glm::value_ptr(myCamera.getCameraTarget()));

	glViewport(0, 0, retina_width, retina_height);
	myCustomShader.useShaderProgram();

	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);
	
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "generateFog"), generateFog);

	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "turnOnFlashLight"), turnOnFlashLight);


	//centaur
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -0.45f, 19.0f));
	model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	centaur.Draw(myCustomShader);

	//building
	model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -0.5f, -1.0f));
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	building.Draw(myCustomShader);

	//electric_building
	model = glm::translate(glm::mat4(1.0f), glm::vec3(47.0f, -0.8f, -59.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	electric_building.Draw(myCustomShader);

	//electric_generator
	model = glm::translate(glm::mat4(1.0f), glm::vec3(48.5f, 3.0f, 8.0f));
	model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	electric_generator.Draw(myCustomShader);

	//oil_pump
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.2f, -45.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	oil_pump.Draw(myCustomShader);

	//oil_tank
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -4.2f, -73.0f));
	model = glm::rotate(model, glm::radians(225.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(3.5f, 3.5f, 3.5f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	oil_tank.Draw(myCustomShader);


	if (!turnOnFlashLight) {
		//robot
		model = glm::translate(glm::mat4(1.0f), glm::vec3(+20.0f + yTranslateRobot, 3.0f, -60.0f + xTranslateRobot));
		model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		robot.Draw(myCustomShader);

		//flashlight
		model = glm::translate(glm::mat4(1.0f), glm::vec3(21.85f + yTranslateRobot, 1.97f, -56.9f + xTranslateRobot));
		model = glm::rotate(model, glm::radians(-180.0f + flashLightAngle), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(0.16f, 0.16f, 0.16f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		flashLight.Draw(myCustomShader);
	}


	//mars
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "generateFog"), false);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "computeAttenuation"), false);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(+200.0f, +200.0f, -200.0f));
	model = glm::scale(model, glm::vec3(40.0f, 40.0f, 40.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	mars.Draw(myCustomShader);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "generateFog"), generateFog);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "computeAttenuation"), true);

	//ground
	model = glm::translate(glm::mat4(1.0f), glm::vec3(+24.0f, -7.2f, -28.0f));
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	ground.Draw(myCustomShader);

	//spaceship
	model = glm::translate(glm::mat4(1.0f), glm::vec3(+25.0f, 7.0f + zTranslateSpaceship, -25.0f));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	spaceship.Draw(myCustomShader);

	//portal
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 5.0f, 27.0f));
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	portal.Draw(myCustomShader);

	//galaxy
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 5.0f, 27.0f));
	model = glm::rotate(model, glm::radians(90.f), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(galaxyRotationAngle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	galaxy.Draw(myCustomShader);


	//first_wall
	model = glm::translate(glm::mat4(1.0f), glm::vec3(+22.4f, -1.5f, -80.0f));
	model = glm::scale(model, glm::vec3(147.0f, 200.0f, 147.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	wall.Draw(myCustomShader);

	//second_wall
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-12.1f, -2.6f, -25.4f));
	model = glm::rotate(model, glm::radians(90.f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(214.0f, 222.5f, 194.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	wall.Draw(myCustomShader);

	//third_wall
	model = glm::translate(glm::mat4(1.0f), glm::vec3(+60.0f, -0.5f, -30.0f));
	model = glm::rotate(model, glm::radians(-90.f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(214.0f, 180.0f, 147.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	wall.Draw(myCustomShader);

	//ufo
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "generateFog"), false);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "computeAttenuation"), false);
	model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(+6.0f, 20.0f, -15.0f));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	ufo.Draw(myCustomShader);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "generateFog"), generateFog);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "computeAttenuation"), true);

	//ufoLight
	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(+5.8f, 22.5f, -14.9f));
	model = glm::scale(model, glm::vec3(14.0f, 14.0f, 14.0f));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	ufoLight.Draw(lightShader);
	skyBoxAngle += 0.005;
	if (skyBoxAngle > 360.0f)
		skyBoxAngle -= 360.0f;
	//skybox
	skyboxShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	model = glm::rotate(glm::mat4(1.0f), glm::radians(-skyBoxAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	mySkyBox.Draw(skyboxShader, view, projection);
}


int main(int argc, const char * argv[]) {

	initOpenGLWindow();
	initOpenGLState();
	initFBOs();
	initModels();
	initShaders();
	initUniforms();	
	initialAnimation();
	secondAnimation();
	glCheckError();
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
