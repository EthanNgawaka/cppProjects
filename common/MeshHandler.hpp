#pragma once

#include <cstdio>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "TextureLoader.hpp"
#include "shader.hpp"

#include <map>
#include <array>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <limits>

using namespace std;

class Obj;
class Mesh {
	private:
		struct Vertex {
			float position[3];
			float texcoord[2];
			float normal[3];
			unsigned int faces[3];

			bool operator==(const Vertex& other) const {
				return memcmp(this, &other, sizeof(Vertex)) == 0;
			}
		};

		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 originalModel = glm::mat4(1.0f);
		glm::vec3 translation = glm::vec3(0.0f);
		glm::vec3 scaleVec = glm::vec3(1.0f);
		float rotationRads = 0.0f;
		glm::vec3 rotationAxis = glm::vec3(0,1,0);

		vector<glm::vec3> localSpaceAABB;
		array<glm::vec2, 3> MaximumMinimums;

		vector<Vertex> vertices;
		vector<unsigned int> indices;
		GLuint vao, vbo, ebo, textureID;

		Obj* parent;
		void setupMesh(){
			glGenVertexArrays(1, &vao);

			glBindVertexArray(vao);

			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			glGenBuffers(1, &ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			// Vertex positions
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			glEnableVertexAttribArray(0);

			// Texture coordinates
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			// Texture coordinates
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float))); //skip 5floats, 3 for pos 2 for texCoords
			glEnableVertexAttribArray(2);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	public:
		Mesh() : vao(0), vbo(0), ebo(0), textureID(0) {}
		Mesh(vector<Vertex> vertices, vector<unsigned int> indices, GLuint textureID, vector<glm::vec3> localAABB, Obj* parent)
			: vertices(vertices), indices(indices), textureID(textureID), localSpaceAABB(localAABB), parent(parent){
			setupMesh();
			/* Debug
			cout << "\nVertices:" << endl;
			for (const Vertex& vert : vertices) {
				cout << "Position: (" << vert.position[0] << ", " << vert.position[1] << ", " << vert.position[2] << ")";
				cout << " TexCoord: (" << vert.texcoord[0] << ", " << vert.texcoord[1] << ")";
				cout << " Normal: (" << vert.normal[0] << ", " << vert.normal[1] << ", " << vert.normal[2] << ")" << endl;
			}
			cout << "Indices:" << endl;
			for (unsigned int index : indices) {
				cout << index << " ";
			}
			cout << "Faces:" << endl;
			for (glm::vec3 index : faces){
				printf("\n%f/%f/%f", index[0], index[1], index[2]);
			}
			cout << endl;
			*/
		}

		static Mesh loadOBJ(const string& filename, Obj* parent){
			ifstream file(filename + ".obj");
			if (!file.is_open()) {
				cerr << "Failed to open " << filename << ".obj" << endl;
				exit(EXIT_FAILURE);
			}
			string line;
			vector<Vertex> out_vertices;
			vector<unsigned int> out_indices;
			vector<glm::vec3> out_faces;

			vector<glm::vec3> temp_vertices;
			vector<glm::vec2> temp_texcoords;
			vector<glm::vec3> temp_normals;

			glm::vec2 xPoints(INFINITY, -INFINITY);
			glm::vec2 yPoints(INFINITY, -INFINITY);
			glm::vec2 zPoints(INFINITY, -INFINITY);

			while(getline(file, line)) {
				if(line.substr(0, 2) == "v ") {
					glm::vec3 vertex;
					sscanf(line.c_str(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
					temp_vertices.push_back(vertex);
				}else if(line.substr(0, 3) == "vt ") {
					glm::vec2 texcoord;
					sscanf(line.c_str(), "vt %f %f", &texcoord.x, &texcoord.y);
					temp_texcoords.push_back(texcoord);
				}else if(line.substr(0, 3) == "vn ") {
					glm::vec3 normal;
					sscanf(line.c_str(), "vn %f %f %f", &normal.x, &normal.y, &normal.z);
					temp_normals.push_back(normal);
				}else if(line.substr(0, 2) == "f ") {
					unsigned int vIndices[3], tIndices[3], nIndices[3];
					const char* ch = line.c_str();
					sscanf(ch, "f %d/%d/%d %d/%d/%d %d/%d/%d",
						&vIndices[0], &tIndices[0], &nIndices[0], 
						&vIndices[1], &tIndices[1], &nIndices[1], 
						&vIndices[2], &tIndices[2], &nIndices[2]);

					for(int i = 0; i < 3; i++){
						Vertex vert;
						vert.position[0] = temp_vertices[vIndices[i] - 1].x;
						vert.position[1] = temp_vertices[vIndices[i] - 1].y;
						vert.position[2] = temp_vertices[vIndices[i] - 1].z;

						if(vert.position[0] < xPoints[0]){
							xPoints[0] = vert.position[0];
						}else if(vert.position[0] > xPoints[1]){
							xPoints[1] = vert.position[0];
						}
						if(vert.position[1] < yPoints[0]){
							yPoints[0] = vert.position[1];
						}else if(vert.position[1] > yPoints[1]){
							yPoints[1] = vert.position[1];
						}
						if(vert.position[2] < zPoints[0]){
							zPoints[0] = vert.position[2];
						}else if(vert.position[2] > zPoints[1]){
							zPoints[1] = vert.position[2];
						}
						vert.texcoord[0] = temp_texcoords[tIndices[i] - 1].x;
						vert.texcoord[1] = -temp_texcoords[tIndices[i] - 1].y;
						vert.normal[0] = temp_normals[nIndices[i] - 1].x;
						vert.normal[1] = temp_normals[nIndices[i] - 1].y;
						vert.normal[2] = temp_normals[nIndices[i] - 1].z;

						// iterate through out_verts and see if vert exists
						auto itr = find(out_vertices.begin(), out_vertices.end(), vert);
						
						if(itr != out_vertices.end()){
							// if it does reuse index
							out_indices.push_back(distance(out_vertices.begin(), itr));
						}else{
							// new vert
							out_vertices.push_back(vert);
							out_indices.push_back(out_vertices.size() - 1);
						}
						glm::vec3 temp_face(vIndices[i], tIndices[i], nIndices[i]);
						out_faces.push_back(temp_face);
					}
				}
			}
			string mtlPath = filename + ".mtl";
			GLuint temp_texture_ID = LoadTextureFromMTL(mtlPath);

			vector<glm::vec3> tempAABB;

			for(int i = 0; i < 8; i++){
				tempAABB.push_back(glm::vec3(
					(i & 1) ? xPoints[1] : xPoints[0],
					(i & 2) ? yPoints[1] : yPoints[0],
					(i & 4) ? zPoints[1] : zPoints[0]));
			}
			return Mesh(out_vertices, out_indices, temp_texture_ID, tempAABB, parent);
		}

		void translate(glm::vec3 translationVec){
			translation = translationVec;
			calculateModel();
		}
		void scale(glm::vec3 scaleVec2){
			scaleVec = scaleVec2;
			calculateModel();
		}
		void rotate(float rotationInDegrees, glm::vec3 rotAxis){
			rotationRads = glm::radians(rotationInDegrees);
			rotationAxis = rotAxis;
			calculateModel();
		}
		void calculateModel(){
			Model = originalModel;
			Model = glm::translate(Model, translation);
			Model = glm::scale(Model, scaleVec);
			Model = glm::rotate(Model, rotationRads, rotationAxis);
		}
		glm::vec3 getMiddleVector(){
			std::array<glm::vec2, 3> maxMins = getWorldSpaceMaxMins();
			glm::vec3 centerB = 0.5f * glm::vec3(
				maxMins[0][0]+maxMins[0][1],
				maxMins[1][0]+maxMins[1][1],
				maxMins[2][0]+maxMins[2][1]
			);
			return centerB;
		}
		void calculateWorldSpaceMaxMins(){
			glm::vec2 xPoints(INFINITY, -INFINITY);
			glm::vec2 yPoints(INFINITY, -INFINITY);
			glm::vec2 zPoints(INFINITY, -INFINITY);

			for(int i = 0; i < 8; i++){
				glm::vec3 temp_vec;
				temp_vec = Model * glm::vec4(localSpaceAABB[i], 1.0f);
				if(temp_vec[0] < xPoints[0]){
					xPoints[0] = temp_vec[0];
				}else if(temp_vec[0] > xPoints[1]){
					xPoints[1] = temp_vec[0];
				}
				if(temp_vec[1] < yPoints[0]){
					yPoints[0] = temp_vec[1];
				}else if(temp_vec[1] > yPoints[1]){
					yPoints[1] = temp_vec[1];
				}
				if(temp_vec[2] < zPoints[0]){
					zPoints[0] = temp_vec[2];
				}else if(temp_vec[2] > zPoints[1]){
					zPoints[1] = temp_vec[2];
				}
			}

			MaximumMinimums[0] = xPoints;
			MaximumMinimums[1] = yPoints;
			MaximumMinimums[2] = zPoints;
		}

		array<glm::vec2, 3> getWorldSpaceMaxMins(){
			return MaximumMinimums;
		}

		void draw(shaderManager* shader){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);

			calculateModel();
			shader->sendMatrix4("model", Model);
			shader->sendInt("textureSampler", 0);
			shader->sendFloat("MATERIALSHININESS", 32);

			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
};
