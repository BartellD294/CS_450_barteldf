#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <GL/glew.h>					
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "MeshData.hpp"
#include "MeshGLData.hpp"
#include "GLSetup.hpp"
#include "Shader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Utility.hpp"

using namespace std;

float rotAngle = 0.0f;
glm::vec3 eye = glm::vec3(0,0,1);
glm::vec3 lookAt = glm::vec3(0,0,0);
glm::vec2 mousePos;


glm::mat4 makeLocalRotate(glm::vec3 offset, glm::vec3 axis, float angle)
{
	glm::mat4 transformation = glm::translate(-offset);
	transformation = glm::rotate(glm::radians(angle), axis) * transformation;
	transformation = glm::translate(offset) * transformation;
	return transformation;
}

static void mouse_position_callback(GLFWwindow *window, double xpos, double ypos)
{
	glm::vec2 relMouse = -(glm::vec2(xpos, ypos) - mousePos);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	if ((width > 0) && (height > 0))
	{
		relMouse = relMouse / glm::vec2(float(width), float(height));
		glm::vec4 lookAtV = makeLocalRotate(eye, glm::vec3(0,1,0), -30.0f * relMouse[0]) * glm::vec4(lookAt, 1.0f);
		lookAtV = makeLocalRotate(eye, glm::cross(glm::vec3(lookAt - eye), glm::vec3(0,1,0)), -30.0f * relMouse[1]) * lookAtV;
		lookAt = glm::vec3(lookAtV);
	}
	mousePos = glm::vec2(xpos, ypos);
}

glm::mat4 makeRotateZ(glm::vec3 offset)
{
	glm::mat4 m = glm::translate(-offset);
	m = glm::rotate(glm::radians(rotAngle), glm::vec3(0,0,1.0)) * m;
	m = glm::translate(offset) * m;
	return m;
}


void renderScene(vector<MeshGL> &allMeshes, aiNode *node, glm::mat4 parentMat, GLint modelMatLoc, int level)
{
	glm::mat4 nodeT;
	aiMatToGLM4(node->mTransformation, nodeT);
	glm::mat4 modelMat = parentMat * nodeT;
	glm::vec3 pos = modelMat[3];
	glm::mat4 R = makeRotateZ(pos);
	glm::mat4 tmpModel = R * modelMat;
	glUniformMatrix4fv(modelMatLoc, 1, false, glm::value_ptr(tmpModel));
	//glUniformMatrix4fv(modelMatLoc, 1, false, glm::value_ptr(modelMat));

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		int index = node->mMeshes[i];
		drawMesh(allMeshes.at(index));
	}
	for (int i = 0; i < node->mNumChildren; i++)
	{
		renderScene(allMeshes, node->mChildren[i], modelMat, modelMatLoc, level + 1);
	}
}

static void key_callback(GLFWwindow *window,
                        int key, int scancode,
                        int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
        if(key == GLFW_KEY_ESCAPE)
		{
            glfwSetWindowShouldClose(window, true);
        }
        else if(key == GLFW_KEY_J)
		{
            rotAngle += 1.0;
        }
        else if(key == GLFW_KEY_K)
		{
            rotAngle -= 1.0;
        }
		if (key == GLFW_KEY_W)
		{
			glm::vec3 change = lookAt - eye;
			glm::normalize(change);
			change = change * 0.1f;
			lookAt += change;
			eye += change;
		}
		if (key == GLFW_KEY_S)
		{
			glm::vec3 change = lookAt - eye;
			glm::normalize(change);
			change = change * 0.1f;
			lookAt -= change;
			eye -= change;
		}
		if (key == GLFW_KEY_A)
		{
			glm::vec3 change = glm::cross(glm::vec3(lookAt - eye), glm::vec3(0,1,0));
			glm::normalize(change);
			change = change * 0.1f;
			lookAt -= change;
			eye -= change;
		}
		if (key == GLFW_KEY_D)
		{
			glm::vec3 change = glm::cross(glm::vec3(lookAt - eye), glm::vec3(0,1,0));
			glm::normalize(change);
			change = change * 0.1f;
			lookAt += change;
			eye += change;
		}
    }
}




// Create very simple mesh: a quad (4 vertices, 6 indices, 2 triangles)
void createSimpleQuad(Mesh &m) {
	// Clear out vertices and elements
	m.vertices.clear();
	m.indices.clear();

	// Create four corners
	Vertex upperLeft, upperRight;
	Vertex lowerLeft, lowerRight;

	// Set positions of vertices
	// Note: glm::vec3(x, y, z)
	upperLeft.position = glm::vec3(-0.5, 0.5, 0.0);
	upperRight.position = glm::vec3(0.5, 0.5, 0.0);
	lowerLeft.position = glm::vec3(-0.5, -0.5, 0.0);
	lowerRight.position = glm::vec3(0.5, -0.5, 0.0);

	// Set vertex colors (red, green, blue, white)
	// Note: glm::vec4(red, green, blue, alpha)
	upperLeft.color = glm::vec4(1.0, 0.0, 0.0, 1.0);
	upperRight.color = glm::vec4(0.0, 1.0, 0.0, 1.0);
	lowerLeft.color = glm::vec4(0.0, 0.0, 1.0, 1.0);
	lowerRight.color = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Add to mesh's list of vertices
	m.vertices.push_back(upperLeft);
	m.vertices.push_back(upperRight);	
	m.vertices.push_back(lowerLeft);
	m.vertices.push_back(lowerRight);
	
	// Add indices for two triangles
	m.indices.push_back(0);
	m.indices.push_back(3);
	m.indices.push_back(1);

	m.indices.push_back(0);
	m.indices.push_back(2);
	m.indices.push_back(3);
}

// Create very simple mesh: a pentagon
void createSimplePentagon(Mesh &m) {
	// Clear out vertices and elements
	m.vertices.clear();
	m.indices.clear();

	// Create four corners
	Vertex upperLeft, upperRight;
	Vertex lowerLeft, lowerRight;
	Vertex far_right;

	// Set positions of vertices
	// Note: glm::vec3(x, y, z)
	upperLeft.position = 	glm::vec3(-0.5,  0.5, 0.0);
	upperRight.position = 	glm::vec3( 0.5,  0.5, 0.0);
	lowerLeft.position = 	glm::vec3(-0.5, -0.5, 0.0);
	lowerRight.position = 	glm::vec3( 0.5, -0.5, 0.0);
	far_right.position = 	glm::vec3( 0.9,  0.0, 0.0);

	// Set vertex colors (red, green, blue, white)
	// Note: glm::vec4(red, green, blue, alpha)
	upperLeft.color = 	glm::vec4(1.0, 0.0, 0.0, 1.0);
	upperRight.color = 	glm::vec4(0.0, 1.0, 0.0, 1.0);
	lowerLeft.color = 	glm::vec4(0.0, 0.0, 1.0, 1.0);
	lowerRight.color = 	glm::vec4(1.0, 1.0, 1.0, 1.0);
	far_right.color = 		glm::vec4(1.0, 0.0, 1.0, 1.0);	//magenta

	// Add to mesh's list of vertices
	m.vertices.push_back(upperLeft);
	m.vertices.push_back(upperRight);	
	m.vertices.push_back(lowerLeft);
	m.vertices.push_back(lowerRight);
	m.vertices.push_back(far_right);
	
	// Add indices for three triangles
	m.indices.push_back(0);
	m.indices.push_back(3);
	m.indices.push_back(1);

	m.indices.push_back(0);
	m.indices.push_back(2);
	m.indices.push_back(3);

	m.indices.push_back(1);	//upper right
	m.indices.push_back(3);	//lower right
	m.indices.push_back(4);	//middle far right
}


void extractMeshData(aiMesh *mesh, Mesh &m)
{
	m.vertices.clear();
	m.indices.clear();
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex v;
		v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		//v.color = glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0);
		v.color = glm::vec4(1.0, 0, 0, 1.0);
		m.vertices.push_back(v);
	}

	for (int i = 0; i < mesh -> mNumFaces; i++)
	{
		aiFace f = mesh->mFaces[i];
		for (int j = 0; j < f.mNumIndices; j++)
		{
			m.indices.push_back(f.mIndices[j]);
		}
	}
}

// Main 
int main(int argc, char **argv) {

	// Are we in debugging mode?
	bool DEBUG_MODE = true;

	// GLFW setup
	// Switch to 4.1 if necessary for macOS
	GLFWwindow* window = setupGLFW("Assign05: barteldf", 4, 3, 800, 800, DEBUG_MODE);

	// GLEW setup
	setupGLEW(window);

	// Check OpenGL version
	checkOpenGLVersion();

	// Set up debugging (if requested)
	if(DEBUG_MODE) checkAndSetupOpenGLDebugging();

	// Set the background color to a shade of blue
	glClearColor(0.25f, 0.0f, 0.25f, 1.0f);	

	glfwSetKeyCallback(window, key_callback);

	// Create and load shaders
	GLuint programID = 0;
	try {		
		// Load vertex shader code and fragment shader code
		string vertexCode = readFileToString("./shaders/Assign05/Basic.vs");
		string fragCode = readFileToString("./shaders/Assign05/Basic.fs");

		// Print out shader code, just to check
		if(DEBUG_MODE) printShaderCode(vertexCode, fragCode);

		// Create shader program from code
		programID = initShaderProgramFromSource(vertexCode, fragCode);
	}
	catch (exception e) {		
		// Close program
		cleanupGLFW(window);
		exit(EXIT_FAILURE);
	}


	// Create simple quad
	//Mesh m;
	//createSimplePentagon(m);

	// Create OpenGL mesh (VAO) from data
	//MeshGL mgl;
	//createMeshGL(m, mgl);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

////////////////////////////////////////////////////////////////////////////////////

	string modelPath = "sampleModels/sphere.obj";
	if (argc >= 2)
	{
		modelPath = (string)argv[1];
	}

	Assimp::Importer importer;
	unsigned int flags = aiProcess_Triangulate | aiProcess_FlipUVs
					| aiProcess_GenNormals | aiProcess_JoinIdenticalVertices;
	const aiScene *scene = importer.ReadFile(modelPath, flags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cerr << "Error: " << importer.GetErrorString() << endl;
		exit(1);
	}

	vector<MeshGL> myVector;
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		Mesh m;
		MeshGL mg;
		extractMeshData(scene->mMeshes[i], m);
		createMeshGL(m, mg);
		myVector.push_back(mg);
	}

///////////////////////////////////////////////////////////////////////////////////////

	GLint modelMatLoc = glGetUniformLocation(programID, "modelMat");

/////////////////////////////////////////////////////////////////////////////
	// assign05 stuff here
	double mx, my;
	glfwGetCursorPos(window, &mx, &my);
	mousePos = glm::vec2(mx, my);
	glfwSetCursorPosCallback(window, mouse_position_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	GLint viewMatLoc = glGetUniformLocation(programID, "viewMat");
	GLint projMatLoc = glGetUniformLocation(programID, "projMat");


	while (!glfwWindowShouldClose(window)) {
		// Set viewport size
		int fwidth, fheight;
		glfwGetFramebufferSize(window, &fwidth, &fheight);
		glViewport(0, 0, fwidth, fheight);

		// Clear the framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use shader program
		glUseProgram(programID);

		glm::mat4 viewMat = glm::lookAt(eye, lookAt, glm::vec3(0,1,0));
		glUniformMatrix4fv(viewMatLoc, 1, false, glm::value_ptr(viewMat));
		float aspectRatio;
		if ((fwidth == 0) || (fheight == 0))
		{
			aspectRatio = 1.0;
		}
		else aspectRatio = float(fwidth) / (float)fheight;
		glm::mat4 projMat = glm::perspective(glm::radians(90.0f), aspectRatio, 0.01f, 50.0f);
		glUniformMatrix4fv(projMatLoc, 1, false, glm::value_ptr(projMat));









		// Draw object
		//drawMesh(mgl);	

		renderScene(myVector, scene->mRootNode, glm::mat4(1.0), modelMatLoc, 0);

		// Swap buffers and poll for window events		
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Sleep for 15 ms
		this_thread::sleep_for(chrono::milliseconds(15));
	}

	// Clean up mesh
	//cleanupMesh(mgl);
	for (int i = 0; i < myVector.size(); i++)
	{
		cleanupMesh(myVector[i]);
	}
	myVector.clear();

	// Clean up shader programs
	glUseProgram(0);
	glDeleteProgram(programID);
		
	// Destroy window and stop GLFW
	cleanupGLFW(window);

	return 0;
}

