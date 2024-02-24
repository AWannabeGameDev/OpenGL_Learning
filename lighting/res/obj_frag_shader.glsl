#version 460 core

struct Material
{
	vec4 ambientColor;
	vec4 diffuseColor;
	vec4 specularColor;
	float shininess;
};

struct LightSource
{
	vec3 position;
	vec4 ambientColor;
	vec4 diffuseColor;
	vec4 specularColor;
};

in vec3 fragWorldCoord;
in vec3 fragWorldNormal;

uniform Material u_material;
uniform LightSource u_lightSrc;
uniform vec3 u_viewPosWorld;

out vec4 fragColor;

void main()
{
	vec4 ambienceColor = u_lightSrc.ambientColor * u_material.ambientColor;

	vec3 lightDisp = u_lightSrc.position - fragWorldCoord;
	float diffuseLevel = dot(lightDisp, fragWorldNormal) / (length(lightDisp) * length(fragWorldNormal));
	if(diffuseLevel < 0) diffuseLevel = 0;

	vec4 diffuseColor = u_lightSrc.diffuseColor * u_material.diffuseColor * diffuseLevel;

	vec3 viewDisp = u_viewPosWorld - fragWorldCoord;
	vec3 reflectedRay = reflect(-lightDisp, fragWorldNormal);
	float specularLevel = dot(viewDisp, reflectedRay) / (length(viewDisp) * length(reflectedRay));
	if(specularLevel < 0) specularLevel = 0;
	else specularLevel = pow(specularLevel, u_material.shininess);

	vec4 specularColor = u_lightSrc.specularColor * u_material.specularColor * specularLevel;

	fragColor = ambienceColor + diffuseColor + specularColor;
}