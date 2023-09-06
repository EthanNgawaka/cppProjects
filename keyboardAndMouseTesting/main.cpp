#include <cstdio>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>

#include <shader.hpp>
#include <map>
using namespace std;

int justWarped = 0;

const int width = 1024;
const int height = 768;

float deltaX = 0.0f;
float deltaY = 0.0f;
float pitch = 0.0f;
float yaw = -90.0f;
const float sensitivity = 0.04f;
const float MAX_DELTA = sensitivity * 2;
const int mouseWarpCooldown = 1; 

const int middleX = width/2;
const int middleY = height/2;

float cameraSpeed = 0.05f;

GLuint vertexArrayID;
GLuint colorBuffer;
GLuint vertexBuffer;
GLuint shaderID;
GLuint matrixID;

glm::mat4 Model;
glm::mat4 View;
glm::mat4 Projection;
glm::mat4 mvp;

glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraPos = glm::vec3(0,0,5);
glm::vec3 cameraFront = glm::vec3(0,0,-1);
glm::vec3 cameraUp = glm::vec3(0,1,0);
glm::vec3 cameraRight;
glm::vec3 playerFront;


map<unsigned char, bool> keys;

shaderManager* shader;

static const GLfloat vertexColorData[] = {
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
};

const int numberOfTriangles = 12;
const GLfloat vertexBufferData[] = {
	-1.0f,-1.0f,-1.0f, // triangle 1 : begin
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, // triangle 1 : end
	1.0f, 1.0f,-1.0f, // triangle 2 : begin
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f, // triangle 2 : end
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f
};

void matrixCalc(){
	Model = glm::mat4(1.0f); // Model matrix
	Projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f); // Proj matrix
	// calculate cameraUp and cameraRight
	glm::vec3 dir;
	dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	dir.y = sin(glm::radians(pitch));
	dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(dir);
	cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
	dir.y = 0;
	playerFront = glm::normalize(dir);

	// View matrix handy dandy lookAt func from glm that handles matrix construction
	View = glm::lookAt(
		cameraPos, // Camera at 4,3,3 in world space
		cameraPos + cameraFront, // looks in dir defined by cameraFront
		cameraUp  // Head is up (0, -1, 0) is upside down
	);

	mvp = Projection * View * Model;
	shader->sendMatrix4("mvp", mvp);
}

void inputHandling(){
	bool redisplayFlag = false;

	if(keys['w']){
		cameraPos = cameraPos + playerFront*cameraSpeed;
		redisplayFlag = true;
	}
	if(keys['s']){
		cameraPos = cameraPos - playerFront*cameraSpeed;
		redisplayFlag = true;
	}
	if(keys['d']){
		cameraPos = cameraPos + cameraRight*cameraSpeed;
		redisplayFlag = true;
	}
	if(keys['a']){
		cameraPos = cameraPos - cameraRight*cameraSpeed;
		redisplayFlag = true;
	}
	if(keys['e']){
		cameraPos = cameraPos + worldUp*cameraSpeed;
		redisplayFlag = true;
	}
	if(keys['q']){
		cameraPos = cameraPos - worldUp*cameraSpeed;
		redisplayFlag = true;
	}

	if(keys[27]){
		glutLeaveMainLoop();
	}

	if(redisplayFlag){
		glutPostRedisplay();
	}
}

void keyboardDown(unsigned char key, int x, int y){
	printf("\nKey: %i", key);
	if(keys.find(key) == keys.end()){
		keys.insert({ key, true });
	}else{
		keys[key] = true;
	}

	glutPostRedisplay();
	return;
}

void keyboardUp(unsigned char key, int x, int y){
	keys[key] = false;

	glutPostRedisplay();
	return;
}

void look(int x, int y) {
	if (justWarped > 0) {
		justWarped -= 1;
		return;
	}
	deltaX = (x - middleX) * sensitivity;
	deltaY = -(y - middleY) * sensitivity;

	yaw += deltaX;
	pitch += deltaY;

	//pitch constraints
	if (pitch < -89.0f) {
		pitch = -89.0f;
	} else if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	printf("\ndeltaX: %f, deltaY: %f\nx: %i, y: %i\n", deltaX, deltaY, x, y);
	justWarped = mouseWarpCooldown;
	glutWarpPointer(width / 2, height / 2);
	glutPostRedisplay();
}

void mainDisplayFunc(){
	inputHandling();
	matrixCalc();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader->getShaderID());
	// 1st atrrib buffer
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); /// MAKE SURE YOURE BINDING THE RIGHT BUFFER!!!!
	glVertexAttribPointer(
		0, // attr identifier numbr
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized???
		0, // stride
		(void*)0 // array buffer offset
	);

	// 2nd atrrib buffer
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(
		1, // attr identifier numbr
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized???
		0, // stride
		(void*)0 // array buffer offset
	);
	glDrawArrays(GL_TRIANGLES, 0, 3 * numberOfTriangles); // GL primitive, first index, number of verts

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glutSwapBuffers();
}


void init()
{
	if(glewInit() != GLEW_OK){
		fprintf(stderr, "Failed to init GLEW\n");
	}

	// init vertexArray
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// init colorBuffer
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColorData), vertexColorData, GL_STATIC_DRAW);

	// init vertexBuffer
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

	// Create and compile our GLSL program from the shaders
	shader = new shaderManager( "../SimpleVertexShader.glsl", "../SimpleFragmentShader.glsl" );

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// glut funcs
	justWarped = mouseWarpCooldown;
	glutWarpPointer(width/2, height/2);
	glutDisplayFunc(mainDisplayFunc);
	glutPassiveMotionFunc(look);
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	matrixCalc();
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(2000, 100);
	glutCreateWindow("ooooo keyboard moment");
	glutSetCursor(GLUT_CURSOR_NONE);
	init();

	glutMainLoop();

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteProgram(shaderID);

	return 0;
}
