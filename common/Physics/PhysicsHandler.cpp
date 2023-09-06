#include "PhysicsHandler.hpp"

void PhysicsHandler::applyImpulse(Obj* obj1, Obj* obj2, glm::vec3 collisNorm){
	PhysicsObj* physObj1 = obj1->getPhysicsObj();
	PhysicsObj* physObj2 = obj2->getPhysicsObj();
	glm::vec3 vel = physObj1->getVelocity();
	glm::vec3 vel2 = physObj2->getVelocity();
	float e = physObj1->getE();
	float e2 = physObj2->getE();
	float inverseMass = physObj1->getInvMass();
	float inverseMass2 = physObj2->getInvMass();

	glm::vec3 Vr = vel - vel2;
	float et = (e+e2)/2;
	float Vj = -(1+et) * dot(Vr, collisNorm);
	float J = Vj/(inverseMass+inverseMass2);
	glm::vec3 deltaV1 = inverseMass*J*collisNorm;
	glm::vec3 deltaV2 = -inverseMass2*J*collisNorm;
	physObj1->addToVel(deltaV1);
	physObj2->addToVel(deltaV2);
	physObj1->setPrevJ(-J);
	physObj2->setPrevJ(J);
}
void PhysicsHandler::updatePosOfObjs(float deltaT, std::map<std::string, Obj*> objMap){
	glm::vec3 accel;
	glm::vec3 tempVel;
	float k = 5;
	glm::vec3 Ffr;
	for(auto& tempobj : objMap){
		PhysicsObj* obj = tempobj.second->getPhysicsObj(); 
		obj->setPrevPos();
		obj->addForce(glm::vec3(0, gravity, 0));
		tempVel = obj->getVelocity();

		// calc friction
		if(obj->getInvMass() > 0){
			for(auto& tempobj2: tempobj.second->getCollidingObjs()){
				Obj* obj2 = std::get<0>(tempobj2);
				glm::vec3 normal = std::get<1>(tempobj2);
				float muStatic = obj2->getPhysicsObj()->getMuStatic();
				float muKinetic = obj2->getPhysicsObj()->getMuKinetic();
				float Fn = (obj->getPrevJ()/deltaT);
				glm::vec3 Vtan = tempVel - dot(tempVel, normal);
				glm::vec3 Ftan = -k * Vtan;
				if(glm::length(Ftan) <= std::abs(Fn*muStatic)){
					Ffr = Ftan;
				}else{
					Ffr = muKinetic * Fn * glm::normalize(Vtan);
				}
				printf("\n%s Ffr: %f, %f, %f | %s, %f", tempobj.second->getID().c_str(), Ffr.x, Ffr.y, Ffr.z, obj2->getID().c_str(), Fn);
				obj->addForce(Ffr);
			}
		}
		printf("\nStaticState: %i", tempobj.second->getPhysicsObj()->getStaticState());

		//

		accel = obj->getForces() * obj->getInvMass();
		tempVel = tempVel + accel*deltaT;
		obj->addToPos(tempVel);
		obj->clearForces();
		//tempVel.y *= 1/(1-velDamping);
		obj->setVel(tempVel*(1-velDamping));
		tempobj.second->getMesh()->calculateWorldSpaceMaxMins();
		tempobj.second->resetCollidingObjs();
	}
}
std::tuple<std::vector<std::array<Obj*, 2>>, std::vector<glm::vec3>> PhysicsHandler::calculateCollisions(std::map<std::string, Obj*> objMap){ 
	// returns vector of (array<Obj*, 2>, collisNorm) corresponding to collision pairs
	std::vector<std::array<Obj*, 2>> collisionPairs;
	std::vector<glm::vec3> collisionNormals;
	for(auto tempobj: objMap){
		tempobj.second->getMesh()->calculateWorldSpaceMaxMins();
		tempobj.second->resetCollidingObjs();
		tempobj.second->getPhysicsObj()->setStaticState(false);
	}

	for(auto obj1: objMap){
		//make sure maxmins are calculated after new worldspacecoords are calculated
		glm::vec3 centerA = obj1.second->getMiddleVector();
		std::array<glm::vec2, 3> obj1MaxMins = obj1.second->getMesh()->getWorldSpaceMaxMins();

		for(auto obj2: objMap){
			if(obj1.first==obj2.first){continue;}
			bool checkTest = false;
			for(auto alreadyChecked: collisionPairs){
				Obj* temp1 = alreadyChecked[0];
				Obj* temp2 = alreadyChecked[1];
				if(obj1.second == temp1 && obj2.second == temp2){
					checkTest = true;
				}else if(obj2.second == temp1 && obj1.second == temp2){
					checkTest = true;
				}
			}
			if(checkTest){
				continue;
			}
			bool individualObjCheck = true;
			glm::vec3 centerB = obj2.second->getMiddleVector();
			std::array<glm::vec2, 3> obj2MaxMins = obj2.second->getMesh()->getWorldSpaceMaxMins();
			glm::vec3 d = centerB-centerA;
			glm::vec2 minOverlap = glm::vec2(INFINITY,0);

			for(int i=0;i<3;i++){
				glm::vec2 a = obj1MaxMins[i]; // (minimum, maximum)
				glm::vec2 b = obj2MaxMins[i];
				float overlap = 0.5f * (a[1]-a[0] + b[1]-b[0]) - std::abs(d[i]);

				float bias = 0.03f;
				if(overlap < 0){
					individualObjCheck = false;
					break;
				}else if(overlap <= minOverlap[0]){	
					if(not(i == 2 && overlap < bias && minOverlap[0] <= bias)){// bias yaxis
						minOverlap = glm::vec2(overlap, i);
					}
				}else if(i == 1 && minOverlap[0] <= bias && overlap <= bias){// bias yaxis
					minOverlap = glm::vec2(overlap, i);
				}
			}
			if(individualObjCheck){
				std::array<Obj*, 2> pair = {obj1.second, obj2.second};
				collisionPairs.push_back(pair);
				glm::vec3 collisNorm;
				bool obj1Static = (
					obj1.second->getPhysicsObj()->getInvMass() == 0
					|| obj1.second->getPhysicsObj()->getStaticState());
				bool obj2Static = (
					obj2.second->getPhysicsObj()->getInvMass() == 0
					|| obj2.second->getPhysicsObj()->getStaticState());
				bool xtest = (d.x > 0);
				bool ytest = (d.y > 0);
				bool ztest = (d.z > 0);
				if(obj1Static){
					xtest = (d.x < 0);
					ytest = (d.y < 0);
					ztest = (d.z < 0);
				}
				// switch statement for collis norm
				switch(static_cast<int>(minOverlap[1])){
					case 0:  // X-axis
						collisNorm = glm::vec3(1.0f, 0.0f, 0.0f);
						if(xtest){
							collisNorm *= -1;
						}
						break;
					case 1:  // Y-axis
						collisNorm = glm::vec3(0.0f, 1.0f, 0.0f);
						if(ytest){
							collisNorm *= -1;
						}
						break;
					case 2:  // Z-axis
						collisNorm = glm::vec3(0.0f, 0.0f, 1.0f);
						if(ztest){
							collisNorm *= -1;
						}
						break;
				}
				if(obj1Static){
					obj2.second->translate(minOverlap[0]*collisNorm);
					obj2.second->getPhysicsObj()->setStaticState(true);
				}else if(obj2Static){
					obj1.second->translate(minOverlap[0]*collisNorm);
					obj1.second->getPhysicsObj()->setStaticState(true);
				}else{
					obj1.second->translate(0.5f*minOverlap[0]*collisNorm);
					obj2.second->translate(-0.5f*minOverlap[0]*collisNorm);
				}
				collisionNormals.push_back(collisNorm);
				obj1.second->addCollidingObj(obj2.second, collisNorm);
				obj2.second->addCollidingObj(obj1.second, collisNorm);
			}
		}
	}

	return std::make_tuple(collisionPairs, collisionNormals);
}

void PhysicsHandler::physicsUpdateAllObjs(float deltaT){
	/*
	updatePosOfObjs();

	//vector<array<string, 2>> collisionPairs;
	//collisionPairs = calculateCollisions();

	for(auto& objIDs: collisionPairs){
		PhysicsObj physObj1 = PhysicsObjs[Meshes[objIDs[0]].getPhysicsObjID()];
		PhysicsObj physObj2 = PhysicsObjs[Meshes[objIDs[1]].getPhysicsObjID()];
		calculateImpulse(
			physObj1.getVelocity(), physObj2.getVelocity(), NORMAL, physObj1.getInvMass(), physObj2.getInvMass(), physObj1.getE(), physObj2.getE()
		);

	}

	*/
}
