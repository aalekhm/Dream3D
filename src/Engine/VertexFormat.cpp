#include "Engine/VertexFormat.h"

VertexFormat::VertexFormat(const Element* elements, unsigned int elementCount)
	: m_iVertexSize(0)
{
	GP_ASSERT(elements);

	// Copy elements and compute vertex size
	for(unsigned int i = 0; i < elementCount; i++) {
		//copy elements
		Element element;
		memcpy(&element, &elements[i], sizeof(Element));
		m_vElements.push_back(element);

		m_iVertexSize += element.size * sizeof(float);
	}
}

VertexFormat::~VertexFormat() {

}

const VertexFormat::Element& VertexFormat::getElement(unsigned int index) const {
	GP_ASSERT(index < m_vElements.size());
	
	return m_vElements[index];
}

unsigned int VertexFormat::getElementCount() const {
	return (unsigned int)m_vElements.size();
}

unsigned int VertexFormat::getVertexSize() const {
	return m_iVertexSize;
}

bool VertexFormat::operator == (VertexFormat& f) const {
	if(m_vElements.size() != f.m_vElements.size())
		return false;

	for(int i = 0, count = m_vElements.size(); i < count; i++) {
		if(m_vElements[i] != f.m_vElements[i])
			return false;
	}

	return true;
}

bool VertexFormat::operator != (VertexFormat& f) const {
	return !(*this == f);
}

VertexFormat::Element::Element() 
	: type(POSITION), size(0)
{

}
		
VertexFormat::Element::Element(TYPE type, unsigned int size) 
	: type(type), size(size)
{

}

bool VertexFormat::Element::operator == (VertexFormat::Element& e) const {
	return (type == e.type && size == e.size);
}

bool VertexFormat::Element::operator != (VertexFormat::Element& e) const {
	return !(*this == e);
}

const char* VertexFormat::toString(TYPE type) {
	switch(type) {
		case POSITION:
			return "POSITION";
		case NORMAL:
			return "NORMAL";
		case COLOR:
			return "COLOR";
		case TANGENT:
			return "TANGENT";
		case BINORMAL:
			return "BINORMAL";
		case BLENDWEIGHTS:
			return "BLENDWEIGHTS";
		case BLENDINDICES:
			return "BLENDINDICES";
		case TEXCOORD0:
			return "TEXCOORD0";
		case TEXCOORD1:
			return "TEXCOORD1";
		case TEXCOORD2:
			return "TEXCOORD2";
		case TEXCOORD3:
			return "TEXCOORD3";
		case TEXCOORD4:
			return "TEXCOORD4";
		case TEXCOORD5:
			return "TEXCOORD5";
		case TEXCOORD6:
			return "TEXCOORD6";
		case TEXCOORD7:
			return "TEXCOORD7";
		default:
			return "UNKNOWN";
	}
}