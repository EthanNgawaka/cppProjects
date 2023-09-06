#version 330 core

// consts
const int MAX_POINT_LIGHTS = 1;
const vec3 ambientLightColor = vec3(1.0);
const float ambientLightStrength = 0.3;

// structs
struct dirLight{
	vec3 pos;
	vec3 diffuse;
	vec3 specular;
	vec3 dir;
};

struct pointLight{
	vec3 pos;
	vec3 diffuse;
	vec3 specular;
	float specStrength;
	float diffStrength;
};

// output data
out vec3 color;

// input data
in vec2 fragTexCoord;
in vec3 Normal;
in vec3 fragPos;

// uniforms
uniform sampler2D textureSampler;
uniform vec3 viewPos;
uniform float MATERIALSHININESS;
uniform pointLight pointLights[MAX_POINT_LIGHTS];

vec3 calcPointLightContrib(pointLight light, vec3 normal, vec3 fragPos, vec3 viewPos){
	vec3 ambient = ambientLightStrength * ambientLightColor;
	
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.pos - fragPos);
	vec3 diffuse = max(dot(norm, lightDir), 0.0) * light.diffuse * light.diffStrength;

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = light.specStrength * pow(max(dot(viewDir, reflectDir), 0.0), MATERIALSHININESS);
	vec3 specular = spec * light.specular;

	return (ambient + diffuse + specular);
}

void main(){
	vec3 k = vec3(0.0);
	for(int i = 0; i < MAX_POINT_LIGHTS; i++){
		k += calcPointLightContrib(pointLights[i], Normal, fragPos, viewPos);
	}

	color =  k * texture(textureSampler, fragTexCoord).rgb;
}
