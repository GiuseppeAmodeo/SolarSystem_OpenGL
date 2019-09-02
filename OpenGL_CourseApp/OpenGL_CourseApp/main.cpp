#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"
#include "Material.h"
#include "DirectionalLight.h"
#include "CommonValues.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Model.h"
#include "Skybox.h"

#include <assimp/Importer.hpp>

const float PI = 3.141592653589793238462643383;
const float toRadians = 3.14159265f / 180.0f;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0,
uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0, uniformOmniLightPos = 0, uniformFarPlane = 0;

#pragma region Variables
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Shader directionalShadowShader;
Shader omniShadowShader;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Skybox skyBox;

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;
float angle = 0.0f;

Material shinyMaterial;
Material dullMaterial;

#pragma region PlanetModels
Model xWing;
Model blackHawk;
Model earthPlanet;
Model moon;
Model sun;
Model mercury;
Model saturn;
Model venus;
Model mars;
Model jupiter;
Model uranus;
Model neptune;
Model orbit;
#pragma endregion

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat dirX, dirY, dirZ;
#pragma endregion

#pragma region Shaders
// Vertex Shader
static const char* vShader = "Shaders/shader.vert.txt";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag.txt";

//DirectionalShadow Vertex Shader
static const char* vdShader = "Shaders/directional_shadowmap.vert.txt";

//DirectionalShadow Fragment Shader
static const char* fdShader = "Shaders/directional_shadowmap.frag.txt";

//OmniShadow Vertex Shader.
static const char* gvShader = "Shaders/omni_shadowmap.vert.txt";

//OmniShadow Fragment Shader.
static const char* gfShader = "Shaders/omni_shadowmap.frag.txt";

//OmniShadow Geom Shader.
static const char* gShader = "Shaders/omni_shadowmap.geom.txt";

#pragma endregion

void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			 u	   v		 nx	   ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] =
	{
		0,2,1,
		1,2,3
	};

	GLfloat floorVertices[] =
	{
		-10.0f,0.0f,-10.0f,  0.0f,0.0f,    0.0f,-1.0f,0.0f,
		 10.0f,0.0f,-10.0f,	 10.0f,0.0f,   0.0f,-1.0f,0.0f,
		-10.0f,0.0f,10.0f,	 0.0f,10.0f,   0.0f,-1.0f,0.0f,
		 10.0f,0.0f,10.0f,	 10.0f,10.0f,  0.0f,-1.0f,0.0f
	};

	unsigned int linesIndices[] =
	{
		0,2,1,
	};

	GLfloat lineVertices[] =
	{
		-10.0f,0.0f,-10.0f,
		 10.0f,0.0f,-10.0f,
		 -10.0f,0.0f,10.0f
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	directionalShadowShader = Shader();
	directionalShadowShader.CreateFromFiles(vdShader, fdShader);

	omniShadowShader = Shader();
	omniShadowShader.CreateFromFiles(gvShader, gShader, gfShader);
}

void RenderScene()
{
	glm::mat4 model;

	#pragma region Comments.
	//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
	////model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//brickTexture.UseTexture();
	//shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//meshList[0]->RenderMesh();

	//model = glm::mat4();
	//model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
	////model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//dirtTexture.UseTexture();
	//dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//meshList[1]->RenderMesh();

	//model = glm::mat4();
	//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	////model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//dirtTexture.UseTexture();
	//dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//meshList[2]->RenderMesh();

	//model = glm::mat4();
	//model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 10.0f));
	//model = glm::scale(model, glm::vec3(0.006f, 0.006f, 0.006f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//xWing.RenderModel();

	/*blackhawkAngle -= 0.08f;
	earthAngle -= 0.04f;
	martsAngle -= 0.5f;*/
	/*if (blackhawkAngle > 360.0f)
	{
		blackhawkAngle -= 0.08f;
	}*/
	//model = glm::mat4();
	//model = glm::rotate(model, blackhawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::translate(model, glm::vec3(-8.0f, 2.0f, 0.0f));
	//model = glm::rotate(model, 20.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//blackHawk.RenderModel();

#pragma endregion

	angle += 2.0f * deltaTime;

	#pragma region PlanetsInit

#pragma region  Sun
	//Sun.
	model = glm::mat4();
	model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, 8.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.004f, 0.004f, 0.004f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	sun.RenderModel();
#pragma endregion

#pragma region Mercury
	//Mercury.
	model = glm::mat4();
	model = glm::rotate(model, 7.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, 360.0f * toRadians * (angle / 88.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(2.0f * 3, 8.0f, 0.0f));
	model = glm::rotate(model, toRadians * (angle / 59.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	mercury.RenderModel();

	//Mercury Orbit.
	model = glm::mat4();
	model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, 8.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.3f, 1.3f, 1.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	orbit.RenderModel();
#pragma endregion

#pragma region Venus
	//Venus.
	model = glm::mat4();
	model = glm::rotate(model, 3.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, 360.0f * toRadians * (angle / 225), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(3.0f * 4, 8.0f, 0.0f));
	model = glm::rotate(model, toRadians * (angle / -243.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	venus.RenderModel();

	//Venus Orbit.
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 8.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	orbit.RenderModel();
#pragma endregion

#pragma region Earth 
	//Earth.
	model = glm::mat4();
	model = glm::rotate(model, 7.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, 360.0f * toRadians * (angle / 365.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(8.0f * 3, 8.0f, 0.0f)); //x=3.5f.
	model = glm::rotate(model, toRadians * angle, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	earthPlanet.RenderModel();

	//Earth Orbit.
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 8.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.2f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	orbit.RenderModel();

#pragma endregion

#pragma region Moon
	//Moon.
	model = glm::mat4();
	model = glm::rotate(model, 7.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, 360.0f * toRadians * (angle / 365.0f), glm::vec3(0.0f, 1.0f, 0.0f));  //Revolution.
	model = glm::translate(model, glm::vec3(8.0f * 3, 8.0f, 0.0f));
	model = glm::rotate(model, 360.0f * toRadians * (angle / 30), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	moon.RenderModel();

	//Moon Orbit.
	model = glm::mat4();
	model = glm::rotate(model, 7.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, 360.0f * toRadians * (angle / 365.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(8.0f * 3, 8.0f, 0.0f)); //x=3.5f.
	model = glm::rotate(model, toRadians * angle, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	orbit.RenderModel();
#pragma endregion

#pragma region Mars
	//Mars.
	model = glm::mat4();
	model = glm::rotate(model, 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, 360.0f * toRadians * (angle / 686), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(12 * 3, 8.0f, 0.0f)); //x=3.5f.
	model = glm::rotate(model, toRadians* (angle / 1), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	mars.RenderModel();

	//Mars Orbit.
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 8.0f, 0.0f));
	model = glm::scale(model, glm::vec3(7.8f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	orbit.RenderModel();
#pragma endregion

#pragma region Jupiter
	//Jupiter.
	model = glm::mat4();
	model = glm::rotate(model, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, 360.0f * toRadians * (angle / 4333), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(15 * 3, 8.0f, 0.0f)); //x=3.5f.
	model = glm::rotate(model, toRadians* (angle / 0.4f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.003f, 0.003f, 0.003f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	jupiter.RenderModel();

	//Jupiter Orbit.
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 8.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.6f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	orbit.RenderModel();
#pragma endregion

#pragma region Saturn
	//Saturn.
	model = glm::mat4();
	model = glm::rotate(model, 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, 360.0f * toRadians * (angle / 10759), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(20 * 3, 8.0f, 0.0f)); //x=3.5f.
	model = glm::rotate(model, toRadians* (angle / 0.4f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	saturn.RenderModel();

	//Saturn Orbit.
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 8.0f, 0.0f));
	model = glm::scale(model, glm::vec3(12.8f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	orbit.RenderModel();

#pragma endregion

#pragma region Uranus
	//Uranus.
	model = glm::mat4();
	model = glm::rotate(model, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, 360.0f * toRadians * (angle / 30685), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(25 * 3, 8.0f, 0.0f)); //x=3.5f.
	model = glm::rotate(model, toRadians* (angle / -0.7f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	uranus.RenderModel();

	//Uranus Orbit.
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 8.0f, 0.0f));
	model = glm::scale(model, glm::vec3(16.1f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	orbit.RenderModel();
#pragma endregion

#pragma region Neptune
	//Neptune.
	model = glm::mat4();
	model = glm::rotate(model, 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, 360.0f * toRadians * (angle / 60190), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(30 * 3, 8.0f, 0.0f)); //x=3.5f.
	model = glm::rotate(model, toRadians* (angle / 0.7f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	neptune.RenderModel();

	//Neptune Orbit.
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 8.0f, 0.0f));
	model = glm::scale(model, glm::vec3(19.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	orbit.RenderModel();
#pragma endregion

#pragma endregion
}

void DirectionalShadowMapPass(DirectionalLight *light)
{
	directionalShadowShader.UseShader();

	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	directionalShadowShader.SetDirectionalLightTransform(&light->CalculateLightTransform());

	directionalShadowShader.Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(PointLight *light)
{
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	omniShadowShader.UseShader();
	uniformModel = omniShadowShader.GetModelLocation();
	uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
	uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

	light->GetShadowMap()->Write();

	glClear(GL_DEPTH_BUFFER_BIT);

	glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
	glUniform1f(uniformFarPlane, light->GetFarPlane());
	omniShadowShader.SetLightMatrices(light->CalculateLightTransform());

	omniShadowShader.Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	glViewport(0, 0, 1366, 768);

	//Clear Window.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skyBox.DrawSkybox(viewMatrix, projectionMatrix);

	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();

	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetPointLights(pointLights, pointLightCount, 3, 0);
	shaderList[0].SetSpotLights(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
	shaderList[0].SetDirectionalLightTransform(&mainLight.CalculateLightTransform());

	mainLight.GetShadowMap()->Read(GL_TEXTURE2); //2 1

	shaderList[0].SetTexture(1);//1  0
	shaderList[0].SetDirectionalShadowMap(2);//2  1

	//mainLight.UseLight(uniformAmbientIntensity, uniformAmbientColour,
	//uniformDiffuseIntensity, uniformDirection); 

	glm::vec3 lowerLight = camera.getCameraPosition();
	lowerLight.y -= 0.3f;
	spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

	shaderList[0].Validate();

	RenderScene();
}

int main()
{
#pragma region General Init
	dirX = -176.0f;
	dirY = -122.0f;
	dirZ = -45.0f;

	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();

	shinyMaterial = Material(4.0f, 256);
	dullMaterial = Material(0.3f, 4);

#pragma region ModelInits
	xWing = Model();
	xWing.LoadModel("Models/x-wing.obj");

	blackHawk = Model();
	blackHawk.LoadModel("Models/uh60.obj");

	earthPlanet = Model();
	earthPlanet.LoadModel("Models/Earth.obj");

	sun = Model();
	sun.LoadModel("Models/Sole.obj");

	moon = Model();
	moon.LoadModel("Models/Luna.obj");

	saturn = Model();
	saturn.LoadModel("Models/Saturno.obj");

	mars = Model();
	mars.LoadModel("Models/Marte.obj");

	mercury = Model();
	mercury.LoadModel("Models/Mercurio.obj");

	venus = Model();
	venus.LoadModel("Models/Venere.obj");

	jupiter = Model();
	jupiter.LoadModel("Models/Giove.obj");

	uranus = Model();
	uranus.LoadModel("Models/Urano.obj");

	neptune = Model();
	neptune.LoadModel("Models/Nettuno.obj");

	orbit = Model();
	orbit.LoadModel("Models/Orbit.obj");
#pragma endregion

#pragma region DirectionalLight
	//(1.0f, 1.0f, 1.0f, 0.1f, 0.3f, 0.0f, 0.0f, -1.0f);
	mainLight = DirectionalLight(2048, 2048,
		2.0f, 2.0f, 2.0f,
		0.1f, 1.0f,
		dirX,dirY,dirZ); //-10.0f, -8.0f, 18.5f.
#pragma endregion

#pragma region PointLight
		pointLights[1] = PointLight(1024, 1024,
			0.01f, 250.0f,
			1.0f, 1.0f, 1.0f,
			0.0f, 3.0f,
			0.0f, 15.0f, 0.0f,
			0.3f, 0.2f, 0.1f);
	pointLightCount++;

	pointLights[0] = PointLight(1024, 1024,
		0.01f, 250.0f,
		1.0, 1.0f, 1.0f,
		0.0f, 3.0f,
		5.0f, 9.0f, 0.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
#pragma endregion

#pragma region SpotLight
	spotLights[0] = SpotLight(1024, 1024,
		0.1f, 100.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[1] = SpotLight(1024, 1024,
		0.1f, 100.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, -1.5f, 0.0f,
		-100.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

#pragma endregion

#pragma region  SkyBox Set
	vector<string> skyboxFaces;
	skyboxFaces.push_back("Textures/skybox/purplenebula_rt.tga");
	skyboxFaces.push_back("Textures/skybox/purplenebula_lf.tga");
	skyboxFaces.push_back("Textures/skybox/purplenebula_up.tga");
	skyboxFaces.push_back("Textures/skybox/purplenebula_dn.tga");
	skyboxFaces.push_back("Textures/skybox/purplenebula_bk.tga");
	skyboxFaces.push_back("Textures/skybox/purplenebula_ft.tga");

	skyBox = Skybox(skyboxFaces);
#pragma endregion

	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

#pragma endregion

#pragma region GameLoop
	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
		deltaTime = now - lastTime; // (now - lastTime)*1000/SDL_GetPerformanceFrequency();
		lastTime = now;

		// Get + Handle User Input
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		
		//On/Off SpotLight.
		if (mainWindow.getsKeys()[GLFW_KEY_L])
		{
			spotLights[0].Toggle();
			mainWindow.getsKeys()[GLFW_KEY_L] = false;
		}

		#pragma region  Debug
	/*if (mainWindow.getsKeys()[GLFW_KEY_UP])
		{
			dirY -= 0.1f;
		}

		if (mainWindow.getsKeys()[GLFW_KEY_DOWN])
		{
			dirY += 0.1f;
		}

		if (mainWindow.getsKeys()[GLFW_KEY_LEFT])
		{
			dirX -= 0.1f;
		}

		if (mainWindow.getsKeys()[GLFW_KEY_RIGHT])
		{
			dirX += 0.1f;
		}

		if (mainWindow.getsKeys()[GLFW_KEY_Q])
		{
			dirZ -= 0.1f;
		}

		if (mainWindow.getsKeys()[GLFW_KEY_R])
		{
			dirZ += 0.1f;
		}*/

	/*	cout << "DirX:\t" << dirX << endl;
		cout << "DirY:\t" << dirY << endl;
		cout << "DirZ:\t" << dirZ << endl;
		system("CLS");*/
#pragma endregion

		DirectionalShadowMapPass(&mainLight);
		mainLight.SetLocationDir(glm::vec3(0.0f), glm::vec3(dirX, dirY, dirZ));

		for (size_t i = 0; i < pointLightCount; i++)
		{
			OmniShadowMapPass(&pointLights[i]);
		}

		for (size_t i = 0; i < spotLightCount; i++)
		{
			OmniShadowMapPass(&spotLights[i]);
		}

		RenderPass(projection, camera.CalculateViewMatrix());

		glUseProgram(0);

		mainWindow.swapBuffers();
	}
#pragma endregion

	return 0;
}