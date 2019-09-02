#pragma once

#pragma region Includes
#include <vector>
#include  <string>

#include <GL\glew.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CommonValues.h"

#include "Shader.h"
#include "Mesh.h"

using namespace std;
#pragma endregion

class Skybox
{
public:
	Skybox();

	Skybox(vector<string> faceLocations);

	void DrawSkybox(glm::mat4 viewMatrix,glm::mat4 projectionMatrix);

	~Skybox();

private:
	Mesh* skyMesh;
	Shader* skyShader;

	GLuint textureID;
	GLuint uniformProjection, uniformView;
};

