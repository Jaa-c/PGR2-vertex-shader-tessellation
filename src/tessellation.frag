#version 330 core

uniform sampler2D u_diffTexture;
uniform vec3 u_lightPos;

in block {
	vec4 v_Position;
	vec3 v_Normal;
	vec2 v_texCoord;
} In;

out vec4 v_FragColor;

void main() {

	vec3 L = normalize(u_lightPos - In.v_Position.xyz);
	float diffuse = max(dot(In.v_Normal, L), 0.0);

	vec3 color = texture2D(u_diffTexture, In.v_texCoord).xyz * diffuse;
	
	v_FragColor = vec4(color, 1.0);
}