#ifndef MATERIALPARAMETER_H
#define MATERIALPARAMETER_H

#include "Common/CCString.h"
#include "Common/Vectors.h"
#include "Common/Matrices.h"
#include "Engine/Texture.h"
#include "Engine/Effect.h"
#include <functional>

class MaterialParameter {

	friend class RenderState;

	public:
		MaterialParameter(const char* sName);

		const char*						getName();
		Texture::Sampler*				getSampler() const;
		
		void							setValue(float value);
		void							setValue(int value);
		void							setValue(const float* values, unsigned int count = 1);
		void							setValue(const int* values, unsigned int count = 1);
		void							setValue(const Vector2& value);
		void							setValue(const Vector2* values, unsigned int count = 1);
		void							setValue(const Vector3& value);
		void							setValue(const Vector3* values, unsigned int count = 1);
		void							setValue(const Vector4& value);
		void							setValue(const Vector4* values, unsigned int count = 1);
		void							setValue(const Matrix4& value);
		void							setValue(const Matrix4* values, unsigned int count = 1);
		void							setValue(const Texture::Sampler* sampler);
		void							setValue(const Texture::Sampler** pSamplers, unsigned int count);
		Texture::Sampler*				setValue(const char* sTexturePath, bool bGenerateMipmaps);

		//void							setFloat(float value);
		//void							setFloatArray(const float* values, unsigned int count, bool copy = false);
		//void							setInt(int value);
		//void							setIntArray(const int* values, unsigned int count, bool copy = false);
		//void							setVector2(Vector2& value);
		//void							setVector2Array(const Vector2* values, unsigned int count, bool copy = false);
		//void							setVector3(Vector3& value);
		//void							setVector3Array(const Vector3* values, unsigned int count, bool copy = false);
		//void							setVector4(Vector4& value);
		//void							setVector4Array(const Vector4* values, unsigned int count, bool copy = false);
		//void							setMatrix(Matrix4& value);
		//void							setMatrixArray(const Matrix4* values, unsigned int count, bool copy = false);
		//Texture::Sampler*				setSampler(const char* sTexturePath, bool bGenerateMipmaps);
		//void							setSampler(const Texture::Sampler* value);
		//void							setSamplerArray(const Texture::Sampler** values, unsigned int count, bool copy = false);

		template <class ClassType, class ParameterType>
		void							bindValue(ClassType* classInstance, ParameterType(ClassType::*valueMethod)() const);

		//template <class ClassType, class ParameterType>
		//void							bindValue(ClassType* classInstance, ParameterType(ClassType::*valueMethod)() const, unsigned int (ClassType::*countMethod)() const);
	private:
		~MaterialParameter();
		MaterialParameter&				operator=(const MaterialParameter&);
		
		/////////////////////////////////////////// MethodBinding ///////////////////////////////////////////
		class MethodBinding
		{
			friend class RenderState;
			public:
				virtual void			setValue(Effect* pEffect) = 0;
				virtual ~MethodBinding() { }
			protected:
				MethodBinding(MaterialParameter* pParam);
				MethodBinding&			operator=(const MethodBinding&);

				MaterialParameter*		m_pMaterialParameter;
				bool					m_bAutoBinding;
		};
		////////////////////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////// MethodValueBinding ///////////////////////////////////////////
		template <class ClassType, class ParameterType>
		class MethodValueBinding : public MethodBinding
		{
			typedef ParameterType(ClassType::*ValueMethod)() const;
		public:
			MethodValueBinding(MaterialParameter* param, ClassType* instance, ValueMethod valueMethod);
			void setValue(Effect* effect);
		private:
			ClassType* _instance;
			ValueMethod _valueMethod;

		};
		////////////////////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////// MethodArrayBinding ///////////////////////////////////////////
		template <class ClassType, class ParameterType>
		class MethodArrayBinding : public MethodBinding
		{
			typedef ParameterType(ClassType::*ValueMethod)() const;
			typedef unsigned int (ClassType::*CountMethod)() const;
			public:
				MethodArrayBinding(MaterialParameter* param, ClassType* instance, ValueMethod valueMethod, CountMethod countMethod);
				void setValue(Effect* effect);
			private:
				ClassType* _instance;
				ValueMethod _valueMethod;
				CountMethod _countMethod;
		};
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void							clearValue();
		union {
			float						floatValue;
			int							intValue;
			float*						floatPtrValue;
			int*						intPtrValue;
			const Texture::Sampler*		samplerValue;
			const Texture::Sampler**	samplerArrayValue;
			MethodBinding*				method;
		} m_Value;

		enum {
			NONE,
			FLOAT,
			FLOAT_ARRAY,
			INT,
			INT_ARRAY,
			VECTOR2,
			VECTOR3,
			VECTOR4,
			MATRIX,
			SAMPLER,
			SAMPLER_ARRAY,
			METHOD
		} m_Type;

		enum LOGGER_DIRTYBITS
		{
			UNIFORM_NOT_FOUND = 0x01,
			PARAMETER_VALUE_NOT_SET = 0x02
		};

		void					bind(Effect* effect);

		unsigned int			m_iCount;
		bool					m_bDynamic;
		CCString				m_sName;

		Uniform*				m_pUniform;
		char					m_LoggerDirtyBits;

};

////////////////////////////////////////////////////////////////////////////////////////////////////
template <class ClassType, class ParameterType>
void MaterialParameter::bindValue(ClassType* classInstance, ParameterType(ClassType::*valueMethod)() const)
{
	clearValue();

	m_Value.method = new MethodValueBinding<ClassType, ParameterType>(this, classInstance, valueMethod);
	m_bDynamic = true;
	m_Type = MaterialParameter::METHOD;
}

//template <class ClassType, class ParameterType>
//void MaterialParameter::bindValue(ClassType* classInstance, ParameterType(ClassType::*valueMethod)() const, unsigned int (ClassType::*countMethod)() const)
//{
//	clearValue();
//
//	m_Value.method = new MethodArrayBinding<ClassType, ParameterType>(this, classInstance, valueMethod, countMethod);
//	m_bDynamic = true;
//	m_Type = MaterialParameter::METHOD;
//}
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
template <class ClassType, class ParameterType>
MaterialParameter::MethodValueBinding<ClassType, ParameterType>::MethodValueBinding(MaterialParameter* param, ClassType* instance, ValueMethod valueMethod) :
MethodBinding(param), _instance(instance), _valueMethod(valueMethod)
{
}

template <class ClassType, class ParameterType>
void MaterialParameter::MethodValueBinding<ClassType, ParameterType>::setValue(Effect* effect)
{
	effect->setValue(m_pMaterialParameter->m_pUniform, (_instance->*_valueMethod)());
}

template <class ClassType, class ParameterType>
MaterialParameter::MethodArrayBinding<ClassType, ParameterType>::MethodArrayBinding(MaterialParameter* param, ClassType* instance, ValueMethod valueMethod, CountMethod countMethod) :
MethodBinding(param), _instance(instance), _valueMethod(valueMethod), _countMethod(countMethod)
{
}

template <class ClassType, class ParameterType>
void MaterialParameter::MethodArrayBinding<ClassType, ParameterType>::setValue(Effect* effect)
{
	effect->setValue(m_pMaterialParameter->m_pUniform, (_instance->*_valueMethod)(), (_instance->*_countMethod)());
}
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif