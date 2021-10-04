#version 330 core
struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  


struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define MAX_POINT_LIGHTS 1
#define MAX_DIR_LIGHTS 10
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform DirLight dir_light;
uniform DirLight dir_lights[MAX_DIR_LIGHTS];
uniform int point_light_count;
uniform int dir_light_count;

out vec4 FragColor;
  
in vec3 f_normal;
in vec3 w_frag_pos;
in vec2 f_texcoords;

uniform vec3 view_pos; //camera position
uniform Material m;

vec3 calculate_directional_light(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), m.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(m.diffuse, f_texcoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(m.diffuse, f_texcoords));
    vec3 specular = light.specular * spec * vec3(texture(m.specular, f_texcoords));
    return (ambient + diffuse + specular);
}  
vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), m.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(m.diffuse, f_texcoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(m.diffuse, f_texcoords));
    vec3 specular = light.specular * spec * vec3(texture(m.specular, f_texcoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 


void main()
{
	vec3 norm = normalize(f_normal);
	//vec3 light_dir = normalize(light.position - w_frag_pos);
	vec3 view_dir = normalize(view_pos - w_frag_pos);
	
	vec3 output_color = vec3(0);
	//calculate ambient light contribution
	output_color = calculate_directional_light(dir_light, norm, view_dir);
	//calculate point light contributions
	for(int i = 0; i < point_light_count; ++i)
		output_color += calculate_point_light(point_lights[i], norm, w_frag_pos, view_dir);
	//calculatte spot light contributions
	//for(int i = 0; i < NR_SPOT_LIGHTS; ++i)
		//output_color += calculate_spot_light(spot_lights[i], normal, w_frag_pos, view_dir);
	
	FragColor = vec4(output_color, 1.0);
}


