#include "ObjManager.hpp"
#include "PhysicsHandler.hpp"

ObjManager::ObjManager(){}

Obj* ObjManager::addObj(
		std::string id, std::string path, glm::vec3 pos,
		float e, float invMass, float muStatic, float muKinetic){

	Obj* newObj = new Obj(this, id, path, pos, e, invMass, muStatic, muKinetic);
	objList[id] = newObj;
	return objList[id];
}
Obj* ObjManager::getObj(std::string id){
	return objList[id];
}

void ObjManager::drawObjs(float deltaT, shaderManager* shader){
	for(const auto obj: objList){
		obj.second->draw(deltaT, shader);
	}
}

void ObjManager::updateObjs(float deltaT, shaderManager* shader){
	PhysicsHandler::updatePosOfObjs(deltaT, objList);
	std::tuple<std::vector<std::array<Obj*,2>>, std::vector<glm::vec3>> collisionPairs;
	int numberOfItrs = 2;
	for(int i=0;i < numberOfItrs;i++){
		collisionPairs = PhysicsHandler::calculateCollisions(objList);

	}
	//unpack norms and pairs
	std::vector<std::array<Obj*,2>> pairs = std::get<0>(collisionPairs);
	std::vector<glm::vec3> norms = std::get<1>(collisionPairs);

	const int length = pairs.size();
	for(int i = 0; i < length; i++){
		std::array<Obj*,2> pair = pairs[i];
		printf("\npair: (%s, %s)", pair[0]->getID().c_str(), pair[1]->getID().c_str());
		glm::vec3 norm = norms[i];
		PhysicsHandler::applyImpulse(pair[0], pair[1], norm);
	}
}
