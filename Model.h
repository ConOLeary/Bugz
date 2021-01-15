#pragma once

#include"Mesh.h"
#include"Texture.h"
#include"Shader.h"
#include"Material.h"
#include"OBJLoader.h"

using namespace std;

class Model
{
private:


	void updateUniforms()
	{

	}

public:

	Material* material;
	Texture* overrideTextureDiffuse;
	Texture* overrideTextureSpecular;
	std::vector<Mesh*> meshes;
	std::vector<BugObj*> myBugObjs;
	glm::vec3 position;
	Model(
		glm::vec3 position,
		Material* material,
		Texture* orTexDif,
		Texture* orTexSpec,
		std::vector<BugObj*>& myBugObjs
	)
	{/*
		for (auto& i : this->myBugObjs)
		{
			i->getScale();
		}
		cout << "MeshObjs created. Size= " << myBugObjs.size() << "\n";

		if (myBugObjs.size() > 0)
			cout << "MeshObjs[0] scale is " << glm::to_string((*myBugObjs[0]).getScale());*/
		//glm::vec3 o = myBugObjs[42]->getScale();

		//glm::vec3 scale = (*myBugObjs[0]).getScale();

		
		//cout << "BugObj scale= " << *(myBugObjs[<0>]).doObjectFunc(); << "\n";
		this->position = position;
		this->material = material;
		this->overrideTextureDiffuse = orTexDif;
		this->overrideTextureSpecular = orTexSpec;

		for (auto* i : myBugObjs)
		{
			this->myBugObjs.push_back(new BugObj(*i));
		}

		for (auto& i : this->myBugObjs)
		{
			i->move(this->position);
			i->setOrigin(this->position);
		}
	}


	Model(
		glm::vec3 position, 
		Material* material,
		Texture* orTexDif,
		Texture* orTexSpec,
		std::vector<Mesh*>& meshes
		)
	{
		this->position = position;
		this->material = material;
		this->overrideTextureDiffuse = orTexDif;
		this->overrideTextureSpecular = orTexSpec;


		for (auto* i : meshes)
		{
			this->meshes.push_back(new Mesh(*i));
		}

		for (auto& i : this->meshes)
		{
			i->move(this->position);
			i->setOrigin(this->position);
		}
	}

	//OBJ file loaded model
	Model(
		glm::vec3 position,
		Material* material,
		Texture* orTexDif,
		Texture* orTexSpec,
		const char* objFile
	)
	{
		this->position = position;
		this->material = material;
		this->overrideTextureDiffuse = orTexDif;
		this->overrideTextureSpecular = orTexSpec;


		std::vector<Vertex> mesh = loadOBJ(objFile);
		this->meshes.push_back(new Mesh(mesh.data(), mesh.size(), NULL, 0, glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(0.f),
			glm::vec3(0.f),
			glm::vec3(1.f)));

		for (auto& i : this->meshes)
		{
			i->move(this->position);
			i->setOrigin(this->position);
		}
	}

	~Model()
	{
		for (auto*& i : this->meshes)
			delete i;
	}

	//Functions
	void rotate(const glm::vec3 rotation)
	{
		for (auto& i : this->meshes)
			i->rotate(rotation);
	}

	void update()
	{

	}

	void render(Shader* shader)
	{
		//Update the uniforms
		this->updateUniforms();

		//Update uniforms
		this->material->sendToShader(*shader);

		//Use a program (BECAUSE SHADER CLASS LAST UNIFORM UPDATE UNUSES IT)
		shader->use();
		//cout << "size of meshes: " << this->meshes.size() << "\n";
		//Draw
		for (auto& i : this->meshes)
		{   
			//Activate texture for each mesh
			this->overrideTextureDiffuse->bind(0);
			this->overrideTextureSpecular->bind(1);

			i->render(shader); //Activates shader also
		}

		//cout << "size of myBugObjs: " << this->myBugObjs.size() << "\n\n";
		//Draw
		for (auto& i : this->myBugObjs)
		{
			//Activate texture for each mesh
			this->overrideTextureDiffuse->bind(0);
			this->overrideTextureSpecular->bind(1);

			i->render(shader); //Activates shader also
		}
	}
};