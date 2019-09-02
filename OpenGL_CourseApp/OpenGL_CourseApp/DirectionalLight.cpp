#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light()
{
	direction = glm::vec3(0.0f, 0.0f, 0.0f);
}

DirectionalLight::DirectionalLight(GLfloat shadowWidth, GLfloat shadowHeight,
	GLfloat red, GLfloat green, GLfloat blue,
	GLfloat aIntensity, GLfloat dIntensity,
	GLfloat xDir, GLfloat yDir, GLfloat zDir) : Light(shadowWidth, shadowHeight, red, green, blue, aIntensity, dIntensity)
{
	direction = glm::vec3(xDir, yDir, zDir);

	lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);
	
}

void DirectionalLight::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
	GLfloat diffuseIntensityLocation, GLfloat directionLocation)
{
	glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
	glUniform1f(ambientIntensityLocation, ambientIntensity);

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

void DirectionalLight::SetLocationDir(glm::vec3 pos, glm::vec3 dir)
{
	direction = dir;
}

glm::mat4 DirectionalLight::CalculateLightTransform()
{
	return lightProj * glm::lookAt(-direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

DirectionalLight::~DirectionalLight()
{
}
