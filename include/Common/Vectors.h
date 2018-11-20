#ifndef VECTORS_H
#define VECTORS_H

#include <cmath>
#include <iostream>

struct Matrix4;

///////////////////////////////////////////////////////////////////////////////
// 2D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector2
{
	float x;
	float y;

	// ctors
	Vector2();
	Vector2(float x, float y);

	static const Vector2& one();

	// utils functions
	void        set(float x, float y);
	float       length() const;                         //
	float       distance(const Vector2& vec) const;     // distance between two vectors
	Vector2&    normalize();                            //
	float       dot(const Vector2& vec) const;          // dot product
	bool        equal(const Vector2& vec, float e) const; // compare with epsilon
	
	bool		isZero() const;
	void		rotate(const Vector2& point, float angle);

	// operators
	Vector2     operator-() const;                      // unary operator (negate)
	Vector2     operator+(const Vector2& rhs) const;    // add rhs
	Vector2     operator-(const Vector2& rhs) const;    // subtract rhs
	Vector2&    operator+=(const Vector2& rhs);         // add rhs and update this object
	Vector2&    operator-=(const Vector2& rhs);         // subtract rhs and update this object
	Vector2     operator*(const float scale) const;     // scale
	Vector2     operator*(const Vector2& rhs) const;    // multiplay each element
	Vector2&    operator*=(const float scale);          // scale and update this object
	Vector2&    operator*=(const Vector2& rhs);         // product each element and update this object
	Vector2     operator/(const float scale) const;     // inverse scale
	Vector2&    operator/=(const float scale);          // scale and update this object
	bool        operator==(const Vector2& rhs) const;   // exact compare, no epsilon
	bool        operator!=(const Vector2& rhs) const;   // exact compare, no epsilon
	bool        operator<(const Vector2& rhs) const;    // comparison for sort
	float       operator[](int index) const;            // subscript operator v[0], v[1]
	float&      operator[](int index);                  // subscript operator v[0], v[1]

	friend Vector2 operator*(const float a, const Vector2 vec);
	friend std::ostream& operator<<(std::ostream& os, const Vector2& vec);
};

///////////////////////////////////////////////////////////////////////////////
// 3D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector3
{
	float x;
	float y;
	float z;

	// ctors
	Vector3();
	Vector3(float x, float y, float z);

	static const Vector3& one();

	// utils functions
	void        set(float x, float y, float z);
	float       length() const;                         //
	float       distance(const Vector3& vec) const;     // distance between two vectors
	Vector3&    normalize();                            //
	float       dot(const Vector3& vec) const;          // dot product
	Vector3     cross(const Vector3& vec) const;        // cross product
	bool        equal(const Vector3& vec, float e) const; // compare with epsilon
	static void	cross(const Vector3& v1, const Vector3& v2, Vector3* dst);
	bool		isZero();
	bool		isOne();

	// operators
	Vector3     operator-() const;                      // unary operator (negate)
	Vector3     operator+(const Vector3& rhs) const;    // add rhs
	Vector3     operator-(const Vector3& rhs) const;    // subtract rhs
	Vector3&    operator+=(const Vector3& rhs);         // add rhs and update this object
	Vector3&    operator-=(const Vector3& rhs);         // subtract rhs and update this object
	Vector3     operator*(const float scale) const;     // scale
	Vector3     operator*(const Vector3& rhs) const;    // multiplay each element
	Vector3&    operator*=(const float scale);          // scale and update this object
	Vector3&    operator*=(const Vector3& rhs);         // product each element and update this object
	Vector3     operator/(const float scale) const;     // inverse scale
	Vector3&    operator/=(const float scale);          // scale and update this object
	bool        operator==(const Vector3& rhs) const;   // exact compare, no epsilon
	bool        operator!=(const Vector3& rhs) const;   // exact compare, no epsilon
	bool        operator<(const Vector3& rhs) const;    // comparison for sort
	float       operator[](int index) const;            // subscript operator v[0], v[1]
	float&      operator[](int index);                  // subscript operator v[0], v[1]
	Vector3&	operator*=(const Matrix4& m);			// 

	friend Vector3 operator*(const float a, const Vector3 vec);
	friend std::ostream& operator<<(std::ostream& os, const Vector3& vec);
};
Vector3 Lerp( const Vector3& v0, const Vector3& v1, float t);

///////////////////////////////////////////////////////////////////////////////
// 4D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector4
{
	float x;
	float y;
	float z;
	float w;

	// ctors
	Vector4();
	Vector4(float x, float y, float z, float w);
	Vector4(const float* array);

	static const Vector4&	one();
	static Vector4			fromColor(unsigned int color);

	// utils functions
	void        set(float x, float y, float z, float w);
	void        set(const float* array);

	float       length() const;                         //
	float       distance(const Vector4& vec) const;     // distance between two vectors
	Vector4&    normalize();                            //
	float       dot(const Vector4& vec) const;          // dot product
	Vector4     cross(const Vector4& vec) const;        // cross product
	bool        equal(const Vector4& vec, float e) const; // compare with epsilon

	// operators
	Vector4     operator-() const;                      // unary operator (negate)
	Vector4     operator+(const Vector4& rhs) const;    // add rhs
	Vector4     operator-(const Vector4& rhs) const;    // subtract rhs
	Vector4&    operator+=(const Vector4& rhs);         // add rhs and update this object
	Vector4&    operator-=(const Vector4& rhs);         // subtract rhs and update this object
	Vector4     operator*(const float scale) const;     // scale
	Vector4     operator*(const Vector4& rhs) const;    // multiplay each element
	Vector4&    operator*=(const float scale);          // scale and update this object
	Vector4&    operator*=(const Vector4& rhs);         // product each element and update this object
	Vector4     operator/(const float scale) const;     // inverse scale
	Vector4&    operator/=(const float scale);          // scale and update this object
	bool        operator==(const Vector4& rhs) const;   // exact compare, no epsilon
	bool        operator!=(const Vector4& rhs) const;   // exact compare, no epsilon
	bool        operator<(const Vector4& rhs) const;    // comparison for sort
	float       operator[](int index) const;            // subscript operator v[0], v[1]
	float&      operator[](int index);                  // subscript operator v[0], v[1]

	friend Vector4 operator*(const float a, const Vector4 vec);
	friend std::ostream& operator<<(std::ostream& os, const Vector4& vec);
};

#endif
