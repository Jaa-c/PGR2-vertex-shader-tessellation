#version 330

in vec3 v_Vertex;
in vec3 v_Normal;
in vec3 v_Color;
in vec2 a_texCoord;

uniform sampler2D u_diffTexture;

const vec3 light_pos = vec3(0.0, 1.0, 0.0);

void main() {

	vec3 N = normalize(v_Normal);
	vec3 E = normalize(-v_Vertex);
	vec3 L = normalize(light_pos - v_Vertex);
	vec3 R = normalize(-reflect(L, N));
	
	float diffuse = max(dot(N, L), 0.0);
	//float specular = pow(max(dot(R, E), 0.0), 32.0);

	vec3 color = texture2D(u_diffTexture, a_texCoord).xyz * diffuse;
	
	gl_FragColor = vec4(color, 1.0);
}