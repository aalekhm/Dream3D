#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <iostream>

struct Rectangle {
	public:
		float x;
		float y;
		float width;
		float height;

		Rectangle();
		Rectangle(float width, float height);
		Rectangle(float x, float y, float width, float height);
		Rectangle(const Rectangle& copy);
	
		static const Rectangle& empty();
		bool isEmpty();
	
		void set(float x, float y, float width, float height);
		void set(const Rectangle& r);
		void setPosition(float x, float y);
	
		float left() const;
		float top() const;
		float right() const;
		float bottom() const;

		bool contains(float x, float y) const;
		bool contains(float x, float y, float width, float height) const;
		bool contains(const Rectangle& r) const;

		bool intersects(float x, float y, float width, float height) const;
		bool intersects(const Rectangle& r) const;

		static void combine(const Rectangle& r1, const Rectangle& r2, Rectangle* dest);
		void inflate(float horizontalAmount, float verticalAmount);
	
		Rectangle& operator = (const Rectangle& r);
		bool operator == (const Rectangle& r) const;
		bool operator != (const Rectangle& r) const;

		friend std::ostream& operator<<(std::ostream& os, const Rectangle& vec);
};

#endif