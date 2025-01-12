#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2/SOIL2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>		// glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "ImportedModel.h"
#include "Utils.h"
#include "Sphere.h"
using namespace std;

#define numVAOs 1
#define numVBOs 3

glm::vec3 cameraPos, cameraFront, cameraUp;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint earthTexture;
GLuint carTexture;

// variable allocation for display
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;

stack<glm::mat4> mvStack;
Sphere mySphere = Sphere(48);
ImportedModel carModel("car.obj");

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

// GLFW callback functions

static void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void processInput(GLFWwindow *window)
{
	const float cameraSpeed = 0.05f; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

float lastX = 400, lastY = 300;
float yaw = -90.0f, pitch = 0.0f;
float fov = 45.0f;
bool firstMouse;

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

void setupVertices(void)
{
	// std::vector<int> ind = mySphere.getIndices();
	// std::vector<glm::vec3> vert = mySphere.getVertices();
	// std::vector<glm::vec2> tex = mySphere.getTexCoords();
	// std::vector<glm::vec3> norm = mySphere.getNormals();

	// std::vector<float> pvalues;
	// std::vector<float> tvalues;
	// std::vector<float> nvalues;

	// int numIndices = mySphere.getNumIndices();
	// for (int i = 0; i < numIndices; i++)
	// {
	// 	pvalues.push_back((vert[ind[i]]).x);
	// 	pvalues.push_back((vert[ind[i]]).y);
	// 	pvalues.push_back((vert[ind[i]]).z);
	// 	tvalues.push_back((tex[ind[i]]).s);
	// 	tvalues.push_back((tex[ind[i]]).t);
	// 	nvalues.push_back((norm[ind[i]]).x);
	// 	nvalues.push_back((norm[ind[i]]).y);
	// 	nvalues.push_back((norm[ind[i]]).z);
	// }

	// glGenVertexArrays(1, vao);
	// glBindVertexArray(vao[0]);
	// glGenBuffers(numVBOs, vbo);

	// glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	// glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	// glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	// glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	// glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
	std::vector<glm::vec3> vert = carModel.getVertices();
	std::vector<glm::vec2> tex = carModel.getTextureCoords();
	std::vector<glm::vec3> norm = carModel.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < carModel.getNumVertices(); i++)
	{
		pvalues.push_back((vert[i]).x);
		pvalues.push_back((vert[i]).y);
		pvalues.push_back((vert[i]).z);
		tvalues.push_back((tex[i]).s);
		tvalues.push_back((tex[i]).t);
		nvalues.push_back((norm[i]).x);
		nvalues.push_back((norm[i]).y);
		nvalues.push_back((norm[i]).z);
	}

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
}

void init(GLFWwindow *window)
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;

	cameraPos = glm::vec3(0.0f, 0.0f, 12.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	firstMouse = true;

	setupVertices();
	earthTexture = Utils::loadTexture("earth.jpg");
	carTexture = Utils::loadTexture("car.png");
}

void display(GLFWwindow *window, double currentTime)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	pMat = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	vMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	mMat = glm::rotate(mMat, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	mMat = glm::rotate(mMat, toRadians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::rotate(mMat, toRadians(35.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, carTexture);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, carModel.getNumVertices());

	// mvStack.push(vMat);
	// // solar 1: sun + planet + moon
	// // ----------------------  sun
	// // Push: sun location
	// mvStack.push(mvStack.top());
	// mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	// // Push: sun rotation
	// mvStack.push(mvStack.top());
	// mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0, 0.0, 0.0));
	// glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

	// glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// glEnableVertexAttribArray(0);
	// glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	// glEnableVertexAttribArray(1);
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, earthTexture);

	// glEnable(GL_CULL_FACE);
	// glFrontFace(GL_CCW);
	// glEnable(GL_DEPTH_TEST);
	// glDepthFunc(GL_LEQUAL);
	// glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	// mvStack.pop(); // Pop: sun rotation

	// //-----------------------  planet
	// // Push: planet location
	// mvStack.push(mvStack.top());
	// mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * 4.0, 0.0f, cos((float)currentTime) * 4.0));
	// // Push: planet rotation
	// mvStack.push(mvStack.top());
	// mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	// glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

	// glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// glEnableVertexAttribArray(0);
	// glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	// glEnableVertexAttribArray(1);
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, earthTexture);

	// glEnable(GL_CULL_FACE);
	// glFrontFace(GL_CCW);
	// glEnable(GL_DEPTH_TEST);
	// glDepthFunc(GL_LEQUAL);
	// glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	// mvStack.pop(); // Pop: planet rotation

	// //-----------------------  moon
	// mvStack.push(mvStack.top());
	// // Push: moon rotation and location
	// mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float)currentTime) * 2.0, cos((float)currentTime) * 2.0));
	// mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.0, 1.0));
	// mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
	// glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

	// glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// glEnableVertexAttribArray(0);
	// glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	// glEnableVertexAttribArray(1);
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, earthTexture);

	// glEnable(GL_CULL_FACE);
	// glFrontFace(GL_CCW);
	// glEnable(GL_DEPTH_TEST);
	// glDepthFunc(GL_LEQUAL);
	// glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	// mvStack.pop(); // Pop: moon rotation and location
	// mvStack.pop(); // Pop: planet location
	// mvStack.pop(); // Pop: sun location

	// // solar 2: sun + planet
	// // ----------------------  sun
	// // Push: sun location
	// mvStack.push(mvStack.top());
	// mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.0f));
	// // Push: sun rotation
	// mvStack.push(mvStack.top());
	// mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0, 0.0, 0.0));
	// glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

	// glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// glEnableVertexAttribArray(0);
	// glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	// glEnableVertexAttribArray(1);
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, earthTexture);

	// glEnable(GL_CULL_FACE);
	// glFrontFace(GL_CCW);
	// glEnable(GL_DEPTH_TEST);
	// glDepthFunc(GL_LEQUAL);
	// glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	// mvStack.pop(); // Pop: sun rotation

	// //-----------------------  planet
	// mvStack.push(mvStack.top());
	// // Push: planet location and rotation
	// mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float)currentTime) * 4.0, cos((float)currentTime) * 4.0));
	// mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	// glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

	// glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// glEnableVertexAttribArray(0);
	// glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	// glEnableVertexAttribArray(1);
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, earthTexture);

	// glEnable(GL_CULL_FACE);
	// glFrontFace(GL_CCW);
	// glEnable(GL_DEPTH_TEST);
	// glDepthFunc(GL_LEQUAL);
	// glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	// mvStack.pop(); // Pop: planet rotation and location
	// mvStack.pop(); // Pop: sun location

	// mvStack.pop(); // Pop: view matrix
}

void window_size_callback(GLFWwindow *win, int newWidth, int newHeight)
{
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
}

int main(void)
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		printf("GLFW initialization failed\n");
		exit(EXIT_FAILURE);
	}
	printf("GLFW initialization succeeded\n");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow(600, 600, "Chapter 4 - program 4", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	// glfw callback functions
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSwapInterval(1);

	if (eglewInit() != GLEW_OK)
	{
		printf("GLEW initialization failed\n");
		exit(EXIT_FAILURE);
	}
	printf("GLEW initialization succeeded\n");

	init(window);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}