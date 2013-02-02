//-----------------------------------------------------------------------------
//  [PGR2] Math functions
//  26/04/2011
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

#include <math.h>
#include <memory.h>

//-----------------------------------------------------------------------------
// Name: pgr2MatrixDeterminant()
// Desc: 
//-----------------------------------------------------------------------------
GLfloat pgr2MatrixDeterminant(const GLfloat mat[16])
{
   GLfloat det = 0;

   if (mat[0] != 0)
   {
      det += mat[0]*(mat[ 5]*(mat[10]*mat[15] - mat[11]*mat[14]) + 
                     mat[ 6]*(mat[11]*mat[13] - mat[ 9]*mat[15]) +
                     mat[ 7]*(mat[ 9]*mat[14] - mat[10]*mat[13]));
   }
   if (mat[1] != 0)
   {
      det -= mat[1]*(mat[ 4]*(mat[10]*mat[15] - mat[11]*mat[14]) +
                     mat[ 6]*(mat[11]*mat[12] - mat[ 8]*mat[15]) +
                     mat[ 7]*(mat[ 8]*mat[14] - mat[10]*mat[12]));
   }
   if (mat[2] != 0)
   {
      det += mat[2]*(mat[ 4]*(mat[ 9]*mat[15] - mat[11]*mat[13]) +
                     mat[ 5]*(mat[11]*mat[12] - mat[ 8]*mat[15]) +
                     mat[ 7]*(mat[ 8]*mat[13] - mat[ 9]*mat[12]));
   }
   if (mat[3] != 0)
   {
      det -= mat[3]*(mat[ 4]*(mat[ 9]*mat[14] - mat[10]*mat[13]) +
                     mat[ 5]*(mat[10]*mat[12] - mat[ 8]*mat[14]) +
                     mat[ 6]*(mat[ 8]*mat[13] - mat[ 9]*mat[12]));
   }
   
   return det;
}


//-----------------------------------------------------------------------------
// Name: pgr2MatrixInverse()
// Desc: 
//-----------------------------------------------------------------------------
bool pgr2MatrixInverse(const GLfloat mat_in[16], GLfloat mat_out[16])
{
   const GLfloat DET = pgr2MatrixDeterminant(mat_in);

   if (DET == 0)
   {
      return false;
   }
   const GLfloat DET_INV = 1.0f / DET;

   GLfloat mat_temp[16] = {0};
   memcpy(mat_temp, mat_in, 16*sizeof(GLfloat));

   mat_out[ 0] = (mat_in[ 5]*(mat_in[10] * mat_in[15] - mat_in[11] * mat_in[14]) + 
                  mat_in[ 6]*(mat_in[11] * mat_in[13] - mat_in[ 9] * mat_in[15]) + 
                  mat_in[ 7]*(mat_in[ 9] * mat_in[14] - mat_in[10] * mat_in[13])) * DET_INV;
   mat_out[ 1] = (mat_in[ 1]*(mat_in[11] * mat_in[14] - mat_in[10] * mat_in[15]) + 
                  mat_in[ 2]*(mat_in[ 9] * mat_in[15] - mat_in[11] * mat_in[13]) + 
                  mat_in[ 3]*(mat_in[10] * mat_in[13] - mat_in[ 9] * mat_in[14])) * DET_INV;
   mat_out[ 2] = (mat_in[13]*(mat_in[ 2] * mat_in[ 7] - mat_in[ 3] * mat_in[ 6]) + 
                  mat_in[14]*(mat_in[ 3] * mat_in[ 5] - mat_in[ 1] * mat_in[ 7]) + 
                  mat_in[15]*(mat_in[ 1] * mat_in[ 6] - mat_in[ 2] * mat_in[ 5])) * DET_INV;
   mat_out[ 3] = (mat_in[10]*(mat_in[ 1] * mat_in[ 7] - mat_in[ 3] * mat_in[ 5]) + 
                  mat_in[11]*(mat_in[ 2] * mat_in[ 5] - mat_in[ 1] * mat_in[ 6]) + 
                  mat_in[ 9]*(mat_in[ 3] * mat_in[ 6] - mat_in[ 2] * mat_in[ 7])) * DET_INV;
   mat_out[ 4] = (mat_in[ 4]*(mat_in[11] * mat_in[14] - mat_in[10] * mat_in[15]) + 
                  mat_in[ 6]*(mat_in[ 8] * mat_in[15] - mat_in[11] * mat_in[12]) + 
                  mat_in[ 7]*(mat_in[10] * mat_in[12] - mat_in[ 8] * mat_in[14])) * DET_INV;
   mat_out[ 5] = (mat_in[ 0]*(mat_in[10] * mat_in[15] - mat_in[11] * mat_in[14]) + 
                  mat_in[ 2]*(mat_in[11] * mat_in[12] - mat_in[ 8] * mat_in[15]) + 
                  mat_in[ 3]*(mat_in[ 8] * mat_in[14] - mat_in[10] * mat_in[12])) * DET_INV;
   mat_out[ 6] = (mat_in[12]*(mat_in[ 3] * mat_in[ 6] - mat_in[ 2] * mat_in[ 7]) + 
                  mat_in[14]*(mat_in[ 0] * mat_in[ 7] - mat_in[ 3] * mat_in[ 4]) + 
                  mat_in[15]*(mat_in[ 2] * mat_in[ 4] - mat_in[ 0] * mat_in[ 6])) * DET_INV;
   mat_out[ 7] = (mat_in[11]*(mat_in[ 0] * mat_in[ 6] -	mat_in[ 2] * mat_in[ 4]) + 
                  mat_in[ 8]*(mat_in[ 2] * mat_in[ 7] - mat_in[ 3] * mat_in[ 6]) + 
                  mat_in[10]*(mat_in[ 3] * mat_in[ 4] - mat_in[ 0] * mat_in[ 7])) * DET_INV;
   mat_out[ 8] = (mat_in[ 4]*(mat_in[ 9] * mat_in[15] - mat_in[11] * mat_in[13]) + 
                  mat_in[ 5]*(mat_in[11] * mat_in[12] - mat_in[ 8] * mat_in[15]) + 
                  mat_in[ 7]*(mat_in[ 8] * mat_in[13] - mat_in[ 9] * mat_in[12])) * DET_INV;
   mat_out[ 9] = (mat_in[ 0]*(mat_in[11] * mat_in[13] - mat_in[ 9] * mat_in[15]) + 
                  mat_in[ 1]*(mat_in[ 8] * mat_in[15] - mat_in[11] * mat_in[12]) + 
                  mat_in[ 3]*(mat_in[ 9] * mat_in[12] - mat_in[ 8] * mat_in[13])) * DET_INV;
   mat_out[10] = (mat_in[12]*(mat_in[ 1] * mat_in[ 7] -	mat_in[ 3] * mat_in[ 5]) + 
                  mat_in[13]*(mat_in[ 3] * mat_in[ 4] - mat_in[ 0] * mat_in[ 7]) + 
                  mat_in[15]*(mat_in[ 0] * mat_in[ 5] - mat_in[ 1] * mat_in[ 4])) * DET_INV;
   mat_out[11] = (mat_in[ 9]*(mat_in[ 0] * mat_in[ 7] - mat_in[ 3] * mat_in[ 4]) + 
                  mat_in[11]*(mat_in[ 1] * mat_in[ 4] - mat_in[ 0] * mat_in[ 5]) + 
                  mat_in[ 8]*(mat_in[ 3] * mat_in[ 5] - mat_in[ 1] * mat_in[ 7])) * DET_INV;
   mat_out[12] = (mat_in[ 4]*(mat_in[10] * mat_in[13] - mat_in[ 9] * mat_in[14]) + 
                  mat_in[ 5]*(mat_in[ 8] * mat_in[14] - mat_in[10] * mat_in[12]) + 
                  mat_in[ 6]*(mat_in[ 9] * mat_in[12] - mat_in[ 8] * mat_in[13])) * DET_INV;
   mat_out[13] = (mat_in[ 0]*(mat_in[ 9] * mat_in[14] - mat_in[10] * mat_in[13]) + 
                  mat_in[ 1]*(mat_in[10] * mat_in[12] - mat_in[ 8] * mat_in[14]) + 
                  mat_in[ 2]*(mat_in[ 8] * mat_in[13] - mat_in[ 9] * mat_in[12])) * DET_INV;
   mat_out[14] = (mat_in[12]*(mat_in[ 2] * mat_in[ 5] -	mat_in[ 1] * mat_in[ 6]) + 
                  mat_in[13]*(mat_in[ 0] * mat_in[ 6] - mat_in[ 2] * mat_in[ 4]) + 
                  mat_in[14]*(mat_in[ 1] * mat_in[ 4] - mat_in[ 0] * mat_in[ 5])) * DET_INV;
   mat_out[15] = (mat_in[10]*(mat_in[ 0] * mat_in[ 5] -	mat_in[ 1] * mat_in[ 4]) + 
                  mat_in[ 8]*(mat_in[ 1] * mat_in[ 6] - mat_in[ 2] * mat_in[ 5]) + 
                  mat_in[ 9]*(mat_in[ 2] * mat_in[ 4] - mat_in[ 0] * mat_in[ 6])) * DET_INV;

   return true;
}


//-----------------------------------------------------------------------------
// Name: pgr2MatrixInverse()
// Desc: 
//-----------------------------------------------------------------------------
void pgr2MultMatrixVertex(const GLfloat mat[16], const GLfloat ver_in[4], GLfloat ver_out[4])
{
   ver_out[0] = mat[ 0]*ver_in[0] + mat[ 4]*ver_in[1] + mat[ 8]*ver_in[2] + mat[12]*ver_in[3];
   ver_out[1] = mat[ 1]*ver_in[0] + mat[ 5]*ver_in[1] + mat[ 9]*ver_in[2] + mat[13]*ver_in[3];
   ver_out[2] = mat[ 2]*ver_in[0] + mat[ 6]*ver_in[1] + mat[10]*ver_in[2] + mat[14]*ver_in[3];
   ver_out[3] = mat[ 3]*ver_in[0] + mat[ 7]*ver_in[1] + mat[11]*ver_in[2] + mat[15]*ver_in[3];
}


//-----------------------------------------------------------------------------
// Name: pgr2MatrixInverse()
// Desc: 
//-----------------------------------------------------------------------------
void pgr2MultMatrixMatrix(const GLfloat mat_A[16], const GLfloat mat_B[16], GLfloat mat_out[16])
{
   mat_out[ 0] = mat_A[ 0] * mat_B[ 0] + mat_A[ 4] * mat_B[ 1] + mat_A[ 8] * mat_B[ 2] + mat_A[12] * mat_B[ 3];
   mat_out[ 1] = mat_A[ 1] * mat_B[ 0] + mat_A[ 5] * mat_B[ 1] + mat_A[ 9] * mat_B[ 2] + mat_A[13] * mat_B[ 3];
   mat_out[ 2] = mat_A[ 2] * mat_B[ 0] + mat_A[ 6] * mat_B[ 1] + mat_A[10] * mat_B[ 2] + mat_A[14] * mat_B[ 3];
   mat_out[ 3] = mat_A[ 3] * mat_B[ 0] + mat_A[ 7] * mat_B[ 1] + mat_A[11] * mat_B[ 2] + mat_A[15] * mat_B[ 3];
   mat_out[ 4] = mat_A[ 0] * mat_B[ 4] + mat_A[ 4] * mat_B[ 5] + mat_A[ 8] * mat_B[ 6] + mat_A[12] * mat_B[ 7];
   mat_out[ 5] = mat_A[ 1] * mat_B[ 4] + mat_A[ 5] * mat_B[ 5] + mat_A[ 9] * mat_B[ 6] + mat_A[13] * mat_B[ 7];
   mat_out[ 6] = mat_A[ 2] * mat_B[ 4] + mat_A[ 6] * mat_B[ 5] + mat_A[10] * mat_B[ 6] + mat_A[14] * mat_B[ 7];
   mat_out[ 7] = mat_A[ 3] * mat_B[ 4] + mat_A[ 7] * mat_B[ 5] + mat_A[11] * mat_B[ 6] + mat_A[15] * mat_B[ 7];
   mat_out[ 8] = mat_A[ 0] * mat_B[ 8] + mat_A[ 4] * mat_B[ 9] + mat_A[ 8] * mat_B[10] + mat_A[12] * mat_B[11];
   mat_out[ 9] = mat_A[ 1] * mat_B[ 8] + mat_A[ 5] * mat_B[ 9] + mat_A[ 9] * mat_B[10] + mat_A[13] * mat_B[11];
   mat_out[10] = mat_A[ 2] * mat_B[ 8] + mat_A[ 6] * mat_B[ 9] + mat_A[10] * mat_B[10] + mat_A[14] * mat_B[11];
   mat_out[11] = mat_A[ 3] * mat_B[ 8] + mat_A[ 7] * mat_B[ 9] + mat_A[11] * mat_B[10] + mat_A[15] * mat_B[11];
   mat_out[12] = mat_A[ 0] * mat_B[12] + mat_A[ 4] * mat_B[13] + mat_A[ 8] * mat_B[14] + mat_A[12] * mat_B[15];
   mat_out[13] = mat_A[ 1] * mat_B[12] + mat_A[ 5] * mat_B[13] + mat_A[ 9] * mat_B[14] + mat_A[13] * mat_B[15];
   mat_out[14] = mat_A[ 2] * mat_B[12] + mat_A[ 6] * mat_B[13] + mat_A[10] * mat_B[14] + mat_A[14] * mat_B[15];
   mat_out[15] = mat_A[ 3] * mat_B[12] + mat_A[ 7] * mat_B[13] + mat_A[11] * mat_B[14] + mat_A[15] * mat_B[15];
}
