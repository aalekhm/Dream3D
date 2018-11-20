#ifndef EFFECT_H
#define EFFECT_H

#include <Common/CCString.h>
#include <Engine/Base.h>
#include <Common/Matrices.h>
#include <Common/Vectors.h>
#include <Engine/Texture.h>

class Uniform;
class Effect {

	public:
		Effect();
		~Effect();

		static Effect*							createFromFile(const char* vshPath, const char* fshPath, const char* defines = NULL);
		static Effect*							createFromSource(const char* vshSource, const char* fshSource, const char* defines = NULL);
		static Effect*							createFromSource(const char* vshPath, const char* vshSource, const char* fshPath, const char* fshSource, const char* defines = NULL);

		const char*								getID() const;

		VertexAttribute							getVertexAttribute(const char* name) const;
		Uniform*								getUniform(const char* sUniformName) const;
		Uniform*								getUniform(unsigned int iIndex) const;
		unsigned int							getUniformCount() const;

		static void								QueryAndStoreVertexAttribsMetaData(Effect* pEffect);
		static void								QueryAndStoreUniforms(Effect* pEffect);

		void									setValue(Uniform* uniform, float value);
		void									setValue(Uniform* uniform, const float* values, unsigned int count = 1);
		void									setValue(Uniform* uniform, int value);
		void									setValue(Uniform* uniform, const int* values, unsigned int count = 1);
		void									setValue(Uniform* uniform, const Matrix4& value);
		void									setValue(Uniform* uniform, const Matrix4* values, unsigned int count = 1);
		void									setValue(Uniform* uniform, const Vector2& value);
		void									setValue(Uniform* uniform, const Vector2* values, unsigned int count = 1);
		void									setValue(Uniform* uniform, const Vector3& value);
		void									setValue(Uniform* uniform, const Vector3* values, unsigned int count = 1);
		void									setValue(Uniform* uniform, const Vector4& value);
		void									setValue(Uniform* uniform, const Vector4* values, unsigned int count = 1);
		void									setValue(Uniform* uniform, const Texture::Sampler* sampler);
		void									setValue(Uniform* uniform, const Texture::Sampler** values, unsigned int count);

		void									bind();
		Effect*									getCurrentEffect();
	private:
		GLuint									m_iProgram;
		std::string								m_sProgramID;
		std::map<std::string, VertexAttribute>	m_mVertexAttributes;
		mutable std::map<std::string, Uniform*> m_mUniforms;
};

/**
* Represents a uniform variable within an effect.
*/
class Uniform {
	friend class Effect;
public:

	Uniform();
	~Uniform();

	const char*			getName() { return m_sName.c_str(); };
	const GLenum		getType() const { return m_eType; };
	Effect*				getEffect() const { return m_pEffect; }

private:
	Uniform(const Uniform& copy);
	Uniform& operator=(const Uniform&);

	CCString			m_sName;
	GLint				m_iLocation;
	GLenum				m_eType;
	unsigned int		m_iIndex;
	Effect*				m_pEffect;
};

#endif