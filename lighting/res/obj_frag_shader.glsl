#version 460 core

struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	float shininess;
	sampler2D emissionMap;
	vec4 emissionColor;
};

struct PointLight
{
	vec3 position;
	vec4 diffuseColor;
	vec4 specularColor;
	float attenConst;
	float attenLinear;
	float attenQuad;
};

struct ConeLight
{
	vec3 direction;
	vec3 position;
	float angleCosine;
	vec4 diffuseColor;
	vec4 specularColor;
	float attenConst;
	float attenLinear;
	float attenQuad;
};

struct DirectionalLight
{
	vec3 direction;
	vec4 diffuseColor;
	vec4 specularColor;
};

in vec3 fragWorldCoord;
in vec3 fragWorldNormal;
in vec2 fragTexCoord;

uniform Material u_material;
uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D emissive0;
uniform vec3 u_viewPosWorld;

uniform vec4 u_ambientColor;
uniform PointLight u_pointLightSrc;
uniform DirectionalLight u_dirLightSrc;
uniform ConeLight u_coneLightSrc;

out vec4 fragColor;

vec4 calcPointLight(PointLight light)
{
	vec3 lightDisp = light.position - fragWorldCoord;
	float lightDist = length(lightDisp);
	float attenRatio = 1 / (light.attenConst + (light.attenLinear * lightDist) + 
						(light.attenQuad * lightDist * lightDist));

	float diffuseLevel = dot(lightDisp, fragWorldNormal) / (lightDist * length(fragWorldNormal));

	if(diffuseLevel < 0) 
		diffuseLevel = 0;
	else 
		diffuseLevel *= attenRatio;

	vec4 diffuseColor = light.diffuseColor * texture(diffuse0, fragTexCoord) * diffuseLevel;

	vec3 viewDisp = u_viewPosWorld - fragWorldCoord;
	vec3 reflectedRay = reflect(-lightDisp, fragWorldNormal);
	float specularLevel = dot(viewDisp, reflectedRay) / (length(viewDisp) * length(reflectedRay));
	if(specularLevel < 0) 
		specularLevel = 0;
	else 
		specularLevel = pow(specularLevel, u_material.shininess) * attenRatio;

	vec4 specularColor = light.specularColor * texture(specular0, fragTexCoord) * specularLevel;

	return (diffuseColor + specularColor);
}

vec4 calcDirectionalLight(DirectionalLight light)
{
	float diffuseLevel = dot(-light.direction, fragWorldNormal) / (length(light.direction) * length(fragWorldNormal));
	if(diffuseLevel < 0) 
		diffuseLevel = 0;

	vec4 diffuseColor = light.diffuseColor * texture(diffuse0, fragTexCoord) * diffuseLevel;

	vec3 viewDisp = u_viewPosWorld - fragWorldCoord;
	vec3 reflectedRay = reflect(light.direction, fragWorldNormal);
	float specularLevel = dot(viewDisp, reflectedRay) / (length(viewDisp) * length(reflectedRay));
	if(specularLevel < 0) 
		specularLevel = 0;

	vec4 specularColor = light.specularColor * texture(specular0, fragTexCoord) * specularLevel;

	return (diffuseColor + specularColor);
}

vec4 calcConeLight(ConeLight light)
{
	vec3 lightDisp = light.position - fragWorldCoord;
	float lightDist = length(lightDisp);
	float attenRatio = 1 / (light.attenConst + (light.attenLinear * lightDist) + 
						(light.attenQuad * lightDist * lightDist));

	float fragDirCosine = dot(-lightDisp, light.direction) / (lightDist * length(light.direction));
	if(fragDirCosine < light.angleCosine)
		return vec4(0.0f, 0.0f, 0.0f, 0.0f);

	float diffuseLevel = dot(lightDisp, fragWorldNormal) / (lightDist * length(fragWorldNormal));

	if(diffuseLevel < 0) 
		diffuseLevel = 0;
	else 
		diffuseLevel *= attenRatio;

	vec4 diffuseColor = light.diffuseColor * texture(diffuse0, fragTexCoord) * diffuseLevel;

	vec3 viewDisp = u_viewPosWorld - fragWorldCoord;
	vec3 reflectedRay = reflect(-lightDisp, fragWorldNormal);
	float specularLevel = dot(viewDisp, reflectedRay) / (length(viewDisp) * length(reflectedRay));
	if(specularLevel < 0) 
		specularLevel = 0;
	else
		specularLevel *= attenRatio;

	vec4 specularColor = light.specularColor * texture(specular0, fragTexCoord) * specularLevel;

	return (diffuseColor + specularColor);
}

vec4 calcEmissionColor()
{
	return texture(emissive0, fragTexCoord) * u_material.emissionColor;
}

void main()
{
	vec4 finalColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	finalColor += u_ambientColor * vec4(texture(diffuse0, fragTexCoord));
	finalColor += calcPointLight(u_pointLightSrc);
	finalColor += calcDirectionalLight(u_dirLightSrc);
	finalColor += calcConeLight(u_coneLightSrc);
	finalColor += calcEmissionColor();

	fragColor = vec4(vec3(gl_FragCoord.z), 1.0f);
}