//-----------------------------------------------------------------------------
//  [PGR2] Some helpful common functions
//  27/02/2011
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

#include <math.h>
#define BUFFER_OFFSET(i) ((char*) NULL + (i))

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% READ FILE                                              %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void* pgr2ReadFile(const char* file_name, long* bytes_read = 0)
{
    char* buffer = NULL;

    // Read input file
    if (file_name != NULL) 
    {
        FILE* fin = fopen(file_name,"rb");
        if (fin != NULL) 
        {
            fseek(fin, 0, SEEK_END);
            long file_size = ftell(fin);
            rewind(fin);

            if (file_size > 0) 
            {
                buffer = new char[file_size];
                int count = fread(buffer, sizeof(char), file_size, fin);
                if (bytes_read) *bytes_read = count;
//              assert(count == file_size);
            }
            fclose(fin);
        }
    }

    return buffer;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% SOME GL FUNCTIONS                                      %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//-----------------------------------------------------------------------------
// Name: pgr2AddQuaternionRotationToTransformation()
// Desc: 
//-----------------------------------------------------------------------------
inline void pgr2AddQuaternionRotationToTransformation(const float* quat)
{
#ifdef USE_ANTTWEAKBAR
    // quat = q(s, v)
    // A rotation about the unit vector U by an angle theta is defined:
    // s = cos(theta / 2), v = U.sin(theta / 2)
    const double THETA     = 2.0 * acos(quat[3]);
    const float  SIN_THETA = float(sin(THETA * 0.5));
    const float  THETA_DEG = float(180 * (THETA / 3.14159265));

    glRotatef(THETA_DEG, quat[0]*SIN_THETA, quat[1]*SIN_THETA, quat[2]*SIN_THETA);
#else
    // Fake implementation, but sufficient
    glRotatef(quat[0], 1.0f, 0.0f, 0.0f);
    glRotatef(quat[1], 0.0f, 1.0f, 0.0f);
    glRotatef(quat[2], 0.0f, 0.0f, 1.0f);
#endif
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% MESH GENERATORS                                        %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//-----------------------------------------------------------------------------
// Name: pgr2CreatePlaneMesh()
// Desc: 
//-----------------------------------------------------------------------------
GLfloat* pgr2CreatePlaneMesh(float size, int slice_x, int slice_y, int* num_vertices)
{
    assert(num_vertices != NULL);

    // Compute position deltas for moving down the X, and Z axis during mesh creation
    const float dX = size / slice_x;
    const float dY = size / slice_y;
    const float startX = -size * 0.5f;
    const float startY = -size * 0.5f;

    const int NUM_VERTICES = 4*slice_x*slice_y;
    if (NUM_VERTICES < 4)
    {
        return NULL;
    }
   
    GLfloat* mesh = new GLfloat[3*NUM_VERTICES];
    GLfloat* ptr  = mesh;

    for (int x = 0; x < slice_x; x++)
    {
        for (int y = 0; y < slice_y; y++)
        {
            *ptr++ = startX + x * dX;
            *ptr++ = startY + y * dY;
            *ptr++ = 0.0f;
            *ptr++ = startX + (x + 1) * dX;
            *ptr++ = startY + y * dY;
            *ptr++ = 0.0f;
            *ptr++ = startX + (x + 1) * dX;
            *ptr++ = startY + (y + 1) * dY;
            *ptr++ = 0.0f;
            *ptr++ = startX + x * dX;
            *ptr++ = startY + (y + 1) * dY;
            *ptr++ = 0.0f;
        }
    }

    if (num_vertices != NULL)
    {
        *num_vertices = NUM_VERTICES;
    }
    return mesh;
}


//-----------------------------------------------------------------------------
// Name: pgr2UpdatePlaneMeshBufferObjects()
// Desc: update mesh VBO and IBO
//-----------------------------------------------------------------------------
bool pgr2UpdatePlaneMeshBufferObjects(GLsizei vertex_density, GLsizei index_density, GLenum mode, GLuint vbo_id, GLuint ibo_id, GLsizei* num_vertices = NULL, GLsizei* num_indices = NULL)
{
    if ((vertex_density < 2) || ((index_density < 2) && (index_density != 0))) return false;
    if ((mode != GL_TRIANGLES) && (mode != GL_TRIANGLE_STRIP) && (mode != GL_QUADS)) return false;

    // Generate mesh vertices
    if (vbo_id > 0)
    {
        int total_vertices = vertex_density*vertex_density;
        const GLfloat OFFSET = 2.0f / (vertex_density-1);

        glm::vec2* vertices = new glm::vec2[total_vertices];
        glm::vec2* ptr_vec2 = vertices;
        for (int y = 0; y < vertex_density - 1; y++)
        {
            for (int x = 0; x < vertex_density - 1; x++)
                *ptr_vec2++ = glm::vec2(-1.0f + x*OFFSET, -1.0f + y*OFFSET);
            *ptr_vec2++ = glm::vec2(1.0f, -1.0f + y*OFFSET);
        }
        for (int x = 0; x < vertex_density - 1; x++)
            *ptr_vec2++ = glm::vec2(-1.0f + x*OFFSET, 1.0f);
        *ptr_vec2 = glm::vec2(1.0f, 1.0f);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
        glBufferData(GL_ARRAY_BUFFER, 2*sizeof(GLfloat)*total_vertices, &vertices[0].x, GL_STATIC_DRAW);

        if (num_vertices) *num_vertices = total_vertices;

        delete [] vertices;
    }

    // Generate mesh indices
    if (ibo_id > 0)
    {
        if (index_density == 0)
        {
            int total_indices = (mode == GL_TRIANGLES) ? 6*(vertex_density - 1)*(vertex_density - 1) : 2*(vertex_density*vertex_density - 2);
            GLuint* indices  = new GLuint[total_indices];

            GLuint index  = 0;
            GLuint* ptr_int = indices;
            if (mode == GL_TRIANGLES)
            {
                for (int y = 0; y < vertex_density - 1; y++)
                {
                    for (int x = 0; x < vertex_density - 1; x++)
                    {
                        *ptr_int++ = index;
                        *ptr_int++ = index + 1;
                        *ptr_int++ = index + vertex_density;
                        *ptr_int++ = index + vertex_density;
                        *ptr_int++ = index + 1;
                        *ptr_int++ = index + 1 + vertex_density;
                        index++;
                    }
                    index++;
                }
            }
            else
            {
                for (int y = 0; y < vertex_density - 1; y++)
                {
                    for (int x = 0; x < vertex_density; x++)
                    {
                        *ptr_int++ = index;
                        *ptr_int++ = (index + vertex_density);
                        index++;
                    }
                    if ((vertex_density > 2) && (y != vertex_density - 2))
                    {
                        *ptr_int++ = (index + vertex_density - 1);
                        *ptr_int++ = index;
                    }
                }
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*total_indices, indices, GL_STATIC_DRAW);

            if (num_indices) *num_indices = total_indices;

            delete [] indices;
        } 
        else // (index_density == 0)
        {
            int total_indices = (mode == GL_TRIANGLES) ? 6*(index_density - 1)*(index_density - 1) : 
                                ((mode == GL_TRIANGLE_STRIP) ? 2*(index_density*index_density - 2) : 4*(index_density - 1)*(index_density - 1));
            GLuint* indices    = new GLuint[total_indices];
            float H_INDEX_OFFSET = static_cast<float>(vertex_density - 1) / (index_density - 1);

            GLuint index  = 0;
            GLuint* ptr_int = indices;
            switch (mode)
            {
            case GL_TRIANGLES:
                {
                    for (int y = 0; y < index_density - 1; y++)
                    {
                        const GLuint vindex1 = glm::clamp(static_cast<int>(glm::round(y * H_INDEX_OFFSET)*vertex_density), 0, vertex_density*(vertex_density - 1));
                        const GLuint vindex2 = glm::clamp(static_cast<int>(glm::round((y + 1)*H_INDEX_OFFSET) * vertex_density), 0, vertex_density*(vertex_density - 1));
                        for (int x = 0; x < index_density - 1; x++)
                        {
                            const GLuint hindex1 = glm::clamp(static_cast<int>(x * H_INDEX_OFFSET + 0.5f), 0, vertex_density - 1);
                            const GLuint hindex2 = glm::clamp(static_cast<int>((x + 1) * H_INDEX_OFFSET + 0.5f), 0, vertex_density - 1);
                            ptr_int[0] = vindex1 + hindex1;
                            ptr_int[1] = ptr_int[4] = vindex1 + hindex2;
                            ptr_int[2] = ptr_int[3] = vindex2 + hindex1;
                            ptr_int[5] = vindex2 + hindex2;
                            ptr_int += 6;
                        }
                    }
                }
                break;
            case GL_TRIANGLE_STRIP:
                {
                    for (int y = 0; y < index_density - 1; y++)
                    {
                        const GLuint vindex1 = glm::clamp(static_cast<int>(glm::round(y * H_INDEX_OFFSET)*vertex_density), 0, vertex_density*(vertex_density - 1));
                        const GLuint vindex2 = glm::clamp(static_cast<int>(glm::round((y + 1)*H_INDEX_OFFSET) * vertex_density), 0, vertex_density*(vertex_density - 1));
                        GLuint* ptr_start = ptr_int;
                        GLuint* ptr_end = ptr_int;
                        for (int x = 0; x < index_density; x++)
                        {
                            const GLuint hindex = glm::clamp(static_cast<int>(x * H_INDEX_OFFSET + 0.5f), 0, vertex_density - 1);
                            ptr_int[0] = hindex + vindex1;
                            ptr_int[1] = hindex + vindex2;
                            ptr_end = ptr_int;
                            ptr_int += 2;
                        }
                        if (y < index_density - 2)
                        {
                            // add 2 degenerated triangles
                            ptr_int[0] = ptr_end[1];
                            ptr_int[1] = ptr_start[1];
                            ptr_int += 2;
                        }
                    }
                }
                break;
            case GL_QUADS:
                {
                    for (int y = 0; y < index_density - 1; y++)
                    {
                        const GLuint vindex1 = glm::clamp(static_cast<int>(glm::round(y * H_INDEX_OFFSET)*vertex_density), 0, vertex_density*(vertex_density - 1));
                        const GLuint vindex2 = glm::clamp(static_cast<int>(glm::round((y + 1)*H_INDEX_OFFSET) * vertex_density), 0, vertex_density*(vertex_density - 1));
                        for (int x = 0; x < index_density - 1; x++)
                        {
                            const GLuint hindex1 = glm::clamp(static_cast<int>(x * H_INDEX_OFFSET + 0.5f), 0, vertex_density - 1);
                            const GLuint hindex2 = glm::clamp(static_cast<int>((x + 1) * H_INDEX_OFFSET + 0.5f), 0, vertex_density - 1);
                            ptr_int[0] = hindex1 + vindex1;
                            ptr_int[1] = hindex1 + vindex2;
                            ptr_int[2] = hindex2 + vindex2;
                            ptr_int[3] = hindex2 + vindex1;
                            ptr_int += 4;
                        }
                    }
                }
                break;
            default:
                assert(0);
                break;
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*total_indices, indices, GL_STATIC_DRAW);

            if (num_indices) *num_indices = total_indices;

            delete [] indices;
        }
    } // (ibo_id > 0)
    
    return true;
}


//-----------------------------------------------------------------------------
// Name: pgr2DrawSimpleMesh()
// Desc: draw simple planar mesh
//-----------------------------------------------------------------------------
bool pgr2DrawSimpleMesh(float size, int slice_x, int slice_y)
{
    int num_vertices = 0;
    GLfloat* vertices = pgr2CreatePlaneMesh(size, slice_x, slice_y, &num_vertices);
    if ((vertices != NULL) && (num_vertices > 0))
    {
        const float INV_SIZE = 1.0f/size;
        GLfloat*  tex_coords = new GLfloat[2*num_vertices];
        for (int iVertex = 0; iVertex < num_vertices; iVertex++)
        {
            tex_coords[2*iVertex + 0] = vertices[3*iVertex + 0]*INV_SIZE + 0.5f;
            tex_coords[2*iVertex + 1] = vertices[3*iVertex + 1]*INV_SIZE + 0.5f;
        }

        GLboolean va_enabled = glIsEnabled(GL_VERTEX_ARRAY);
        GLboolean ta_enabled = glIsEnabled(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glTexCoordPointer(2, GL_FLOAT, 0, tex_coords);
        glDrawArrays(GL_QUADS, 0, num_vertices);
        delete [] vertices;
        delete [] tex_coords;

        if (va_enabled != GL_TRUE) glDisableClientState(GL_VERTEX_ARRAY);
        if (ta_enabled != GL_TRUE) glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        return true;
    }
    return false;
}


//-----------------------------------------------------------------------------
// Name: pgr2DrawScene()
// Desc: draw "complex" scene,
//       scene created by Miro Miksik, micro@sin.cvut.cz
//-----------------------------------------------------------------------------
#include "models\scene_miro.h"
bool pgr2DrawScene()
{
    GLint program_id = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program_id);
    if (program_id < 1) return false;
    
    GLint iVertex   = glGetAttribLocation(program_id, "a_Vertex");
    GLint iNormal   = glGetAttribLocation(program_id, "a_Normal");
    GLint iTexCoord = glGetAttribLocation(program_id, "a_TexCoord");

    if (iVertex > -1)
    {
        glVertexAttribPointer(iVertex, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), scene_miro);
        glEnableVertexAttribArray(iVertex);
    }
    else
    {
        return false;
    }
    if (iNormal> -1)
    {
        glVertexAttribPointer(iNormal, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), scene_miro + 3);
        glEnableVertexAttribArray(iNormal);
    }
    if (iTexCoord > -1)
    {
        glVertexAttribPointer(iTexCoord, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), scene_miro + 6);
        glEnableVertexAttribArray(iTexCoord);
    }

    const int NUM_VERTICES = sizeof(scene_miro) / (8*sizeof(GLfloat));
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);

    if (iTexCoord > -1) glDisableVertexAttribArray(iTexCoord);
    if (  iNormal > -1) glDisableVertexAttribArray(iNormal);
    glDisableVertexAttribArray(iVertex);

    return true;
}


//-----------------------------------------------------------------------------
// Name: pgr2CreateSphereMesh()
// Desc: based on OpenGL SuperBible example by Richard S. Wright Jr.
//-----------------------------------------------------------------------------
GLfloat* pgr2CreateSphereMesh(float radius, int slices, int stacks, int* num_vertices)
{
    assert(num_vertices != NULL);

    const float DRHO = 3.1415926536f / stacks;
    const float DTHETA = 2.0f * 3.1415926536f / slices;
   
    const int NUM_VERTICES = stacks*(2*slices+2);
    if (NUM_VERTICES < 1)
    {
        return 0;
    }

    GLfloat* mesh = new GLfloat[3*NUM_VERTICES];
    GLfloat* ptr   = mesh;
    for (int iStack = 0; iStack < stacks; iStack++)
    {
        const GLfloat RHO = iStack * DRHO;
        const GLfloat SRHO = (GLfloat)(sin(RHO));
        const GLfloat CRHO = (GLfloat)(cos(RHO));
        const GLfloat SRHODRHO = (GLfloat)(sin(RHO + DRHO));
        const GLfloat CRHODRHO = (GLfloat)(cos(RHO + DRHO));

        for (int iSlice = 0; iSlice <= slices; iSlice++)
        {
            const GLfloat THETA  = (iSlice == slices) ? 0.0f : iSlice * DTHETA;
            const GLfloat STHETA = (GLfloat)(-sin(THETA));
            const GLfloat CTHETA = (GLfloat)(cos(THETA));

            *ptr++ = STHETA * SRHODRHO * radius;
            *ptr++ = CTHETA * SRHODRHO * radius;
            *ptr++ = CRHODRHO * radius;
            *ptr++ = STHETA * SRHO * radius;
            *ptr++ = CTHETA * SRHO * radius;
            *ptr++ = CRHO * radius;
        }
    }

    *num_vertices = NUM_VERTICES;
    return mesh;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% TEXTURE TOOLS                                          %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//-----------------------------------------------------------------------------
// Name: pgr2ShowTexture()
// Desc: 
//-----------------------------------------------------------------------------
inline void pgr2ShowTexture(GLuint tex_id, GLint x, GLint y, GLsizei wight, GLsizei height)
{
    GLint viewport[4] = {0};
    glGetIntegerv(GL_VIEWPORT, viewport);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, viewport[2], 0, viewport[3], -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    GLboolean depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean lighting   = glIsEnabled(GL_LIGHTING);
    GLboolean texturing  = glIsEnabled(GL_TEXTURE_2D);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2i(x, y);
        glTexCoord2f(1.0f, 0.0f); glVertex2i(x+wight, y);
        glTexCoord2f(1.0f, 1.0f); glVertex2i(x+wight, y+height);
        glTexCoord2f(0.0f, 1.0f); glVertex2i(x, y+height);
	glEnd();
	if (texturing  == GL_TRUE) glDisable(GL_TEXTURE_2D);
	if (lighting   == GL_TRUE) glEnable(GL_LIGHTING);
    if (depth_test == GL_TRUE) glEnable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


//-----------------------------------------------------------------------------
// Name: prg2Load8bitTextureFromRawFile()
// Desc: 
//-----------------------------------------------------------------------------
inline GLuint prg2Load8bitTextureFromRawFile(const char* filename, GLsizei* num_texels = NULL)
{
    // Create mipmapped texture with image
    long bytes_read = 0;
    void* rgb_data = pgr2ReadFile(filename, &bytes_read);
    if (rgb_data == NULL) return 0;

    const GLsizei width = static_cast<GLsizei>(sqrtf(bytes_read));
    assert(width*width == bytes_read);

    GLuint texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, width, GL_RED, GL_UNSIGNED_BYTE, rgb_data);

    delete [] rgb_data;

    if (num_texels) *num_texels = width*width;

    return texId;
}


//-----------------------------------------------------------------------------
// Name: prg2LoadRGBTextureFromRawFile()
// Desc: 
//-----------------------------------------------------------------------------
inline GLuint prg2LoadRGBTextureFromRawFile(const char* filename, GLsizei* num_texels = NULL)
{
    // Create mipmapped texture with image
    long bytes_read = 0;
    void* rgb_data = pgr2ReadFile(filename, &bytes_read);
    if (rgb_data == NULL) return 0;

    const GLsizei width = static_cast<GLsizei>(sqrtf(bytes_read / 3));
    assert(width*width*3 == bytes_read);

    GLuint texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, width, GL_RGB, GL_UNSIGNED_BYTE, rgb_data);

    delete [] rgb_data;

    if (num_texels) *num_texels = width*width;

    return texId;
}


//-----------------------------------------------------------------------------
// Name: prg2CreateSimpleTexture()
// Desc: 
//-----------------------------------------------------------------------------
inline GLuint prg2CreateSimpleTexture(GLint width, GLint height)
{
    if (width * height <= 0)
    {
        return 0;
    }

    GLubyte* data = new GLubyte[width*height*4];
    assert(data != NULL);

    const GLubyte pattern_width = 16;
    GLubyte* ptr = data; 
    GLubyte pattern = 0xFF;

    for (int h = 0; h < height; h++) 
    {              
        for (int w = 0; w < width; w++) 
        {
            *ptr++ = pattern; 
            *ptr++ = pattern;
            *ptr++ = pattern;
            *ptr++ = 255;
            if (w % pattern_width == 0) pattern = ~pattern;
        }

        if (h % pattern_width == 0) pattern = ~pattern;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    GLuint tex_id = 0;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    delete [] data;

    return tex_id;
} 


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% SHADER RELATED FUNCTIONS                               %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//-----------------------------------------------------------------------------
// Name: pgr2CheckShaderInfoLog()
// Desc: 
//-----------------------------------------------------------------------------
void pgr2CheckShaderInfoLog(GLuint shader_id)
{
    if (shader_id == 0)
    {
        return;
    }

    int log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    if (log_length > 0)
    {
        char* buffer  = new char[log_length];
        int   written = 0;
        glGetShaderInfoLog(shader_id, log_length, &written, buffer);
        printf("%s\n", buffer);
        delete [] buffer;
    }
}


//-----------------------------------------------------------------------------
// Name: pgr2CheckShaderInfoLog()
// Desc: 
//-----------------------------------------------------------------------------
void pgr2CheckProgramInfoLog(GLuint program_id)
{
    if (program_id == 0)
    {
        return;
    }

    int log_length = 0;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

    if (log_length > 0)
    {
        char* buffer  = new char[log_length];
        int   written = 0;
        glGetProgramInfoLog(program_id, log_length, &written, buffer);
        printf("%s\n", buffer);
        delete [] buffer;
    }
}


//-----------------------------------------------------------------------------
// Name: pgr2CheckShaderCompileStatus()
// Desc: 
//-----------------------------------------------------------------------------
GLint pgr2CheckShaderCompileStatus(GLuint shader_id)
{
    GLint status = GL_FALSE;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    return status;
}


//-----------------------------------------------------------------------------
// Name: pgr2CheckShaderCompileStatus()
// Desc: 
//-----------------------------------------------------------------------------
GLint pgr2CheckProgramLinkStatus(GLuint program_id)
{
    GLint status = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &status);
    return status;
}


//-----------------------------------------------------------------------------
// Name: pgr2CreateShaderFromSource()
// Desc: 
//-----------------------------------------------------------------------------
GLuint pgr2CreateShaderFromSource(GLenum shader_type, const char* source)
{
    if (source == NULL)
    {
        return 0;
    }

    switch (shader_type)
    {
        case GL_VERTEX_SHADER         : printf("vertex shader creation ... "); break;
        case GL_FRAGMENT_SHADER       : printf("fragment shader creation ... "); break;
        case GL_GEOMETRY_SHADER       : printf("geometry shader creation ... "); break;
        case GL_TESS_CONTROL_SHADER   : printf("tesselation control shader creation ... "); break;
        case GL_TESS_EVALUATION_SHADER: printf("tesselation evaluation shader creation ... "); break;
        default                       : return 0;
    }

    GLuint shader_id = glCreateShader(shader_type);
    if (shader_id == 0)
    {
        return 0;
    }

    glShaderSource(shader_id, 1, &source, NULL);
    glCompileShader(shader_id);

    if (pgr2CheckShaderCompileStatus(shader_id) != GL_TRUE)
    {
        printf("failed.\n");
        pgr2CheckShaderInfoLog(shader_id);
        glDeleteShader(shader_id);
        return 0;
    }
    else
    {
        printf("successfull.\n");
        return shader_id;
    }
}

//-----------------------------------------------------------------------------
// Name: pgr2CreateShaderFromFile()
// Desc: 
//-----------------------------------------------------------------------------
GLuint pgr2CreateShaderFromFile(GLenum shader_type, const char* file_name)
{
    if (file_name == NULL)
    {
        return 0;
    }

    char* buffer = NULL;

    // Read input file
    if (file_name != NULL) 
    {
        FILE* fin = fopen(file_name,"rb");
        if (fin != NULL) 
        {
            fseek(fin, 0, SEEK_END);
            long file_size = ftell(fin);
            rewind(fin);

            if (file_size > 0) 
            {
                buffer = new char[file_size + 1];
                int count = fread(buffer, sizeof(char), file_size, fin);
//              assert(count == file_size);
                buffer[count] = '\0';
            }
            fclose(fin);
        }
        else
        {
            return 0;
        }
    }

    assert(buffer != NULL);

    GLuint shader_id = pgr2CreateShaderFromSource(shader_type, buffer);
    delete [] buffer;

    return shader_id;
}


//-----------------------------------------------------------------------------
// Name: pgr2CreateShaderProgramFromFile()
// Desc: 
//-----------------------------------------------------------------------------
GLuint pgr2CreateShaderProgramFromFile(GLint count, const GLenum* shader_types, const char** source_file_names)
{
    if ((shader_types == NULL) || (source_file_names == NULL)) return 0;

    // Create shader program object
    GLuint pr_id = glCreateProgram();
    for (int i = 0; i < count; i++)
    {
        GLuint shader_id = pgr2CreateShaderFromFile(shader_types[i], source_file_names[i]);
        glAttachShader(pr_id, shader_id);
        glDeleteShader(shader_id);
    }
    glLinkProgram(pr_id);
    if (!pgr2CheckProgramLinkStatus(pr_id))
    {
        pgr2CheckProgramInfoLog(pr_id);
        printf("program linking failed.\n");
        glDeleteProgram(pr_id);
        return 0;
    }
    return pr_id;
}


//-----------------------------------------------------------------------------
// Name: pgr2DestroyShaderProgram()
// Desc: 
//-----------------------------------------------------------------------------
void pgr2DestroyShaderProgram(GLuint* program_Id, GLuint* vs_Id, 
                                GLuint* gs_Id, GLuint* fs_Id)
{
    if ((program_Id != NULL) && (*program_Id > 0))
    {
        if ((vs_Id != NULL) && (*vs_Id > 0))
        {
            glDeleteShader(*vs_Id);
            *vs_Id = 0;
        }
        if ((gs_Id != NULL) && (*gs_Id != 0))
        {
            glDeleteShader(*gs_Id);
            *gs_Id = 0;
        }
        if ((fs_Id != NULL) && (*fs_Id  != 0))
        {
            glDeleteShader(*fs_Id);
            *fs_Id = 0;
        }

        glDeleteProgram(*program_Id);
        *program_Id = 0;
    }
}