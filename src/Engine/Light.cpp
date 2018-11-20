#include <Engine/Light.h>
#include <Engine/Base.h>
#include <Engine/Properties.h>

Light::Light(LightType eLightType, const Vector3& v3Colour)
: m_eLightType(eLightType)
, m_pNode(NULL)
{
	m_pDirectional = new Directional(v3Colour);
}

Light::Light(LightType eLightType, const Vector3& v3Colour, float fRange)
: m_eLightType(eLightType)
, m_pNode(NULL)
{
	m_pPoint = new Point(v3Colour, fRange);
}

Light::Light(LightType eLightType, const Vector3& v3Colour, float fRange, float fInnerAngle, float fOuterAngle)
: m_eLightType(eLightType)
, m_pNode(NULL)
{
	m_pSpot = new Spot(v3Colour, fRange, fInnerAngle, fOuterAngle);
}

Light::~Light()
{
	switch (m_eLightType)
	{
		case DIRECTIONAL:
			SAFE_DELETE( m_pDirectional );
		break;
		case POINT:
			SAFE_DELETE( m_pPoint );
		break;
		case SPOT:
			SAFE_DELETE( m_pSpot );
		break;
	}
}

Light* Light::createDirectionalLight(const Vector3& v3Colour)
{
	return new Light(DIRECTIONAL, v3Colour);
}

Light* Light::createDirectionalLight(float fRed, float fGreen, float fBlue)
{
	return new Light(DIRECTIONAL, Vector3(fRed, fGreen, fBlue));
}

Light* Light::createPointLight(const Vector3& v3Colour, float fRange)
{
	return new Light(POINT, v3Colour, fRange);
}

Light* Light::createPointLight(float fRed, float fGreen, float fBlue, float fRange)
{
	return new Light(POINT, Vector3(fRed, fGreen, fBlue), fRange);
}

Light* Light::createSpotLight(const Vector3& v3Colour, float fRange, float fInnerAngle, float fOuterAngle)
{
	return new Light(SPOT, v3Colour, fRange, fInnerAngle, fOuterAngle);
}

Light* Light::createSpotLight(float fRed, float fGreen, float fBlue, float fRange, float fInnerAngle, float fOuterAngle)
{
	return new Light(POINT, Vector3(fRed, fGreen, fBlue), fRange, fInnerAngle, fOuterAngle);
}

Light* Light::create(Properties* pProperties)
{
	// TODO
	return NULL;
}

Light::LightType Light::getLightType() const
{
	return m_eLightType;
}

const Vector3& Light::getLightColour() const
{
	switch (m_eLightType)
	{
		case Light::DIRECTIONAL:
			return m_pDirectional->m_v3Colour;
		break;
		case Light::POINT:
			return m_pPoint->m_v3Colour;
		break;
		case Light::SPOT:
			return m_pSpot->m_v3Colour;
		break;
		default:
			GP_ERROR("Unsupported Light Type");
			return Vector3::zero();
		break;
	}
}

void Light::setLightColour(const Vector3& v3Colour)
{
	switch (m_eLightType)
	{
		case Light::DIRECTIONAL:
			GP_ASSERT( m_pDirectional );
			m_pDirectional->m_v3Colour = v3Colour;
		break;
		case Light::POINT:
			GP_ASSERT( m_pPoint );
			m_pPoint->m_v3Colour = v3Colour;
		break;
		case Light::SPOT:
			GP_ASSERT( m_pSpot );
			m_pSpot->m_v3Colour = v3Colour;
		break;
		default:
			GP_ERROR("Unsupported Light Type");
		break;
	}	
}

void Light::setLightColour(float fRed, float fGreen, float fBlue)
{
	setLightColour(Vector3(fRed, fGreen, fBlue));
}

Node* Light::getNode() const
{
	return m_pNode;
}

void Light::setNode(Node* pNode)
{
	GP_ASSERT(pNode);
	m_pNode = pNode;
}

float Light::getRange() const
{
	GP_ASSERT( m_eLightType != DIRECTIONAL );

	switch (m_eLightType)
	{
		case Light::POINT:
			GP_ASSERT( m_pPoint );
			return m_pPoint->m_fRange;
		break;
		case Light::SPOT:
			GP_ASSERT( m_pSpot );
			return m_pSpot->m_fRange;
		break;
		default:
			GP_ERROR("Unsupported Light Type");
			return 0.0f;
		break;
	}
}

void Light::setRange(float fRange)
{
	GP_ASSERT( m_eLightType != DIRECTIONAL );

	switch (m_eLightType)
	{
		case Light::POINT:
			GP_ASSERT( m_pPoint );
			m_pPoint->m_fRange = fRange;
			m_pPoint->m_fRangeInverse = 1.0f / fRange;
		break;
		case Light::SPOT:
			GP_ASSERT( m_pSpot );
			m_pSpot->m_fRange = fRange;
			m_pSpot->m_fRangeInverse = 1.0f / fRange;
		break;
		default:
			GP_ERROR("Unsupported Light Type");
		break;
	}
}

float Light::getRangeInverse() const
{
	GP_ASSERT( m_eLightType != DIRECTIONAL );

	switch (m_eLightType)
	{
		case Light::POINT:
			GP_ASSERT( m_pPoint );
			return m_pPoint->m_fRangeInverse;
		break;
		case Light::SPOT:
			GP_ASSERT( m_pSpot );
			return m_pSpot->m_fRangeInverse;
		break;
		default:
			GP_ERROR("Unsupported Light Type");
			return 0.0f;
		break;
	}
}

float Light::getInnerAngle() const
{
	GP_ASSERT( m_eLightType == SPOT );
	return m_pSpot->m_fInnerAngle;
}

float Light::getInnerAngleCosine() const
{
	GP_ASSERT( m_eLightType == SPOT );
	return m_pSpot->m_fInnerAngleCosine;
}

void Light::setInnerAngle(float fInnerAngle)
{
	GP_ASSERT( m_eLightType == SPOT );

	m_pSpot->m_fInnerAngle = fInnerAngle;
	m_pSpot->m_fInnerAngleCosine = cos(fInnerAngle);
}

float Light::getOuterAngle() const
{
	GP_ASSERT(m_eLightType == SPOT);
	return m_pSpot->m_fOuterAngle;
}

float Light::getOuterAngleCosine() const
{
	GP_ASSERT(m_eLightType == SPOT);
	return m_pSpot->m_fOuterAngleCosine;
}

void Light::setOuterAngle(float fOuterAngle)
{
	GP_ASSERT(m_eLightType == SPOT);

	m_pSpot->m_fOuterAngle = fOuterAngle;
	m_pSpot->m_fOuterAngleCosine = cos(fOuterAngle);
}

Light::Directional::Directional(const Vector3& v3Colour)
: m_v3Colour(v3Colour)
{

}

Light::Point::Point(const Vector3& v3Colour, float fRange)
: m_v3Colour(v3Colour)
, m_fRange(fRange)
{
	m_fRangeInverse = 1.0f / fRange;
}

Light::Spot::Spot(const Vector3& v3Colour, float fRange, float fInnerAngle, float fOuterAngle)
: m_v3Colour(v3Colour)
, m_fRange(fRange)
, m_fInnerAngle(fInnerAngle)
, m_fOuterAngle(fOuterAngle)
{
	m_fRangeInverse = 1.0f / fRange;
	m_fInnerAngleCosine = cos(fInnerAngle);
	m_fOuterAngleCosine = cos(fOuterAngle);
}


