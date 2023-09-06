#include "Obj.hpp"
#include "PhysicsObj.hpp"

PhysicsObj::PhysicsObj(glm::vec3 pos, float e, float inverseMass, float muStatic, float muKinetic, Obj* parent)
	: position(pos), e(e), inverseMass(inverseMass), forces(glm::vec3(0.0f)), velocity(glm::vec3(0.0f)), muStatic(muStatic), muKinetic(muKinetic), parent(parent){
}
float PhysicsObj::getMuStatic(){
	return muStatic;
}
float PhysicsObj::getMuKinetic(){
	return muKinetic;
}
float PhysicsObj::getPrevJ(){
	return prevJ;
}
void PhysicsObj::setPrevJ(float J){
	prevJ = J;
}
void PhysicsObj::setStaticState(bool state){
	staticState = state;
}
bool PhysicsObj::getStaticState(){
	return staticState;
}
void PhysicsObj::addForce(glm::vec3 forceVec){
	forces += forceVec;
}
void PhysicsObj::setPos(glm::vec3 pos){
	position=pos;
}
void PhysicsObj::addToVel(glm::vec3 vel){
	velocity+=vel;
}
void PhysicsObj::setVel(glm::vec3 vel){
	velocity=vel;
}
void PhysicsObj::addToPos(glm::vec3 vel){
	position += vel;
	parent->getMesh()->translate(position);
	// SET CORRESPONDING MESH POS TO THIS.POS
}
void PhysicsObj::clearForces(){
	forces=glm::vec3(0.0f);
}
float PhysicsObj::getInvMass(){
	return inverseMass;
}
float PhysicsObj::getE(){
	return e;
}
glm::vec3 PhysicsObj::getVelocity(){
	return velocity;
}
glm::vec3 PhysicsObj::getPos(){
	return position;
}
glm::vec3 PhysicsObj::getForces(){
	return forces;
}
Obj* PhysicsObj::getParentObj(){
	return parent;
}
void PhysicsObj::setPrevPos(){
	prevPos = position;
}
void PhysicsObj::rollbackPos(){
	position = prevPos;
	position += velocity;
}
