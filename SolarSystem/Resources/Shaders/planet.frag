#version 430

in vec3 color;
in vec3 normal;
in vec3 fragPos;
in vec3 cameraPos;
out vec4 fragColor;

void main() 
{		
	vec3 lightColor = vec3(1, 0.8, 0.79);
	//vec3 lightPos = vec3(3788300.0, 3788300.0, 3788300.0);
	vec3 lightPos = vec3(-100000000, 100000000, 0);

	// ambient
	vec3 ambient = 0.2 * lightColor;

	// diffuse
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
	vec3 specular = 0.5 * spec * lightColor;

	vec3 result = (ambient + 1.2*diffuse + 0*specular) * color;


	fragColor = vec4(result, 1.0);
	//fragColor = vec4(color, 1.0);
}