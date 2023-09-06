#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>

class Obj;

class PhysicsObj{
	private:
		Obj* parent;
		glm::vec3 position;
		glm::vec3 prevPos;
		glm::vec3 velocity;
		glm::vec3 forces;
		float inverseMass;
		float prevJ;
		float e;
		float muStatic;
		float muKinetic;
		bool staticState;
	public:
		PhysicsObj();
		PhysicsObj(glm::vec3 pos, float e, float inverseMass, float muStatic, float muKinetic, Obj* parent);
		float getPrevJ();
		void setPrevJ(float J);
		void addForce(glm::vec3 forceVec);
		void setPos(glm::vec3 pos);
		void setPrevPos();
		void setVel(glm::vec3 vel);
		void addToVel(glm::vec3 vel);
		void addToPos(glm::vec3 vel);
		void clearForces();
		void rollbackPos();
		void setStaticState(bool state);
		float getInvMass();
		float getE();
		float getMuStatic();
		float getMuKinetic();
		bool getStaticState();
		glm::vec3 getVelocity();
		glm::vec3 getPos();
		glm::vec3 getForces();
		Obj* getParentObj();
};
