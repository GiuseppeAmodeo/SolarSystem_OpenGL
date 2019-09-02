#include "Material.h"

Material::Material()
{
	specularIntensity = 0.0f;
	shininess = 0.0f;
}

Material::Material(GLfloat intensity, GLfloat shine)
{
	specularIntensity = intensity;
	shininess = shine;
}

void Material::UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation)
{
	glUniform1f(specularIntensityLocation,specularIntensity);
	glUniform1f(shininessLocation, shininess);

}

Material::~Material()
{
}
