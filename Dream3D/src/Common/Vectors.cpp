#include "Common/Vectors.h"
#include "Common/Matrices.h"

///////////////////////////////////////////////////////////////////////////////
// functions for Vector2
///////////////////////////////////////////////////////////////////////////////

Vector2::Vector2() 
	: x(0), y(0)
{

}

Vector2::Vector2(float x, float y) 
	: x(x), y(y)
{

}

const Vector2& Vector2::one() {
	static Vector2 value(1.0f, 1.0f);
	return value;
}

Vector2 Vector2::operator-() const {
	return Vector2(-x, -y);
}

Vector2 Vector2::operator+(const Vector2& rhs) const {
	return Vector2(x+rhs.x, y+rhs.y);
}

Vector2 Vector2::operator-(const Vector2& rhs) const {
	return Vector2(x-rhs.x, y-rhs.y);
}

Vector2& Vector2::operator+=(const Vector2& rhs) {
	x += rhs.x; y += rhs.y; return *this;
}

Vector2& Vector2::operator-=(const Vector2& rhs) {
	x -= rhs.x; y -= rhs.y; return *this;
}

Vector2 Vector2::operator*(const float a) const {
	return Vector2(x*a, y*a);
}

Vector2 Vector2::operator*(const Vector2& rhs) const {
	return Vector2(x*rhs.x, y*rhs.y);
}

Vector2& Vector2::operator*=(const float a) {
	x *= a; y *= a; return *this;
}

Vector2& Vector2::operator*=(const Vector2& rhs) {
	x *= rhs.x; y *= rhs.y; return *this;
}

Vector2 Vector2::operator/(const float a) const {
	return Vector2(x/a, y/a);
}

Vector2& Vector2::operator/=(const float a) {
	x /= a; y /= a; return *this;
}

bool Vector2::operator==(const Vector2& rhs) const {
	return (x == rhs.x) && (y == rhs.y);
}

bool Vector2::operator!=(const Vector2& rhs) const {
	return (x != rhs.x) || (y != rhs.y);
}

bool Vector2::operator<(const Vector2& rhs) const {
	if(x < rhs.x) return true;
	if(x > rhs.x) return false;

	if(y < rhs.y) return true;
	if(y > rhs.y) return false;

	return false;
}

float Vector2::operator[](int index) const {
	return (&x)[index];
}

float& Vector2::operator[](int index) {
	return (&x)[index];
}

void Vector2::set(float x, float y) {
	this->x = x; this->y = y;
}

float Vector2::length() const {
	return sqrtf(x*x + y*y);
}

float Vector2::distance(const Vector2& vec) const {
	return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y));
}

Vector2& Vector2::normalize() {
	const float EPSILON = 0.000001f;
	float xxyy = x*x + y*y;
	if(xxyy < EPSILON)
		return *this; // do nothing if it is ~zero vector

	//float invLength = invSqrt(xxyyzz);
	float invLength = 1.0f / sqrtf(xxyy);
	x *= invLength;
	y *= invLength;

	return *this;
}

float Vector2::dot(const Vector2& rhs) const {
	return (x*rhs.x + y*rhs.y);
}

bool Vector2::equal(const Vector2& rhs, float epsilon) const {
	return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon;
}

bool Vector2::isZero() const {
	return x == 0.0f && y == 0.0f;
}

void Vector2::rotate(const Vector2& point, float angle) {

	float sinAngle = sin(angle);
	float cosAngle = cos(angle);
	
	//////////////////////////////////////////
	//	┌    ┐	  ┌					┐┌   ┐	//
	//	│ X' │ =  │ cos(0)	-sin(0)	││ X │	//
	//	│ Y' │	  │ sin(0)	 cos(0)	││ Y │	//
	//	└    ┘	  └					┘└   ┘	//
	//	X' = X * cos(0) - Y * sin(0);		//
	//	Y' = X * sin(0) + Y * cos(0);		//
	//////////////////////////////////////////

	if(point.isZero()) {
		float tempX = x;
		float tempY = y;

		x = tempX * cosAngle - tempX * sinAngle;
		y = tempX * sinAngle + tempY * cosAngle;
	}
	else {
		float tempX = x - point.x;
		float tempY = y - point.y;

		x = tempX * cosAngle - tempY * sinAngle;
		y = tempX * sinAngle + tempY * cosAngle;
		
		x += point.x;
		y += point.y;
	}
}

Vector2 operator*(const float a, const Vector2 vec) {
	return Vector2(a*vec.x, a*vec.y);
}

std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
	os << "(" << vec.x << ", " << vec.y << ")";
	return os;
}
// END OF VECTOR2 /////////////////////////////////////////////////////////////

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

const Vector3& Vector3::one() {
	static Vector3 value(1.0f, 1.0f, 1.0f);
	return value;
}

const Vector3& Vector3::zero() {
	static Vector3 value(0.0f, 0.0f, 0.0f);
	return value;
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

Vector3& Vector3::operator*=(const Matrix4& m) {
	x = x * m.m[0] + y * m.m[3] + z * m.m[6]; 
	y = x * m.m[1] + y * m.m[4] + z * m.m[7]; 
	z = x * m.m[2] + y * m.m[5] + z * m.m[8]; 
	
	return *this;
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

void Vector3::cross(const Vector3& v1, const Vector3& v2, Vector3* dst) {
	dst = &(v1.cross(v2));
}

bool Vector3::equal(const Vector3& rhs, float epsilon) const {
	return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon && fabs(z - rhs.z) < epsilon;
}

Vector3 operator*(const float a, const Vector3 vec) {
	return Vector3(a*vec.x, a*vec.y, a*vec.z);
}

bool Vector3::isZero() {
	return (x == 0.0f && y == 0.0f && z == 0.0f);
}

bool Vector3::isOne() {
	return (x == 1.0f && y == 1.0f && z == 1.0f);
}

std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return os;
}

Vector3 Lerp( const Vector3& v0, const Vector3& v1, float t) {

	Vector3 v2;
	v2 = v0 + t * (v1 - v0);

	return v2;
}

// END OF VECTOR3 /////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// functions for Vector4
///////////////////////////////////////////////////////////////////////////////

Vector4::Vector4() 
	: x(0), y(0), z(0), w(0)
{

}

Vector4::Vector4(float x, float y, float z, float w) 
	: x(x), y(y), z(z), w(w) 
{

}

Vector4::Vector4(const float* array) {
	set(array);
}

const Vector4& Vector4::one() 
{
	static Vector4 value(1.0f, 1.0f, 1.0f, 1.0f);
	return value;
}

Vector4 Vector4::operator-() const {
	return Vector4(-x, -y, -z, -w);
}

Vector4 Vector4::operator+(const Vector4& rhs) const {
	return Vector4(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w);
}

Vector4 Vector4::operator-(const Vector4& rhs) const {
	return Vector4(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w);
}

Vector4& Vector4::operator+=(const Vector4& rhs) {
	x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;
}

Vector4& Vector4::operator-=(const Vector4& rhs) {
	x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;
}

Vector4 Vector4::operator*(const float a) const {
	return Vector4(x*a, y*a, z*a, w*a);
}

Vector4 Vector4::operator*(const Vector4& rhs) const {
	return Vector4(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
}

Vector4& Vector4::operator*=(const float a) {
	x *= a; y *= a; z *= a; w *= a; return *this;
}

Vector4& Vector4::operator*=(const Vector4& rhs) {
	x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this;
}

Vector4 Vector4::operator/(const float a) const {
	return Vector4(x/a, y/a, z/a, w/a);
}

Vector4& Vector4::operator/=(const float a) {
	x /= a; y /= a; z /= a; w /= a; return *this;
}

bool Vector4::operator==(const Vector4& rhs) const {
	return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
}

bool Vector4::operator!=(const Vector4& rhs) const {
	return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
}

bool Vector4::operator<(const Vector4& rhs) const {
	if(x < rhs.x) return true;
	if(x > rhs.x) return false;

	if(y < rhs.y) return true;
	if(y > rhs.y) return false;

	if(z < rhs.z) return true;
	if(z > rhs.z) return false;

	if(w < rhs.w) return true;
	if(w > rhs.w) return false;
	return false;
}

float Vector4::operator[](int index) const {
	return (&x)[index];
}

float& Vector4::operator[](int index) {
	return (&x)[index];
}

void Vector4::set(float x, float y, float z, float w) {
	this->x = x; this->y = y; this->z = z; this->w = w;
}

void Vector4::set(const float* array) {
	this->x = array[0]; this->y = array[1]; this->z = array[2]; this->w = array[3];
}

float Vector4::length() const {
	return sqrtf(x*x + y*y + z*z + w*w);
}

float Vector4::distance(const Vector4& vec) const {
	return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y) + (vec.z-z)*(vec.z-z) + (vec.w-w)*(vec.w-w));
}

Vector4& Vector4::normalize() {
	const float EPSILON = 0.000001f;
	float xxyyzzww = x*x + y*y + z*z + w*w;
	if(xxyyzzww < EPSILON)
		return *this; // do nothing if it is ~zero vector

	//float invLength = invSqrt(xxyyzz);
	float invLength = 1.0f / sqrtf(xxyyzzww);
	x *= invLength;
	y *= invLength;
	z *= invLength;
	w *= invLength;

	return *this;
}

float Vector4::dot(const Vector4& rhs) const {
	return (x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w);
}

Vector4 Vector4::cross(const Vector4& rhs) const {
	return Vector4(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x, 1.0f);
}

bool Vector4::equal(const Vector4& rhs, float epsilon) const {
	return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon && fabs(z - rhs.z) < epsilon && fabs(w - rhs.w) < epsilon;
}

Vector4 Vector4::fromColor(unsigned int color)
{
	float components[4];
	int componentIndex = 0;
	for (int i = 3; i >= 0; --i)
	{
		int component = (color >> i*8) & 0x000000ff;

		components[componentIndex++] = static_cast<float>(component) / 255.0f;
	}

	Vector4 value(components);
	return value;
}

Vector4 operator*(const float a, const Vector4 vec) {
	return Vector4(a*vec.x, a*vec.y, a*vec.z, a*vec.w);
}

std::ostream& operator<<(std::ostream& os, const Vector4& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
	return os;
}
// END OF VECTOR4 /////////////////////////////////////////////////////////////
