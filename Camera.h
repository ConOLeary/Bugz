#pragma once

#include<iostream>
using namespace std;

#include<glew.h>
#include<glfw3.h>

#include<glm.hpp>
#include<vec3.hpp>
#include<mat4x4.hpp>
#include<gtc\matrix_transform.hpp>

enum direction {FORWARD = 0, BACKWARD, LEFT, RIGHT, PASSIVE, UP, DOWN};
enum movement {TERMINAL_VELOCITY = 30};

class Camera
{
private:
	glm::mat4 ViewMatrix;

	GLfloat lateralMomentum;
	GLfloat longitudinalMomentum;
	GLfloat verticalMomentum;
	GLfloat passiveLateralSlow;
	GLfloat passiveLongitudinalSlow;
	GLfloat passiveVerticalSlow;
	GLfloat lateralAcceleration;
	GLfloat longitudinalAcceleration;
	GLfloat verticalAcceleration;
	GLfloat sensitivity;

	glm::vec3 worldUp;
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	GLfloat pitch;
	GLfloat yaw;
	GLfloat roll;

	void updateCameraVectors()
	{
		this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front.y = sin(glm::radians(this->pitch));
		this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

		this->front = glm::normalize(this->front);
		this->right = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}

public:
	Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 worldUp)
	{
		this->ViewMatrix = glm::mat4(1.f);


		this->lateralMomentum = 0.f;
		this->longitudinalMomentum = 0.f;
		this->verticalMomentum = 0.f;

		this->passiveLateralSlow = 0.3f;
		this->passiveLongitudinalSlow = 0.3f;
		this->passiveVerticalSlow = 0.3f;

		this->lateralAcceleration = 0.4f;
		this->longitudinalAcceleration = 0.4f;
		this->verticalAcceleration = 0.4f;

		this->sensitivity = 1.0f;

		this->worldUp = worldUp;
		this->position = position;
		this->right = glm::vec3(0.f);
		this->up = worldUp;

		this->pitch = 0.f;
		this->yaw = -90.f;
		this->roll = 0.f;

		this->updateCameraVectors();
	}

	~Camera() {}

	//Accessors
	const glm::mat4 getViewMatrix()
	{
		this->updateCameraVectors();

		this->ViewMatrix = glm::lookAt(this->position, this->position + this->front, this->up);

		return this->ViewMatrix;
	}

	const glm::vec3 getPosition()
	{
		return this->position;
	}
	//Functions
	void move(const float& dt, const int direction)
	{
		
		/*cout << "[camera] position: " << glm::to_string(position) << "\n\n";
		cout << "front: " << glm::to_string(front) << "\n\n";
		cout << "right: " << glm::to_string(right) << "\n\n";
		cout << "up: " << glm::to_string(up) << "\n\n\n\n";*/
		glm::vec3 position;
		glm::vec3 front;
		glm::vec3 right;
		glm::vec3 up;
		//Update position vector
		switch (direction)
		{
		case PASSIVE:
			this->position += this->right * this->lateralMomentum * dt;
			this->position += this->front * this->longitudinalMomentum * dt;
			this->position += this->up * this->verticalMomentum * dt;
			if (lateralMomentum > passiveLateralSlow)
			{
				lateralMomentum -= passiveLateralSlow;
			}
			else if (lateralMomentum < -passiveLateralSlow)
			{
				lateralMomentum += passiveLateralSlow;
			}
			else
			{
				lateralMomentum = 0;
			}
			if (longitudinalMomentum > passiveLongitudinalSlow)
			{
				longitudinalMomentum -= passiveLongitudinalSlow;
			}
			else if (longitudinalMomentum < -passiveLongitudinalSlow)
			{
				longitudinalMomentum += passiveLongitudinalSlow;
			}
			else
			{
				longitudinalMomentum = 0;
			}
			if (verticalMomentum > passiveVerticalSlow)
			{
				verticalMomentum -= passiveVerticalSlow;
			}
			else if (verticalMomentum < -passiveVerticalSlow)
			{
				verticalMomentum += passiveVerticalSlow;
			}
			else
			{
				verticalMomentum = 0;
			}
			break;
		case UP:
			if (verticalMomentum < TERMINAL_VELOCITY)
				verticalMomentum += verticalAcceleration;
			break;
		case DOWN:
			if (verticalMomentum > -TERMINAL_VELOCITY)
				verticalMomentum -= verticalAcceleration * 1.5;
			break;
		case FORWARD:
			if(longitudinalMomentum < TERMINAL_VELOCITY)
				longitudinalMomentum += longitudinalAcceleration * 1.5;
			break;
		case BACKWARD:
			if (longitudinalMomentum > -TERMINAL_VELOCITY)
				longitudinalMomentum -= longitudinalAcceleration;
			break;
		case RIGHT:
			if (lateralMomentum < TERMINAL_VELOCITY)
				lateralMomentum += lateralAcceleration;
			break;
		case LEFT:
			if (lateralMomentum > -TERMINAL_VELOCITY)
				lateralMomentum -= lateralAcceleration;
			break;
		default:
			break;
		}
	}

	void updateMouseInput(const float& dt, const double& offsetX, const double& offsetY)
	{
		//Update pitch yaw and roll
		this->pitch += static_cast<GLfloat>(offsetY) * this->sensitivity * dt;
		this->yaw += static_cast<GLfloat>(offsetX) * this->sensitivity * dt;

		if (this->pitch > 80.f)
			this->pitch = 80.f;
		else if (this->pitch < -80.f)
			this->pitch = -80.f;

		if (this->yaw > 360.f || this->yaw < -360.f)
			this->yaw = 0.f;
	}

	void updateInput(const float& dt, const int direction, const double& offsetX, const double& offsetY)
	{
		this->updateMouseInput(dt, offsetX, offsetY);
	}

};