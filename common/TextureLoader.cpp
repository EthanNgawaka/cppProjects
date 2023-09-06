#include "TextureLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <ostream>
#include <fstream>

GLuint LoadTextureFromMTL(const std::string& mtlPath) {
	std::ifstream mtlFile(mtlPath);
	std::string line;
	std::string texturePath;

	if (!mtlFile.is_open()) {
		std::cerr << "Failed to open MTL file: " << mtlPath << std::endl;
		return 0;
	}

	while (std::getline(mtlFile, line)) {
		if (line.substr(0, 7) == "map_Kd ") {
			texturePath = line.substr(7);
			break;
		}
	}

	mtlFile.close();

	if (texturePath.empty()) {
		std::cerr << "No texture path found in MTL file: " << mtlPath << std::endl;
		return 0;
	}

	int width, height, channels;
	unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &channels, 0);
	std::cout << std::endl << texturePath;
	if(!data){
		std::cerr << "Failed to load texture: " << mtlPath << std::endl;
		return 0;
	}

	GLuint textureID;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	std::cout << "\nDetected channels: ";
	std::cout << channels << std::endl;

	if(channels == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}else if(channels == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	return textureID;
}
