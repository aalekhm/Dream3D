#ifndef VERTEX_H
#define VERTEX_H

#include "Engine/Base.h"

/**
 * Defines the format of a vertex layout used by a mesh.
 *
 * A VertexFormat is immutable and cannot be changed
 * once created.
 */
class VertexFormat {

	public:
		enum TYPE {
			POSITION = 1,
			NORMAL = 2,
			COLOR = 3,
			TANGENT = 4,
			BINORMAL = 5,
			BLENDWEIGHTS = 6,
			BLENDINDICES = 7,
			TEXCOORD0 = 8,
			TEXCOORD1 = 9,
			TEXCOORD2 = 10,
			TEXCOORD3 = 11,
			TEXCOORD4 = 12,
			TEXCOORD5 = 13,
			TEXCOORD6 = 14,
			TEXCOORD7 = 15
		};

		enum SIZE {
			TWO = 2,
			THREE = 3,
			FOUR = 4
		};

		/**
		 * Defines a single element within a vertex format.
		 *
		 * All vertex elements are assumed to be of type float, but can
		 * have a varying number of float values (1-4), which is represented
		 * by the size attribute. Additionally, vertex elements are assumed
		 * to be tightly packed.
		 */
		class Element {
			public:
				//The vertex element type
				TYPE type;

				//Number of values in an element
				unsigned int size;

				Element();
				Element(TYPE type, unsigned int size);
				bool operator == (Element& e) const;
				bool operator != (Element& e) const;
			private:
		};

		VertexFormat(const Element* elements, unsigned int elementCount);
		~VertexFormat();

		const Element& getElement(unsigned int index) const;
		unsigned int getElementCount() const;
		unsigned int getVertexSize() const;
		bool operator == (VertexFormat& f) const;
		bool operator != (VertexFormat& f) const;
		static const char* toString(TYPE type);
	private:
		std::vector<Element>	m_vElements;
		unsigned int			m_iVertexSize;
};

#endif