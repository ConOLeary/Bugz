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
	Vertex* vertexArray;
	unsigned nrOfVertices;
	GLuint* indexArray;
	unsigned nrOfIndices;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	glm::vec3 position;
	glm::vec3 origin;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::mat4 ModelMatrix;

	ModelData mesh_data;

	void initAssimpVAO(ModelData mesh_data)
	{
		/*----------------------------------------------------------------------------
		LOAD MESH HERE AND COPY INTO BUFFERS
		----------------------------------------------------------------------------*/

		//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
		//Might be an idea to do a check for that before generating and binding the buffer.


		unsigned int vp_vbo = 0;
		//loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
		//loc2 = glGetAttribLocation(shaderProgramID, "vertex_normal");
		//loc3 = glGetAttribLocation(shaderProgramID, "vertex_texture");

		glGenBuffers(1, &vp_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
		glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(glm::vec3), &mesh_data.mVertices[0], GL_STATIC_DRAW);
		unsigned int vn_vbo = 0;
		glGenBuffers(1, &vn_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
		glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(glm::vec3), &mesh_data.mNormals[0], GL_STATIC_DRAW);

		//	This is for texture coordinates which you don't currently need, so I have commented it out
		//	unsigned int vt_vbo = 0;
		//	glGenBuffers (1, &vt_vbo);
		//	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
		//	glBufferData (GL_ARRAY_BUFFER, monkey_head_data.mTextureCoords * sizeof (vec2), &monkey_head_data.mTextureCoords[0], GL_STATIC_DRAW);

		unsigned int vao = 0;
		glCreateVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		//	This is for texture coordinates which you don't currently need, so I have commented it out
		//	glEnableVertexAttribArray (loc3);
		//	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
		//	glVertexAttribPointer (loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
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

	MeshObj(ModelData modelData,
			glm::vec3 position = glm::vec3(0.f),
			glm::vec3 origin = glm::vec3(0.f),
			glm::vec3 rotation = glm::vec3(0.f),
			glm::vec3 scale = glm::vec3(0.f))
	{
		this->initAssimpVAO(modelData);
		this->updateModelMatrix();
		this->position = position;
		this->origin = origin;
		this->rotation = rotation;
		this->scale = scale;
	}

	~MeshObj()
	{
		glDeleteVertexArrays(1, &this->VAO);
		glDeleteBuffers(1, &this->VBO);

		if (this->nrOfIndices > 0)
		{
			glDeleteBuffers(1, &this->EBO);
		}

		delete[] this->vertexArray;
		delete[] this->indexArray;
	}

	//Accessors
	glm::vec3 getScale()
	{
		return this->scale;
	}

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
		if (this->nrOfIndices == 0) {
			glDrawArrays(GL_TRIANGLES, 0, this->nrOfVertices);
		}
		else
			glDrawElements(GL_TRIANGLES, this->nrOfIndices, GL_UNSIGNED_INT, 0);

		//Cleanup
		glBindVertexArray(0);
		glUseProgram(0);
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};