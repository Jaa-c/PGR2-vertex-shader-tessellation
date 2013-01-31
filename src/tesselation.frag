#version 330

in block {
	vec4 v_Vertex;
	vec3 v_Normal;
	vec2 v_texCoord;
} In;

out vec4 v_FragColor;

uniform sampler2D u_diffTexture;

const vec3 light_pos = vec3(0.0, 1.0, 0.0);

void main() {

	vec3 N = In.v_Normal;
	vec3 L = normalize(light_pos - In.v_Vertex.xyz);
	float diffuse = max(dot(N, L), 0.0);
	
	//vec3 E = normalize(v_Vertex);
	//vec3 R = normalize(reflect(L, -N));
	//float specular = pow(max(dot(R, E), 0.0), 32.0);

	vec3 color = texture2D(u_diffTexture, In.v_texCoord).xyz;// * diffuse;
	
	v_FragColor = vec4(color, 1.0);
}