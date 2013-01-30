#version 330

uniform mat4 u_ModelViewMatrix; 
uniform mat4 u_ProjectionMatrix;
uniform int u_subtriangles;
uniform int u_tessFactor;

uniform samplerBuffer u_vertexTBO;
uniform sampler2D u_heightTexture;

out vec3 v_Vertex;
out vec3 v_Normal;
out vec3 v_Color;
out vec2 a_texCoord;

vec4 a_Vertex;


const vec2 size = vec2(2.0,0.0);
const ivec3 off = ivec3(-1,0,1);

const float heightMult = 3.0f;

void main () {

	//first find distance from camera	
	int vertexID = ((gl_VertexID / 3) / u_subtriangles) * 3 + (gl_VertexID % 3);

	int index = (vertexID / 3) * 3; //round to % 3 == 0
	//original triangle verticies	
	vec4 v1 = texelFetch(u_vertexTBO, index);
	vec4 v2 = texelFetch(u_vertexTBO, index+1);
	vec4 v3 = texelFetch(u_vertexTBO, index+2);

	vec4 c = (v1 + v2 + v3) / 3;

	a_texCoord = ((c.xz/5.0f) * 0.5) + 0.5;

	float height = texture2D(u_heightTexture, a_texCoord).r * heightMult;// * 0.3f;
	c.y += height;

	vec4 viewC = u_ProjectionMatrix * u_ModelViewMatrix * c;
	float dist = sqrt(viewC.x * viewC.x + viewC.y * viewC.y + viewC.z * viewC.z); 
	
	float param = min(1.0f/(dist/1.5f), 1.0f);//2

	int tessFactor = max(int(floor(u_tessFactor * param)), 1);
	int subTriangles = tessFactor * tessFactor;


	v_Color = vec3(1.0, 1.0, param);


	int subTriID = (gl_VertexID / 3) % subTriangles;

	//barycentric coords u, v , w
	float fRow = floor(sqrt(subTriID));
	float incuv = 1.0f / tessFactor;
	float u = (1.0f + fRow) / tessFactor;
	
	float fCol = subTriID - (fRow * fRow);
	int uCol = int(fCol);
	float v = incuv * floor(fCol * .5f);
	u-= v;

	float w = 1.0f - u - v;
	
	switch(vertexID % 3) {
		case 0:
			if((uCol & 1) != 0) {
				v += incuv;
				u -= incuv;
			}
			break;
		case 1:
			v += incuv;
			u -= incuv;
			if((uCol & 1) != 0) {
				w += incuv;
				u -= incuv;		
			}
			break;
		case 2:
			w += incuv;
			u -= incuv;		
			break;
	}

	a_Vertex = u * v1 + v * v2 + w * v3;

	

	a_texCoord = ((a_Vertex.xz/5.0f) * 0.5) + 0.5;

	//this code fragment is taken from http://stackoverflow.com/a/5284527/683905
    vec4 wave = texture2D(u_heightTexture, a_texCoord);
    float s11 = wave.x;
    float s01 = textureOffset(u_heightTexture, a_texCoord, off.xy).x;
    float s21 = textureOffset(u_heightTexture, a_texCoord, off.zy).x;
    float s10 = textureOffset(u_heightTexture, a_texCoord, off.yx).x;
    float s12 = textureOffset(u_heightTexture, a_texCoord, off.yz).x;
    vec3 va = normalize(vec3(size.xy,s21-s11));
    vec3 vb = normalize(vec3(size.yx,s12-s10));
    vec4 bump = vec4( cross(va,vb), s11 );



	
	//height = texture2D(u_heightTexture, a_texCoord).r * 5.0;// * 0.3f;
	a_Vertex.y += bump.w * heightMult;
	
	vec4 viewPos = u_ModelViewMatrix * a_Vertex;
	v_Normal = mat3(u_ModelViewMatrix) * bump.xyz;//normalize(mat3(u_ModelViewMatrix) * vec3(0, 0, 1.0));
	
	v_Vertex = viewPos.xyz;
	gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * a_Vertex;
	//v_Vertex = gl_Position.xyz;
	//gl_Position = a_Vertex;

}