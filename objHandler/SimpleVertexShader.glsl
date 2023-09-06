#version 330 core

// vertex data for all execs of this shader
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 vertNormal;

// output data
out vec2 fragTexCoord;
out vec3 Normal;
out vec3 fragPos;

// values that stays const for whole mesh
uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main(){
	// output vertex xyzw in terms of clip space	
	gl_Position = proj * view * model * vec4(vertexPosition_modelspace,1);
	
	fragTexCoord = inTexCoord;
	Normal = inverse(transpose(mat3(model))) * vertNormal;
	fragPos = vec3(model * vec4(vertexPosition_modelspace, 1.0f));
}
