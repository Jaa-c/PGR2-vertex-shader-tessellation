//-----------------------------------------------------------------------------
//  [PGR2] Cube model
//  27/02/2008
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

#ifndef __CUBE_MODEL_H__
#define __CUBE_MODEL_H__

const GLfloat CUBE_VERTEX_ARRAY[] =
{
	 -1.0f,-1.0f, 1.0f,  1.0f,-1.0f, 1.0f,  1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
	 -1.0f,-1.0f,-1.0f, -1.0f, 1.0f,-1.0f,  1.0f, 1.0f,-1.0f,  1.0f,-1.0f,-1.0f,
	 -1.0f, 1.0f,-1.0f, -1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f,
	 -1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f,  1.0f,-1.0f, 1.0f, -1.0f,-1.0f, 1.0f,
	  1.0f,-1.0f,-1.0f,  1.0f, 1.0f,-1.0f,  1.0f, 1.0f, 1.0f,  1.0f,-1.0f, 1.0f,
	 -1.0f,-1.0f,-1.0f, -1.0f,-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f
};

const GLfloat CUBE_VERTEX_ARRAY_TRIANGLE_VERTICES_4D[] =
{
   -1.0f,-1.0f, 1.0f, 1.0f,    1.0f,-1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,
   -1.0f,-1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   -1.0f, 1.0f, 1.0f, 1.0f,
   -1.0f,-1.0f,-1.0f, 1.0f,   -1.0f, 1.0f,-1.0f, 1.0f,    1.0f, 1.0f,-1.0f, 1.0f,
   -1.0f,-1.0f,-1.0f, 1.0f,    1.0f, 1.0f,-1.0f, 1.0f,    1.0f,-1.0f,-1.0f, 1.0f,
   -1.0f, 1.0f,-1.0f, 1.0f,   -1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,
   -1.0f, 1.0f,-1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,-1.0f, 1.0f,
   -1.0f,-1.0f,-1.0f, 1.0f,    1.0f,-1.0f,-1.0f, 1.0f,    1.0f,-1.0f, 1.0f, 1.0f,
   -1.0f,-1.0f,-1.0f, 1.0f,    1.0f,-1.0f, 1.0f, 1.0f,   -1.0f,-1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f, 1.0f,    1.0f, 1.0f,-1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f,-1.0f, 1.0f, 1.0f,
   -1.0f,-1.0f,-1.0f, 1.0f,   -1.0f,-1.0f, 1.0f, 1.0f,   -1.0f, 1.0f, 1.0f, 1.0f,
   -1.0f,-1.0f,-1.0f, 1.0f,   -1.0f, 1.0f, 1.0f, 1.0f,   -1.0f, 1.0f,-1.0f, 1.0f
};

const GLfloat CUBE_VERTEX_ARRAY_TRIANGLE_NORMALS[] =
{
   0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,
   0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,
   0.0f, 0.0f,-1.0f,    0.0f, 0.0f,-1.0f,    0.0f, 0.0f,-1.0f,
   0.0f, 0.0f,-1.0f,    0.0f, 0.0f,-1.0f,    0.0f, 0.0f,-1.0f,
   0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 0.0f,
   0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 0.0f,
   0.0f,-1.0f, 0.0f,    0.0f,-1.0f, 0.0f,    0.0f,-1.0f, 0.0f,
   0.0f,-1.0f, 0.0f,    0.0f,-1.0f, 0.0f,    0.0f,-1.0f, 0.0f,
   1.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,
   1.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,
  -1.0f, 0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
  -1.0f, 0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
};

const GLfloat CUBE_COLOR_ARRAY[] =
{
	1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 1.0f,
};

const GLfloat CUBE_TEX_COORD_ARRAY[] =
{
	 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
	 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
	 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
	 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
	 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
	 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
};

const GLfloat CUBE_NORMAL_ARRAY[] =
{
    0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
    0.0f, 0.0f,-1.0f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,-1.0f,
    0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
    0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
   -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
};

const GLfloat CUBE_COLOR_AND_VERTEX_ARRAY[] =
{
   // <cr, cg, cb>, <vx, vy, vx>
    1.0f, 0.0f, 0.0f, -1.0f,-1.0f, 1.0f,   0.0f, 1.0f, 0.0f,  1.0f,-1.0f, 1.0f,
    0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, -1.0f,-1.0f,-1.0f,   0.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f,   1.0f, 0.0f, 0.0f,  1.0f,-1.0f,-1.0f,  
    0.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f,   1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f,
    1.0f, 0.0f, 1.0f, -1.0f,-1.0f,-1.0f,   1.0f, 0.0f, 0.0f,  1.0f,-1.0f,-1.0f,
    0.0f, 1.0f, 0.0f,  1.0f,-1.0f, 1.0f,   1.0f, 0.0f, 0.0f, -1.0f,-1.0f, 1.0f,
    1.0f, 0.0f, 0.0f,  1.0f,-1.0f,-1.0f,   1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f,
    0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,  1.0f,-1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, -1.0f,-1.0f,-1.0f,   1.0f, 0.0f, 0.0f, -1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f,
};


const GLubyte CUBE_INDEX_ARRAY[] =
{
   0, 1, 2, 3,
	4, 5, 6, 7,
	5, 3, 2, 6,
	4, 7, 1, 0,
	7, 6, 2, 1,
	4, 0, 3, 5
};

const int NUM_CUBE_VERTICES = sizeof(CUBE_VERTEX_ARRAY) / (3*sizeof(GLfloat));
const int NUM_CUBE_INDICES  = sizeof(CUBE_INDEX_ARRAY) / sizeof(GLubyte);



//-----------------------------------------------------------------------------
// Name: drawCube()
// Desc: 
//-----------------------------------------------------------------------------
inline void pgr2DrawCube()
{
   GLboolean vertex_array   = glIsEnabled(GL_VERTEX_ARRAY);
   GLboolean normal_array   = glIsEnabled(GL_NORMAL_ARRAY);
   GLboolean texcoord_array = glIsEnabled(GL_TEXTURE_COORD_ARRAY);

   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, CUBE_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glNormalPointer(GL_FLOAT, 0, CUBE_NORMAL_ARRAY);
   if (glIsEnabled(GL_TEXTURE_2D) == GL_TRUE)
   {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, CUBE_TEX_COORD_ARRAY);
   }

	glDrawArrays(GL_QUADS, 0, NUM_CUBE_VERTICES);
//   glDrawElements(GL_QUADS, NUM_CUBE_INDICES, GL_UNSIGNED_BYTE, 
//                  CUBE_INDEX_ARRAY);

   if (vertex_array == GL_FALSE)    glDisableClientState(GL_VERTEX_ARRAY);
   if (normal_array == GL_FALSE)    glDisableClientState(GL_NORMAL_ARRAY); 
   if (texcoord_array == GL_FALSE)  glDisableClientState(GL_TEXTURE_COORD_ARRAY); 
}


#endif // __CUBE_MODEL_H__