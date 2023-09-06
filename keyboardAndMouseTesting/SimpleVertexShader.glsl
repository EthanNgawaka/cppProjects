#version 330 core

// vertex data for all execs of this shader
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;

// output data
out vec3 fragmentColor;
// value that stays const for whole mesh
uniform mat4 mvp;

void main(){
	// output vertex xyzw in terms of clip space	
	gl_Position = mvp * vec4(vertexPosition_modelspace,1);
	
	fragmentColor = vertexColor;
}
