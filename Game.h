#pragma once

#include "libs.h"
#include "Camera.h"

#pragma warning(disable:5208)
#define NOMINMAX
#include <limits>


//ENUMERATIONS
enum shader_enum { SHADER_CORE_PROGRAM = 0, ALT_SHADER = 1};
enum texture_enum { TEX_PUSHEEN = 0, TEX_PUSHEEN_SPECULAR, TEX_CONTAINER, TEX_CONTAINER_SPECULAR };
enum material_enum { MAT_1 = 0 };
enum mesh_enum { MESH_QUAD = 0 };

class Game
{
private:
//Variables
	//Window
	GLFWwindow* window;
	const int WINDOW_WIDTH;
	const int WINDOW_HEIGHT;
	int framebufferWidth;
	int framebufferHeight;

	//OpenGL Context
	const int GL_VERSION_MAJOR;
	const int GL_VERSION_MINOR;

	//Delta time
	float dt;
	float curTime;
	float lastTime;

	//Mouse Input
	double lastMouseX;
	double lastMouseY;
	double mouseX;
	double mouseY;
	double mouseOffsetX;
	double mouseOffsetY;
	bool firstMouse;

	//Camera
	Camera camera;

	//Matrices
	glm::mat4 ViewMatrix;
	glm::vec3 camPosition;
	glm::vec3 worldUp;
	glm::vec3 camFront;

	glm::mat4 ProjectionMatrix;
	float fov;
	float nearPlane;
	float farPlane;

	//Shaders
	std::vector<Shader*> shaders;

	//Textures
	std::vector<Texture*> textures;

	//Materials
	std::vector<Material*> materials;

	//Models
	std::vector<Model*> models;

	//Terrain
	std::vector<Model*> terrain;

	//Lights
	std::vector<PointLight*> pointLights;

//Private functions
	void initGLFW();
	void initWindow(
		const char* title,
		bool resizable
	);
	void initGLEW(); //AFTER CONTEXT CREATION!!!
	void initOpenGLOptions();
	void initMatrices();
	void initShaders();
	void initTextures();
	void initMaterials();
	void initOBJModels();
	void genChunkModels(unsigned int, unsigned int);
	void initPointLights();
	void initLights();
	void initUniforms();

	void updateUniforms();

//Static variables

public:
//Constructors / Destructors
	Game(
		const char* title,
		const int WINDOW_WIDTH, const int WINDOW_HEIGHT,
		const int GL_VERSION_MAJOR, const int GL_VERSION_MINOR,
		bool resizable
	);
	virtual ~Game();

//Accessors
	int getWindowShouldClose();

//Modifiers
	void setWindowShouldClose();

//Functions
	void updateDt();
	void updateMouseInput();
	void updateKeyboardInput();
	void updateChunks(glm::vec3);
	void updateInput();
	void updateBugDecisions(glm::vec3 cameraPos);
	void bugDriver();
	void update();
	void render();

//Static functions
	static void framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH);
};