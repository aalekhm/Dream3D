#include "Common/Vectors.h"

///////////////////////////////////////////////////////////////////////////////
// functions for Vector3
///////////////////////////////////////////////////////////////////////////////

Vector3::Vector3() 
	: x(0), y(0), z(0) 
{

}

Vector3::Vector3(float x, float y, float z) 
	: x(x), y(y), z(z) 
{

}

Vector3 Vector3::operator-() const {
	return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator+(const Vector3& rhs) const {
	return Vector3(x+rhs.x, y+rhs.y, z+rhs.z);
}

Vector3 Vector3::operator-(const Vector3& rhs) const {
	return Vector3(x-rhs.x, y-rhs.y, z-rhs.z);
}

Vector3& Vector3::operator+=(const Vector3& rhs) {
	x += rhs.x; y += rhs.y; z += rhs.z; return *this;
}

Vector3& Vector3::operator-=(const Vector3& rhs) {
	x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this;
}

Vector3 Vector3::operator*(const float a) const {
	return Vector3(x*a, y*a, z*a);
}

Vector3 Vector3::operator*(const Vector3& rhs) const {
	return Vector3(x*rhs.x, y*rhs.y, z*rhs.z);
}

Vector3& Vector3::operator*=(const float a) {
	x *= a; y *= a; z *= a; return *this;
}

Vector3& Vector3::operator*=(const Vector3& rhs) {
	x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this;
}

Vector3 Vector3::operator/(const float a) const {
	return Vector3(x/a, y/a, z/a);
}

Vector3& Vector3::operator/=(const float a) {
	x /= a; y /= a; z /= a; return *this;
}

bool Vector3::operator==(const Vector3& rhs) const {
	return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}

bool Vector3::operator!=(const Vector3& rhs) const {
	return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
}

bool Vector3::operator<(const Vector3& rhs) const {
	if(x < rhs.x) return true;
	if(x > rhs.x) return false;
	if(y < rhs.y) return true;
	if(y > rhs.y) return false;
	if(z < rhs.z) return true;
	if(z > rhs.z) return false;
	return false;
}

float Vector3::operator[](int index) const {
	return (&x)[index];
}

float& Vector3::operator[](int index) {
	return (&x)[index];
}

void Vector3::set(float x, float y, float z) {
	this->x = x; this->y = y; this->z = z;
}

float Vector3::length() const {
	return sqrtf(x*x + y*y + z*z);
}

float Vector3::distance(const Vector3& vec) const {
	return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y) + (vec.z-z)*(vec.z-z));
}

Vector3& Vector3::normalize() {
	const float EPSILON = 0.000001f;
	float xxyyzz = x*x + y*y + z*z;
	if(xxyyzz < EPSILON)
		return *this; // do nothing if it is ~zero vector

	//float invLength = invSqrt(xxyyzz);
	float invLength = 1.0f / sqrtf(xxyyzz);
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return *this;
}

float Vector3::dot(const Vector3& rhs) const {
	return (x*rhs.x + y*rhs.y + z*rhs.z);
}

Vector3 Vector3::cross(const Vector3& rhs) const {
	return Vector3(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x);
}

bool Vector3::equal(const Vector3& rhs, float epsilon) const {
	return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon && fabs(z - rhs.z) < epsilon;
}

Vector3 operator*(const float a, const Vector3 vec) {
	return Vector3(a*vec.x, a*vec.y, a*vec.z);
}

std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return os;
}
// END OF VECTOR3 /////////////////////////////////////////////////////////////
