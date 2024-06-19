#version 430

uniform vec3 lightPos;

in vec3 color;
in vec3 normal;
in vec3 fragPos;
in vec3 cameraPos;
in int spec;
out vec4 fragColor;

void main() 
{		
	vec3 lightColor = vec3(1, 0.8, 0.79);

	// ambient
	vec3 ambient = 0.1 * lightColor;

	// diffuse
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(normal, lightDir), 0);
	vec3 diffuse = diff * lightColor;

	float specularStrength = 0.5;
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + spec*specular) * color;
	//result = color;

	fragColor = vec4(result, 1.0);
}