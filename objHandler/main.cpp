#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <shader.hpp>
#include <TextureLoader.hpp>
#include <MeshHandler.hpp>
#include <Physics/ObjManager.hpp>
#include <Physics/Obj.hpp>

#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <array>
#include <tuple>
using namespace std;

const int width = 1024;
const int height = 768;

double prevTime;
float deltaX = 0.0f;
float deltaY = 0.0f;
float pitch = 0.0f;
float prevPitch = 0.0f;
float yaw = -90.0f;
float prevYaw = -90.0f;
const float sensitivity = 0.04f;

const double middleX = width/2;
const double middleY = height/2;

float cameraSpeed = 0.05f;

glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 0.0f);

map<int, bool> keys;

shaderManager* shader;

GLFWwindow* window;

ObjManager objManager;

glm::vec3 playerFront;
class Camera{
	public:
		glm::vec3 worldUp = glm::vec3(0,1,0);
		glm::vec3 position = glm::vec3(0,0,0);
		glm::vec3 front;
		glm::vec3 right;
		glm::vec3 dir;
		glm::vec3 up;

		glm::mat4 Projection;
		glm::mat4 View;

	void matrixCalc(){
		Projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 1000.0f); // Proj matrix
																							
		// calculate cameraUp and cameraRight
		dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		dir.y = sin(glm::radians(pitch));
		dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		glm::vec3 cameraDir = dir;
		front = glm::normalize(cameraDir);
		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
		cameraDir.y = 0;
		playerFront = glm::normalize(cameraDir);

		// View matrix handy dandy lookAt func from glm that handles matrix construction
		View = glm::lookAt(
			position, 
			position + front, // looks in dir defined by cameraFront
			up	
		);

		glm::vec3 lightCol(1.0f, 1.0f, 1.0f);

		shader->sendVec3("pointLights[0].pos", lightPos);
		shader->sendVec3("pointLights[0].diffuse", lightCol);
		shader->sendVec3("pointLights[0].specular", lightCol);
		shader->sendFloat("pointLights[0].diffStrength", 0.8f);
		shader->sendFloat("pointLights[0].specStrength", 0.5f);

		shader->sendVec3("viewPos", position);
		shader->sendMatrix4("view", View);
		shader->sendMatrix4("proj", Projection);
	}

};
Camera camera;


class Player{
	private:
		Obj* obj;
		float speed = 2.5f;
		float jump = 1.0f;
		float zoom = 20;
		float dashPower = 1.7f;
		float dashTimer = 0;
		float dashTimerMax = 80;
		int jumps = 0;
		bool grounded = false;
		bool canDash = false;
		float jumpTimer = 0;
		float floatTimer = 5;
		float cameraOffset = 2;
		float cameraMaxOffset = 2;
		bool jumping = false;
	public:
		Player(){}
		Player(glm::vec3 pos){
			this->obj= objManager.addObj(
				"player", "/home/ethan/Desktop/c++/objHandler/resources/guy",
				pos, 0, 1, 0.1, 0.1);// pos, e, invMass, uS, uK
		}
		void addToZoom(float value){
			zoom += value;
		}
		void input(){
			//cout << get<0>(obj->getCollidingObjs()[0]);
			glm::vec3 inputDir = glm::vec3(0,0,0);
			glm::vec3 vel = obj->getVel();
			float currentSpeed = glm::length(glm::vec3(vel.x,0,vel.z));
			float maxSpeed = 0.65f;
			grounded = false;

			
			for(auto& tup: obj->getCollidingObjs()){
				glm::vec3 norm= get<1>(tup);
				if(norm.y == 1){
					canDash = true;
					grounded = true;
				}
			}
			if(jumpTimer > 0){
				jumpTimer--;
				if(vel.y < 0){
					if(keys[GLFW_KEY_SPACE]){
						obj->addForce(glm::vec3(0, 8, 0));
					}
				}
			}
			if(not(grounded) && not(keys[GLFW_KEY_SPACE]) && jumping && vel.y > 0.3){
				obj->setVel(glm::vec3(vel.x, 0.3, vel.z));
			}
			if(jumping && grounded){
				jumping = false;
			}
			if(grounded && keys[GLFW_KEY_SPACE]){
				obj->setVel(glm::vec3(vel.x, jump, vel.z));
				jumpTimer = floatTimer;
				jumping = true;
				grounded = false;
			}
			if(keys[GLFW_KEY_W]){
				if(currentSpeed < maxSpeed){
					obj->addForce(playerFront * speed);
				}
				inputDir += playerFront;
			}
			if(keys[GLFW_KEY_A]){
				if(currentSpeed < maxSpeed){
					obj->addForce(camera.right * -speed);
				}
				inputDir += -camera.right;
			}
			if(keys[GLFW_KEY_S]){
				if(currentSpeed < maxSpeed){
					obj->addForce(playerFront * -speed);
				}
				inputDir += -playerFront;
			}
			if(keys[GLFW_KEY_D]){
				if(currentSpeed < maxSpeed){
					obj->addForce(camera.right * speed);
				}
				inputDir += camera.right;
			}
			if(dashTimer <= 0 && canDash && keys[GLFW_KEY_LEFT_SHIFT]){
				if(glm::length(inputDir) == 0){
					inputDir = playerFront;
				}
				inputDir = glm::normalize(inputDir);
				obj->addToVel(inputDir * dashPower);
				canDash = false;
				dashTimer = dashTimerMax;
			}
			if(dashTimer > dashTimerMax){
				obj->addToVel(glm::vec3(0,-vel.y,0));
			}
			if(dashTimer > 0){
				dashTimer--;
			}


		}
		void setCamera(){
			float cameraDist = zoom;
			float above = 4;
			float t = 1;
			glm::vec3 target = obj->getMiddleVector()+glm::vec3(0,above,0)+camera.right*cameraOffset-camera.dir*cameraDist;
			camera.position = target; 		
		}
		void rotatePlayer(){
			obj->getMesh()->rotate(90-yaw, camera.worldUp);
		}
		void changeCamOffset(float deltaX){
			if(cameraOffset > -cameraMaxOffset && deltaX < 0){
				cameraOffset += deltaX;
			}
			if(cameraOffset < cameraMaxOffset && deltaX > 0){
				cameraOffset += deltaX;
			}
		}
};
Player* player;

bool debug = false;
void inputHandling(){

	player->setCamera();
	player->rotatePlayer();
	player->input();
	if(keys[GLFW_KEY_ESCAPE]){
		glfwSetWindowShouldClose(window, true);
	}
	
}

void look(GLFWwindow* window, double x, double y) {
	deltaX = (x - middleX) * sensitivity;
	deltaY = -(y - middleY) * sensitivity;

	player->changeCamOffset(deltaX/15);
	yaw += deltaX;
	pitch += deltaY;

	//pitch constraints
	if (pitch < -89.0f) {
		pitch = -89.0f;
	} else if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	glfwSetCursorPos(window, middleX, middleY);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if(action == GLFW_PRESS){
		if(keys.find(key) == keys.end()){
			keys.insert({ key, true });
		}else{
			keys[key] = true;
		}
		return;
	}
	if(action == GLFW_RELEASE){
		keys[key] = false;
		return;
	}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
	float dY = -yoffset*2;
	if(keys[GLFW_KEY_LEFT_SHIFT]){
		dY *= 2;
	}
	player->addToZoom(dY);
}

float accum = 0.0f;
const float FIXED_TIMESTEP = 1.0f/60.0f;

void mainDisplayFunc(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader->getShaderID());
	float currTime = glfwGetTime();
	
	float deltaTime = currTime - prevTime;
	accum += deltaTime;

	while(accum >= FIXED_TIMESTEP){
		objManager.updateObjs(FIXED_TIMESTEP, shader);
		accum -= FIXED_TIMESTEP;
	}

	inputHandling();
	camera.matrixCalc();
	objManager.drawObjs(deltaTime, shader);

	prevPitch = pitch;
	prevYaw = yaw;
	prevTime = currTime;
	glfwPollEvents();

	glfwSwapBuffers(window);
}

void init(){
	//glfwInit
    glfwMakeContextCurrent(window);
	if(glewInit() != GLEW_OK){
		fprintf(stderr, "Failed to init GLEW\n");
	}

	//glfw funcs
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, look);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, middleX, middleY);
	glfwSetWindowPos(window, 100, 100);

	// shader init
	shader = new shaderManager( "../SimpleVertexShader.glsl", "../SimpleFragmentShader.glsl" );
	glUseProgram(shader->getShaderID());

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Meshes
	string bingCubePath = "/home/ethan/Desktop/c++/objHandler/resources/untitled";
	glm::vec3 pos2 = glm::vec3(0,-5,0);
	int num = 5;

	for(int i = 0; i < num; i ++){
		int sign = 1;
		if(rand()%2 <= 1){
			sign = -1;
		}
		glm::vec3 pos = glm::vec3(sign*rand()%100,rand()%50,sign*rand()%100);
		string name = "bingCube";
		float scalef = rand()%5;
		name.append(to_string(i));
		objManager.addObj(name, bingCubePath, pos, 0, 2, 0.45, 0.3); 
		objManager.getObj(name)->scale(glm::vec3(1,1,1)*scalef);
	}
	
	objManager.addObj("bingPlatform", bingCubePath, pos2, 0, 0, 0.53, 0.38); 
	objManager.getObj("bingPlatform")->scale(glm::vec3(100,1,100));
	player = new Player(glm::vec3(0,0,0));

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	camera.matrixCalc();
	prevTime = glfwGetTime();
	
}

int main(int argc, char** argv){
	if(!glfwInit()){
		return -1;
	}
	window = glfwCreateWindow(width, height, "Window! :3", NULL, NULL);
    if (!window){
        glfwTerminate();
        return -1;
    }
	init();

    while(!glfwWindowShouldClose(window)){
		mainDisplayFunc();
    }

	glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
