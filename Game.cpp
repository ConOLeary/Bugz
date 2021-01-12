




#include "Game.h"
#include <random>
#include <math.h>
#include <queue>
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 

#define CHUNK_SIZE 150
#define ROCKS_PER_CHUNK 400
#define BUGS_PER_CHUNK 10 ///////////////////////////// CMDFBUGS
#define MAX_ROCK_SIZE 3
#define MAX_CHUNK_DIMENSION 100
#define BIG_ROCK_RARITY 800
#define CHECK_CHUNKS_EVERY 10
#define RARITY_BEHAVIOUR_CHANGE 15
//GLuint texture0;
glm::vec3 DIRTY_ROCK = glm::vec3(0.39f, 0.33f, 0.28f);
glm::vec3 WILD_FIELD = glm::vec3(0.59f, 0.60f, 0.39f);
glm::vec3 NUKE_LIGHT = glm::vec3(1.f, 0.3f, 0.1f);
glm::vec3 SUN_LIGHT = glm::vec3(1.f, 1.f, 0.95f);

glm::vec3 SUN_LOCATION = glm::vec3(0.f, -4.f, 0.f);

float WALK = 6.0f;
float RUN = 30.0f;
glm::vec3 ROTATE = glm::vec3(0.f, 5.0f, 0.f);
std::vector<MeshObj*>bugz;
float ROCK_HEIGHT = -0.2;
int chunkMap[MAX_CHUNK_DIMENSION][MAX_CHUNK_DIMENSION];
unsigned int updateCounter;
ModelData myModelData;
int headlightOn;


//MESH TO LOAD
//----------------------------------------------------------------------------*/
//// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
//// put the mesh in your project directory, or provide a filepath for it here
#define MESH_NAME "Objects/ladybug_black.obj"


//Private functions
void Game::initGLFW()
{
	//INIT GLFW
	if (glfwInit() == GLFW_FALSE)
	{
		std::cout << "ERROR::GLFW_INIT_FAILED" << "\n";
		glfwTerminate();
	}
}

void Game::initWindow(
	const char* title,
	bool resizable
)
{
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->GL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->GL_VERSION_MINOR);
	glfwWindowHint(GLFW_RESIZABLE, resizable);

	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); MAC OS

	this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, title, NULL, NULL);

	if (this->window == nullptr)
	{
		std::cout << "ERROR::GLFW_WINDOW_INIT_FAILED" << "\n";
		glfwTerminate();
	}

	glfwGetFramebufferSize(this->window, &this->framebufferWidth, &this->framebufferHeight);
	glfwSetFramebufferSizeCallback(window, Game::framebuffer_resize_callback);
	//IMPORTANT WHITH PERSPECTIVE MATRIX!!!

	//glViewport(0, 0, framebufferWidth, framebufferHeight);

	glfwMakeContextCurrent(this->window); //IMPORTANT!!
}

void Game::initGLEW()
{
	//INIT GLEW (NEEDS WINDOW AND OPENGL CONTEXT)
	glewExperimental = GL_TRUE;

	//Error
	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR::MAIN.CPP::GLEW_INIT_FAILED" << "\n";
		glfwTerminate();
	}
}

void Game::initOpenGLOptions()
{
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Input
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Game::initMatrices()
{
	this->ViewMatrix = glm::mat4(1.f);
	this->ViewMatrix = glm::lookAt(this->camPosition, this->camPosition + this->camFront, this->worldUp);

	this->ProjectionMatrix = glm::mat4(1.f);
	this->ProjectionMatrix = glm::perspective(
		glm::radians(this->fov),
		static_cast<float>(this->framebufferWidth) / this->framebufferHeight,
		this->nearPlane,
		this->farPlane
	);
}

void Game::initShaders()
{
	this->shaders.push_back(new Shader(this->GL_VERSION_MAJOR, this->GL_VERSION_MINOR,
		"vertex_core.glsl", "fragment_core.glsl"));
}

void Game::initTextures()
{
	////////////////////////////////////
	/*
	int image_width = 0;
	int image_height = 0;
	unsigned char* image = SOIL_load_image("Images/hud1.png", &image_width, &image_height, NULL, SOIL_LOAD_RGBA);
	//GLuint texture0;
	glGenTextures(1, &texture0);
	glBindTexture(GL_TEXTURE_2D, texture0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if (image)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "texture load fail";
	}
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);*/
	///////////////////////////////////////////////////////////////////////////////////////
	
	//TEXTURE 0
	this->textures.push_back(new Texture("Images/pusheen.png", GL_TEXTURE_2D));
	this->textures.push_back(new Texture("Images/pusheen_specular.png", GL_TEXTURE_2D));

	//TEXTURE 1
	this->textures.push_back(new Texture("Images/container.png", GL_TEXTURE_2D));
	this->textures.push_back(new Texture("Images/container_specular.png", GL_TEXTURE_2D));

	//TEXTURE 2
	this->textures.push_back(new Texture("Images/hud1.png", GL_TEXTURE_2D));
	
}

void Game::initMaterials()
{
	this->materials.push_back(new Material(DIRTY_ROCK, DIRTY_ROCK, DIRTY_ROCK,
		0, 1));
	glm::vec3 lessShinyField = glm::vec3(WILD_FIELD[0] * 0.5, WILD_FIELD[1] * 0.5, WILD_FIELD[2] * 0.5);
	this->materials.push_back(new Material(lessShinyField, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f),
		0, 1));
}

ModelData readInObj(const char* file_name)
{

	ModelData modelData;
	const aiScene* scene = aiImportFile(
		file_name,
		aiProcess_Triangulate | aiProcess_PreTransformVertices
	);

	if (!scene) {
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return modelData;
	}

	printf("  %i materials\n", scene->mNumMaterials);
	printf("  %i meshes\n", scene->mNumMeshes);
	printf("  %i textures\n", scene->mNumTextures);

	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		printf("    %i vertices in mesh\n", mesh->mNumVertices);
		modelData.mPointCount += mesh->mNumVertices;
		for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
			if (mesh->HasPositions()) {
				const aiVector3D* vp = &(mesh->mVertices[v_i]);
				modelData.mVertices.push_back(glm::vec3(vp->x, vp->y, vp->z));
			}
			if (mesh->HasNormals()) {
				const aiVector3D* vn = &(mesh->mNormals[v_i]);
				modelData.mNormals.push_back(glm::vec3(vn->x, vn->y, vn->z));
			}
			if (mesh->HasTextureCoords(0)) {
				const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
				modelData.mTextureCoords.push_back(glm::vec2(vt->x, vt->y));
			}
			if (mesh->HasTangentsAndBitangents()) {
				/* You can extract tangents and bitangents here              */
				/* Note that you might need to make Assimp generate this     */
				/* data for you. Take a look at the flags that aiImportFile  */
				/* can take.                                                 */
			}
		}
	}
	aiReleaseImport(scene);
	return modelData;
}

void Game::initOBJModels()
{
	myModelData = readInObj(MESH_NAME);
	bugz.push_back(new MeshObj(myModelData,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(.05f)
	)
	);
}

int genRandy(int upper, int lower)
{
	
	int num = (rand() % (upper - lower + 1)) + lower;
	return num;
}

Material* genMaterial()
{
	glm::vec3 ambient = glm::vec3(((float)genRandy(100.0, 0.0)) / 100, ((float)genRandy(100.0, 0.0)) / 100, ((float)genRandy(100.0, 0.0)) / 100);
	srand(time(0));
	glm::vec3 diffuse = glm::vec3(((float)genRandy(100.0, 0.0)) / 100, ((float)genRandy(100.0, 0.0)) / 100, ((float)genRandy(100.0, 0.0)) / 100);
	srand(time(0));
	glm::vec3 specular = glm::vec3(((float)genRandy(100.0, 0.0)) / 100, ((float)genRandy(100.0, 0.0)) / 100, ((float)genRandy(100.0, 0.0)) / 100);
	/*cout << "ambient: " << glm::to_string(ambient) << "\n\n";
	cout << "diffuse: " << glm::to_string(diffuse) << "\n\n";
	cout << "specular: " << glm::to_string(specular) << "\n\n";*/

	return (new Material(ambient, diffuse, specular, 0, 1));
	/*Material(
		glm::vec3 ambient,
		glm::vec3 diffuse,
		glm::vec3 specular,
		GLint diffuseTex,
		GLint specularTex
	)*/
}
void Game::genChunkModels(unsigned int chunkZ, unsigned int chunkX) //################################# CHUNK LOADER #################################
{
	srand(time(0));
	//cout << "chunkZ: " << to_string(chunkZ) << "\n\n";
	//cout << "chunkX: " << to_string(chunkX) << "\n\n";
	
	std::vector<Mesh*>rocks;
	std::vector<Mesh*>grounds;
	

	int xChunkOffset = chunkX * CHUNK_SIZE;
	int zChunkOffset = chunkZ * CHUNK_SIZE;
	//cout << "floor origin: " << glm::to_string(glm::vec3(xChunkOffset - (float)(CHUNK_SIZE / 2), ROCK_HEIGHT - 0.5f, zChunkOffset - (float)(CHUNK_SIZE / 2))) << "\n\n";

	int amountBugz = genRandy(BUGS_PER_CHUNK, 0);

	for (int i = 0; i < amountBugz; i++)
	{
		float bugX = (float)(genRandy(0, -CHUNK_SIZE));
		float bugZ = (float)(genRandy(0, -CHUNK_SIZE));
		//bugzzz
		this->models.push_back(new Model(
			glm::vec3(bugX + xChunkOffset, ROCK_HEIGHT, -bugZ - zChunkOffset),
			genMaterial(),
			this->textures[TEX_CONTAINER],
			this->textures[TEX_CONTAINER_SPECULAR],
			bugz
		)
		);
	}


	int amountRocks = genRandy(ROCKS_PER_CHUNK, 0);
	//cout << "AMOUNT ROCKS: " << to_string(amountRocks) << "\n\n";
	#pragma omp parallel 
	{
		for (int i = 0; i < amountRocks; i++)
		{
			float rockSize = 1.f;
			int num = genRandy(BIG_ROCK_RARITY, 0);
			//cout << "num: " << to_string(num) << "\n\n";
			switch (num)
			{
			case 0:
				rockSize = 32.f;
				break;
			case 1:
			case 2:
				rockSize = 16.f;
				break;
			case 3:case 4:case 5:case 6:
				rockSize = 8.f;
				break;
			case 7:case 8:case 9:case 10:case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:case 19:case 20:case 21:case 22:
				rockSize = 4.f;
				break;
			case 23:case 24:case 25:case 26:case 27:case 28:case 29:case 30:case 31:case 32:case 33:case 34:case 35:case 36:case 37:case 38:
			case 39:case 40:case 41:case 42:case 43:case 44:case 45:case 46:case 47:case 48:case 49:case 50:case 51:case 52:case 53:case 54:
				rockSize = 2.f;
				break;
			defaut: break;
			}

			float rockX = (float)(genRandy(0, -CHUNK_SIZE));
			float rockZ = (float)(genRandy(0, -CHUNK_SIZE));
			/*cout << "rockX: " << to_string(rockX) << "\n\n";
			cout << "rockZ: " << to_string(rockZ) << "\n\n";
			cout << "zChunkOffset: " << to_string(zChunkOffset) << "\n\n";
			cout << "xChunkOffset: " << to_string(xChunkOffset) << "\n\n";*/

			rocks.push_back(
				new Mesh(
					&Pyramid(),
					glm::vec3(rockX + xChunkOffset, ROCK_HEIGHT, -rockZ - zChunkOffset),
					glm::vec3(0.f),
					glm::vec3(0.f),
					glm::vec3(rockSize)
				)
			);
		}

	}


	// teh ffloooorr
	grounds.push_back(
		new Mesh(
			&Quad(),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(-90.f, 0.f, 0.f),
			glm::vec3((float)CHUNK_SIZE)
		)
	);

	this->terrain.push_back(new Model(
		glm::vec3(0.f),
		this->materials[0],
		this->textures[TEX_CONTAINER],
		this->textures[TEX_CONTAINER_SPECULAR],
		rocks
	)
	);

	//teh flooorr
	this->terrain.push_back(new Model(
		glm::vec3(xChunkOffset - (float)(CHUNK_SIZE / 2), ROCK_HEIGHT - 0.5f, -zChunkOffset + (float)(CHUNK_SIZE / 2)),
		//glm::vec3(zChunkOffset - (float)(CHUNK_SIZE / 2), ROCK_HEIGHT - 0.5f, -xChunkOffset + (float)(CHUNK_SIZE / 2)),
		this->materials[1],
		this->textures[TEX_CONTAINER],
		this->textures[TEX_CONTAINER_SPECULAR],
		grounds
	)
	);


	/*this->models.push_back(new Model(
		glm::vec3(rockX + xChunkOffset, ROCK_HEIGHT, rockZ + zChunkOffset),
		this->materials[0],
		this->textures[TEX_CONTAINER],
		this->textures[TEX_CONTAINER_SPECULAR],
		rocks
	)
	);*/

	/*
	this->models.push_back(new Model(
		glm::vec3(4.f, 0.f, 4.f),
		this->materials[0],
		this->textures[TEX_CONTAINER],
		this->textures[TEX_CONTAINER_SPECULAR],
		"OBJFiles/ddh.obj"
	)
	);*/
	
	for (auto*& i : rocks)
		delete i;

	for (auto*& i : grounds)
		delete i;
}


void Game::initPointLights()
{
	this->pointLights.push_back(new PointLight(glm::vec3(0.f))); //click-placed nuke light
	//this->pointLights.push_back(new PointLight(glm::vec3(0.f, 50.f, 0.f), 1.f, SUN_LIGHT)); //sun
	
	
}

void Game::initLights()
{
	this->initPointLights();
}

void Game::initUniforms()
{
	//INIT UNIFORMS
	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(ViewMatrix, "ViewMatrix");
	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(ProjectionMatrix, "ProjectionMatrix");
	
	for each (PointLight* pl in this->pointLights)
	{

		pl->sendToShader(*this->shaders[SHADER_CORE_PROGRAM]);
	}
	for each (Material * mtl in this->materials) {
		mtl->sendToShader(*this->shaders[SHADER_CORE_PROGRAM]);
	}
}

void Game::updateUniforms()
{
	//Update view matrix (camera)
	this->ViewMatrix = this->camera.getViewMatrix();

	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(this->ViewMatrix, "ViewMatrix");
	this->shaders[SHADER_CORE_PROGRAM]->setVec3f(this->camera.getPosition(), "cameraPos");

	//int i = 0;
	for each (PointLight* pl in this->pointLights)
	{
		//cout << "i: " << to_string(i) << "\n";
		//cout << "i: " << to_string(i) << ". Position: " << glm::to_string(pl->getPosition()) << "\n";
		//i++;
		pl->sendToShader(*this->shaders[SHADER_CORE_PROGRAM]);
	}

	//for each (Material * mtl in this->materials) {
	//	mtl->sendToShader(*this->shaders[SHADER_CORE_PROGRAM]);
	//}

	//Update framebuffer size and projection matrix
	glfwGetFramebufferSize(this->window, &this->framebufferWidth, &this->framebufferHeight);

	this->ProjectionMatrix = glm::perspective(
		glm::radians(this->fov),
		static_cast<float>(this->framebufferWidth) / this->framebufferHeight,
		this->nearPlane,
		this->farPlane
	);

	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(this->ProjectionMatrix, "ProjectionMatrix");
	
}

//Constructors / Destructors
Game::Game(
	const char* title,
	const int WINDOW_WIDTH, const int WINDOW_HEIGHT,
	const int GL_VERSION_MAJOR, const int GL_VERSION_MINOR,
	bool resizable
)
	:
	WINDOW_WIDTH(WINDOW_WIDTH),
	WINDOW_HEIGHT(WINDOW_HEIGHT),
	GL_VERSION_MAJOR(GL_VERSION_MAJOR),
	GL_VERSION_MINOR(GL_VERSION_MINOR),
	camera(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f))
{

	//Init variables
	for (int i = 0; i < MAX_CHUNK_DIMENSION; i++)
	{
		for (int j = 0; j < MAX_CHUNK_DIMENSION; j++)
		{
			chunkMap[i][j] = 0;
		}
	}
	updateCounter = 0;
	headlightOn = 0;

	this->window = nullptr;
	this->framebufferWidth = this->WINDOW_WIDTH;
	this->framebufferHeight = this->WINDOW_HEIGHT;

	this->camPosition = glm::vec3(0.f, 0.f, 1.f);
	this->worldUp = glm::vec3(0.f, 1.f, 0.f);
	this->camFront = glm::vec3(0.f, 0.f, -1.f);

	this->fov = 90.f;
	this->nearPlane = 0.1f;
	this->farPlane = 1000.f;

	this->dt = 0.f;
	this->curTime = 0.f;
	this->lastTime = 0.f;

	this->lastMouseX = 0.0;
	this->lastMouseY = 0.0;
	this->mouseX = 0.0;
	this->mouseY = 0.0;
	this->mouseOffsetX = 0.0;
	this->mouseOffsetY = 0.0;
	this->firstMouse = true;

	this->initGLFW();
	this->initWindow(title, resizable);
	this->initGLEW();
	this->initOpenGLOptions();

	this->initMatrices();
	this->initShaders();
	this->initTextures();
	this->initMaterials();
	this->initOBJModels();

	/*Mesh(ModelData modelData,
		glm::vec3 position = glm::vec3(0.f),
		glm::vec3 origin = glm::vec3(0.f),
		glm::vec3 rotation = glm::vec3(0.f),
		glm::vec3 scale = glm::vec3(1.f))*/
	this->genChunkModels(0, 0);
	this->genChunkModels(0, 1);
	this->genChunkModels(1, 0);
	this->genChunkModels(1, 1);
	chunkMap[49][49] = 1;
	chunkMap[49][50] = 1;
	chunkMap[50][49] = 1;            
	chunkMap[50][50] = 1;
	this->initLights();
	this->initUniforms();
	int j = 0;

}

Game::~Game()
{
	glfwDestroyWindow(this->window);
	glfwTerminate();

	for (unsigned int i = 0; i < this->shaders.size(); i++)
		delete this->shaders[i];
	
	for (unsigned int i = 0; i < this->textures.size(); i++)
		delete this->textures[i];

	for (unsigned int i = 0; i < this->materials.size(); i++)
		delete this->materials[i];

	for (auto*& i : this->terrain)
		delete i;

	for (unsigned int i = 0; i < this->pointLights.size(); i++)
		delete this->pointLights[i];
}

//Accessor
int Game::getWindowShouldClose()
{
	return glfwWindowShouldClose(this->window);
}

//Modifier
void Game::setWindowShouldClose()
{
	glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}

//Functions
void Game::updateDt()
{
	this->curTime = static_cast<float>(glfwGetTime());
	this->dt = this->curTime - this->lastTime;
	this->lastTime = this->curTime;
}

void Game::updateMouseInput()
{
	glfwGetCursorPos(this->window, &this->mouseX, &this->mouseY);

	if (this->firstMouse)
	{
		this->lastMouseX = this->mouseX;
		this->lastMouseY = this->mouseY;
		this->firstMouse = false;
	}

	//Calc offset
	this->mouseOffsetX = this->mouseX - this->lastMouseX;
	this->mouseOffsetY = this->lastMouseY - this->mouseY;

	//Set last X and Y
	this->lastMouseX = this->mouseX;
	this->lastMouseY = this->mouseY;

	//Move light
	//if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	//{

		this->pointLights[0]->setPosition(this->camera.getPosition());
	//}
}

void Game::updateKeyboardInput()
{
	//Program
	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->window, GLFW_TRUE);
	}

	static int oldState = GLFW_RELEASE;
	int newState = glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_LEFT);
	if (newState == GLFW_RELEASE && oldState == GLFW_PRESS) {
		this->pointLights[0]->changeIntensity();
		if (headlightOn == 0) headlightOn = 1;
		else headlightOn = 0;
	}
	oldState = newState;

	//Camera
	this->camera.move(this->dt, PASSIVE);
	if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
	{
		this->camera.move(this->dt, FORWARD);
	}
	if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS)
	{
		this->camera.move(this->dt, BACKWARD);
	}
	if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
	{
		this->camera.move(this->dt, LEFT);
	}
	if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
	{
		this->camera.move(this->dt, RIGHT);
	}
	if (glfwGetKey(this->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{		
		this->camera.move(this->dt, DOWN);
		//this->camPosition.y -= 5.f;
	}
	if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		this->camera.move(this->dt, UP);
		//this->camPosition.y += 5.f;
	}
}
void Game::updateChunks(glm::vec3 position)
{
	//cout << "Position: " << glm::to_string(position) << "\n";
	unsigned int xChunkAdrs = 0;
	unsigned int zChunkAdrs = 0;
	if (position[0] >= 0.0)
	{
		//cout << to_string(ceil(position[0] / CHUNK_SIZE)) << "\n";
		xChunkAdrs = (unsigned int)((ceil(position[0] / CHUNK_SIZE)) + 49);
	}
	else if (position[0] < 0.0)
		xChunkAdrs = (unsigned int)(50 + (floor(position[0] / CHUNK_SIZE)));// cout << to_string(floor(position[0] / CHUNK_SIZE)) << "\n";

	float invertedZPos = 0 - position[2];
	if (invertedZPos >= 0) {
		zChunkAdrs = (unsigned int)(ceil(invertedZPos / CHUNK_SIZE) + 49);// cout << to_string(ceil(invertedZPos / CHUNK_SIZE)) << "\n";
	}
	else if (invertedZPos < 0.0)
		zChunkAdrs = (unsigned int)(50 + (floor(invertedZPos / CHUNK_SIZE)));// cout << to_string(floor(invertedZPos / CHUNK_SIZE)) << "\n";

	//cout << "zChunkAdrs: " << to_string(zChunkAdrs) << "\n";
	//cout << "xChunkAdrs: " << to_string(xChunkAdrs) << "\n\n";
	cout << "[camera] position: " << to_string(position[0]) << "  " << to_string(-position[2]) << "\n\n";
	queue<int> toRender;
	
	if (((xChunkAdrs + 1) < MAX_CHUNK_DIMENSION) && (chunkMap[zChunkAdrs][xChunkAdrs + 1] == 0))
	{
		//cout << "(DODGY)zChunkAdrs " << to_string(zChunkAdrs) << "\n";
		//cout << "(DODGY)xChunkAdrs " << to_string(xChunkAdrs) << "\n\n";
		unsigned int toPush = xChunkAdrs + (zChunkAdrs * MAX_CHUNK_DIMENSION) + 1;
		//cout << "Pushed " << to_string(toPush) << "\n\n";
		toRender.push(toPush);
	}
	if (((xChunkAdrs + 1) < MAX_CHUNK_DIMENSION) && ((zChunkAdrs + 1) < MAX_CHUNK_DIMENSION) && (chunkMap[zChunkAdrs + 1][xChunkAdrs + 1] == 0))
	{
		//cout << "xChunkAdrs " << to_string(xChunkAdrs) << '\n\n';
		//cout << "zChunkAdrs " << to_string(zChunkAdrs) << '\n\n';
		//cout << "Pushed " << to_string(xChunkAdrs + (zChunkAdrs * MAX_CHUNK_DIMENSION) + 1) << '\n\n';
		toRender.push(xChunkAdrs + (zChunkAdrs * MAX_CHUNK_DIMENSION) + MAX_CHUNK_DIMENSION + 1);
	}
	if (((xChunkAdrs + 1) < MAX_CHUNK_DIMENSION) && ((zChunkAdrs - 1) >= 0) && (chunkMap[zChunkAdrs - 1][xChunkAdrs + 1] == 0))
	{
		//cout << "xChunkAdrs " << to_string(xChunkAdrs) << '\n\n';
		//cout << "zChunkAdrs " << to_string(zChunkAdrs) << '\n\n';
		//cout << "Pushed " << to_string(xChunkAdrs + (zChunkAdrs * 10) + 1) << '\n\n';
		toRender.push(xChunkAdrs + (zChunkAdrs * MAX_CHUNK_DIMENSION) - MAX_CHUNK_DIMENSION + 1);
	}
	if (((xChunkAdrs - 1) >= 0) && (chunkMap[zChunkAdrs][xChunkAdrs - 1] == 0))
	{
		//cout << "Pushed " << to_string(xChunkAdrs + (zChunkAdrs * 10) + 1) << '\n\n';
		toRender.push(xChunkAdrs + (zChunkAdrs * MAX_CHUNK_DIMENSION) - 1);
	}
	if (((xChunkAdrs - 1) >= 0) && ((zChunkAdrs + 1) < MAX_CHUNK_DIMENSION) && (chunkMap[zChunkAdrs + 1][xChunkAdrs - 1] == 0))
	{
		//cout << "Pushed " << to_string(xChunkAdrs + (zChunkAdrs * 10) + 1) << '\n\n';
		toRender.push(xChunkAdrs + (zChunkAdrs * MAX_CHUNK_DIMENSION) + MAX_CHUNK_DIMENSION - 1);
	}
	if (((xChunkAdrs - 1) >= 0) && ((zChunkAdrs - 1) >= 0) && (chunkMap[zChunkAdrs - 1][xChunkAdrs - 1] == 0))
	{
		//cout << "Pushed " << to_string(xChunkAdrs + (zChunkAdrs * 10) + 1) << '\n\n';
		toRender.push(xChunkAdrs + (zChunkAdrs * MAX_CHUNK_DIMENSION) - MAX_CHUNK_DIMENSION - 1);
	}
	if (((zChunkAdrs + 1) < MAX_CHUNK_DIMENSION) && (chunkMap[zChunkAdrs + 1][xChunkAdrs] == 0))
	{
		//cout << "Pushed " << to_string(xChunkAdrs + (zChunkAdrs * 10) + 1) << '\n\n';
		toRender.push(xChunkAdrs + (zChunkAdrs * MAX_CHUNK_DIMENSION) + MAX_CHUNK_DIMENSION);
	}
	if (((zChunkAdrs - 1) >= 0) && (chunkMap[zChunkAdrs][xChunkAdrs - 1] == 0))
	{
		//cout << "Pushed " << to_string(xChunkAdrs + (zChunkAdrs * 10) + 1) << '\n\n';
		toRender.push(xChunkAdrs + (zChunkAdrs * MAX_CHUNK_DIMENSION) - MAX_CHUNK_DIMENSION);
	}
	
	

	// Printing content of queue 
	while (!toRender.empty()) {
		//cout << ' ' << toRender.front() << "\n\n";
		xChunkAdrs = toRender.front() % MAX_CHUNK_DIMENSION;
		zChunkAdrs = (unsigned int)(toRender.front() / MAX_CHUNK_DIMENSION);
		//cout << "zChunkAdrs(at chunk genloop thing): " << to_string(zChunkAdrs) << "\n";
		//cout << "xChunkAdrs(at chunk genloop thing): " << to_string(xChunkAdrs) << "\n\n";
		toRender.pop();
		this->genChunkModels(zChunkAdrs - ((MAX_CHUNK_DIMENSION / 2) - 1), xChunkAdrs - ((MAX_CHUNK_DIMENSION / 2) - 1));
		chunkMap[zChunkAdrs][xChunkAdrs] = 1;
	}
}
void Game::updateInput()
{
	updateCounter++;
	glfwPollEvents();

	this->updateKeyboardInput();
	this->updateMouseInput();
	if(updateCounter % CHECK_CHUNKS_EVERY == 0)
		this->updateChunks(this->camera.getPosition());
	this->camera.updateInput(dt, -1, this->mouseOffsetX, this->mouseOffsetY);
}

void Game::updateBugz()
{
	/*genType::value_type glm::distance(genType const& p0,
	genType const& p1
	)*/
	//int behaviour; //0= flake. 1=paro
	//int lastDecision; //0= stall, 1= turnleft, 2= walk, 3= runaway, 4= turnRight
	for (unsigned int i = 0; i < this->models.size(); i++)
	{
		Model* thisModel = models[i];
		//std::vector<MeshObj*>& meshObjs
		//cout << "model pos: " << to_string(models[i]->meshObjs[0]->position) << "\n";
		glm::vec3 pos = this->camera.getPosition();
		float distance = glm::distance(thisModel->meshObjs[0]->position, pos);
		//cout << "distance: " << to_string(distance) << "\n";
		//cout << "thisModel->behaviour: " << to_string(thisModel->behaviour) << "\n";
		if ((distance < 80) && (headlightOn == 1))
		{
			thisModel->behaviour = 1;
			thisModel->lastDecision = 3;
		}
		else if ((distance > 100) || (headlightOn == 0))
		{
			thisModel->behaviour = 0;
			thisModel->lastDecision = 2;
		}/*
		else if (distance > 300)
		{
			models.erase(i);
		}*/
		srand(time(0));
		int rand = genRandy(RARITY_BEHAVIOUR_CHANGE, 0);
		//thisModel->meshObjs[0]->rotateThis();
		thisModel->meshObjs[0]->updateCameraVectors();
		if (thisModel->behaviour == 0)
		{
			switch (rand)
			{
			case 0:
				thisModel->lastDecision = 0;
				break;
			case 2:
			case 5:
				thisModel->lastDecision = 2;
				break;
			case 4:
				thisModel->lastDecision = 4;
				break;
			case 1:
				thisModel->lastDecision = 1;
				break;
			default:
				break;
			}
		}
		else
		{
			thisModel->lastDecision = 3;

		}
		//cout << "thisModel->lastDecision: " << to_string(thisModel->lastDecision) << "\n";
		switch (thisModel->lastDecision)
		{
		case 1:
			//thisModel->meshObjs[0]->rotate(ROTATE);
			thisModel->meshObjs[0]->yaw += 15.0f;
			thisModel->meshObjs[0]->rotateThis();
			break;
		case 4:
			//thisModel->meshObjs[0]->rotate(-ROTATE);
			//thisModel->meshObjs[0]->glRotatef(45.0, 0.0, 0.0);
			thisModel->meshObjs[0]->yaw -= 15.0f;

			thisModel->meshObjs[0]->rotateThis();
			break;
		case 2:
			thisModel->meshObjs[0]->position -= thisModel->meshObjs[0]->front * WALK * dt;
			break;
		case 3:
			thisModel->meshObjs[0]->position -= thisModel->meshObjs[0]->front * RUN * dt;
			break;

		}
		

		
		//int lastDecision; //0= stall, 1= turnleft, 2= walk, 3= runaway, 4= turnRight

		//this->models[i]->meshObjs[0]->scaleUp(glm::vec3(0.1f, 0.1f, 0.1f));
	}




}

void Game::update()
{
	//UPDATE INPUT ---
	this->updateBugz();
	this->updateDt();
	this->updateInput();
}

void Game::render()
{

	// counter = counter + 1;
	//if (counter > 50) {
	//	decayRate = 0.5;
	//	glUniform1f()
	//}


	//UPDATE --- 
	//updateInput(window);

	//DRAW ---
	//Clear
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Update the uniforms
	this->updateUniforms();

	//Render terrain
	for (auto&i : this->terrain)
		i->render(this->shaders[SHADER_CORE_PROGRAM]);

	//Render models
	for (auto& i : this->models)
		i->render(this->shaders[SHADER_CORE_PROGRAM]);

	//Activate texture
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture0);

	//End Draw
	glfwSwapBuffers(window);
	glFlush();

	glBindVertexArray(0);
	glUseProgram(0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//Static functions
void Game::framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH)
{
	glViewport(0, 0, fbW, fbH);
};


