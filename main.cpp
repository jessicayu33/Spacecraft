
/*********************************************************
Student Information
Student ID: 1155077785
Student Name: Li Ho Yin
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
int width_W = 1024;
int height_W = 800;
// Could define the Vao&Vbo and interaction parameter here
// init Vertex Array Object
GLuint vaoID[10];
// init Vertex Buffer Object
GLuint vboID[10];
GLuint uvboID[10];
GLuint nboID[10];
//obj SpaceCraft
std::vector<glm::vec3> verticesA;
std::vector<glm::vec2> uvsA;
std::vector<glm::vec3> normalsA;
//obj Earth
std::vector<glm::vec3> verticesB;
std::vector<glm::vec2> uvsB;
std::vector<glm::vec3> normalsB;
//obj Rings
std::vector<glm::vec3> verticesC[3];
std::vector<glm::vec2> uvsC[3];
std::vector<glm::vec3> normalsC[3];

//obj Wonder Star
int wonderstarID = sizeof(verticesC) / sizeof(verticesC[0]) + 2;
std::vector<glm::vec3> verticesD;
std::vector<glm::vec2> uvsD;
std::vector<glm::vec3> normalsD;

//obj Rock
int rockID = wonderstarID + 1;
std::vector<glm::vec3> verticesE;
std::vector<glm::vec2> uvsE;
std::vector<glm::vec3> normalsE;
glm::mat4* modelMatrices;
GLuint amount = 400;

GLuint texture[10];
vec3 ringCoordinates[3];
//camera setting
glm::vec3 cameraPos = glm::vec3(0.0f, 20.0f, 100.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;
//a series utilities for setting shader parameters 
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
int tri_rot_x, tri_rot_y = 0;
vec3 center = vec3(0.0f, 0.0f, -5.0f);
glm::vec3 up = vec3(0.0f, +1.0f, 0.0f);

GLfloat brightness_diffuse = 1.0f;
GLfloat brightness_specular = 0.0f;
int texID = 1;
int current_texID = 1;

int block_rotate = 1;
int block_rot_x = 1;

int ctrl = 0;

float light_x,light_y,light_z = 0.0f;

float lastposition = 0.0f;

float camAngle = 0;
float iX = 0, iY = -0.8, iZ = 0, lX = 0, lY = -0.8, lZ = -1, uX = 0, uY = 1, uZ = 0;

void keyboard(unsigned char key, int x, int y)
{
	//TODO: Use keyboard to do interactive events and animation
	if (key == 'f')
	{
		light_x -= 1;
	}
	if (key == 'h')
	{
		light_x += 1;
	}
	if (key == 't')
	{
		light_z -= 1;
	}
	if (key == 'g')
	{
		light_z += 1;
	}

	if (key == 'q') {
		brightness_diffuse += 0.1f;
		printf("%f\n", brightness_diffuse);
	}
	if (key == 'w') {
		brightness_diffuse -= 0.1f;
		printf("%f\n", brightness_diffuse);
	}
	if (key == 'z') {
		brightness_specular += 0.1f;
		printf("%f\n", brightness_specular);
	}
	if (key == 'x') {
		if (brightness_specular <= 0.0f)
			brightness_specular = 0.0f;
		else
			brightness_specular -= 0.1f;
		printf("%f\n", brightness_specular);
	}

	if (key == '1') {
		texID = 1;
	}
	if (key == '2') {
		texID = 2;
	}
	if (key == '3') {
		texID = 3;
	}

	if (key == 's') {
		if (block_rotate == 1)
			block_rotate = 0;
		else
			block_rotate = 1;
	}
	
	if (key == ' ') {
		if (ctrl == 1)
			ctrl = 0;
		else
			ctrl = 1;
	}
	float fraction = 1.5f;
	switch (key) {
	case 'a':
		camAngle -= 0.1f;
		lX = sin(camAngle);
		lZ = -cos(camAngle);
		break;
	case 'd':
		camAngle += 0.1f;
		lX = sin(camAngle);
		lZ = -cos(camAngle);
		break;
	case 'w':
		iX += lX * fraction;
		iZ += lZ * fraction;
		break;
	case 's':
		iX -= lX * fraction;
		iZ -= lZ * fraction;
		break;
	default:
		break;
	}
}

float car_x = 0;
float car_z = 0;
float car_rot_y = 0;
float car_rot_z = M_PI;
int direction = 1;
int current_direction = 1;

int cam_z = 0;
int cam_y = +20;
int cam_x = 0;

vec3 SCTranslation;
vec4 SC_world_Front_Direction;
vec4 SC_world_Right_Direction;
vec3 SCInitialPos = vec3(0, 0, 0);
glm::mat4 SC_Rot_M;
vec4 SC_world_pos;
vec3 SC_local_pos = vec3(0, +100.0f, -100.0f);
vec3 SC_local_front = vec3(0, 0, -1);
vec3 SC_local_right = vec3(1, 0, 0);
vec4 Camera_world_position = vec4(0, 0, 0,1.0f);
// direction
void move(int key, int x, int y)
{
	//TODO: Use arrow keys to do interactive events and animation
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
		//car_rot_y += angle;
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
		//car_rot_y -= angle;
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
int oldx = 400;
vec3 viewRotateDegree = vec3(0,0,0);
void PassiveMouse(int xpos, int ypos)
{
	//TODO: Use Mouse to do interactive events and animation

	//if (firstMouse)
	//{
	//	lastX = xpos;
	//	lastY = ypos;
	//	firstMouse = false;
	//}

	//float xoffset = -xpos + lastX;
	//float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	//lastX = xpos;
	//lastY = ypos;

	//float sensitivity = 0.1f; // change this value to your liking
	//xoffset *= sensitivity;
	//yoffset *= sensitivity;

	//yaw += xoffset;
	//pitch += yoffset;

	//// make sure that when pitch is out of bounds, screen doesn't get flipped
	//if (pitch > 89.0f)
	//	pitch = 89.0f;
	//if (pitch < -89.0f)
	//	pitch = -89.0f;

	//glm::vec3 front;
	//front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	////front.y = sin(glm::radians(pitch));
	//front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	////car_rot_y =-glm::radians(yaw)+M_PI/2;
	//cameraFront = glm::normalize(front);
	//std::cout<< cameraFront.x <<cameraFront.z<<"\n";

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

	//ok la
	if (xpos < lastX) {
		car_rot_y -= 0.03f;
	}
	if (xpos > lastX) {
		car_rot_y += 0.03f;
	}
	lastX = (float)xpos;
}

void mousewheel(int button, int dir, int x, int y) {

}
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

	// load obj
	loadOBJ("spaceCraft.obj", verticesA, uvsA, normalsA);
	glBindVertexArray(vaoID[0]);
	// for vbo
	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, verticesA.size() * sizeof(glm::vec3), &verticesA[0], GL_STATIC_DRAW);
	// for uvbo
	glBindBuffer(GL_ARRAY_BUFFER, uvboID[0]);
	glBufferData(GL_ARRAY_BUFFER, uvsA.size() * sizeof(glm::vec2), &uvsA[0], GL_STATIC_DRAW);
	// for normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, nboID[0]);
	glBufferData(GL_ARRAY_BUFFER, normalsA.size() * sizeof(glm::vec3), &normalsA[0], GL_STATIC_DRAW);

	// load obj
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

	// load obj
	loadOBJ("planet.obj", verticesD, uvsD, normalsD);
	cout <<"wonderstar ID "<< wonderstarID << "\n";
	glBindVertexArray(vaoID[wonderstarID]);

	glBindBuffer(GL_ARRAY_BUFFER, vboID[wonderstarID]);
	glBufferData(GL_ARRAY_BUFFER, verticesD.size() * sizeof(glm::vec3), &verticesD[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uvboID[wonderstarID]);
	glBufferData(GL_ARRAY_BUFFER, uvsD.size() * sizeof(glm::vec2), &uvsD[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, nboID[wonderstarID]);
	glBufferData(GL_ARRAY_BUFFER, normalsD.size() * sizeof(glm::vec3), &normalsD[0], GL_STATIC_DRAW);

	//OBJ E
	// load rocks
	loadOBJ("rock.obj", verticesE, uvsE, normalsE);
	glBindVertexArray(vaoID[rockID]);

	glBindBuffer(GL_ARRAY_BUFFER, vboID[rockID]);
	glBufferData(GL_ARRAY_BUFFER, verticesE.size() * sizeof(glm::vec3), &verticesE[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uvboID[rockID]);
	glBufferData(GL_ARRAY_BUFFER, uvsE.size() * sizeof(glm::vec2), &uvsE[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, nboID[rockID]);
	glBufferData(GL_ARRAY_BUFFER, normalsE.size() * sizeof(glm::vec3), &normalsE[0], GL_STATIC_DRAW);



	texture[0] = loadBMP_custom("texture/spacecraftTexture.bmp");
	texture[1] = loadBMP_custom("texture/earthTexture.bmp");
	texture[2] = loadBMP_custom("texture/ringTexture.bmp");
	texture[3] = loadBMP_custom("texture/WonderStarTexture.bmp"); 
	texture[4] = loadBMP_custom("texture/RockTexture.bmp");

	

}

void paintGL(void)
{
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //specify the background color
	glClearColor(0.1f, 0.1f, 0.1f, 0.1f); //specify the background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 800, 800);

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

	//lighting

	// light source
	

	// ambient
	GLint ambientLightUniformlocation = glGetUniformLocation(programID, "ambientLight");
	vec3 ambientLight(0.8, 0.8f, 0.8f); // RGB
	glUniform3fv(ambientLightUniformlocation, 1, &ambientLight[0]);

	//specular
	GLint eyePositionUniformLocation = glGetUniformLocation(programID, "eyePositionWorld");
	//vec3 eyePosition(0.0f, 0.0f, 0.0f);
	vec3 eyePosition = cameraPos;
	glUniform3fv(eyePositionUniformLocation, 1, &eyePosition[0]);
	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPositionWorld");
	//vec3 lightPosition(5.0f, 15.0f, -10.0f);
	vec3 lightPosition(light_x, 50.0f,light_z);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);
	
	//user control 
	GLint brightness_diffuse_location = glGetUniformLocation(programID, "brightness_diffuse");
	glUniform1f(brightness_diffuse_location, brightness_diffuse);

	GLint brightness_specular_location = glGetUniformLocation(programID, "brightness_specular");
	glUniform1f(brightness_specular_location, brightness_specular);

	//if (texID != current_texID) {
	//	switch (texID) {
	//	case 1: texture[2] = loadBMP_custom("theme1.bmp"); break;
	//	case 2: texture[2] = loadBMP_custom("theme2.bmp"); break;
	//	case 3: texture[2] = loadBMP_custom("theme3.bmp"); break;
	//	default:texture[2] = loadBMP_custom("theme1.bmp"); break;
	//	}
	//	current_texID = texID;
	//}
	

	mat4 modelTransformMatrix = glm::mat4(1.0f);

	GLint modelTransformMatrixUniformLocation = glGetUniformLocation(programID, "modelTransformMatrix");
	// load spaceCraft 
	glBindVertexArray(vaoID[0]);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glVertexAttribPointer(
		0,//attribute
		3,//size
		GL_FLOAT,//type
		GL_FALSE,//normalize
		0,//stride
		(void*)0//array buffer offset
	);
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

	// shading
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]); //bind texture 
	glUniform1i(TextureID, 0);
	// transform
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
	vec3 movement = vec3(0, 20.0f, -20.0f) + carCam;
	modelTransformMatrix =
		glm::translate(glm::mat4(), movement)
		* glm::rotate(mat4(), car_rot_y, vec3(0, 1, 0))
		* glm::rotate(mat4(), car_rot_z, vec3(1, 0, 0))
		* glm::scale(glm::mat4(), glm::vec3(0.05f, 0.05f, 0.05f));
		//*glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, verticesA.size());

	// load eaerth 
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
	// shading
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[1]); //bind texture 
	glUniform1i(TextureID, 0);
	// transform
	block_rot_x += 1;
	modelTransformMatrix =
		glm::translate(glm::mat4(), vec3(0.0f, 30.0f, -2500.0f))
		* glm::rotate(mat4(), 0.001f*block_rot_x, vec3(0, 1, 0))
		* glm::rotate(mat4(), 0.0f, vec3(1, 0, 0))
		* glm::scale(glm::mat4(), glm::vec3(20.0f, 20.0f, 20.0f));

	
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, verticesB.size());

	// load rings
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
		// shading
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[2]); //bind texture 
		glUniform1i(TextureID, 0);
	
		// transform
		float ring_rot = M_PI/2.0f;
		modelTransformMatrix = glm::mat4(1.0f);
		ringCoordinates[i] = vec3(0.0f, 60.0f, -i*500.0f - 1000.0f);
		modelTransformMatrix =
			glm::translate(glm::mat4(), ringCoordinates[i])
			* glm::rotate(mat4(), block_rot_x*0.0005f, vec3(0, 1, 0))
			* glm::rotate(mat4(), ring_rot*1.0f, vec3(1, 0, 0))
			* glm::scale(glm::mat4(), glm::vec3(0.8f, 0.8f, 0.8f));
		glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, verticesC[i].size());
	}

	// load wonderstar 
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
	// shading
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[3]); //bind texture 
	glUniform1i(TextureID, 0);
	// transform
	block_rot_x += 1;
	modelTransformMatrix =
		glm::translate(glm::mat4(), vec3(-10.0f, 20.0f, -200.0f))
		* glm::rotate(mat4(), 0.001f*block_rot_x, vec3(0, 1, 0))
		* glm::rotate(mat4(), 0.0f, vec3(1, 0, 0))
		* glm::scale(glm::mat4(), glm::vec3(20.0f, 20.0f, 20.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, verticesD.size());


	// asteroid cloud
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
		// shading
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[4]); //bind texture 
		glUniform1i(TextureID, 0);
		// transform
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
	glutInitWindowSize(width_W, height_W);
	glutCreateWindow("Assignment 2");

	//TODO:
	/*Register different CALLBACK function for GLUT to response
	with different events, e.g. window sizing, mouse click or
	keyboard stroke */
	initializedGL();
	glutDisplayFunc(paintGL);
	glEnable(GL_DEPTH_TEST);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(move);
	glutPassiveMotionFunc(PassiveMouse);
	glutMouseWheelFunc(mousewheel);
	CreateRand_ModelM();
	
	glutMainLoop();

	return 0;
}