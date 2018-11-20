#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <iostream>

struct Rectangle_ {
	public:
		float x;
		float y;
		float width;
		float height;

		Rectangle_();
		Rectangle_(float width, float height);
		Rectangle_(float x, float y, float width, float height);
		Rectangle_(const Rectangle_& copy);
	
		static const Rectangle_& empty();
		bool isEmpty();
	
		void set(float x, float y, float width, float height);
		void set(const Rectangle_& r);
		void setPosition(float x, float y);
	
		float left() const;
		float top() const;
		float right() const;
		float bottom() const;

		bool contains(float x, float y) const;
		bool contains(float x, float y, float width, float height) const;
		bool contains(const Rectangle_& r) const;

		bool intersects(float x, float y, float width, float height) const;
		bool intersects(const Rectangle_& r) const;

		static void combine(const Rectangle_& r1, const Rectangle_& r2, Rectangle_* dest);
		void inflate(float horizontalAmount, float verticalAmount);
	
		Rectangle_& operator = (const Rectangle_& r);
		bool operator == (const Rectangle_& r) const;
		bool operator != (const Rectangle_& r) const;

		friend std::ostream& operator<<(std::ostream& os, const Rectangle_& vec);
};

#endif