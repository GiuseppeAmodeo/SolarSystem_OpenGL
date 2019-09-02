#include "Mesh.h"

Mesh::Mesh()
{
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;
}

void Mesh::CreateMesh(GLfloat * vertices, unsigned int * indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
	indexCount = numOfIndices;

	//Vertex Specification:
	//Creating VAO/VBO
	glGenVertexArrays(1, &VAO); //Generate VAO ID
	glBindVertexArray(VAO);	   //Bind the VAO with ID.

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);	//Generate VBO ID.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//Bind the VBO with ID.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);  //Attach the vertex data to that VBO.

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0); //Define the Attribute Pointer Formatting.
	glEnableVertexAttribArray(0);	//Enable the Attribute Pointer.
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));
	glEnableVertexAttribArray(2);
		
	glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);	//Unbind VAO
}

void Mesh::RenderMesh()
{
	glBindVertexArray(VAO);  //Bind the VAO.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);//Unbind VAO.
}

void Mesh::ClearMesh()
{
	if (IBO != 0)
	{
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	indexCount = 0;
}

Mesh::~Mesh()
{
	ClearMesh();
}
