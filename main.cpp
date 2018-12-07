
/*********************************************************
Group Information
Student 1: LI Ho Yin (1155077785)
Student 2: YU Yun Chi (1155078184)
*********************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include "camera.h"

using namespace std;
using glm::vec3;
using glm::vec4;
using glm::mat4;

# define M_PI 3.14159265358979323846
float ninety = M_PI / 2.0;
float angle = M_PI / 8.0;

GLint programID;

// window size
int width_W = 800;
int height_W = 800;

int countingdummy = 0;

// Vao, Vbo and interaction parameters

// init Vertex Array Object
GLuint vaoID[10];
// init Vertex Buffer Objects
GLuint vboID[10];
GLuint uvboID[10];
GLuint nboID[10];

//obj SpaceCraft
std::vector<glm::vec3> verticesA;
std::vector<glm::vec2> uvsA;
std::vector<glm::vec3> normalsA;
// control spacecraft rotation
float spacecraft_rot_y = 0;
float spacecraft_rot_z = M_PI;

//obj Earth
std::vector<glm::vec3> verticesB;
std::vector<glm::vec2> uvsB;
std::vector<glm::vec3> normalsB;

//obj Rings[3]
std::vector<glm::vec3> verticesC[3];
std::vector<glm::vec2> uvsC[3];
std::vector<glm::vec3> normalsC[3];
vec3 ringCoordinates[3];

//obj Wonder Star
int wonderstarID = sizeof(verticesC) / sizeof(verticesC[0]) + 2;
std::vector<glm::vec3> verticesD;
std::vector<glm::vec2> uvsD;
std::vector<glm::vec3> normalsD;

// control rotation of earth, rings and wonder star
int block_rot_x = 1;

//obj Rock
int rockID = wonderstarID + 1;
std::vector<glm::vec3> verticesE;
std::vector<glm::vec2> uvsE;
std::vector<glm::vec3> normalsE;
glm::mat4* modelMatrices;
GLuint amount = 400;  // # of rocks

// texture array
GLuint texture[10];

//camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 20.0f, 100.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
// camera controllers
bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// lighting colors
GLfloat brightness_diffuse = 1.0f;
GLfloat brightness_specular = 1.0f;
GLfloat brightness_specular2 = 0.5f;

// FOR TESTING: light x, y, postiion
//float light_x,light_y,light_z = 3.0f;

// dumped variables

//vec3 center = vec3(0.0f, 0.0f, -5.0f);
//glm::vec3 up = vec3(0.0f, +1.0f, 0.0f);

//float car_x = 0;
//float car_z = 0;

//int direction = 1;

//int cam_z = 0;
//int cam_y = +20;
//int cam_x = 0;

//vec3 SCTranslation;
//vec4 SC_world_Front_Direction;
//vec4 SC_world_Right_Direction;
//vec3 SCInitialPos = vec3(0, 0, 0);
//glm::mat4 SC_Rot_M;
//vec4 SC_world_pos;
//vec3 SC_local_pos = vec3(0, +100.0f, -100.0f);
//vec3 SC_local_front = vec3(0, 0, -1);
//vec3 SC_local_right = vec3(1, 0, 0);
//vec4 Camera_world_position = vec4(0, 0, 0,1.0f);

//int oldx = 400;
//vec3 viewRotateDegree = vec3(0,0,0);


// utilities for setting shader parameters 
void setMat4(const std::string &name, glm::mat4& value)
{
	unsigned int transformLoc = glGetUniformLocation(programID, name.c_str());
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(value));
}

void setVec4(const std::string &name, glm::vec4 value)
{
	glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}

void setVec3(const std::string &name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}

void setFloat(const std::string &name, float value)
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void setInt(const std::string &name, int value)
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;

		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

void installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);
}

// keyboard interations
void keyboard(unsigned char key, int x, int y)
{
	// control diffuse light: 'q' increase, 'w' decrease
	if (key == 'q') {
		brightness_diffuse += 0.1f;
		printf("%f\n", brightness_diffuse);
	}
	if (key == 'w') {
		brightness_diffuse -= 0.1f;
		printf("%f\n", brightness_diffuse);
	}

	// control specular light 1 (white): 'a' increase, 's' decrease
	if (key == 'a') {
		brightness_specular += 0.1f;
		printf("%f\n", brightness_specular);
	}
	if (key == 's') {
		if (brightness_specular <= 0.0f)
			brightness_specular = 0.0f;
		else
			brightness_specular -= 0.1f;
		printf("%f\n", brightness_specular);
	}

	// control specular light 2 (red): 'z' increase, 'x' decrease
	if (key == 'z') {
		brightness_specular2 += 0.1f;
		printf("%f\n", brightness_specular2);
	}
	if (key == 'x') {
		if (brightness_specular2 <= 0.0f)
			brightness_specular2 = 0.0f;
		else
			brightness_specular2 -= 0.1f;
		printf("%f\n", brightness_specular2);
	}

	// FOR TESTING: control light x, y, postiion
	/*
	if (key == 'f') {
		light_x -= 50;
		printf("%f\n", light_x);
	}
	if (key == 'h') {
		light_x += 50;
		printf("%f\n", light_x);
	}
	if (key == 'u') {
		light_y += 50;
		printf("%f\n", light_y);
	}
	if (key == 'j') {
		light_y -= 50;
		printf("%f\n", light_y);
	}
	if (key == 't') {
		light_z -= 50;
		printf("%f\n", light_z);
	}
	if (key == 'g') {
		light_z += 50;
		printf("%f\n", light_z);
	}
	if (key == '0') {  // reset to 0
		light_x = 0;
		light_y = 0;
		light_z = 0;
	}
	*/
}

// move spacecraft and camera
void move(int key, int x, int y)
{
	//	cam_x = x * 0.3 - 100;
	//	cam_z = y * 0.3 - 100;
	float cameraSpeed = 200.0f*0.1f;

	if (key == GLUT_KEY_DOWN) {
		cameraPos -= cameraSpeed * cameraFront;
		//SCTranslation[0] = SCTranslation[0] - 15*SC_world_Front_Direction[0];
		//SCTranslation[2] = SCTranslation[2] - 15*SC_world_Front_Direction[2];
		//switch (direction) {
		//case 1: 
		//	cam_x-= 1; 
		//	//car_x -= 1;
		//	break;
		//case 2: 
		//	cam_z-= 1; 
		//	//car_z -= 1;
		//	break;
		//case 3:
		//	cam_x+= 1; 
		//	//car_x -= 1; 
		//	break;
		//case 4: 
		//	cam_z+= 1; 
		//	//car_z += 1; 
		//	break;
		//}
		//printf("move to (%d,0,%d) \n", cam_x,cam_z);
	}
	if (key == GLUT_KEY_UP) {
		cameraPos += cameraSpeed * cameraFront;
		//SCTranslation[0] = SCTranslation[0] + 15*SC_world_Front_Direction[0];
		//SCTranslation[2] = SCTranslation[2] + 15*SC_world_Front_Direction[2];

		//switch (direction) {
		//case 1: 
		//	cam_x+= 1; 
		//	//car_x += 1; 
		//	break;
		//case 2: 
		//	cam_z+= 1;
		//	//car_z += 1; 
		//	break;
		//case 3: 
		//	cam_x-= 1;
		//	//car_x -= 1; 
		//	break;
		//case 4: 
		//	cam_z-= 1;
		//	//car_z -= 1; 
		//	break;
		//}
		//printf("move to (%d,0,%d) \n", cam_x,cam_z);
	}
	if (key == GLUT_KEY_LEFT) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		//SCTranslation[0] = SCTranslation[0] - 15*SC_world_Right_Direction[0];
		//SCTranslation[2] = SCTranslation[2] - 15*SC_world_Right_Direction[2];
		//spacecraft_rot_y += angle;
		//if (direction == 1) {
		//	direction = 4;
		//}
		//else
		//	direction++;
		//printf("rot: %d\n", direction);
	}
	if (key == GLUT_KEY_RIGHT) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		//SCTranslation[0] = SCTranslation[0] + 15*SC_world_Right_Direction[0];
		//SCTranslation[2] = SCTranslation[2] + 15*SC_world_Right_Direction[2];
		//spacecraft_rot_y -= angle;
		//if (direction == 4) {
		//	direction = 1;
		//}
		//else
		//	direction--;
		//printf("rot: %d\n", direction);
	}
	//printf("SC loc %f %f %f\n", SCTranslation[0], SCTranslation[1], SCTranslation[2]);
	//vec3 jojo = glm::vec3(SCInitialPos[0] + SCTranslation[0], SCInitialPos[1] + SCTranslation[1], SCInitialPos[2] + SCTranslation[2]);
	//printf("tranlation  %f %f %f\n", jojo[0], jojo[1], jojo[2]);
	//vec4 HAHA = glm::normalize(SC_world_Front_Direction);
	//printf("SCWR %f %f %f\n", SC_world_Right_Direction[0], SC_world_Right_Direction[1], SC_world_Right_Direction[2]);
	//printf("SCWF %f %f %f\n", HAHA[0], HAHA[1], HAHA[2]);
}

// mouse interaction
void PassiveMouse(int xpos, int ypos)
{

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = -xpos + lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	//front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	//spacecraft_rot_y =-glm::radians(yaw)+M_PI/2;
	cameraFront = glm::normalize(front);
	std::cout<< cameraFront.x <<cameraFront.z<<"\n";
	//ok la
	if (xpos < lastX) {
		spacecraft_rot_y -= 0.01f;
	}
	if (xpos > lastX) {
		spacecraft_rot_y += 0.01f;
	}
	lastX = (float)xpos;

	lastX = xpos;
	lastY = ypos;

	//tutorial
	//cout << "mouse " << xpos<<'\n';
	//if (xpos < oldx) {
	//	viewRotateDegree[1] += 1.0f;
	//	SC_Rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(viewRotateDegree[1]), glm::vec3(0.0f, 1.0f, 0.0f));
	//}
	//if(xpos > oldx) {
	//	viewRotateDegree[1] -= 1.0f;
	//	SC_Rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(viewRotateDegree[1]), glm::vec3(0.0f, 1.0f, 0.0f));
	//}
	//oldx = xpos;
	//printf("rotate %f", viewRotateDegree[1]);
}

// create random matrix for asteroid cloud
void CreateRand_ModelM() {
	modelMatrices = new glm::mat4[amount];
	// initialize random seed
	srand(glutGet(GLUT_ELAPSED_TIME));
	GLfloat radius = 80.0f;
	GLfloat offset = 8.5f;
	GLfloat displacement;
	for (GLuint i = 0; i < amount; i++) {
		glm::mat4 model;
		// 1. Translation: randomly displace along circle with radius 'radius' in range [-offset, offset]
		GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
		// x
		displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat x = sin(angle) * radius + displacement;
		// y
		displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat y = displacement * 0.4f + 1;
		// x
		displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. Scale: scale b/w 0.05 & 0.25f
		GLfloat scale = (rand() % 10) / 10.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. Rotation: add random rotation around a (semi)randomly picked potation axis vector
		GLfloat rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. Add to list of matrices
		modelMatrices[i] = model;
	}
}

bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 6 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i<vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}
	std::cout << "LOADED!";
	return true;
}

GLuint loadBMP_custom(const char * imagepath) {

	printf("Reading image %s\n", imagepath);

	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	unsigned char * data;

	FILE * file = fopen(imagepath, "rb");
	if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }

	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	if (imageSize == 0)    imageSize = width * height * 3;
	if (dataPos == 0)      dataPos = 54;

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);


	GLuint textureID;
	//TODO: Create one OpenGL texture and set the texture parameter 

	glGenTextures(1, &textureID);
	// bind created texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	// give image to opgl
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	

	delete[] data;

	// specify texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);


	return textureID;
}

void sendDataToOpenGL()
{
	//TODO:
	//Load objects and bind to VAO & VBO
	//Load texture

	//gen buffers
	glGenVertexArrays(10, vaoID);
	glGenBuffers(10, vboID);
	glGenBuffers(10, uvboID);
	glGenBuffers(10, nboID);

	// load spacecraft
	loadOBJ("spaceCraft.obj", verticesA, uvsA, normalsA);
	// vao
	glBindVertexArray(vaoID[0]);
	// vbo
	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, verticesA.size() * sizeof(glm::vec3), &verticesA[0], GL_STATIC_DRAW);
	// uvbo
	glBindBuffer(GL_ARRAY_BUFFER, uvboID[0]);
	glBufferData(GL_ARRAY_BUFFER, uvsA.size() * sizeof(glm::vec2), &uvsA[0], GL_STATIC_DRAW);
	// nbo
	glBindBuffer(GL_ARRAY_BUFFER, nboID[0]);
	glBufferData(GL_ARRAY_BUFFER, normalsA.size() * sizeof(glm::vec3), &normalsA[0], GL_STATIC_DRAW);

	// load earth
	loadOBJ("planet.obj", verticesB, uvsB, normalsB);
	glBindVertexArray(vaoID[1]);

	glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, verticesB.size() * sizeof(glm::vec3), &verticesB[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uvboID[1]);
	glBufferData(GL_ARRAY_BUFFER, uvsB.size() * sizeof(glm::vec2), &uvsB[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, nboID[1]);
	glBufferData(GL_ARRAY_BUFFER, normalsB.size() * sizeof(glm::vec3), &normalsB[0], GL_STATIC_DRAW);

	// load rings
	for (int i = 0; i < sizeof(verticesC) / sizeof(verticesC[0]); i++) {
		std::cout << 2+i <<" ringID\n";
		loadOBJ("ring.obj", verticesC[i], uvsC[i], normalsC[i]);
		glBindVertexArray(vaoID[2+i]);

		glBindBuffer(GL_ARRAY_BUFFER, vboID[2 + i]);
		glBufferData(GL_ARRAY_BUFFER, verticesC[i].size() * sizeof(glm::vec3), &verticesC[i][0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uvboID[2 + i]);
		glBufferData(GL_ARRAY_BUFFER, uvsC[i].size() * sizeof(glm::vec2), &uvsC[i][0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, nboID[2 + i]);
		glBufferData(GL_ARRAY_BUFFER, normalsC[i].size() * sizeof(glm::vec3), &normalsC[i][0], GL_STATIC_DRAW);
	}

	// load wonder star
	loadOBJ("planet.obj", verticesD, uvsD, normalsD);
	cout <<"wonderstar ID "<< wonderstarID << "\n";
	glBindVertexArray(vaoID[wonderstarID]);

	glBindBuffer(GL_ARRAY_BUFFER, vboID[wonderstarID]);
	glBufferData(GL_ARRAY_BUFFER, verticesD.size() * sizeof(glm::vec3), &verticesD[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uvboID[wonderstarID]);
	glBufferData(GL_ARRAY_BUFFER, uvsD.size() * sizeof(glm::vec2), &uvsD[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, nboID[wonderstarID]);
	glBufferData(GL_ARRAY_BUFFER, normalsD.size() * sizeof(glm::vec3), &normalsD[0], GL_STATIC_DRAW);

	// load rock
	loadOBJ("rock.obj", verticesE, uvsE, normalsE);
	glBindVertexArray(vaoID[rockID]);

	glBindBuffer(GL_ARRAY_BUFFER, vboID[rockID]);
	glBufferData(GL_ARRAY_BUFFER, verticesE.size() * sizeof(glm::vec3), &verticesE[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uvboID[rockID]);
	glBufferData(GL_ARRAY_BUFFER, uvsE.size() * sizeof(glm::vec2), &uvsE[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, nboID[rockID]);
	glBufferData(GL_ARRAY_BUFFER, normalsE.size() * sizeof(glm::vec3), &normalsE[0], GL_STATIC_DRAW);

	// load texture
	texture[0] = loadBMP_custom("texture/spacecraftTexture.bmp");
	texture[1] = loadBMP_custom("texture/earthTexture.bmp");
	texture[2] = loadBMP_custom("texture/ringTexture.bmp");
	texture[3] = loadBMP_custom("texture/WonderStarTexture.bmp"); 
	texture[4] = loadBMP_custom("texture/RockTexture.bmp");
	texture[5] = loadBMP_custom("texture/green.bmp"); 
	texture[6] = loadBMP_custom("texture/earth_normal.bmp");
}

void paintGL(void)
{
	// bg color: black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width_W, height_W);

	// camera library

	//cameraPos = vec3(0.0f, 40.0f, 0);
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	GLint viewMatrixUniformLocation = glGetUniformLocation(programID, "viewMatrix");
	glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, &view[0][0]);
	
	////camera new
	//glm::vec3 cameraSight = center = vec3(0, 0.0f, -10.0f);
	//glm::vec3 cameraPostition = glm::vec3(cam_x, cam_y*1.0f, cam_z);
	//mat4 viewMatrix = glm::lookAt(cameraPostition, cameraPostition + cameraSight, up);
	//GLint viewMatrixUniformLocation = glGetUniformLocation(programID, "viewMatrix");
	//glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	//projection setting
	mat4 projectionMatrix = glm::perspective(100.0f, 1.0f, 1.0f, 2500.0f);
	GLint projectionMatrixUniformLocation = glGetUniformLocation(programID, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	//vec3 carCam = cameraPos;
	vec3 carCam = cameraPos + vec3(0.0f, 30.0f, -150.0f);

	//camera
	//glm::vec3 cameraSight = center = vec3(0.0f, -10.0f, -1.0f);
	//glm::vec3 cameraPostition = glm::vec3(cam_x, cam_y, cam_z);
	//mat4 viewMatrix = glm::lookAt(cameraPostition, cameraPostition + cameraSight, up);
	//GLint viewMatrixUniformLocation = glGetUniformLocation(programID, "viewMatrix");
	//glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	////projection setting
	//mat4 projectionMatrix = glm::perspective(20.0f, 1.0f, 1.0f, 100.0f);
	//GLint projectionMatrixUniformLocation = glGetUniformLocation(programID, "projectionMatrix");
	//glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, &projectionMatrix[0][0]);


	// lighting

	// ambient
	GLint ambientLightUniformlocation = glGetUniformLocation(programID, "ambientLight");
	vec3 ambientLight(0.1f, 0.1f, 0.1f); // RGB
	glUniform3fv(ambientLightUniformlocation, 1, &ambientLight[0]);

	// diffuse
	GLint diffuseLightUniformLocation = glGetUniformLocation(programID, "diffuseLight");
	vec3 diffuseLight(1.0f, 1.0f, 1.0f);
	glUniform3fv(diffuseLightUniformLocation, 1, &diffuseLight[0]);

	//specular
	GLint eyePositionUniformLocation = glGetUniformLocation(programID, "eyePositionWorld");
	vec3 eyePosition = cameraPos;
	glUniform3fv(eyePositionUniformLocation, 1, &eyePosition[0]);
	// specular light 1 position
	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPositionWorld");
	vec3 lightPosition(200.0f, -200.0f, -50.0f);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);
	// specular light 2 position
	GLint lightPositionUniformLocation2 = glGetUniformLocation(programID, "lightPositionWorld2");
	vec3 lightPosition2(-300.0f, -300.0f, -200.0f);
	glUniform3fv(lightPositionUniformLocation2, 1, &lightPosition2[0]);
	
	// control lighting
	// diffuse brightness
	GLint brightness_diffuse_location = glGetUniformLocation(programID, "brightness_diffuse");
	glUniform1f(brightness_diffuse_location, brightness_diffuse);

	// specular 1 (white) brightness
	GLint brightness_specular_location = glGetUniformLocation(programID, "brightness_specular");
	glUniform1f(brightness_specular_location, brightness_specular);

	// specular 2 (red) brightness
	GLint brightness_specular_location2 = glGetUniformLocation(programID, "brightness_specular2");
	glUniform1f(brightness_specular_location2, brightness_specular2);

	// define trasnformation matrix
	mat4 modelTransformMatrix = glm::mat4(1.0f);
	GLint modelTransformMatrixUniformLocation = glGetUniformLocation(programID, "modelTransformMatrix");

	// define shading
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	GLuint TextureID1 = glGetUniformLocation(programID, "myTextureSampler2");

	// draw spaceCraft
	// bind vao
	glBindVertexArray(vaoID[0]);
	glEnableVertexAttribArray(0);
	// bind vbo
	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glVertexAttribPointer(
		0,//attribute
		3,//size
		GL_FLOAT,//type
		GL_FALSE,//normalize
		0,//stride
		(void*)0//array buffer offset
	);
	// bind uvbo
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvboID[0]);
	glVertexAttribPointer(
		1,//attribute
		2,//size
		GL_FLOAT,//type
		GL_FALSE,//normalize
		0,//stride
		(void*)0//array buffer offset
	);
	// bind nbo
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, nboID[0]);
	glVertexAttribPointer(
		2,//attribute
		3,//size
		GL_FLOAT,//type
		GL_FALSE,//normalize
		0,//stride
		(void*)0//array buffer offset
	);
	// bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glUniform1i(TextureID, 0);
	// transformation
	modelTransformMatrix = glm::mat4(1.0f);

	//translate
	//tutorial
	//float  scale = 0.001;
	//glm::mat4 SC_scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
	//glm::mat4 SC_trans_M = glm::translate
	//(
	//	glm::mat4(1.0f),
	//	glm::vec3(SCInitialPos[0] + SCTranslation[0], SCInitialPos[1] + SCTranslation[1], SCInitialPos[2] + SCTranslation[2])
	//);
	//glm::mat4 Model_matrix = SC_trans_M*SC_Rot_M*SC_scale_M;
	//SC_world_pos = Model_matrix * glm::vec4(SC_local_pos, 1.0f);
	//SC_world_Front_Direction = Model_matrix * vec4(SC_local_front, 1.0f);
	//SC_world_Right_Direction = Model_matrix * vec4(SC_local_right, 1.0f);
	//SC_world_Front_Direction = glm::normalize(SC_world_Front_Direction);
	//SC_world_Right_Direction = glm::normalize(SC_world_Right_Direction);
	
	//cameraPos = vec3(30.0f, 30.0f, 0);

	//Camera_world_position = Model_matrix * glm::vec4(cameraPos, 1.0f);

	//glm::mat4 view = glm::lookAt(vec3(Camera_world_position), vec3(Camera_world_position) + vec3(0,0,-100), vec3(0, 1.0f, 0));
	//glm::mat4 view = glm::lookAt(vec3(Camera_world_position), vec3(SC_world_pos), vec3(0,1.0f,0));
	//GLint viewMatrixUniformLocation = glGetUniformLocation(programID, "viewMatrix");
	//glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, &view[0][0]);
	//glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &Model_matrix[0][0]);
	//glDrawArrays(GL_TRIANGLES, 0, verticesA.size());
		//onlinecode
	//vec3 movement = vec3(car_x,0.0f,car_z);
	//vec3 movement = vec3(car_x, 0.0f, car_z) + carCam;

	vec3 movement = vec3(0, 20.0f, -40.0f) + carCam;
	int ringGreenSignal[3] = { 0 };
	countingdummy += 1;
	if (countingdummy % 1000 == 0) {
		printf("SC loc %f %f %f\n", movement[0], movement[1], movement[2]);
	}	
	
	if (movement[0]<=40.0f && movement[0] >=-40.0f) {
		for (int i = 0; i < sizeof(verticesC) / sizeof(verticesC[0]); i++) {
			if (movement[2] <= ringCoordinates[i][2]+100.0f && movement[2] >= ringCoordinates[i][2]-100.0f) {
				ringGreenSignal[i] = 1;
				//printf("ring %d is penetrated!\n", i);
				glBindTexture(GL_TEXTURE_2D, texture[5]); //bind green texture 
				glUniform1i(TextureID, 0);
				break;
			}
		}
	}
	modelTransformMatrix =
		glm::translate(glm::mat4(), movement)
		* glm::rotate(mat4(), spacecraft_rot_y, vec3(0, 1, 0))
		* glm::rotate(mat4(), spacecraft_rot_z, vec3(1, 0, 0))
		* glm::scale(glm::mat4(), glm::vec3(0.05f, 0.05f, 0.05f));
		//*glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, verticesA.size());


	// draw earth 
	glBindVertexArray(vaoID[1]);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
	glVertexAttribPointer(
		0,//attribute
		3,//size
		GL_FLOAT,//type
		GL_FALSE,//normalize
		0,//stride
		(void*)0//array buffer offset
	);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvboID[1]);
	glVertexAttribPointer(
		1,//attribute
		2,//size
		GL_FLOAT,//type
		GL_FALSE,//normalize
		0,//stride
		(void*)0//array buffer offset
	);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, nboID[1]);
	glVertexAttribPointer(
		2,//attribute
		3,//size
		GL_FLOAT,//type
		GL_FALSE,//normalize
		0,//stride
		(void*)0//array buffer offset
	);
	// bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glUniform1i(TextureID, 0);
	// normal mapping
	// set normal mapping flag = true
	GLint normalmap_flag = glGetUniformLocation(programID, "normalMapping_flag");
	glUniform1f(normalmap_flag, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glUniform1i(TextureID1, 1);
	// transformation
	block_rot_x += 1;
	modelTransformMatrix =
		glm::translate(glm::mat4(), vec3(0.0f, 30.0f, -2500.0f))
		* glm::rotate(mat4(), 0.001f*block_rot_x, vec3(0, 1, 0))
		* glm::rotate(mat4(), 0.0f, vec3(1, 0, 0))
		* glm::scale(glm::mat4(), glm::vec3(20.0f, 20.0f, 20.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, verticesB.size());
	// set normal mapping flag back to false
	glUniform1f(normalmap_flag, 0);


	// draw rings
	for (int i = 0; i < sizeof(verticesC)/sizeof(verticesC[0]); i++) {
		glBindVertexArray(vaoID[2 + i]);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vboID[2 + i]);
		glVertexAttribPointer(
			0,//attribute
			3,//size
			GL_FLOAT,//type
			GL_FALSE,//normalize
			0,//stride
			(void*)0//array buffer offset
		);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvboID[2 + i]);
		glVertexAttribPointer(
			1,//attribute
			2,//size
			GL_FLOAT,//type
			GL_FALSE,//normalize
			0,//stride
			(void*)0//array buffer offset
		);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, nboID[2 + i]);
		glVertexAttribPointer(
			2,//attribute
			3,//size
			GL_FLOAT,//type
			GL_FALSE,//normalize
			0,//stride
			(void*)0//array buffer offset
		);
		// bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glUniform1i(TextureID, 0);
	
		// transformation
		float ring_rot = M_PI/2.0f;
		modelTransformMatrix = glm::mat4(1.0f);
		ringCoordinates[i] = vec3(0.0f, 60.0f, -i*500.0f - 1000.0f);
		if (countingdummy % 1000 == 0) {
			printf("ring %d loc %f %f %f\n", i,ringCoordinates[i][0], ringCoordinates[i][1], ringCoordinates[i][2]);
		}
		if (ringGreenSignal[i] == 1) {
			glBindTexture(GL_TEXTURE_2D, texture[5]); //bind texture 
			glUniform1i(TextureID, 0);
		}

		modelTransformMatrix =
			glm::translate(glm::mat4(), ringCoordinates[i])
			* glm::rotate(mat4(), block_rot_x*0.0005f, vec3(0, 1, 0))
			* glm::rotate(mat4(), ring_rot*1.0f, vec3(1, 0, 0))
			* glm::scale(glm::mat4(), glm::vec3(0.8f, 0.8f, 0.8f));
		glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, verticesC[i].size());
	}


	// draw wonderstar 
	glBindVertexArray(vaoID[wonderstarID]);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vboID[wonderstarID]);
	glVertexAttribPointer(
		0,//attribute
		3,//size
		GL_FLOAT,//type
		GL_FALSE,//normalize
		0,//stride
		(void*)0//array buffer offset
	);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvboID[wonderstarID]);
	glVertexAttribPointer(
		1,//attribute
		2,//size
		GL_FLOAT,//type
		GL_FALSE,//normalize
		0,//stride
		(void*)0//array buffer offset
	);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, nboID[wonderstarID]);
	glVertexAttribPointer(
		2,//attribute
		3,//size
		GL_FLOAT,//type
		GL_FALSE,//normalize
		0,//stride
		(void*)0//array buffer offset
	);
	// bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glUniform1i(TextureID, 0);
	// transformation
	block_rot_x += 1;
	modelTransformMatrix =
		glm::translate(glm::mat4(), vec3(-10.0f, 20.0f, -200.0f))
		* glm::rotate(mat4(), 0.001f*block_rot_x, vec3(0, 1, 0))
		* glm::rotate(mat4(), 0.0f, vec3(1, 0, 0))
		* glm::scale(glm::mat4(), glm::vec3(20.0f, 20.0f, 20.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, verticesD.size());


	// draw asteroid cloud
	for (GLuint i = 0; i < amount; i++) {
		glBindVertexArray(vaoID[rockID]);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vboID[rockID]);
		glVertexAttribPointer(
			0,//attribute
			3,//size
			GL_FLOAT,//type
			GL_FALSE,//normalize
			0,//stride
			(void*)0//array buffer offset
		);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvboID[rockID]);
		glVertexAttribPointer(
			1,//attribute
			2,//size
			GL_FLOAT,//type
			GL_FALSE,//normalize
			0,//stride
			(void*)0//array buffer offset
		);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, nboID[rockID]);
		glVertexAttribPointer(
			2,//attribute
			3,//size
			GL_FLOAT,//type
			GL_FALSE,//normalize
			0,//stride
			(void*)0//array buffer offset
		);
		// bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[4]);
		glUniform1i(TextureID, 0);
		// transformation
		modelTransformMatrix =
			glm::translate(glm::mat4(), vec3(-10.0f, 30.0f, -200.0f))
			* glm::rotate(mat4(), 0.005f*block_rot_x, vec3(0, 1, 0))
			* glm::rotate(mat4(), 0.0f, vec3(1, 0, 0))
			* modelMatrices[i];
		glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, verticesE.size());
	}

	glFlush();
	glutPostRedisplay();
}

void initializedGL(void) //run only once
{
	glewInit();
	installShaders();
	sendDataToOpenGL();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(width_W, height_W);  // window size
	glutCreateWindow("CSCI3260 Project");  // window title

	// callback functions
	initializedGL();
	glutDisplayFunc(paintGL);
	glEnable(GL_DEPTH_TEST);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(move);
	glutPassiveMotionFunc(PassiveMouse);
	CreateRand_ModelM();
	
	glutMainLoop();

	return 0;
}