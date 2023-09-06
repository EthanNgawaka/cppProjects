#version 330 core

// output data
out vec3 color;

// input data
in vec3 fragmentColor;

void main(){
	color = fragmentColor;
	//color = vec3(1,0,0);
}
