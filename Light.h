#pragma once

#include"libs.h"

using namespace std;


class Light
{


protected:
	float intensity;
	glm::vec3 color;

public:
	Light(float intensity, glm::vec3 color)
	{
		this->intensity = intensity;
		this->color = color;

	}
	
	~Light()
	{

	}

	//Functions
	virtual void sendToShader(Shader& program) = 0;
};

class PointLight : public Light
{
protected:
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;
	float bright = 50; ///////////////////////////// CMDFPHONG
	float dim = 2.5;

public:
	PointLight(glm::vec3 position, float intensity = 1.f, glm::vec3 color = glm::vec3(1.f, 1.f, 1.f),
		float constant = 1.f, float linear = .0015f, float quadratic = .01f)
		: Light(intensity, color)
	{
		this->position = position;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
		this->constant = dim;
	}

	~PointLight()
	{

	}

	glm::vec3 getPosition()
	{
		return this->position;
	}

	void setPosition(const glm::vec3 position)
	{
		this->position = position;
	}

	void changeIntensity()
	{
		if (this->constant == dim)
		{
			this->constant = bright;
		}
		else
		{
			this->constant = dim;
		}
	}

	void sendToShader(Shader& program)
	{/*
		cout << "this->position: " << to_string(this->position) << "\n";
		cout << "this->intensity: " << to_string(this->intensity) << "\n";
		cout << "this->color: " << to_string(this->color) << "\n";
		cout << "this->constant: " << to_string(this->constant) << "\n";
		cout << "this->linear: " << to_string(this->linear) << "\n";
		cout << "this->quadratic: " << to_string(this->quadratic) << "\n\n";*/
		program.setVec3f(this->position, "pointLight.position");
		program.set1f(this->intensity, "pointLight.intensity");
		program.setVec3f(this->color, "pointLight.color");
		program.set1f(this->constant, "pointLight.constant");
		program.set1f(this->linear, "pointLight.linear");
		program.set1f(this->quadratic, "pointLight.quadratic");
	}
};