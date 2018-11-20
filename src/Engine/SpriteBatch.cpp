#include "Engine/SpriteBatch.h"
#include "Engine/MeshBatch.h"
#include "Engine/Texture.h"

// Default size of a newly created sprite batch
#define SPRITE_BATCH_DEFAULT_SIZE 128

// Factor to grow a sprite batch by when its size is exceeded
#define SPRITE_BATCH_GROW_FACTOR 2.0f

// Macro for adding a sprite to the batch
#define SPRITE_ADD_VERTEX(vtx, vx, vy, vz, vu, vv, vr, vg, vb, va) \
				vtx.X = vx; vtx.Y = vy; vtx.Z = vz; \
				vtx.U = vu; vtx.V = vv; \
				vtx.R = vr; vtx.G = vg; vtx.B = vb; vtx.A = va

SpriteBatch::SpriteBatch() 
	:	m_pMeshBatch(NULL),
		m_fTextureWidthRatio(0.0f),
		m_fTextureHeightRatio(0.0f)
{

}

SpriteBatch::~SpriteBatch() {
	SAFE_DELETE( m_pMeshBatch );
}

SpriteBatch* SpriteBatch::create(const char* pTexturePath, /*Effect* pEffect = NULL, */unsigned int iInitialCapacity) {
	
	Texture* pTexture = Texture::createEx(pTexturePath);
	SpriteBatch* pSpriteBatch = SpriteBatch::create(pTexture);

	return pSpriteBatch;
}

SpriteBatch* SpriteBatch::create(Texture* pTexture, /*Effect* pEffect = NULL, */unsigned int iInitialCapacity) {

	GP_ASSERT( pTexture );

	// Define the vertex format for the batch
	VertexFormat::Element vertexElements[] = {
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::TEXCOORD0, 2),
		VertexFormat::Element(VertexFormat::COLOR, 4)
	};
	unsigned int elementCount = sizeof(vertexElements) / sizeof(VertexFormat::Element);
	VertexFormat* vertexFormat = new VertexFormat(vertexElements, elementCount);

	// Create the mesh batch
	MeshBatch* pMeshBatch = MeshBatch::create(*vertexFormat, Mesh::TRIANGLE_STRIP, true, (iInitialCapacity > 0) ? iInitialCapacity : SPRITE_BATCH_DEFAULT_SIZE);
	pMeshBatch->setTexture(pTexture);

	// Create the batch
	SpriteBatch* pSpriteBatch = new SpriteBatch();
	pSpriteBatch->m_pMeshBatch = pMeshBatch;
	pSpriteBatch->m_fTextureWidthRatio = 1.0f/(float)pTexture->getWidth();
	pSpriteBatch->m_fTextureHeightRatio = 1.0f/(float)pTexture->getHeight();

	return pSpriteBatch;
}

void SpriteBatch::start() {
	m_pMeshBatch->start();

	// Enable & Set the type of Blending.
	GL_ASSERT( glEnable(GL_BLEND) );
	GL_ASSERT( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
	//GL_ASSERT( glBlendFunc(GL_ONE, GL_ONE) );
}

void SpriteBatch::stop() {
	
	// Finish and draw the batch
	m_pMeshBatch->stop();
	m_pMeshBatch->draw();

	// Disable Blending
	GL_ASSERT( glDisable(GL_BLEND) );
}

//void SpriteBatch::draw(const Rectangle& dest, const Rectangle& src, const Vector4& color) {
//
//	// Calculate uvs.
//	float u1 = m_fTextureWidthRatio * src.x;
//	float v1 = m_fTextureHeightRatio * src.y;
//
//	float u2 = u1 + m_fTextureWidthRatio * src.width;
//	float v2 = v1 - m_fTextureHeightRatio * src.height;
//
//	draw(dest.x, dest.y, dest.width, dest.height, u1, v1, u2, v2, color);
//}

void SpriteBatch::draw(const Vector4& dest, const Vector4& src, const Vector4& color) {
	
	// Calculate uvs.
	float u1 = m_fTextureWidthRatio * src.x;
	float v1 = m_fTextureHeightRatio * src.y;
	
	float u2 = u1 + m_fTextureWidthRatio * src.z;//width;
	float v2 = v1 + m_fTextureHeightRatio * src.w;//height;

	draw(dest.x, dest.y, dest.z/*width*/, dest.w/*height*/, u1, v1, u2, v2, color);
}

void SpriteBatch::draw(Vector3& dst, Vector4& src, Vector2& scale, const Vector4& color) {

	// Calculate uvs.
	float u1 = m_fTextureWidthRatio * src.x;
	float v1 = m_fTextureHeightRatio * src.y;

	float u2 = u1 + m_fTextureWidthRatio * src.z;//width;
	float v2 = v1 + m_fTextureHeightRatio * src.w;//height;

	if(clipSprite(m_ClipRect, dst.x, dst.y, scale.x, scale.y, u1, v1, u2, v2))
		draw(dst.x, dst.y, dst.z, scale.x, scale.y, u1, v1, u2, v2, color);
}

void SpriteBatch::draw(float x, float y, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color) {

	if(clipSprite(m_ClipRect, x, y, width, height, u1, v1, u2, v2))
		draw(x, y, 0, width, height, u1, v1, u2, v2, color);
}

void SpriteBatch::draw(float x, float y, float z, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color, bool positionIsCenter) {
	
	// Treat the given position as the center if the user specified it as such.
	if (positionIsCenter) {
		x -= 0.5f * width;
		y -= 0.5f * height;
	}

	// Write sprite vertex data.
	const float x2 = x + width;
	const float y2 = y + height;
	static SpriteVertex vtx[4];
	SPRITE_ADD_VERTEX(vtx[0], x, y, z, u1, v1, color.x, color.y, color.z, color.w);	
	SPRITE_ADD_VERTEX(vtx[1], x, y2, z, u1, v2, color.x, color.y, color.z, color.w);
	SPRITE_ADD_VERTEX(vtx[2], x2, y, z, u2, v1, color.x, color.y, color.z, color.w);
	SPRITE_ADD_VERTEX(vtx[3], x2, y2, z, u2, v2, color.x, color.y, color.z, color.w);

	static unsigned short indices[4] = { 0, 1, 2, 3 };

	m_pMeshBatch->add(vtx, 4, indices, 4);
}

void SpriteBatch::draw(Vector3& dst, const Vector4& src, Vector2& scale, const Vector4& color, const Vector2& rotationPoint, float rotationAngle) {

	// Calculate uvs.
	float u1 = m_fTextureWidthRatio * src.x;
	float v1 = m_fTextureHeightRatio * src.y;

	float u2 = u1 + m_fTextureWidthRatio * src.z;//width;
	float v2 = v1 + m_fTextureHeightRatio * src.w;//height;
	
	if(clipSprite(m_ClipRect, dst.x, dst.y, scale.x, scale.y, u1, v1, u2, v2))
		draw(dst, scale.x, scale.y, u1, v1, u2, v2, color, rotationPoint, rotationAngle);
}

void SpriteBatch::draw(const Vector3& dst, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color, const Vector2& rotationPoint, float rotationAngle, bool positionIsCenter) {
	draw(dst.x, dst.y, dst.z, width, height, u1, v1, u2, v2, color, rotationPoint, rotationAngle, positionIsCenter);
}

void SpriteBatch::draw(float x, float y, float z, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color, const Vector2& rotationPoint, float rotationAngle, bool positionIsCenter) {

	// Treat the given position as the center if the user specified it as such.
	if (positionIsCenter)
	{
		x -= 0.5f * width;
		y -= 0.5f * height;
	}

	// Expand the destination position by scale into 4 points.
	float x2 = x + width;
	float y2 = y + height;

	Vector2 upLeft(x, y);
	Vector2 upRight(x2, y);
	Vector2 downLeft(x, y2);
	Vector2 downRight(x2, y2);

	// Rotate points around rotationAxis by rotationAngle.
	Vector2 pivotPoint(rotationPoint);
	pivotPoint.x *= width;
	pivotPoint.y *= height;
	pivotPoint.x += x;
	pivotPoint.y += y;
	upLeft.rotate(pivotPoint, rotationAngle);
	upRight.rotate(pivotPoint, rotationAngle);
	downLeft.rotate(pivotPoint, rotationAngle);
	downRight.rotate(pivotPoint, rotationAngle);

	// Write sprite vertex data.
	static SpriteVertex vtx[4];
	SPRITE_ADD_VERTEX(vtx[0], downLeft.x,	downLeft.y,		z, u1, v1, color.x, color.y, color.z, color.w);
	SPRITE_ADD_VERTEX(vtx[1], upLeft.x,		upLeft.y,		z, u1, v2, color.x, color.y, color.z, color.w);
	SPRITE_ADD_VERTEX(vtx[2], downRight.x,	downRight.y,	z, u2, v1, color.x, color.y, color.z, color.w);
	SPRITE_ADD_VERTEX(vtx[3], upRight.x,	upRight.y,		z, u2, v2, color.x, color.y, color.z, color.w);

	static unsigned short indices[4] = { 0, 1, 2, 3 };

	m_pMeshBatch->add(vtx, 4, indices, 4);
}

void SpriteBatch::draw(float x, float y, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color, const Vector4& clip) {
    // Only draw if at least part of the sprite is within the clip region.
    if (clipSprite(clip, x, y, width, height, u1, v1, u2, v2))
        draw(x, y, 0, width, height, u1, v1, u2, v2, color);
}

bool SpriteBatch::clipSprite(const Vector4& clip, float& x, float& y, float& width, float& height, float& u1, float& v1, float& u2, float& v2) {

    // Clip the rectangle given by { x, y, width, height } into clip.
    // We need to scale the uvs accordingly as we do this.

    // First check to see if we need to draw at all.
    if (x + width < clip.x || x > clip.x + clip.z/*width*/ ||
        y + height < clip.y || y > clip.y + clip.w/*height*/)
    {
        return false;
    }

    const float uvWidth = u2 - u1;
    const float uvHeight = v2 - v1;

    // Moving x to the right.
    if (x < clip.x)
    {
        const float percent = (clip.x - x) / width;
        const float dx = clip.x - x;
        x = clip.x;
        width -= dx;
        u1 += uvWidth * percent;
    }

    // Moving y down.
    if (y < clip.y)
    {
        const float percent = (clip.y - y) / height;
        const float dy = clip.y - y;
        y = clip.y;
        height -= dy;
        v1 += uvHeight * percent;
    }

    // Moving width to the left.
    const float clipX2 = clip.x + clip.z/*width*/;
    float x2 = x + width;
    if (x2 > clipX2)
    {
        const float percent = (x2 - clipX2) / width;
        width = clipX2 - x;
        u2 -= uvWidth * percent;
    }

    // Moving height up.
    const float clipY2 = clip.y + clip.w/*height*/;
    float y2 = y + height;
    if (y2 > clipY2)
    {
        const float percent = (y2 - clipY2) / height;
        height = clipY2 - y;
        v2 -= uvHeight * percent;
    }

    return true;
}

void SpriteBatch::addSprite(float x, float y, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color, SpriteBatch::SpriteVertex* vertices) {
    GP_ASSERT(vertices);

    const float x2 = x + width;
    const float y2 = y + height;
    SPRITE_ADD_VERTEX(vertices[0], x, y, 0, u1, v1, color.x, color.y, color.z, color.w);
    SPRITE_ADD_VERTEX(vertices[1], x, y2, 0, u1, v2, color.x, color.y, color.z, color.w);
    SPRITE_ADD_VERTEX(vertices[2], x2, y, 0, u2, v1, color.x, color.y, color.z, color.w);
    SPRITE_ADD_VERTEX(vertices[3], x2, y2, 0, u2, v2, color.x, color.y, color.z, color.w);
}

void SpriteBatch::addSprite(float x, float y, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color, const Vector4& clip, SpriteBatch::SpriteVertex* vertices) {
    GP_ASSERT(vertices);

    // Only add a sprite if at least part of the sprite is within the clip region.
    if (clipSprite(clip, x, y, width, height, u1, v1, u2, v2))
    {
        const float x2 = x + width;
        const float y2 = y + height;
        SPRITE_ADD_VERTEX(vertices[0], x, y, 0, u1, v1, color.x, color.y, color.z, color.w);
        SPRITE_ADD_VERTEX(vertices[1], x, y2, 0, u1, v2, color.x, color.y, color.z, color.w);
        SPRITE_ADD_VERTEX(vertices[2], x2, y, 0, u2, v1, color.x, color.y, color.z, color.w);
        SPRITE_ADD_VERTEX(vertices[3], x2, y2, 0, u2, v2, color.x, color.y, color.z, color.w);
    }
}

void SpriteBatch::draw(SpriteBatch::SpriteVertex* pVertices, unsigned int iVertexCount, unsigned short* pIndices, unsigned int iIndexCount) {
    GP_ASSERT(pVertices);
    GP_ASSERT(pIndices);

    m_pMeshBatch->add(pVertices, iVertexCount, pIndices, iIndexCount);
}

void SpriteBatch::draw(const Vector3& position, const Vector3& right, const Vector3& forward, float width, float height, float u1, float v1, float u2, float v2, const Vector4& color, const Vector2& rotationPoint, float rotationAngle) {

	// Calculate the vertex positions.
    Vector3 tRight(right);
    tRight *= width * 0.5f;
    Vector3 tForward(forward);
    tForward *= height * 0.5f;
    
    Vector3 p0 = position;
    p0 -= tRight;
    p0 -= tForward;

    Vector3 p1 = position;
    p1 += tRight;
    p1 -= tForward;

    tForward = forward;
    tForward *= height;
    Vector3 p2 = p0;
    p2 += tForward;
    Vector3 p3 = p1;
    p3 += tForward;

    // Calculate the rotation point.
    Vector3 rp = p0;
    tRight = right;
    tRight *= width * rotationPoint.x;
    tForward *= rotationPoint.y;
    rp += tRight;
    rp += tForward;

    // Rotate all points the specified amount about the given point (about the up vector).
    static Vector3 u;
    Vector3::cross(right, forward, &u);
    static Matrix4 rotation;
    rotation.rotate(rotationAngle, u);

    p0 -= rp;
    p0 *= rotation;
    p0 += rp;

    p1 -= rp;
    p1 *= rotation;
    p1 += rp;

    p2 -= rp;
    p2 *= rotation;
    p2 += rp;

    p3 -= rp;
    p3 *= rotation;
    p3 += rp;

    // Add the sprite vertex data to the batch.
    static SpriteVertex v[4];
    SPRITE_ADD_VERTEX(v[0], p0.x, p0.y, p0.z, u1, v1, color.x, color.y, color.z, color.w);
    SPRITE_ADD_VERTEX(v[1], p1.x, p1.y, p1.z, u2, v1, color.x, color.y, color.z, color.w);
    SPRITE_ADD_VERTEX(v[2], p2.x, p2.y, p2.z, u1, v2, color.x, color.y, color.z, color.w);
    SPRITE_ADD_VERTEX(v[3], p3.x, p3.y, p3.z, u2, v2, color.x, color.y, color.z, color.w);
    
    static const unsigned short indices[4] = { 0, 1, 2, 3 };
    m_pMeshBatch->add(v, 4, const_cast<unsigned short*>(indices), 4);
}

void SpriteBatch::setClip(int x, int y, int width, int height) {
	if(width <= 0 || height <= 0)
		return;

	m_ClipRect.x = (float)x;
	m_ClipRect.y = (float)y;
	m_ClipRect.z = (float)width;
	m_ClipRect.w = (float)height;
}
