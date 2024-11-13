#version 450 core 

#define MAX_NUM_OF_LIGHTS 5

// =========================================
in FS_IN
{
    vec3 worldPos;
    vec3 norm;
} i;

layout(location = 0) out vec4 oColor;
// =========================================

struct Light
{
    vec4 position;
	vec4 ambientColor;
    vec4 diffuseColor;
    vec4 specularColor;
};
struct Material
{
    vec3 ka, kd, ks;
    float shininess;
};

uniform Light light;
uniform Material material;
uniform vec4 objectColor;
uniform vec3 cameraPos;


vec3 Phong(vec3 worldPos, vec3 norm, vec3 view, vec3 surfaceColor)
{
    vec3 N = normalize(norm);
    vec3 V = normalize(view);

    // ambient 
    vec3 color = material.ka * light.ambientColor.rgb;

	// diffuse 
	vec3 L = normalize(light.position.xyz - worldPos);
	color += material.kd * light.diffuseColor.rgb * surfaceColor * max(dot(N, L), 0.0f);

	// specular
	vec3 R = reflect(-L, N);
	color += material.ks * light.specularColor.rgb * pow(max(dot(R, V), 0.0f), material.shininess);

    return color;
}


void main()
{
	// vec3 view = normalize(cameraPos - i.worldPos);
	vec3 color = objectColor.rgb; // Phong(i.worldPos, N, view, objectColor);
	
	oColor = vec4(color, 1.0f);
}