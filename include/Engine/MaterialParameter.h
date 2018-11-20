#ifndef MATERIALPARAMETER_H
#define MATERIALPARAMETER_H

#include "Common/CCString.h"
#include "Engine/Texture.h"

class MaterialParameter {

	public:
		MaterialParameter(const char* sName);

		const char*			getName();
		Texture::Sampler*	getSampler() const;
		
		void						setValue(float value);
		void						setValue(int value);
		void						setValue(const float* values, unsigned int count = 1);
		void						setValue(const int* values, unsigned int count = 1);
		void						setValue(const Texture::Sampler* sampler);
		Texture::Sampler*	setValue(const char* sTexturePath, bool bGenerateMipmaps);
	private:
		~MaterialParameter();
		MaterialParameter& operator=(const MaterialParameter&);

		union {
			float								floatValue;
			int								intValue;
			float*							floatPtrValue;
			int*								intPtrValue;
			const Texture::Sampler*	samplerPtrValue;
			//MethodBinding*				method;
		} m_Value;

		enum {
			NONE,
			FLOAT,
			INT,
			VECTOR2,
			VECTOR3,
			VECTOR4,
			MATRIX,
			SAMPLER,
			METHOD
		} m_Type;

		unsigned int			m_iCount;
		CCString				m_sName;

};

#endif