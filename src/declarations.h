/**
 * @file	declarations.h
 * @brief	This file contans all global variables
 */


// GLOBAL CONSTANTS____________________________________________________________
const char*	HEIGHTMAP_TEXTURE_FILE_NAME = "data/world_height_8192.raw";
const char*	DIFFUSE_TEXTURE_FILE_NAME	= "data/world_diffuse_8192.raw";

const char*	VS_TESS_FILE_NAME			= "src/tesselation.vert";
const char*	GS_FILE_NAME				= "src/tesselation.geom";
const char*	FS_TESS_FILE_NAME			= "src/tesselation.frag";

const char*	VS_HIGHLIGHT_FILE_NAME		= "src/highlight.vert";
const char*	FS_HIGHLIGHT_FILE_NAME		= "src/highlight.frag";

// GLOBAL VARIABLES____________________________________________________________
GLint    g_WindowWidth       = 800;    // Window width
GLint    g_WindowHeight      = 600;    // Window height

bool     g_UseShaders        = true;  // Programmable pipeline on/off
bool     g_UseVertexShader   = true;  // Use vertex shader
bool     g_UseGeometryShader = true;  // Use geometry shader
bool     g_UseFragmentShader = true;  // Use fragment shader

bool     g_WireMode          = true;  // Wire mode enabled/disabled

bool	 g_highlightOrig	 = false; //highlight original triangles
bool	 g_freeze			 = false; //freeze tesselation in current location

float	 g_maxTessDistance	 = 2.5f;  //maximal distance, in which tesselation is done
GLint	 g_tesselationFactor = 10;	  //tesselation factor, factor of n generates n^2 triangles
GLint	 g_subtriangles;			  //number of produced subtriangles, tesselation factor square

float	 g_fps					= 0;  //FPS counter
bool	 g_MouseRotationEnabled = false;

//Transformation matrixes
mat4	g_CameraProjectionMatrix;     // Camera projection transformation
mat4	g_CameraViewMatrix;           // Camera view transformation

//Buffer IDs
GLuint	emptyVBO				= 0;
GLuint	originalTrianglesVBO	= 0;
GLuint	vertexTBO				= 0;
GLuint	indicesTBO				= 0;
GLuint	indicesVBO				= 0;
GLuint	textureBO				= 0;

//texture IDs
GLuint	 g_DiffuseTexId			= 0;  //diffuse texture
GLuint   g_HeightMapTexId		= 0;  //height texture

//Program IDs
GLuint	g_tesselationProgramId	= 0;
GLuint	g_highlightProgramId	= 0;

//Moving the camera
vec3	cameraPos(0.0f, -1.0f, -1.0f);
vec3	cameraRot(0.0f, 0.0f, 0.0f);
vec3	cameraPosLag(cameraPos);
vec3	cameraRotLag(cameraRot);

//view params (mouse)
int ox, oy;
const float inertia			= 0.1f;  //mouse inertia
const float rotateSpeed		= 0.4f;  //mouse rotate speed (sensitivity)
const float walkSpeed			= 0.02f; //walking speed (wasd)

const	vec3 lightPos(0.0f, 10.0f, 0.0f);//Light position

vec3	g_freezePos(cameraPos);		  //current freeze position

float*	triangles				= 0;  //pointer to mesh
int		triangleCount			= 0;  //number of triangles to draw

int * indices;

const int max_vertices			= 10000000; //max number of vertices, that can be produced by tesselation 

long	fps_begin, fps_time;		  //Additional variables storing timestamps for time measuring
int		fpsCounter				= 0;  //FPS measurment

float empty[] = {0};				  //empty buffer


// FORWARD DECLARATIONS________________________________________________________
void TW_CALL cbSetShaderStatus(const void*, void*);
void TW_CALL cbGetShaderStatus(void*, void*);
void TW_CALL cbFreeze(const void *value, void *clientData);
void TW_CALL cbGetFreeze(void *value, void *clientData);
void TW_CALL cbCompileShaderProgram(void *clientData);
float* genPlainMesh(float size, int width, int height, int * count);