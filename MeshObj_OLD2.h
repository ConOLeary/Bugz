#pragma once

#include<iostream>
#include<vector>

#include"libs.h"

typedef struct
{
	size_t mPointCount = 0;
	std::vector<glm::vec3> mVertices;
	std::vector<glm::vec3> mNormals;
	std::vector<glm::vec2> mTextureCoords;
} ModelData;

class MeshObj
{
private:
	GLuint VAO;
	GLuint VBO1, VBO2;



	void initAssimpVAO(ModelData mesh_data)
	{

		glGenBuffers(1, &this->VBO1);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO1);
		glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(glm::vec3), &mesh_data.mVertices[0], GL_STATIC_DRAW);
		glGenBuffers(1, &this->VBO2);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO2);
		glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(glm::vec3), &mesh_data.mNormals[0], GL_STATIC_DRAW);

		glCreateVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindVertexArray(0);
	}

	void updateUniforms(Shader* shader)
	{
		shader->setMat4fv(this->ModelMatrix, "ModelMatrix");
	}

	void updateModelMatrix()
	{
		this->ModelMatrix = glm::mat4(1.f);
		this->ModelMatrix = glm::translate(this->ModelMatrix, this->origin);
		//this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
		//this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.y), glm::vec3(0.f, 1.f, 0.f));
		//this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.z), glm::vec3(0.f, 0.f, 1.f));
		this->ModelMatrix = glm::translate(this->ModelMatrix, this->position - this->origin);
		this->ModelMatrix = glm::scale(this->ModelMatrix, this->scale);
	}

public:


	glm::vec3 worldUp;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	GLfloat pitch;
	GLfloat yaw;
	GLfloat roll;

	glm::vec3 position;
	glm::vec3 origin;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::mat4 ModelMatrix;

	ModelData mesh_data;
	MeshObj(ModelData modelData,
			glm::vec3 position = glm::vec3(0.f),
			glm::vec3 origin = glm::vec3(0.f),
			glm::vec3 rotation = glm::vec3(0.f),
			glm::vec3 scale = glm::vec3(0.1f))
	{
		this->initAssimpVAO(modelData);

		//std::cout << glm::to_string(modelData.mVertices[0]) << std::endl;

		this->mesh_data = modelData;

		this->updateModelMatrix();

		this->position = position;
		this->origin = origin;
		this->rotation = rotation;
		this->scale = scale;

		this->worldUp = worldUp;
		this->position = position;
		this->right = glm::vec3(0.f);
		this->up = worldUp;

		this->pitch = 0.f;
		this->yaw = -90.f;
		this->roll = 0.f;

		this->updateCameraVectors();
	}

	~MeshObj()
	{
		glDeleteVertexArrays(1, &this->VAO);
		glDeleteBuffers(1, &this->VBO1);
		glDeleteBuffers(1, &this->VBO2);

	}

	void rotateThis()
	{
		this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(10.f), glm::vec3(20.f, 0.f, 0.f));
	}

	void updateCameraVectors()
	{
		this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front.y = sin(glm::radians(this->pitch));
		this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

		this->front = glm::normalize(this->front);
		this->right = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}

	//Accessors

	//Modifiers
	void setPosition(const glm::vec3 position)
	{
		this->position = position;
	}

	void setOrigin(const glm::vec3 origin)
	{
		this->origin = origin;
	}

	void setRotation(const glm::vec3 rotation)
	{
		this->rotation = rotation;
	}

	void setScale(const glm::vec3 setScale)
	{
		this->scale = scale;
	}

	//Functions

	void move(const glm::vec3 position)
	{
		this->position += position;
	}

	void rotate(const glm::vec3 rotation)
	{
		this->rotation += rotation;
	}

	void scaleUp(const glm::vec3 scale)
	{
		this->scale += scale;
	}

	void update()
	{

	}

	void render(Shader* shader)
	{
		//std::cout << "please" << "\n";
		//std::cout << glm::to_string(this->position) << "\n";
		//Update uniforms

		this->updateModelMatrix();
		this->updateUniforms(shader);

		shader->use();

		//Bind VAO
		glBindVertexArray(this->VAO);

		//RENDER

		glDrawArrays(GL_TRIANGLES, 0, this->mesh_data.mPointCount);


		//Cleanup
		glBindVertexArray(0);
		glUseProgram(0);
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};