#include <GL/glew.h>
#include <GL/glut.h>
#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>

#include <shader.hpp>

int width = 800;
int height = 600;

GLuint vertexArrayID;
GLuint vertexBuffer;
GLuint shaderID;
GLuint matrixID;
GLuint colorBuffer;
shaderManager* shader;

glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
glm::mat4 mvp;

static const GLfloat g_color_buffer_data[] = {
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	// init vertexBuffer
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

	// Create and compile our GLSL program from the shaders
	shader = new shaderManager( "../SimpleVertexShader.glsl", "../SimpleFragmentShader.glsl" );

	// Matrix initialization

	glVertexAttribPointer(
		0, // attribute idk y 0
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized???
		0, // stride
		(void*)0 // array buffer offset
	);
	// Model matrix, identity matrix for model space to be origin
	Model = glm::mat4(1.0f);
	// View matrix handly dandy lookAt func from glm that handles matrix construction
	View = glm::lookAt(
		glm::vec3(4,3,3), // Camera at 4,3,3 in world space
		glm::vec3(0,0,0), // looks at origin
		glm::vec3(0,1,0)  // Head is up (0, -1, 0) is upside down
	);
	// Projection matrix 45o fov, 4:3 ratio, display range: 0.1 -> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
		
	mvp = Projection * View * Model;

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 
}

void displayFunc(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader->getShaderID());
	shader->sendMatrix4("mvp", mvp);
	// 1st atrrib buffer
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, // attribute idk y 0
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
		1, // attribute idk y 1
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

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OpenGL Learning! uwu");
	init();
	glutDisplayFunc(displayFunc);
	glutMainLoop();
	return 0;
}
