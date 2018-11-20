#include "Common/Matrices.h"

///////////////////////////////////////////////////////////////////////////
// functions for Matrix4
///////////////////////////////////////////////////////////////////////////
Matrix4::Matrix4()
{
	// initially identity matrix
	setIdentity();
}

Matrix4::Matrix4(const float src[16])
{
	set(src);
}

Matrix4::Matrix4(float xx, float xy, float xz, float xw,
						float yx, float yy, float yz, float yw,
						float zx, float zy, float zz, float zw,
						float wx, float wy, float wz, float ww)
{
	set(xx, xy, xz, xw,  yx, yy, yz, yw,  zx, zy, zz, zw,  wx, wy, wz, ww);
}

void Matrix4::set(const float src[16])
{
	m[0] = src[0];  m[1] = src[1];  m[2] = src[2];  m[3] = src[3];
	m[4] = src[4];  m[5] = src[5];  m[6] = src[6];  m[7] = src[7];
	m[8] = src[8];  m[9] = src[9];  m[10]= src[10]; m[11]= src[11];
	m[12]= src[12]; m[13]= src[13]; m[14]= src[14]; m[15]= src[15];
}

void Matrix4::set(float xx, float xy, float xz, float xw,
						 float yx, float yy, float yz, float yw,
						 float zx, float zy, float zz, float zw,
						 float wx, float wy, float wz, float ww)
{
	m[0] = xx;  m[1] = xy;  m[2] = xz;  m[3] = xw;
	m[4] = yx;  m[5] = yy;  m[6] = yz;  m[7] = yw;
	m[8] = zx;  m[9] = zy;  m[10]= zz;  m[11]= zw;
	m[12]= wx;  m[13]= wy;  m[14]= wz;  m[15]= ww;
}

void Matrix4::setColumn(int index, const float row[4])
{
	m[index*4 + 0] = row[0];  
	m[index*4 + 1] = row[1];  
	m[index*4 + 2] = row[2];  
	m[index*4 + 3] = row[3];
}

//void Matrix4::setColumn(int index, const Vector4& v)
//{
//	m[index*4 + 0] = v.x;  
//	m[index*4 + 1] = v.y;  
//	m[index*4 + 2] = v.z;  
//	m[index*4 + 3] = v.w;
//}

void Matrix4::setColumn(int index, const Vector3& v)
{
	m[index*4 + 0] = v.x;  
	m[index*4 + 1] = v.y;  
	m[index*4 + 2] = v.z;
}

void Matrix4::setRow(int index, const float col[4])
{
	m[index] = col[0];  m[index + 4] = col[1];  m[index + 8] = col[2];  m[index + 12] = col[3];
}

Vector3 Matrix4::getColumn(int columnNo)
{
	Vector3 retVector;
	retVector.x = m[columnNo*4];
	retVector.y = m[columnNo*4 + 1];
	retVector.z = m[columnNo*4 + 2];

	return retVector;
}

Vector3 Matrix4::getRow(int rowNo)
{
	Vector3 retVector;
	retVector.x = m[rowNo + 4+0];
	retVector.y = m[rowNo + 4*1];
	retVector.z = m[rowNo + 4*2];

	return retVector;
}

//void Matrix4::setRow(int index, const Vector4& v)
//{
//	m[index] = v.x;  m[index + 4] = v.y;  m[index + 8] = v.z;  m[index + 12] = v.w;
//}

void Matrix4::setRow(int index, const Vector3& v)
{
	m[index] = v.x;  m[index + 4] = v.y;  m[index + 8] = v.z;
}

const float* Matrix4::get() const
{
	return m;
}

const float* Matrix4::getTranspose()
{
	tm[0] = m[0];   tm[1] = m[4];   tm[2] = m[8];   tm[3] = m[12];
	tm[4] = m[1];   tm[5] = m[5];   tm[6] = m[9];   tm[7] = m[13];
	tm[8] = m[2];   tm[9] = m[6];   tm[10]= m[10];  tm[11]= m[14];
	tm[12]= m[3];   tm[13]= m[7];   tm[14]= m[11];  tm[15]= m[15];
	return tm;
}

Matrix4& Matrix4::identity()
{
	static Matrix4 m(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 );
	return m;
}

Matrix4& Matrix4::setIdentity() {

	m[0] = m[5] = m[10] = m[15] = 1.0f;
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// transpose 4x4 matrix
///////////////////////////////////////////////////////////////////////////////
Matrix4& Matrix4::transpose()
{
    std::swap(m[1],  m[4]);
    std::swap(m[2],  m[8]);
    std::swap(m[3],  m[12]);
    std::swap(m[6],  m[9]);
    std::swap(m[7],  m[13]);
    std::swap(m[11], m[14]);

    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// inverse 4x4 matrix
///////////////////////////////////////////////////////////////////////////////
//Matrix4& Matrix4::invert()
//{
//    // If the 4th row is [0,0,0,1] then it is affine matrix and
//    // it has no projective transformation.
//    if(m[12] == 0 && m[13] == 0 && m[14] == 0 && m[15] == 1)
//        this->invertAffine();
//    else
//    {
//        this->invertGeneral();
//        /*@@ invertProjective() is not optimized (slower than generic one)
//        if(fabs(m[0]*m[5] - m[1]*m[4]) > 0.00001f)
//            this->invertProjective();   // inverse using matrix partition
//        else
//            this->invertGeneral();      // generalized inverse
//        */
//    }
//
//    return *this;
//}

///////////////////////////////////////////////////////////////////////////////
// compute the inverse of 4x4 Euclidean transformation matrix
//
// Euclidean transformation is translation, rotation, and reflection.
// With Euclidean transform, only the position and orientation of the object
// will be changed. Euclidean transform does not chane the shape of an object
// (no scaling). Length and angle are reserved.
//
// Use inverseAffine() if the matrix has scale and shear transformation.
//
//  [ R | T ]-1    [ R^T | -R^T * T ]    (R denotes 3x3 rotation matrix)
//  [ --+-- ]   =  [ ----+--------- ]    (T denotes 1x3 translation)
//  [ 0 | 1 ]      [  0  |     1    ]    (R^T denotes R-transpose)
///////////////////////////////////////////////////////////////////////////////
//Matrix4& Matrix4::invertEuclidean()
//{
//    // transpose 3x3 rotation matrix part
//    // | R^T | 0 |
//    // | ----+-- |
//    // |  0  | 1 |
//    float tmp;
//    tmp = m[1];  m[1] = m[4];  m[4] = tmp;
//    tmp = m[2];  m[2] = m[8];  m[8] = tmp;
//    tmp = m[6];  m[6] = m[9];  m[9] = tmp;
//
//    // compute translation part -R^T * T
//    // | 0 | -R^T x |
//    // | --+------- |
//    // | 0 |   0    |
//    float x = m[12];
//    float y = m[13];
//    float z = m[14];
//    m[12] = -(m[0] * x + m[4] * y + m[8] * z);
//    m[12] = -(m[1] * x + m[5] * y + m[9] * z);
//    m[12] = -(m[2] * x + m[6] * y + m[10]* z);
//
//    return *this;
//}

///////////////////////////////////////////////////////////////////////////////
// compute the inverse of a 4x4 affine transformation matrix
//
// Affine transformations are generalizations of Euclidean transformations.
// Affine transformation includes translation, rotation, reflection, scaling,
// and shearing. Length and angle are NOT preserved.
// M = [ R | T ]
//     [ --+-- ]    (R denotes 3x3 rotation/scale/shear matrix)
//     [ 0 | 1 ]    (T denotes 1x3 translation matrix)
//
// y = M*x  ->  y = R*x + T  ->  x = R^-1*(y - T)  ->  x = R^-1*y - R^-1*T
//
//  [ R | T ]-1   [ R^-1 | -R^-1 * T ]
//  [ --+-- ]   = [ -----+---------- ]
//  [ 0 | 1 ]     [  0   +     1     ]
///////////////////////////////////////////////////////////////////////////////
//Matrix4& Matrix4::invertAffine()
//{
//    // R^-1
//    Matrix3 r(m[0],m[1],m[2], m[4],m[5],m[6], m[8],m[9],m[10]);
//    r.invert();
//    m[0] = r[0];  m[4] = r[3];  m[8] = r[6];
//    m[1] = r[1];  m[5] = r[4];  m[9] = r[7];
//    m[2] = r[2];  m[6] = r[5];  m[10]= r[8];
//
//    // -R^-1 * T
//    float x = m[12];
//    float y = m[13];
//    float z = m[14];
//    m[12] = -(r[0] * x + r[3] * y + r[6] * z);
//    m[13] = -(r[1] * x + r[4] * y + r[7] * z);
//    m[14] = -(r[2] * x + r[5] * y + r[8] * z);
//
//    // last row should be unchanged (0,0,0,1)
//    m[3] = m[7] = m[11] = 0.0f;
//    m[15] = 1.0f;
//
//    return * this;
//}

///////////////////////////////////////////////////////////////////////////////
// inverse matrix using matrix partitioning (blockwise inverse)
// It devides a 4x4 matrix into 4 of 2x2 matrices. It works in case of where
// det(A) != 0. If not, use the generic inverse method
// inverse formula.
// M = [ A | B ]    A, B, C, D are 2x2 matrix blocks
//     [ --+-- ]    det(M) = |A| * |D - ((C * A^-1) * B)|
//     [ C | D ]
//
// M^-1 = [ A' | B' ]   A' = A^-1 - (A^-1 * B) * C'
//        [ ---+--- ]   B' = (A^-1 * B) * -D'
//        [ C' | D' ]   C' = -D' * (C * A^-1)
//                      D' = (D - ((C * A^-1) * B))^-1
//
// NOTE: I wrap with () if it it used more than once.
//       The matrix is invertable even if det(A)=0, so must check det(A) before
//       calling this function, and use invertGeneric() instead.
///////////////////////////////////////////////////////////////////////////////
//Matrix4& Matrix4::invertProjective()
//{
//    // partition
//    Matrix2 a(m[0], m[1], m[4], m[5]);
//    Matrix2 b(m[8], m[9], m[12], m[13]);
//    Matrix2 c(m[2], m[3], m[6], m[7]);
//    Matrix2 d(m[10], m[11], m[14], m[15]);
//
//    // pre-compute repeated parts
//    a.invert();             // A^-1
//    Matrix2 ab = a * b;     // A^-1 * B
//    Matrix2 ca = c * a;     // C * A^-1
//    Matrix2 cab = ca * b;   // C * A^-1 * B
//    Matrix2 dcab = d - cab; // D - C * A^-1 * B
//
//    // check determinant if |D - C * A^-1 * B| = 0
//    //NOTE: this function assumes det(A) is already checked. if |A|=0 then,
//    //      cannot use this function.
//    float determinant = dcab[0] * dcab[3] - dcab[1] * dcab[2];
//    if(fabs(determinant) <= 0.00001f)
//    {
//        return identity();
//    }
//
//    // compute D' and -D'
//    Matrix2 d1 = dcab;      //  (D - C * A^-1 * B)
//    d1.invert();            //  (D - C * A^-1 * B)^-1
//    Matrix2 d2 = -d1;       // -(D - C * A^-1 * B)^-1
//
//    // compute C'
//    Matrix2 c1 = d2 * ca;   // -D' * (C * A^-1)
//
//    // compute B'
//    Matrix2 b1 = ab * d2;   // (A^-1 * B) * -D'
//
//    // compute A'
//    Matrix2 a1 = a - (ab * c1); // A^-1 - (A^-1 * B) * C'
//
//    // assemble inverse matrix
//    m[0] = a1[0];  m[4] = a1[2];  m[8] = b1[0];  m[12] = b1[2];
//    m[1] = a1[1];  m[5] = a1[3];  m[9] = b1[1];  m[13] = b1[3];
//    m[2] = c1[0];  m[6] = c1[2];  m[10]= d1[0];  m[14] = d1[2];
//    m[3] = c1[1];  m[7] = c1[3];  m[11]= d1[1];  m[15] = d1[3];
//
//    return *this;
//}

///////////////////////////////////////////////////////////////////////////////
// compute the inverse of a general 4x4 matrix
// If cannot find inverse, set indentity matrix
// M^-1 = adj(M) / det(M)
///////////////////////////////////////////////////////////////////////////////
//Matrix4& Matrix4::invertGeneral()
//{
//    // get cofactors of minor matrices
//    float cofactor0 = getCofactor(m[5],m[6],m[7], m[9],m[10],m[11], m[13],m[14],m[15]);
//    float cofactor1 = getCofactor(m[4],m[6],m[7], m[8],m[10],m[11], m[12],m[14],m[15]);
//    float cofactor2 = getCofactor(m[4],m[5],m[7], m[8],m[9], m[11], m[12],m[13],m[15]);
//    float cofactor3 = getCofactor(m[4],m[5],m[6], m[8],m[9], m[10], m[12],m[13],m[14]);
//
//    // get determinant
//    float determinant = m[0] * cofactor0 - m[1] * cofactor1 + m[2] * cofactor2 - m[3] * cofactor3;
//    if(fabs(determinant) <= 0.00001f)
//    {
//        return identity();
//    }
//
//    // get rest of cofactors for adj(M)
//    float cofactor4 = getCofactor(m[1],m[2],m[3], m[9],m[10],m[11], m[13],m[14],m[15]);
//    float cofactor5 = getCofactor(m[0],m[2],m[3], m[8],m[10],m[11], m[12],m[14],m[15]);
//    float cofactor6 = getCofactor(m[0],m[1],m[3], m[8],m[9], m[11], m[12],m[13],m[15]);
//    float cofactor7 = getCofactor(m[0],m[1],m[2], m[8],m[9], m[10], m[12],m[13],m[14]);
//
//    float cofactor8 = getCofactor(m[1],m[2],m[3], m[5],m[6], m[7],  m[13],m[14],m[15]);
//    float cofactor9 = getCofactor(m[0],m[2],m[3], m[4],m[6], m[7],  m[12],m[14],m[15]);
//    float cofactor10= getCofactor(m[0],m[1],m[3], m[4],m[5], m[7],  m[12],m[13],m[15]);
//    float cofactor11= getCofactor(m[0],m[1],m[2], m[4],m[5], m[6],  m[12],m[13],m[14]);
//
//    float cofactor12= getCofactor(m[1],m[2],m[3], m[5],m[6], m[7],  m[9], m[10],m[11]);
//    float cofactor13= getCofactor(m[0],m[2],m[3], m[4],m[6], m[7],  m[8], m[10],m[11]);
//    float cofactor14= getCofactor(m[0],m[1],m[3], m[4],m[5], m[7],  m[8], m[9], m[11]);
//    float cofactor15= getCofactor(m[0],m[1],m[2], m[4],m[5], m[6],  m[8], m[9], m[10]);
//
//    // build inverse matrix = adj(M) / det(M)
//    // adjugate of M is the transpose of the cofactor matrix of M
//    float invDeterminant = 1.0f / determinant;
//    m[0] =  invDeterminant * cofactor0;
//    m[1] = -invDeterminant * cofactor4;
//    m[2] =  invDeterminant * cofactor8;
//    m[3] = -invDeterminant * cofactor12;
//
//    m[4] = -invDeterminant * cofactor1;
//    m[5] =  invDeterminant * cofactor5;
//    m[6] = -invDeterminant * cofactor9;
//    m[7] =  invDeterminant * cofactor13;
//
//    m[8] =  invDeterminant * cofactor2;
//    m[9] = -invDeterminant * cofactor6;
//    m[10]=  invDeterminant * cofactor10;
//    m[11]= -invDeterminant * cofactor14;
//
//    m[12]= -invDeterminant * cofactor3;
//    m[13]=  invDeterminant * cofactor7;
//    m[14]= -invDeterminant * cofactor11;
//    m[15]=  invDeterminant * cofactor15;
//
//    return *this;
//}

///////////////////////////////////////////////////////////////////////////////
// return determinant of 4x4 matrix
///////////////////////////////////////////////////////////////////////////////
//float Matrix4::getDeterminant()
//{
//    return m[0] * getCofactor(m[5],m[6],m[7], m[9],m[10],m[11], m[13],m[14],m[15]) -
//           m[1] * getCofactor(m[4],m[6],m[7], m[8],m[10],m[11], m[12],m[14],m[15]) +
//           m[2] * getCofactor(m[4],m[5],m[7], m[8],m[9], m[11], m[12],m[13],m[15]) -
//           m[3] * getCofactor(m[4],m[5],m[6], m[8],m[9], m[10], m[12],m[13],m[14]);
//}

///////////////////////////////////////////////////////////////////////////////
// compute cofactor of 3x3 minor matrix without sign
// input params are 9 elements of the minor matrix
// NOTE: The caller must know its sign.
///////////////////////////////////////////////////////////////////////////////
float Matrix4::getCofactor(float m0, float m1, float m2,
                           float m3, float m4, float m5,
                           float m6, float m7, float m8)
{
    return m0 * (m4 * m8 - m5 * m7) -
           m1 * (m3 * m8 - m5 * m6) +
           m2 * (m3 * m7 - m4 * m6);
}



///////////////////////////////////////////////////////////////////////////////
// translate this matrix by (x, y, z)
///////////////////////////////////////////////////////////////////////////////
Matrix4& Matrix4::translate(const Vector3& v)
{
    return translate(v.x, v.y, v.z);
}

Matrix4& Matrix4::translate(float x, float y, float z)
{
    m[0] += m[12]*x;   m[1] += m[13]*x;   m[2] += m[14]*x;   m[3] += m[15]*x;
    m[4] += m[12]*y;   m[5] += m[13]*y;   m[6] += m[14]*y;   m[7] += m[15]*y;
    m[8] += m[12]*z;   m[9] += m[13]*z;   m[10]+= m[14]*z;   m[11]+= m[15]*z;
	
	return *this;
}

Matrix4& Matrix4::setTranslate(const Vector3& v) {
	return setTranslate(v.x, v.y, v.z);
}

Matrix4& Matrix4::setTranslate(float x, float y, float z)
{
    m[3] = x;
    m[7] = y;
    m[11]= z;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// uniform scale
///////////////////////////////////////////////////////////////////////////////
Matrix4& Matrix4::scale(float s)
{
    return scale(s, s, s);
}

Matrix4& Matrix4::scale(float x, float y, float z)
{
    m[0] = m[0]*x;   m[1] = m[1]*x;   m[2] = m[2]*x;   m[3] = m[3]*x;
    m[4] = m[4]*y;   m[5] = m[5]*y;   m[6] = m[6]*y;   m[7] = m[7]*y;
    m[8] = m[8]*z;   m[9] = m[9]*z;   m[10]= m[10]*z;  m[11]= m[11]*z;
    
	return *this;
}

Matrix4& Matrix4::scale(const Vector3& v) {
	scaleX(v.x);
	scaleY(v.y);
	scaleZ(v.z);

	return *this;
}

Matrix4& Matrix4::scaleX(float sx) {
	m[0] = m[0]*sx;   m[1] = m[1]*sx;   m[2] = m[2]*sx;   m[3] = m[3]*sx;

	return *this;
}

Matrix4& Matrix4::scaleY(float sy) {
	m[4] = m[4]*sy;   m[5] = m[5]*sy;   m[6] = m[6]*sy;   m[7] = m[7]*sy;

	return *this;
}

Matrix4& Matrix4::scaleZ(float sz) {
	m[8] = m[8]*sz;   m[9] = m[9]*sz;   m[10]= m[10]*sz;  m[11]= m[11]*sz;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// build a rotation matrix with given angle(degree) and rotation axis, then
// multiply it with this object
///////////////////////////////////////////////////////////////////////////////
Matrix4& Matrix4::rotate(float angle, const Vector3& axis)
{
    return rotate(angle, axis.x, axis.y, axis.z);
}

Matrix4& Matrix4::rotate(float angle, float x, float y, float z)
{
    float c = cosf(angle * DEG2RAD);    // cosine
    float s = sinf(angle * DEG2RAD);    // sine
    float xx = x * x;
    float xy = x * y;
    float xz = x * z;
    float yy = y * y;
    float yz = y * z;
    float zz = z * z;

    // build rotation matrix
    Matrix4 m;
    m[0] = xx * (1 - c) + c;
    m[1] = xy * (1 - c) - z * s;
    m[2] = xz * (1 - c) + y * s;
    m[3] = 0;
    m[4] = xy * (1 - c) + z * s;
    m[5] = yy * (1 - c) + c;
    m[6] = yz * (1 - c) - x * s;
    m[7] = 0;
    m[8] = xz * (1 - c) - y * s;
    m[9] = yz * (1 - c) + x * s;
    m[10]= zz * (1 - c) + c;
    m[11]= 0;
    m[12]= 0;
    m[13]= 0;
    m[14]= 0;
    m[15]= 1;

    // multiply it
    *this = m * (*this);

    return *this;
}

Matrix4& Matrix4::rotate(const Vector3& vAngles)
{
	rotateZ(vAngles.z);	// roll
	rotateY(vAngles.y);	// heading
	rotateX(vAngles.x);	// pitch

	return *this;
}

Matrix4& Matrix4::rotateX(float angle)
{
    float c = cosf(angle * DEG2RAD);
    float s = sinf(angle * DEG2RAD);
    float m4 = m[4], m5 = m[5], m6 = m[6],  m7 = m[7],		
          m8 = m[8], m9 = m[9], m10= m[10], m11= m[11];

    m[4] = m4 * c + m8 *-s;
    m[5] = m5 * c + m9 *-s;
    m[6] = m6 * c + m10*-s;
    m[7] = m7 * c + m11*-s;
    m[8] = m4 * s + m8 * c;
    m[9] = m5 * s + m9 * c;
    m[10]= m6 * s + m10* c;
    m[11]= m7 * s + m11* c;

    return *this;
}

Matrix4& Matrix4::rotateY(float angle)
{
    float c = cosf(angle * DEG2RAD);
    float s = sinf(angle * DEG2RAD);
    float m0 = m[0], m1 = m[1], m2 = m[2],  m3 = m[3],
          m8 = m[8], m9 = m[9], m10= m[10], m11= m[11];

    m[0] = m0 * c + m8 * s;
    m[1] = m1 * c + m9 * s;
    m[2] = m2 * c + m10* s;
    m[3] = m3 * c + m11* s;
    m[8] = m0 *-s + m8 * c;
    m[9] = m1 *-s + m9 * c;
    m[10]= m2 *-s + m10* c;
    m[11]= m3 *-s + m11* c;

    return *this;
}

Matrix4& Matrix4::rotateZ(float angle)
{
    float c = cosf(angle * DEG2RAD);
    float s = sinf(angle * DEG2RAD);
    float m0 = m[0], m1 = m[1], m2 = m[2],  m3 = m[3],
          m4 = m[4], m5 = m[5], m6 = m[6],  m7 = m[7];

    m[0] = m0 * c + m4 *-s;
    m[1] = m1 * c + m5 *-s;
    m[2] = m2 * c + m6 *-s;
    m[3] = m3 * c + m7 *-s;
    m[4] = m0 * s + m4 * c;
    m[5] = m1 * s + m5 * c;
    m[6] = m2 * s + m6 * c;
    m[7] = m3 * s + m7 * c;

    return *this;
}

void Matrix4::multiply(const Matrix4& m1, const Matrix4& m2, Matrix4* dst) {

	if( dst != NULL) {
		multiplyMatrix(m1.m, m2.m, dst->m);
	}
}

void Matrix4::multiplyMatrix(const float* m1, const float* m2, float* dst) {

	// Support the case where m1 or m2 is the same array as dst.
	float product[16];

	product[0]  = m1[0] * m2[0]  + m1[4] * m2[1] + m1[8]   * m2[2]  + m1[12] * m2[3];
	product[1]  = m1[1] * m2[0]  + m1[5] * m2[1] + m1[9]   * m2[2]  + m1[13] * m2[3];
	product[2]  = m1[2] * m2[0]  + m1[6] * m2[1] + m1[10]  * m2[2]  + m1[14] * m2[3];
	product[3]  = m1[3] * m2[0]  + m1[7] * m2[1] + m1[11]  * m2[2]  + m1[15] * m2[3];

	product[4]  = m1[0] * m2[4]  + m1[4] * m2[5] + m1[8]   * m2[6]  + m1[12] * m2[7];
	product[5]  = m1[1] * m2[4]  + m1[5] * m2[5] + m1[9]   * m2[6]  + m1[13] * m2[7];
	product[6]  = m1[2] * m2[4]  + m1[6] * m2[5] + m1[10]  * m2[6]  + m1[14] * m2[7];
	product[7]  = m1[3] * m2[4]  + m1[7] * m2[5] + m1[11]  * m2[6]  + m1[15] * m2[7];

	product[8]  = m1[0] * m2[8]  + m1[4] * m2[9] + m1[8]   * m2[10] + m1[12] * m2[11];
	product[9]  = m1[1] * m2[8]  + m1[5] * m2[9] + m1[9]   * m2[10] + m1[13] * m2[11];
	product[10] = m1[2] * m2[8]  + m1[6] * m2[9] + m1[10]  * m2[10] + m1[14] * m2[11];
	product[11] = m1[3] * m2[8]  + m1[7] * m2[9] + m1[11]  * m2[10] + m1[15] * m2[11];

	product[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8]  * m2[14] + m1[12] * m2[15];
	product[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9]  * m2[14] + m1[13] * m2[15];
	product[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
	product[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];

	memcpy(dst, product, MATRIX_SIZE);
}

bool Matrix4::invert(Matrix4* dst) const {
	float a0 = m[0] * m[5] - m[1] * m[4];
	float a1 = m[0] * m[6] - m[2] * m[4];
	float a2 = m[0] * m[7] - m[3] * m[4];
	float a3 = m[1] * m[6] - m[2] * m[5];
	float a4 = m[1] * m[7] - m[3] * m[5];
	float a5 = m[2] * m[7] - m[3] * m[6];
	float b0 = m[8] * m[13] - m[9] * m[12];
	float b1 = m[8] * m[14] - m[10] * m[12];
	float b2 = m[8] * m[15] - m[11] * m[12];
	float b3 = m[9] * m[14] - m[10] * m[13];
	float b4 = m[9] * m[15] - m[11] * m[13];
	float b5 = m[10] * m[15] - m[11] * m[14];

	// Calculate the determinant.
	float det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

	// Close to zero, can't invert.
	if (fabs(det) <= MATH_TOLERANCE)
		return false;

	// Support the case where m == dst.
	Matrix4 inverse;
	inverse.m[0]  = m[5] * b5 - m[6] * b4 + m[7] * b3;
	inverse.m[1]  = -m[1] * b5 + m[2] * b4 - m[3] * b3;
	inverse.m[2]  = m[13] * a5 - m[14] * a4 + m[15] * a3;
	inverse.m[3]  = -m[9] * a5 + m[10] * a4 - m[11] * a3;

	inverse.m[4]  = -m[4] * b5 + m[6] * b2 - m[7] * b1;
	inverse.m[5]  = m[0] * b5 - m[2] * b2 + m[3] * b1;
	inverse.m[6]  = -m[12] * a5 + m[14] * a2 - m[15] * a1;
	inverse.m[7]  = m[8] * a5 - m[10] * a2 + m[11] * a1;

	inverse.m[8]  = m[4] * b4 - m[5] * b2 + m[7] * b0;
	inverse.m[9]  = -m[0] * b4 + m[1] * b2 - m[3] * b0;
	inverse.m[10] = m[12] * a4 - m[13] * a2 + m[15] * a0;
	inverse.m[11] = -m[8] * a4 + m[9] * a2 - m[11] * a0;

	inverse.m[12] = -m[4] * b3 + m[5] * b1 - m[6] * b0;
	inverse.m[13] = m[0] * b3 - m[1] * b1 + m[2] * b0;
	inverse.m[14] = -m[12] * a3 + m[13] * a1 - m[14] * a0;
	inverse.m[15] = m[8] * a3 - m[9] * a1 + m[10] * a0;

	Matrix4::multiply(inverse, 1.0f / det, dst);

	return true;
}

void Matrix4::multiply(const Matrix4& m, float scalar, Matrix4* dst) {
	if(dst != NULL) {
		Matrix4::multiplyMatrix(m.m, scalar, dst->m);
	}
}

void Matrix4::multiplyMatrix(const float* m, float scalar, float* dst) {
	dst[0]  = m[0]  * scalar;
	dst[1]  = m[1]  * scalar;
	dst[2]  = m[2]  * scalar;
	dst[3]  = m[3]  * scalar;
	dst[4]  = m[4]  * scalar;
	dst[5]  = m[5]  * scalar;
	dst[6]  = m[6]  * scalar;
	dst[7]  = m[7]  * scalar;
	dst[8]  = m[8]  * scalar;
	dst[9]  = m[9]  * scalar;
	dst[10] = m[10] * scalar;
	dst[11] = m[11] * scalar;
	dst[12] = m[12] * scalar;
	dst[13] = m[13] * scalar;
	dst[14] = m[14] * scalar;
	dst[15] = m[15] * scalar;
}

Matrix4 Matrix4::operator+(const Matrix4& rhs) const
{
	return Matrix4(m[0]+rhs[0],   m[1]+rhs[1],   m[2]+rhs[2],   m[3]+rhs[3],
		m[4]+rhs[4],   m[5]+rhs[5],   m[6]+rhs[6],   m[7]+rhs[7],
		m[8]+rhs[8],   m[9]+rhs[9],   m[10]+rhs[10], m[11]+rhs[11],
		m[12]+rhs[12], m[13]+rhs[13], m[14]+rhs[14], m[15]+rhs[15]);
}

Matrix4 Matrix4::operator-(const Matrix4& rhs) const
{
	return Matrix4(m[0]-rhs[0],   m[1]-rhs[1],   m[2]-rhs[2],   m[3]-rhs[3],
		m[4]-rhs[4],   m[5]-rhs[5],   m[6]-rhs[6],   m[7]-rhs[7],
		m[8]-rhs[8],   m[9]-rhs[9],   m[10]-rhs[10], m[11]-rhs[11],
		m[12]-rhs[12], m[13]-rhs[13], m[14]-rhs[14], m[15]-rhs[15]);
}

Matrix4& Matrix4::operator+=(const Matrix4& rhs)
{
	m[0] += rhs[0];    m[1] += rhs[1];    m[2] += rhs[2];    m[3] += rhs[3];
	m[4] += rhs[4];    m[5] += rhs[5];    m[6] += rhs[6];    m[7] += rhs[7];
	m[8] += rhs[8];    m[9] += rhs[9];    m[10] += rhs[10];  m[11] += rhs[11];
	m[12] += rhs[12];  m[13] += rhs[13];  m[14] += rhs[14];  m[15] += rhs[15];
	return *this;
}

Matrix4& Matrix4::operator-=(const Matrix4& rhs)
{
	m[0] -= rhs[0];    m[1] -= rhs[1];    m[2] -= rhs[2];    m[3] -= rhs[3];
	m[4] -= rhs[4];    m[5] -= rhs[5];    m[6] -= rhs[6];    m[7] -= rhs[7];
	m[8] -= rhs[8];    m[9] -= rhs[9];    m[10] -= rhs[10];  m[11] -= rhs[11];
	m[12] -= rhs[12];  m[13] -= rhs[13];  m[14] -= rhs[14];  m[15] -= rhs[15];
	return *this;
}

//Vector4 Matrix4::operator*(const Vector4& rhs) const
//{
//	return Vector4(m[0]*rhs.x  + m[1]*rhs.y  + m[2]*rhs.z  + m[3]*rhs.w,
//		m[4]*rhs.x  + m[5]*rhs.y  + m[6]*rhs.z  + m[7]*rhs.w,
//		m[8]*rhs.x  + m[9]*rhs.y  + m[10]*rhs.z + m[11]*rhs.w,
//		m[12]*rhs.x + m[13]*rhs.y + m[14]*rhs.z + m[15]*rhs.w);
//}

Vector3 Matrix4::operator*(const Vector3& rhs) const
{
	return Vector3(m[0]*rhs.x + m[1]*rhs.y + m[2]*rhs.z,
		m[4]*rhs.x + m[5]*rhs.y + m[6]*rhs.z,
		m[8]*rhs.x + m[9]*rhs.y + m[10]*rhs.z);
}

Matrix4 Matrix4::operator*(const Matrix4& n) const
{
	return Matrix4(m[0]*n[0]  + m[1]*n[4]  + m[2]*n[8]  + m[3]*n[12],   m[0]*n[1]  + m[1]*n[5]  + m[2]*n[9]  + m[3]*n[13],   m[0]*n[2]  + m[1]*n[6]  + m[2]*n[10]  + m[3]*n[14],   m[0]*n[3]  + m[1]*n[7]  + m[2]*n[11]  + m[3]*n[15],
		m[4]*n[0]  + m[5]*n[4]  + m[6]*n[8]  + m[7]*n[12],   m[4]*n[1]  + m[5]*n[5]  + m[6]*n[9]  + m[7]*n[13],   m[4]*n[2]  + m[5]*n[6]  + m[6]*n[10]  + m[7]*n[14],   m[4]*n[3]  + m[5]*n[7]  + m[6]*n[11]  + m[7]*n[15],
		m[8]*n[0]  + m[9]*n[4]  + m[10]*n[8] + m[11]*n[12],  m[8]*n[1]  + m[9]*n[5]  + m[10]*n[9] + m[11]*n[13],  m[8]*n[2]  + m[9]*n[6]  + m[10]*n[10] + m[11]*n[14],  m[8]*n[3]  + m[9]*n[7]  + m[10]*n[11] + m[11]*n[15],
		m[12]*n[0] + m[13]*n[4] + m[14]*n[8] + m[15]*n[12],  m[12]*n[1] + m[13]*n[5] + m[14]*n[9] + m[15]*n[13],  m[12]*n[2] + m[13]*n[6] + m[14]*n[10] + m[15]*n[14],  m[12]*n[3] + m[13]*n[7] + m[14]*n[11] + m[15]*n[15]);
}

Matrix4& Matrix4::operator*=(const Matrix4& rhs)
{
	*this = *this * rhs;
	return *this;
}

bool Matrix4::operator==(const Matrix4& n) const
{
	return (m[0] == n[0])   && (m[1] == n[1])   && (m[2] == n[2])   && (m[3] == n[3]) &&
		(m[4] == n[4])   && (m[5] == n[5])   && (m[6] == n[6])   && (m[7] == n[7]) &&
		(m[8] == n[8])   && (m[9] == n[9])   && (m[10] == n[10]) && (m[11] == n[11]) &&
		(m[12] == n[12]) && (m[13] == n[13]) && (m[14] == n[14]) && (m[15] == n[15]);
}

bool Matrix4::operator!=(const Matrix4& n) const
{
	return (m[0] != n[0])   || (m[1] != n[1])   || (m[2] != n[2])   || (m[3] != n[3]) ||
		(m[4] != n[4])   || (m[5] != n[5])   || (m[6] != n[6])   || (m[7] != n[7]) ||
		(m[8] != n[8])   || (m[9] != n[9])   || (m[10] != n[10]) || (m[11] != n[11]) ||
		(m[12] != n[12]) || (m[13] != n[13]) || (m[14] != n[14]) || (m[15] != n[15]);
}

float Matrix4::operator[](int index) const
{
	return m[index];
}

float& Matrix4::operator[](int index)
{
	return m[index];
}

Matrix4 operator-(const Matrix4& rhs)
{
	return Matrix4(-rhs[0], -rhs[1], -rhs[2], -rhs[3], -rhs[4], -rhs[5], -rhs[6], -rhs[7], -rhs[8], -rhs[9], -rhs[10], -rhs[11], -rhs[12], -rhs[13], -rhs[14], -rhs[15]);
}

Matrix4 operator*(float s, const Matrix4& rhs)
{
	return Matrix4(s*rhs[0], s*rhs[1], s*rhs[2], s*rhs[3], s*rhs[4], s*rhs[5], s*rhs[6], s*rhs[7], s*rhs[8], s*rhs[9], s*rhs[10], s*rhs[11], s*rhs[12], s*rhs[13], s*rhs[14], s*rhs[15]);
}

//Vector4 operator*(const Vector4& v, const Matrix4& m)
//{
//	return Vector4(v.x*m[0] + v.y*m[4] + v.z*m[8] + v.w*m[12],  v.x*m[1] + v.y*m[5] + v.z*m[9] + v.w*m[13],  v.x*m[2] + v.y*m[6] + v.z*m[10] + v.w*m[14], v.x*m[3] + v.y*m[7] + v.z*m[11] + v.w*m[15]);
//}

Vector3 operator*(const Vector3& v, const Matrix4& m)
{
	return Vector3(v.x*m[0] + v.y*m[4] + v.z*m[8],  v.x*m[1] + v.y*m[5] + v.z*m[9],  v.x*m[2] + v.y*m[6] + v.z*m[10]);
}

std::ostream& operator<<(std::ostream& os, const Matrix4& m)
{
	os << "(" << m[0]  << ",\t" << m[1]  << ",\t" << m[2]  <<  ",\t" << m[3] << ")\n"
		<< "(" << m[4]  << ",\t" << m[5]  << ",\t" << m[6]  <<  ",\t" << m[7] << ")\n"
		<< "(" << m[8]  << ",\t" << m[9]  << ",\t" << m[10] <<  ",\t" << m[11] << ")\n"
		<< "(" << m[12] << ",\t" << m[13] << ",\t" << m[14] <<  ",\t" << m[15] << ")\n";
	return os;
}
// END OF MATRIX4 INLINE //////////////////////////////////////////////////////