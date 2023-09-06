#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <array>
#include <tuple>
#include <map>

#include "PhysicsObj.hpp"
#include "Obj.hpp"

class PhysicsHandler{
	public:
		constexpr static float gravity = -2.5;
		constexpr static float velDamping = 0.02;
	
		static void applyImpulse(Obj* obj1, Obj* obj2, glm::vec3 collisNorm);
		static void updatePosOfObjs(float deltaT, std::map<std::string, Obj*> objMap);
		static std::tuple<std::vector<std::array<Obj*, 2>>, std::vector<glm::vec3>> calculateCollisions(std::map<std::string, Obj*> objMap);
		static void physicsUpdateAllObjs(float deltaT);
};
