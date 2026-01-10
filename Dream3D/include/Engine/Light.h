#ifndef LIGHT_H
#define LIGHT_H

#include <Common/Vectors.h>

class Light 
{
	friend class Node;
	friend class Properties;
	public:
		enum LightType
		{
			DIRECTIONAL = 1,
			POINT = 2,
			SPOT = 3
		};

		static Light*		createDirectionalLight(const Vector3& v3Colour);
		static Light*		createDirectionalLight(float fRed, float fGreen, float fBlue);

		static Light*		createPointLight(const Vector3& v3Colour, float fRange);
		static Light*		createPointLight(float fRed, float fGreen, float fBlue, float fRange);

		static Light*		createSpotLight(const Vector3& v3Colour, float fRange, float fInnerAngle, float fOuterAngle);
		static Light*		createSpotLight(float fRed, float fGreen, float fBlue, float fRange, float fInnerAngle, float fOuterAngle);

		static Light*		create(Properties* pProperties);

		virtual				~Light();
		Light&				operator=(const Light&);

		LightType			getLightType() const;
		const Vector3&		getLightColour() const;
		
		void				setLightColour(const Vector3& v3Colour);
		void				setLightColour(float fRed, float fGreen, float fBlue);

		Node*				getNode() const;
		void				setNode(Node* pNode);

		float				getRange() const;
		void				setRange(float fRange);
		float				getRangeInverse() const;

		void				setInnerAngle(float fInnerAngle);
		float				getInnerAngle() const;
		float				getInnerAngleCosine() const;

		void				setOuterAngle(float fOuterAngle);
		float				getOuterAngle() const;
		float				getOuterAngleCosine() const;
	private:

		class Directional
		{
			public:
				Vector3		m_v3Colour;

				Directional(const Vector3& v3Colour);
		};

		class Point
		{
			public:
				Vector3		m_v3Colour;
				float		m_fRange;
				float		m_fRangeInverse;

				Point(const Vector3& v3Colour, float fRange);
		};

		class Spot
		{
			public:
				Vector3		m_v3Colour;
				float		m_fRange;
				float		m_fRangeInverse;
				float		m_fInnerAngle;
				float		m_fInnerAngleCosine;
				float		m_fOuterAngle;
				float		m_fOuterAngleCosine;

				Spot(const Vector3& v3Colour, float fRange, float fInnerAngle, float fOuterAngle);
		};

		Light(LightType eLightType, const Vector3& v3Colour);
		Light(LightType eLightType, const Vector3& v3Colour, float fRange);
		Light(LightType eLightType, const Vector3& v3Colour, float fRange, float fInnerAngle, float fOuterAngle);

		LightType			m_eLightType;

		union
		{
			Directional*	m_pDirectional;
			Point*			m_pPoint;
			Spot*			m_pSpot;
		};

		Node*				m_pNode;
};

#endif