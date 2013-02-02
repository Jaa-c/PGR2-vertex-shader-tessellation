#version 330 core

uniform mat4 u_ModelViewMatrix;

uniform int u_tessFactor;			//tessellation factor
uniform int u_subtriangles;			//number of produced triangles (tessellation factor square)
uniform float u_maxTessDistance;	//behind this distance, no tessellation is done

uniform bool u_freeze;				//flag, if the tessellation is freezed
uniform vec3 u_freezePos;			//position of tessellation center

uniform samplerBuffer u_vertexTBO;	//vertex data
//uniform isamplerBuffer u_indicesTBO; //indices data
uniform sampler2D u_heightTexture;	//height texture for displacement mapping

out block {
	vec4 v_Position;				//position in object space
	vec3 v_Normal;					//normal in object space
	vec2 v_texCoord;				//texture coordinates
} Out;

out int v_discard;					//if not 0, vertex is dicarded on geometry shader

const float heightMult = 0.1f;		//height correction in height map
const float minDistance = 0.5;		//minimal distance, where tessellation has always maximal factor

const ivec3 off = ivec3(-1, 0, 1); 	//offsets for normals computation
const vec2 size = vec2(2.0, 0.0);

void main () {

	int vertexID = ((gl_VertexID / 3) / u_subtriangles) * 3 + (gl_VertexID % 3);
	int index = (vertexID / 3) * 3; //round to % 3 == 0
	
	/*
	ivec4 i1 = texelFetch(u_indicesTBO, index);
	ivec4 i2 = texelFetch(u_indicesTBO, index+1);
	ivec4 i3 = texelFetch(u_indicesTBO, index+2);

	vec4 v1 = texelFetch(u_vertexTBO, i1.r);
	vec4 v2 = texelFetch(u_vertexTBO, i2.r);
	vec4 v3 = texelFetch(u_vertexTBO, i3.r);*/

	//original triangle verticies
	vec4 v1 = texelFetch(u_vertexTBO, index);
	vec4 v2 = texelFetch(u_vertexTBO, index+1);
	vec4 v3 = texelFetch(u_vertexTBO, index+2);

	vec4 c = (v1 + v2 + v3) / 3; //centroid of the original triangle

	vec2 texCoord = ((c.xz/5.0f) * 0.5) + 0.5; //centroid coordinates in height texture
	texCoord.y += 0.25;

	c.y = texture2D(u_heightTexture, texCoord).r * heightMult;//correct centroid height

	//calculate the distance of tessellation centre from centroid
	vec3 d; 
	if(u_freeze) {
		d = c.xyz + u_freezePos; //distance from freezed point (in object space)
	}
	else {
		vec4 viewC = u_ModelViewMatrix * c;
		d = viewC.xyz; //distance from camera (in eye space)
	}

	float dist = sqrt(d.x * d.x + d.y * d.y + d.z * d.z); //the final distance

	//compute the final adaptive tessellation factor
	int tessFactor, subTriangles;
	if(dist < minDistance) {
		tessFactor = u_tessFactor;
		subTriangles = u_subtriangles;
	}
	else {
		dist -= minDistance;
		dist /= (u_maxTessDistance - minDistance);

		tessFactor = max(int(floor(u_tessFactor * (1 - dist))), 1);
		subTriangles = tessFactor * tessFactor;
	}

	int subTriID = (gl_VertexID / 3) % subTriangles; //ID of the subtriangle with new factor
	int origSubTriID = (gl_VertexID / 3) % (u_subtriangles); //ID of the subtriangle with old factor

	//if all triangles have been created, we discard the remaining verticies (on geometry shader)
	if(tessFactor != u_tessFactor && origSubTriID > subTriangles) {
		v_discard = 1;
	}
	else { //otherwise, we compute vertex coordiantes
		v_discard = 0;

		//barycentric coords u, v, w
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

		//this is the vertex position 
		vec4 vertex = u * v1 + v * v2 + w * v3;
		
		Out.v_texCoord = ((vertex.xz/5.0f) * 0.5f) + 0.5f; //coordinates in the textures
		Out.v_texCoord.y += 0.25;

		//the following code fragment is taken from http://stackoverflow.com/a/5284527/683905
		//it computes normals from the height map
		vec4 height = texture2D(u_heightTexture, Out.v_texCoord);
		float s01 = textureOffset(u_heightTexture, Out.v_texCoord, off.xy).x;
		float s21 = textureOffset(u_heightTexture, Out.v_texCoord, off.zy).x;
		float s10 = textureOffset(u_heightTexture, Out.v_texCoord, off.yx).x;
		float s12 = textureOffset(u_heightTexture, Out.v_texCoord, off.yz).x;
		vec3 va = normalize(vec3(size.x, s21-s01, size.y));
		vec3 vb = normalize(vec3(size.y, s12-s10, -size.x));
		vec4 bump = vec4(cross(va,vb), height.x);

		vertex.y += (bump.w) * heightMult; //final vertex position
	
		Out.v_Position = vertex;
		Out.v_Normal = normalize(bump.xyz);
	}
}
