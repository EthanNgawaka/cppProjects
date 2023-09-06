#pragma once

#include <cstdio>
#include <string>
#include <tuple>
#include <vector>
#include <array>
#include <glm/gtc/matrix_transform.hpp>

#include "MeshHandler.hpp"
#include "shader.hpp"

class PhysicsObj;
class ObjManager; //forward dec

class Obj {
	private:
		Mesh meshObj;
		PhysicsObj* physicsObj;
		ObjManager* manager;
		const std::string id;
		std::vector<std::tuple<Obj*, glm::vec3>> collidingObjs;

	public:
		Obj(ObjManager* parent, std::string id, std::string path, glm::vec3 pos, float e, float invMass, float muStatic, float muKinetic);
		std::string getID();
		Mesh* getMesh();
		PhysicsObj* getPhysicsObj();
		void addMesh(std::string path);
		void addPhysicsObj(glm::vec3 pos, float e, float invMass, float muStatic, float muKinetic);
		void draw(float deltaT, shaderManager* shader);
		void setVel(glm::vec3 vel);
		void addForce(glm::vec3 force);
		void translate(glm::vec3 trans);
		void scale(glm::vec3 scaleVec);
		void addToVel(glm::vec3 vel);
		std::vector<std::tuple<Obj*, glm::vec3>> getCollidingObjs();
		glm::vec3 getMiddleVector();
		void resetCollidingObjs();
		void addCollidingObj(Obj* collidingObj, glm::vec3 normal);
		glm::vec3 getVel();
};
