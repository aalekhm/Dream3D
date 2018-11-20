#ifndef SPRITEBATCH_H
#define SPRITEBATCH_H

#include "Engine/Base.h"
#include "Common/Rectangle.h"
#include "Common/Vectors.h"
#include "Common/Matrices.h"
#include "Engine/Texture.h"

class MeshBatch;
class Effect;

class SpriteBatch {

	public:
		static SpriteBatch* create(const char* pTexturePath, Effect* pEffect = NULL, unsigned int iInitialCapacity = 0);
		static SpriteBatch* create(Texture* pTexture, Effect* pEffect = NULL, unsigned int iInitialCapacity = 0);
		
		void start();
		void stop();

		//void draw(const Rectangle& dest, const Rectangle& src, const Vector4& color = Vector4::one());
		void draw(const Vector4& dest, const Vector4& src, const Vector4& color = Vector4::one());

		void draw(float x, float y, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color);
		void draw(float x, float y, float z, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color, bool positionIsCenter = false);

		void draw(Vector3& dst, Vector4& src, Vector2& scale, const Vector4& color = Vector4::one());
		
		void draw(Vector3& dst, const Vector4& src, Vector2& scale, const Vector4& color, const Vector2& rotationPoint, float rotationAngle);
		void draw(const Vector3& dst, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color, const Vector2& rotationPoint, float rotationAngle, bool positionIsCenter = false);
		void draw(float x, float y, float z, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color, const Vector2& rotationPoint, float rotationAngle, bool positionIsCenter = false);

		void draw(float x, float y, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color, const Vector4& clip);
		bool clipSprite(const Vector4& clip, float& x, float& y, float& width, float& height, float& u1, float& v1, float& u2, float& v2);

		void draw(const Vector3& position, const Vector3& right, const Vector3& forward, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color, const Vector2& rotationPoint, float rotationAngle);

		void setClip(int x, int y, int width, int height);
		void resetClip();

		Texture*	getTexture();
	private:
		struct SpriteVertex {
			float X;
			float Y;
			float Z;

			float U;
			float V;

			float R;
			float G;
			float B;
			float A;
		};

		SpriteBatch();
		virtual ~SpriteBatch();

		void draw(SpriteBatch::SpriteVertex* pVertices, unsigned int iVertexCount, unsigned short* pIndices, unsigned int iIndexCount);

		void addSprite(float x, float y, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color, SpriteVertex* vertices);
		void addSprite(float x, float y, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color, const Vector4& clip, SpriteBatch::SpriteVertex* vertices);
		
		MeshBatch*			m_pMeshBatch;
		Texture::Sampler*	m_pSampler;
		bool				m_bCustomEffect;
		float				m_fTextureWidthRatio;
		float				m_fTextureHeightRatio;
		Vector4				m_ClipRect;
		mutable Matrix4		m_ProjectionMatrix;
};

#endif