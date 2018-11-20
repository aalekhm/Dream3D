
#include "Engine/Base.h"
#include "Engine/MaterialParameter.h"

MaterialParameter::MaterialParameter(const char* sName) 
	: m_sName(sName ? sName : "")
	, m_Type(MaterialParameter::NONE)
	, m_iCount(1)
	, m_bDynamic(false)
	, m_pUniform(NULL)
	, m_LoggerDirtyBits(0)

{
	clearValue();
}

MaterialParameter::~MaterialParameter() {

	clearValue();
}

void MaterialParameter::clearValue() {

	// Release parameters
	switch (m_Type)
	{
		case MaterialParameter::SAMPLER:
			if (m_Value.samplerValue) {
				//const_cast<Texture::Sampler*>(m_Value.samplerPtrValue)->release();
			}
		break;
		case MaterialParameter::SAMPLER_ARRAY:
		    if (m_Value.samplerArrayValue)
		    {
		        for (unsigned int i = 0; i < m_iCount; ++i)
		        {
		            //const_cast<Texture::Sampler*>(m_Value.samplerArrayValue[i])->release();
		        }
		    }
		break;
		default:
		    // Ignore all other cases.
		break;
	}

	// Free dynamic data
	if (m_bDynamic) {

		switch (m_Type) {
			case MaterialParameter::FLOAT:
			case MaterialParameter::FLOAT_ARRAY:
			case MaterialParameter::VECTOR2:
			case MaterialParameter::VECTOR3:
			case MaterialParameter::VECTOR4:
			case MaterialParameter::MATRIX:
				SAFE_DELETE_ARRAY(m_Value.floatPtrValue);
			break;
			case MaterialParameter::INT:
			case MaterialParameter::INT_ARRAY:
				SAFE_DELETE_ARRAY(m_Value.intPtrValue);
			break;
			case MaterialParameter::SAMPLER_ARRAY:
				SAFE_DELETE_ARRAY(m_Value.samplerArrayValue);
			break;
			case MaterialParameter::METHOD:
				SAFE_DELETE(m_Value.method);//SAFE_RELEASE(m_Value.method);
			break;

			default:
				// Ignore all other cases.
			break;
		}

		m_bDynamic = false;
		m_iCount = 0;
	}

	memset(&m_Value, 0, sizeof(m_Value));
	m_Type = MaterialParameter::NONE;
}

const char* MaterialParameter::getName() {
	return m_sName.c_str();
}

Texture::Sampler* MaterialParameter::getSampler() const {
	if(m_Type == MaterialParameter::SAMPLER)
		return const_cast<Texture::Sampler*>(m_Value.samplerValue);

	return NULL;
}

void MaterialParameter::setValue(float value) {

	clearValue();

	m_Value.floatValue = value;
	m_Type = MaterialParameter::FLOAT;
}

void MaterialParameter::setValue(int value) {

	clearValue();

	m_Value.intValue = value;
	m_Type = MaterialParameter::INT;
}

void MaterialParameter::setValue(const float* values, unsigned int count) {

	clearValue();

	m_Value.floatPtrValue = const_cast<float*>(values);
	m_iCount = count;
	m_Type = MaterialParameter::FLOAT_ARRAY;
}

void MaterialParameter::setValue(const int* values, unsigned int count) {

	clearValue();

	m_Value.intPtrValue = const_cast<int*>(values);
	m_iCount = count;
	m_Type = MaterialParameter::INT_ARRAY;

}

void MaterialParameter::setValue(const Vector2& value) {

	clearValue();

	// Copy data by-value into a dynamic array.
	float* fArray = new float[2];
	memcpy(fArray, &value.x, sizeof(float) * 2);

	m_Value.floatPtrValue = fArray;
	m_bDynamic = true;
	m_iCount = 1;
	m_Type = MaterialParameter::VECTOR2;
}

void MaterialParameter::setValue(const Vector2* values, unsigned int count) {

	clearValue();

	m_Value.floatPtrValue = const_cast<float*>(&values[0].x);
	m_iCount = count;
	m_Type = MaterialParameter::VECTOR2;
}

void MaterialParameter::setValue(const Vector3& value) {

	clearValue();

	// Copy data by-value into a dynamic array.
	float* fArray = new float[3];
	memcpy(fArray, &value.x, sizeof(float)* 3);

	m_Value.floatPtrValue = fArray;
	m_bDynamic = true;
	m_iCount = 1;
	m_Type = MaterialParameter::VECTOR3;
}

void MaterialParameter::setValue(const Vector3* values, unsigned int count) {

	clearValue();

	m_Value.floatPtrValue = const_cast<float*>(&values[0].x);
	m_iCount = count;
	m_Type = MaterialParameter::VECTOR3;
}

void MaterialParameter::setValue(const Vector4& value) {

	clearValue();

	// Copy data by-value into a dynamic array.
	float* fArray = new float[4];
	memcpy(fArray, &value.x, sizeof(float)* 4);

	m_Value.floatPtrValue = fArray;
	m_bDynamic = true;
	m_iCount = 1;
	m_Type = MaterialParameter::VECTOR4;
}

void MaterialParameter::setValue(const Vector4* values, unsigned int count) {

	clearValue();

	m_Value.floatPtrValue = const_cast<float*>(&values[0].x);
	m_iCount = count;
	m_Type = MaterialParameter::VECTOR4;
}

void MaterialParameter::setValue(const Matrix4& value) {

	// If this parameter is already storing a single dynamic matrix, no need to clear it.
	if ( !(m_bDynamic && m_iCount == 1 && m_Type == MaterialParameter::MATRIX && m_Value.floatPtrValue != NULL) ) {

		clearValue();

		// Allocate a new dynamic matrix.
		m_Value.floatPtrValue = new float[16];
	}
	
	memcpy(m_Value.floatPtrValue, value.m, sizeof(float) * 16);

	m_bDynamic = true;
	m_iCount = 1;
	m_Type = MaterialParameter::MATRIX;
}

void MaterialParameter::setValue(const Matrix4* values, unsigned int count) {

	clearValue();

	m_Value.floatPtrValue = const_cast<float*>(values[0].m);
	m_iCount = count;
	m_Type = MaterialParameter::MATRIX;
}

void MaterialParameter::setValue(const Texture::Sampler* pSampler) {

	GP_ASSERT(pSampler);
	clearValue();

	//const_cast<Texture::Sampler*>(pSampler)->addRef();
	m_Value.samplerValue = pSampler;
	m_Type = MaterialParameter::SAMPLER;
}

void MaterialParameter::setValue(const Texture::Sampler** pSamplers, unsigned int count) {

	GP_ASSERT( pSamplers );
	clearValue();

	for (unsigned int i = 0; i < count; i++) {
		//const_cast<Texture::Sampler*>(pSamplers[i])->addRef();
	}

	m_Value.samplerArrayValue = pSamplers;
	m_iCount = count;
	m_Type = MaterialParameter::SAMPLER_ARRAY;
}

Texture::Sampler* MaterialParameter::setValue(const char* sTexturePath, bool bGenerateMipmaps) {
	
	GP_ASSERT(sTexturePath);
	clearValue();

	Texture::Sampler* pSampler = Texture::Sampler::create(sTexturePath, bGenerateMipmaps);
	if(pSampler) {

		m_Value.samplerValue = pSampler;
		m_Type = MaterialParameter::SAMPLER;
	}

	return pSampler;
}

void MaterialParameter::bind(Effect* pEffect) {

	GP_ASSERT( pEffect );

	// If we had a Uniform cached that is not from the passed in effect,
	// we need to update our uniform to point to the new effect's uniform.
	if (!m_pUniform || m_pUniform->getEffect() != pEffect) {

		m_pUniform = pEffect->getUniform(m_sName.c_str());

		if (!m_pUniform) {

			if ((m_LoggerDirtyBits & UNIFORM_NOT_FOUND) == 0) {

				// This parameter was not found in the specified effect, so do nothing.
				//GP_WARN("Material parameter for uniform '%s' not found in effect: '%s'.", m_sName.c_str(), pEffect->getID());
				m_LoggerDirtyBits|= UNIFORM_NOT_FOUND;
			}

			return;
		}
	}

	switch (m_Type) {

		case MaterialParameter::FLOAT:
			pEffect->setValue(m_pUniform, m_Value.floatValue);
		break;
		case MaterialParameter::FLOAT_ARRAY:
			pEffect->setValue(m_pUniform, m_Value.floatPtrValue, m_iCount);
		break;
		case MaterialParameter::INT:
			pEffect->setValue(m_pUniform, m_Value.intValue);
		break;
		case MaterialParameter::INT_ARRAY:
			pEffect->setValue(m_pUniform, m_Value.intPtrValue, m_iCount);
		break;
		case MaterialParameter::VECTOR2:
			pEffect->setValue(m_pUniform, reinterpret_cast<Vector2*>(m_Value.floatPtrValue), m_iCount);
		break;
		case MaterialParameter::VECTOR3:
			pEffect->setValue(m_pUniform, reinterpret_cast<Vector3*>(m_Value.floatPtrValue), m_iCount);
		break;
		case MaterialParameter::VECTOR4:
			pEffect->setValue(m_pUniform, reinterpret_cast<Vector4*>(m_Value.floatPtrValue), m_iCount);
		break;
		case MaterialParameter::MATRIX:
			pEffect->setValue(m_pUniform, reinterpret_cast<Matrix4*>(m_Value.floatPtrValue), m_iCount);
		break;
		case MaterialParameter::SAMPLER:
			pEffect->setValue(m_pUniform, m_Value.samplerValue);
		break;
		case MaterialParameter::SAMPLER_ARRAY:
			pEffect->setValue(m_pUniform, m_Value.samplerArrayValue, m_iCount);
		break;
		case MaterialParameter::METHOD:
			if (m_Value.method)
				m_Value.method->setValue(pEffect);
		break;

		default:
		{
			if ((m_LoggerDirtyBits & PARAMETER_VALUE_NOT_SET) == 0)
			{
				//GP_WARN("Material parameter value not set for: '%s' in effect: '%s'.", m_sName.c_str(), pEffect->getID());
				m_LoggerDirtyBits |= PARAMETER_VALUE_NOT_SET;
			}
			break;
		}
	}
}

MaterialParameter::MethodBinding::MethodBinding(MaterialParameter* param) :
m_pMaterialParameter(param), m_bAutoBinding(false)
{
}