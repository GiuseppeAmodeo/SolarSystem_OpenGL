#pragma once
#include "Light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight();

	DirectionalLight(GLfloat shadowWidth, GLfloat shadowHeight,
					GLfloat red, GLfloat green, GLfloat blue,
		GLfloat intensity, GLfloat dIntensity, GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
		GLfloat diffuseIntensityLocation, GLfloat directionLocation);

	void SetLocationDir(glm::vec3 pos,glm::vec3 dir);

	glm::mat4 CalculateLightTransform();

	~DirectionalLight();

private:
	glm::vec3 direction;

};

