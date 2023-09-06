#include "Obj.hpp"
#include "PhysicsObj.hpp"

Obj::Obj(ObjManager* parent, std::string id, std::string path, glm::vec3 pos, float e, float invMass, float muStatic, float muKinetic)
	:id(id), manager(parent){
	addMesh(path);
	meshObj.translate(pos);
	addPhysicsObj(pos, e, invMass, muStatic, muKinetic);
}
std::string Obj::getID(){
	return id;
}
Mesh* Obj::getMesh(){
	return &this->meshObj;
}
PhysicsObj* Obj::getPhysicsObj(){
	return this->physicsObj;
}
void Obj::addMesh(std::string path){
	Mesh tempMesh = Mesh::loadOBJ(path, this);
	meshObj = tempMesh;
}
void Obj::setVel(glm::vec3 vel){
	physicsObj->setVel(vel);
}
void Obj::addPhysicsObj(glm::vec3 pos, float e, float invMass, float muStatic, float muKinetic){
	PhysicsObj* tempObj = new PhysicsObj(pos, e, invMass, muStatic, muKinetic, this);
	physicsObj = tempObj;
	//pos, e, mass-1, parent
}
void Obj::draw(float deltaT, shaderManager* shader){
	glm::vec3 tempPos = physicsObj->getPos();
	meshObj.draw(shader);
}
void Obj::translate(glm::vec3 trans){
	physicsObj->addToPos(trans);
}
void Obj::scale(glm::vec3 scaleVec){
	meshObj.scale(scaleVec);
}
void Obj::addForce(glm::vec3 force){
	physicsObj->addForce(force);
}
void Obj::addToVel(glm::vec3 vel){
	physicsObj->addToVel(vel);
}
glm::vec3 Obj::getVel(){
	return physicsObj->getVelocity();
}
void Obj::resetCollidingObjs(){
	std::vector<std::tuple<Obj*, glm::vec3>> emptyCollidingObjs;
	this->collidingObjs = emptyCollidingObjs;
}
std::vector<std::tuple<Obj*, glm::vec3>> Obj::getCollidingObjs(){
	return this->collidingObjs;
}
void Obj::addCollidingObj(Obj* collidingObj, glm::vec3 normal){
	collidingObjs.push_back(std::make_tuple(collidingObj, normal));
}
glm::vec3 Obj::getMiddleVector(){
	std::array<glm::vec2, 3> maxMins = meshObj.getWorldSpaceMaxMins();
	glm::vec3 centerB = 0.5f * glm::vec3(
		maxMins[0][0]+maxMins[0][1],
		maxMins[1][0]+maxMins[1][1],
		maxMins[2][0]+maxMins[2][1]
	);
	return centerB;
}
