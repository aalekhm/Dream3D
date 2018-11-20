#include "Common/Rectangle.h"

Rectangle_::Rectangle_() 
	:	x(0),
		y(0),
		width(0),
		height(0)
{

}

Rectangle_::Rectangle_(float width, float height) 
	:	x(0),
		y(0)
{
	this->width = width;
	this->height = height;
}

Rectangle_::Rectangle_(float x, float y, float width, float height) 
	:	x(x),
		y(y),
		width(width),
		height(height)
{

}

Rectangle_::Rectangle_(const Rectangle_& copy) {
	set(copy);
}

const Rectangle_& Rectangle_::empty() {
	static Rectangle_ empty;
	return empty;
}

bool Rectangle_::isEmpty() {
	return (x == 0 && y == 0 && width == 0 && height == 0);
}

void Rectangle_::set(float x, float y, float width, float height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

void Rectangle_::set(const Rectangle_& r) {
	set(r.x, r.y, r.width, r.height);
}

void Rectangle_::setPosition(float x, float y) {
	this->x = x;
	this->y = y;
}

float Rectangle_::left() const {
	return x;
}

float Rectangle_::top() const {
	return y;
}

float Rectangle_::right() const {
	return x + width;
}

float Rectangle_::bottom() const {
	return y + height;
}

bool Rectangle_::contains(float x, float y) const {
	return (x >= this->x && x <= (this->x + this->width) && y >= this->y && y <= (this->y + this->height));
}

bool Rectangle_::contains(float x, float y, float width, float height) const {
	return contains(x, y) && contains(width, height);
}

bool Rectangle_::contains(const Rectangle_& r) const {
	return contains(r.x, r.y, r.width, r.height);
}

bool Rectangle_::intersects(float x, float y, float width, float height) const {
	float t;
	
	if ((t = x - this->x) > this->width || -t > width)
		return false;
	if ((t = y - this->y) > this->height || -t > height)
		return false;
	
	return true;
}

bool Rectangle_::intersects(const Rectangle_& r) const {
	return intersects(r.x, r.y, r.width, r.height);
}

void Rectangle_::combine(const Rectangle_& r1, const Rectangle_& r2, Rectangle_* dest) {
	//GP_ASSERT(dest);

	//dest->x = min(r1.x, r2.x);
	//dest->y = min(r1.y, r2.y);
	//dest->width = max(r1.x + r1.width, r2.x + r2.width) - dst->x;
	//dest->height = max(r1.y + r1.height, r2.y + r2.height) - dst->y;
}

void Rectangle_::inflate(float horizontalAmount, float verticalAmount) {
	x -= horizontalAmount;
	y -= verticalAmount;
	width += horizontalAmount * 2;
	height += verticalAmount * 2;
}

Rectangle_& Rectangle_::operator = (const Rectangle_& r) {
	x = r.x;
	y = r.y;
	width = r.width;
	height = r.height;

	return *this;
}

bool Rectangle_::operator == (const Rectangle_& r) const {
	return (x == r.x && width == r.width && y == r.y && height == r.height);
}

bool Rectangle_::operator != (const Rectangle_& r) const {
	return (x != r.x || width != r.width || y != r.y || height != r.height);
}

std::ostream& operator<<(std::ostream& os, const Rectangle_& rect) {
	os << "(" << rect.x << ", " << rect.y << ", " << rect.width << ", " << rect.height << ")";
	return os;
}
