#version 460 core

out vec4 FragColor;

in VS_OUT
{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} fs_in;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vec3 color = texture(texture_diffuse, fs_in.TexCoords).rgb;
	vec3 normal = texture(texture_normal, fs_in.TexCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	vec3 ambient = 0.3 * color;

	vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * color;

	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	vec3 specular = spec * texture(texture_specular, fs_in.TexCoords).rgb;

	FragColor = vec4(ambient + diffuse + specular, 1.0);
}