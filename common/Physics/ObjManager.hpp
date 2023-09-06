#pragma once

#include <map>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>
#include <string>
#include "Obj.hpp"

class ObjManager{
	public:
		std::map<std::string, Obj*> objList;

		ObjManager();
		Obj* addObj(std::string id, std::string path, glm::vec3 pos,
			float e, float invMass, float muStatic, float muKinetic);
		void drawObjs(float deltaT, shaderManager* shader);
		void updateObjs(float deltaT, shaderManager* shader);
		Obj* getObj(std::string id);
};
