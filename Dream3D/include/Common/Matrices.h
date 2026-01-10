#ifndef MATRICES_H
#define MATRICES_H

#include <iostream>
#include "Common/Vectors.h"

#define MATH_TOLERANCE             2e-37f
#define MATRIX_SIZE					( sizeof(float) * 16)
const float DEG2RAD =				3.141593f / 180;

///////////////////////////////////////////////////////////////////////////
// 4x4 matrix
///////////////////////////////////////////////////////////////////////////
struct Matrix4
{
	public:
		// constructors
								Matrix4();  // init with identity
								Matrix4(const float src[16]);
								Matrix4(float xx, float xy, float xz, float xw,
										float yx, float yy, float yz, float yw,
										float zx, float zy, float zz, float zw,
										float wx, float wy, float wz, float ww);

		void					set(const float src[16]);
		void					set(float xx, float xy, float xz, float xw,
									float yx, float yy, float yz, float yw,
									float zx, float zy, float zz, float zw,
									float wx, float wy, float wz, float ww);
		void					setColumn(int index, const float row[4]);
		//void					setColumn(int index, const Vector4& v);
		void					setColumn(int index, const Vector3& v);
		void					setRow(int index, const float col[4]);
		Vector3					getColumn(int columnNo);
		Vector3					getRow(int rowNo);
		//void					setRow(int index, const Vector4& v);
		void					setRow(int index, const Vector3& v);

		const float*			get() const;
		const float*			getTranspose();										// return transposed matrix
		//float					getDeterminant();

		static Matrix4&			identity();
		Matrix4&				setIdentity();
		Matrix4&				transpose();										// transpose itself and return reference
		//Matrix4&				invert();											// check best inverse method before inverse
		//Matrix4&				invertEuclidean();									// inverse of Euclidean transform matrix
		//Matrix4&				invertAffine();										// inverse of affine transform matrix
		//Matrix4&				invertProjective();									// inverse of projective matrix using partitioning
		//Matrix4&				invertGeneral();									// inverse of generic matrix

		// transform matrix
		Matrix4&				translate(float x, float y, float z);				// translation by (x,y,z)
		Matrix4&				setTranslate(const Vector3& v);
		Matrix4&				setTranslate(float x, float y, float z);
		Matrix4&				translate(const Vector3& v);

		void					getTranslation(Vector3* translation) const;
		void					getUpVector(Vector3* dst) const;
		void					getDownVector(Vector3* dst) const;
		void					getLeftVector(Vector3* dst) const;
		void					getRightVector(Vector3* dst) const;
		void					getForwardVector(Vector3* dst) const;
		void					getBackVector(Vector3* dst) const;
		
		Matrix4&				rotate(float angle, const Vector3& axis);			// rotate angle(degree) along the given axix
		Matrix4&				rotate(float angle, float x, float y, float z);
		Matrix4&				rotate(const Vector3& vAngles);						// rotate on X,Y & Z-axis with angle in the Vector3
		Matrix4&				rotateX(float angle);								// rotate on X-axis with degree
		Matrix4&				rotateY(float angle);								// rotate on Y-axis with degree
		Matrix4&				rotateZ(float angle);								// rotate on Z-axis with degree
		
		Matrix4&				scale(float scale);									// uniform scale
		Matrix4&				scale(float sx, float sy, float sz);				// scale by (sx, sy, sz) on each axis
		Matrix4&				scale(const Vector3& v);
		Matrix4&				scaleX(float sx);
		Matrix4&				scaleY(float sy);
		Matrix4&				scaleZ(float sz);

		static void				multiply(const Matrix4& m, float scalar, Matrix4* dst);
		static void				multiply(const Matrix4& m1, const Matrix4& m2, Matrix4* dst);
		static void				multiplyMatrix(const float* m1, const float* m2, float* dst);
		static void				multiplyMatrix(const float* m, float scalar, float* dst);
		bool					invert(Matrix4* dst) const;
		
		void					transformPoint(Vector3* point) const;
		void					transformVector(Vector3* vector) const;
		void					transformVector(float x, float y, float z, float w, Vector3* dst) const;
		void					transformVector(const Vector4& vector, Vector4* dst) const;
		void					transformVector(float x, float y, float z, float w, Vector4* dst) const;
		

		// operators
		Matrix4					operator+(const Matrix4& rhs) const;			// add rhs
		Matrix4					operator-(const Matrix4& rhs) const;			// subtract rhs
		Matrix4&				operator+=(const Matrix4& rhs);					// add rhs and update this object
		Matrix4&				operator-=(const Matrix4& rhs);					// subtract rhs and update this object
		//Vector4				operator*(const Vector4& rhs) const;			// multiplication: v' = M * v
		Vector3					operator*(const Vector3& rhs) const;			// multiplication: v' = M * v
		Matrix4					operator*(const Matrix4& rhs) const;			// multiplication: M3 = M1 * M2
		Matrix4&				operator*=(const Matrix4& rhs);					// multiplication: M1' = M1 * M2
		bool					operator==(const Matrix4& rhs) const;			// exact compare, no epsilon
		bool					operator!=(const Matrix4& rhs) const;			// exact compare, no epsilon
		float					operator[](int index) const;					// subscript operator v[0], v[1]
		float&					operator[](int index);							// subscript operator v[0], v[1]

		friend Matrix4			operator-(const Matrix4& m);                     // unary operator (-)
		friend Matrix4			operator*(float scalar, const Matrix4& m);       // pre-multiplication
		friend Vector3			operator*(const Vector3& vec, const Matrix4& m); // pre-multiplication
		//friend Vector4		operator*(const Vector4& vec, const Matrix4& m); // pre-multiplication
		friend std::ostream&	operator<<(std::ostream& os, const Matrix4& m);

		float					m[16];
		float					tm[16];											// transpose m
	protected:

	private:
		float					getCofactor(float m0, float m1, float m2,
											float m3, float m4, float m5,
											float m6, float m7, float m8);
};

#endif