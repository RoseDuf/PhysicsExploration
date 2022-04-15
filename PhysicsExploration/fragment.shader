#version 330 core

#define NBR_POINT_LIGHTS 4

// The fragment shader only requires one output variable.
out vec4 result_color;

in VS_OUT
{
	vec3 fragment_position; //interpolated
	vec3 normal;
	vec2 texture_coords;
	vec4 fragment_position_lightSpace;
} fs_in;

struct Material
{
	// vec3 ambient; We also remove the ambient material color vector since the ambient color is equal to the diffuse color when using textures
	sampler2D texture_ambient;
	sampler2D texture_diffuse;
	sampler2D texture_specular;

	vec3 color_ambient;
	vec3 color_diffuse;
	vec3 color_specular;

	float shininess;
};

struct Light 
{
	vec4 position; // if w = 1, it is a point light, if w = 0, it is a directional lightvec3  direction;
	vec3  direction;
	float cutOff; // for spotlight
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// For attenuation
	float constant;
	float linearK;
	float quadratic;
};

uniform Light dirLight;
uniform Light pointLights[NBR_POINT_LIGHTS];
uniform Light spotLight;
uniform Material materials[2];
uniform vec3 view_position;

// Depth test
uniform sampler2D shadowMap;
float near = 0.1;
float far = 100.0f;

// Booleans
uniform bool hasTexture;
uniform bool hasDirLight;
uniform bool hasPointLight;
uniform bool hasSpotLight;

// Functions
vec3 ComputePointLight(Light _light, Material _material);
vec3 ComputeDirectionalLight(Light _light, Material _material);
vec3 ComputeSpotLight(Light _light, Material _material);
float LinearizeDepth(float depth);
float ShadowCalculation(vec4 frag_pos_light/*, vec3 light_dir*/);

void main()
{
	vec3 color;

	for (int i = 0; i < 1; i++) 
	{
		if (hasDirLight)
		{
			color += ComputeDirectionalLight(dirLight, materials[i]);
		}

		if (hasPointLight)
		{
			for (int j = 0; j < NBR_POINT_LIGHTS - 1; j++)
			{
				color += ComputePointLight(pointLights[j], materials[i]);
			}
		}
		
		if (hasSpotLight)
		{
			color += ComputeSpotLight(spotLight, materials[i]);
		}
	}

	//float depth = LinearizeDepth(gl_FragCoord.z) / far;
	result_color = vec4(color, 1.0f);
} 

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (2.0 * near * far) / (far + near - z * (far - near));
}

vec3 ComputePointLight(Light _light, Material _material)
{
	vec3 norm = normalize(fs_in.normal);
	vec3 lightDir = normalize(vec3(_light.position) - fs_in.fragment_position);
	float diff = max(dot(norm, lightDir), 0.0); //use max to avoid negative results

	vec3 viewDir = normalize(view_position - fs_in.fragment_position);
	vec3 reflectDir = reflect(-lightDir, fs_in.normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), _material.shininess);

	// Adjust for distance to add realism (attenuation)
	float distance = length(vec3(_light.position) - fs_in.fragment_position);
	float attenuation = 1.0 / (_light.constant + _light.linearK * distance + _light.quadratic * (distance * distance));

	vec3 ambient = _light.ambient * _material.color_ambient;
	vec3 diffuse = _light.diffuse * (diff * _material.color_diffuse);
	vec3 specular = _light.specular * (spec * _material.color_specular) ;

	if (hasTexture)
	{
		ambient *= texture(_material.texture_diffuse, fs_in.texture_coords).rgb;
		diffuse *= texture(_material.texture_diffuse, fs_in.texture_coords).rgb;
		specular *= texture(_material.texture_specular, fs_in.texture_coords).rgb;
	}

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// calculate shadow
	float shadow = ShadowCalculation(fs_in.fragment_position_lightSpace);

	vec3 color = ambient + ((1.0 - shadow) * (diffuse + specular));

	return color;
}

vec3 ComputeDirectionalLight(Light _light, Material _material)
{
	vec3 norm = normalize(fs_in.normal);
	vec3 lightDir = normalize(-vec3(_light.position));
	float diff = max(dot(norm, lightDir), 0.0); //use max to avoid negative results

	vec3 viewDir = normalize(view_position - fs_in.fragment_position);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), _material.shininess);

	vec3 ambient = _light.ambient * _material.color_ambient;
	vec3 diffuse = _light.diffuse * (diff * _material.color_diffuse);
	vec3 specular = _light.specular * (spec * _material.color_specular);

	if (hasTexture)
	{
		ambient *= texture(_material.texture_diffuse, fs_in.texture_coords).rgb;
		diffuse *= texture(_material.texture_diffuse, fs_in.texture_coords).rgb;
		specular *= texture(_material.texture_specular, fs_in.texture_coords).rgb;
	}

	// calculate shadow
	float shadow = ShadowCalculation(fs_in.fragment_position_lightSpace);

	vec3 color = ambient + ((1.0 - shadow) * (diffuse + specular));

	return color;
}

vec3 ComputeSpotLight(Light _light, Material _material)
{
	vec3 color;

	vec3 lightDir = normalize(vec3(_light.position) - fs_in.fragment_position);

	float theta = dot(lightDir, normalize(-_light.direction));


	//if (theta > _light.cutOff)
	//{
	vec3 norm = normalize(fs_in.normal);

	float diff = max(dot(norm, lightDir), 0.0); //use max to avoid negative results

	vec3 viewDir = normalize(view_position - fs_in.fragment_position);
	vec3 reflectDir = reflect(-lightDir, fs_in.normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), _material.shininess); 

	vec3 ambient = _light.ambient * _material.color_ambient;
	vec3 diffuse = _light.diffuse * (diff * _material.color_diffuse);
	vec3 specular = _light.specular * (spec * _material.color_specular);

	if (hasTexture)
	{
		ambient *= texture(_material.texture_diffuse, fs_in.texture_coords).rgb;
		diffuse *= texture(_material.texture_diffuse, fs_in.texture_coords).rgb;
		specular *= texture(_material.texture_specular, fs_in.texture_coords).rgb;
	}

	// To smooth the edges of the spotlight
	float epsilon = _light.cutOff - _light.outerCutOff;
	float intensity = clamp((theta - _light.outerCutOff) / epsilon, 0.0, 1.0);
	diffuse *= intensity;
	specular *= intensity;

	// Adjust for distance to add realism (attenuation)
	float distance = length(vec3(_light.position) - fs_in.fragment_position);
	float attenuation = 1.0 / (_light.constant + _light.linearK * distance + _light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// calculate shadow
	float shadow = ShadowCalculation(fs_in.fragment_position_lightSpace);

	color = ambient + ((1.0 - shadow) * (diffuse + specular));
	//}
	//else  // else, use ambient light so scene isn't completely dark outside the spotlight.
	//{
	//	color = ambient;
	//}

	return color;
}


float ShadowCalculation(vec4 frag_pos_light/*, vec3 light_dir*/) 
{
	// perform perspective divide
	vec3 projCoords = frag_pos_light.xyz / frag_pos_light.w; // This returns the fragment's light-space position in the range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	//float bias = max(0.05*(1.0 - dot(fs_in.normal, light_dir)), 0.005);
	float shadow = (currentDepth/*-bias*/) > closestDepth ? 1.0 : 0.0;

	if (projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}




