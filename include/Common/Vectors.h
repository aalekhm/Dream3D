#ifndef VECTORS_H
#define VECTORS_H

#include <cmath>
#include <iostream>

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

	// utils functions
	void        set(float x, float y, float z);
	float       length() const;                         //
	float       distance(const Vector3& vec) const;     // distance between two vectors
	Vector3&    normalize();                            //
	float       dot(const Vector3& vec) const;          // dot product
	Vector3     cross(const Vector3& vec) const;        // cross product
	bool        equal(const Vector3& vec, float e) const; // compare with epsilon

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

	friend Vector3 operator*(const float a, const Vector3 vec);
	friend std::ostream& operator<<(std::ostream& os, const Vector3& vec);
};

#endif
