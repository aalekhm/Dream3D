#include "Engine/Base.h"
#include "Engine/RenderState.h"
#include "Engine/Node.h"
#include "Engine/Pass.h"
#include "Engine/Technique.h"
#include "Engine/MaterialParameter.h"

// Render state override bits
#define RS_BLEND		1
#define RS_BLEND_FUNC	2
#define RS_CULL_FACE	4
#define RS_DEPTH_TEST	8
#define RS_DEPTH_WRITE	16

RenderState::StateBlock* RenderState::StateBlock::m_pDefaultState = NULL;

///////////////////////////////////////////////////////////////////////////
//RenderState
///////////////////////////////////////////////////////////////////////////
RenderState::RenderState()
	:	m_pNodeBinding(NULL),
		m_pStateBlock(NULL),
		m_pParent(NULL)
{

}

RenderState::~RenderState() {

	SAFE_DELETE( m_pStateBlock );
}

void RenderState::initialize() {

	if(StateBlock::m_pDefaultState == NULL) {
		StateBlock::m_pDefaultState = StateBlock::create();
	}
}

void RenderState::finalize() {

	SAFE_DELETE( StateBlock::m_pDefaultState );
}

void RenderState::setStateBlock(StateBlock* pStateBlock) {

	if(m_pStateBlock != pStateBlock) {

		SAFE_DELETE( m_pStateBlock );
		m_pStateBlock = pStateBlock;

		if(pStateBlock) {
			//pState->addRef();
		}
	}
}

RenderState::StateBlock* RenderState::getStateBlock() const {

	if(m_pStateBlock == NULL) {
		m_pStateBlock = StateBlock::create();
	}

	return m_pStateBlock;
}

void RenderState::setNodeBinding(Node* pNode) {

	if(m_pNodeBinding != pNode) {

		m_pNodeBinding = pNode;
	}
}

void RenderState::bind(Pass* pPass) {

	GP_ASSERT( pPass );

	// Get the combined modified state bits for our RenderState hierarchy.
	long lStateOverrideBits = m_pStateBlock ? m_pStateBlock->m_lBits : 0;
	RenderState* pParent = m_pParent;

	while(pParent) {

		if(pParent->m_pStateBlock) {
			lStateOverrideBits |= pParent->m_pStateBlock->m_lBits;
		}

		pParent = pParent->m_pParent;
	}

	// Restore renderer state to its default, except for explicitly specified states
	StateBlock::restore(lStateOverrideBits);

	// Apply parameter bindings and renderer state for the entire hierarchy, top-down.
	pParent = NULL;
	while(pParent->getTopmost(pParent)) {

		if(pParent->m_pStateBlock) {
			pParent->m_pStateBlock->bindNoRestore();
		}
	}
}

RenderState* RenderState::getTopmost(RenderState* below) {

	RenderState* rs = NULL;
	if(rs == below) {
		
		// Nothing below ourself.
		return NULL;
	}

	while(rs) {

		if(rs->m_pParent == below || rs->m_pParent == NULL) {

			// Stop traversing up here.
			return rs;
		}

		rs = rs->m_pParent;
	}

	return NULL;
}

MaterialParameter* RenderState::getparameter(const char* sName) const {
	
	GP_ASSERT( sName );

	// Search for an existing parameter with this name.
	MaterialParameter* pMaterialParameter;
	for(size_t i = 0; i < m_vParameters.size(); i++) {
		pMaterialParameter = m_vParameters[i];
		
		GP_ASSERT( pMaterialParameter );
		if(strcmp(pMaterialParameter->getName(), sName) == 0) {
			return pMaterialParameter;
		}
	}

	// Create a new parameter and store it in our list.
	pMaterialParameter = new MaterialParameter(sName);
	m_vParameters.push_back(pMaterialParameter);
	
	return pMaterialParameter;
}

///////////////////////////////////////////////////////////////////////////
//RenderState
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//StateBlock
///////////////////////////////////////////////////////////////////////////

RenderState::StateBlock::StateBlock() 
	:	m_bBlendEnabled(false),
		m_eBlendSrc(RenderState::BLEND_ONE),
		m_eBlendDst(RenderState::BLEND_ZERO),
		m_bCullFaceEnabled(false),
		m_bDepthTestEnabled(false),
		m_bDepthWriteEnabled(false),
		m_lBits(0L)
{

}

RenderState::StateBlock::StateBlock(const StateBlock& copy) {
	// Hidden
}

RenderState::StateBlock::~StateBlock() {

}

RenderState::StateBlock* RenderState::StateBlock::create() {

	return new RenderState::StateBlock();
}

void RenderState::StateBlock::bind() {

	// When the public bind() is called with no RenderState object passed in,
	// we assume we are being called to bind the state of a single StateBlock,
	// irrespective of whether it belongs to a hierarchy of RenderStates.
	// Therefore, we call restore() here with only this StateBlock's override
	// bits to restore state before applying the new state.
	StateBlock::restore(m_lBits);

	bindNoRestore();
}

void RenderState::StateBlock::bindNoRestore() {

	GP_ASSERT(m_pDefaultState);

	// Update any state that differs from _defaultState and flip _defaultState bits
	if( (m_lBits & RS_BLEND) 
		&& 
		(m_bBlendEnabled != m_pDefaultState->m_bBlendEnabled)
	) {
			if(m_bBlendEnabled)
				GL_ASSERT( glEnable(GL_BLEND) );
			else
				GL_ASSERT( glDisable(GL_BLEND) );

			m_pDefaultState->m_bBlendEnabled = m_bBlendEnabled;
	}

	if(	(m_lBits & RS_BLEND_FUNC)
		&&
		(	m_eBlendSrc != m_pDefaultState->m_eBlendSrc 
			||
			m_eBlendDst != m_pDefaultState->m_eBlendDst
		)
	) {
		GL_ASSERT( glBlendFunc((GLenum)m_eBlendSrc, (GLenum)m_eBlendDst) );

		m_pDefaultState->m_eBlendSrc = m_eBlendSrc;
		m_pDefaultState->m_eBlendDst = m_eBlendDst;
	}

	if(	(m_lBits & RS_CULL_FACE)
		&&
		(m_bCullFaceEnabled != m_pDefaultState->m_bCullFaceEnabled)
	) {
		if(m_bCullFaceEnabled)
			GL_ASSERT( glEnable(GL_CULL_FACE) );
		else
			GL_ASSERT( glDisable(GL_CULL_FACE) );

		m_pDefaultState->m_bCullFaceEnabled = m_bCullFaceEnabled;
	}

	if(	(m_lBits & RS_DEPTH_TEST)
		&&
		(m_bDepthTestEnabled != m_pDefaultState->m_bDepthTestEnabled)
		) {
			if(m_bCullFaceEnabled)
				GL_ASSERT( glEnable(GL_DEPTH_TEST) );
			else
				GL_ASSERT( glDisable(GL_DEPTH_TEST) );

			m_pDefaultState->m_bDepthTestEnabled = m_bDepthTestEnabled;
	}

	if(	(m_lBits & RS_DEPTH_WRITE)
		&&
		(m_bDepthWriteEnabled != m_pDefaultState->m_bDepthWriteEnabled)
		) {
			GL_ASSERT( glDepthMask( m_bDepthWriteEnabled ? GL_TRUE : GL_FALSE) );

			m_pDefaultState->m_bDepthWriteEnabled = m_bDepthWriteEnabled;
	}

	m_pDefaultState->m_lBits |= m_lBits;
}

void RenderState::StateBlock::restore(long stateOverrideBits) {

	GP_ASSERT( m_pDefaultState );

	// If there is no state to restore (i.e. no non-default state), do nothing.
	if(m_pDefaultState->m_lBits == 0) {
		return;
	}

	// Restore any state that is not overridden and is not default
	if (!(stateOverrideBits & RS_BLEND) && (m_pDefaultState->m_lBits & RS_BLEND)) {

		GL_ASSERT( glDisable(GL_BLEND) );
		m_pDefaultState->m_lBits &= ~RS_BLEND;
		m_pDefaultState->m_bBlendEnabled = false;
	}

	if (!(stateOverrideBits & RS_BLEND_FUNC) && (m_pDefaultState->m_lBits & RS_BLEND_FUNC)) {

		GL_ASSERT( glBlendFunc(GL_ONE, GL_ZERO) );

		m_pDefaultState->m_lBits &= ~RS_BLEND_FUNC;
		m_pDefaultState->m_eBlendSrc = RenderState::BLEND_ONE;
		m_pDefaultState->m_eBlendDst = RenderState::BLEND_ZERO;
	}

	if (!(stateOverrideBits & RS_CULL_FACE) && (m_pDefaultState->m_lBits & RS_CULL_FACE)) {

		GL_ASSERT( glDisable(GL_CULL_FACE) );

		m_pDefaultState->m_lBits &= ~RS_CULL_FACE;
		m_pDefaultState->m_bCullFaceEnabled = false;
	}

	if (!(stateOverrideBits & RS_DEPTH_TEST) && (m_pDefaultState->m_lBits & RS_DEPTH_TEST)) {

		GL_ASSERT( glDisable(RS_DEPTH_TEST) );

		m_pDefaultState->m_lBits &= ~RS_DEPTH_TEST;
		m_pDefaultState->m_bDepthTestEnabled = false;
	}

	if (!(stateOverrideBits & RS_DEPTH_WRITE) && (m_pDefaultState->m_lBits & RS_DEPTH_WRITE)) {

		GL_ASSERT( glDepthMask(GL_TRUE) );

		m_pDefaultState->m_lBits &= ~RS_DEPTH_WRITE;
		m_pDefaultState->m_bDepthWriteEnabled = false;
	}
}

void RenderState::StateBlock::enableDepthWrite() {

	GP_ASSERT( m_pDefaultState );

	// Internal method used by Game::clear() to restore depth writing before a
	// clear operation. This is necessary if the last code to draw before the
	// next frame leaves depth writing disabled.
	if (!m_pDefaultState->m_bDepthWriteEnabled) {

		GL_ASSERT( glDepthMask(GL_TRUE) );
		m_pDefaultState->m_lBits &= ~RS_DEPTH_WRITE;
		m_pDefaultState->m_bDepthWriteEnabled = true;
	}
}

static bool parseBoolean(const char* value) {

	GP_ASSERT(value);

	if (strlen(value) == 4) {
		return (
			tolower(value[0]) == 't' &&
			tolower(value[1]) == 'r' &&
			tolower(value[2]) == 'u' &&
			tolower(value[3]) == 'e' );
	}

	return false;
}

static RenderState::Blend parseBlend(const char* value) {

	GP_ASSERT(value);

	// Convert the string to uppercase for comparison.
	std::string upper(value);
	std::transform(upper.begin(), upper.end(), upper.begin(), (int(*)(int))toupper);
	if (upper == "ZERO")
		return RenderState::BLEND_ZERO;
	else 
	if (upper == "ONE")
		return RenderState::BLEND_ONE;
	else 
	if (upper == "SRC_ALPHA")
		return RenderState::BLEND_SRC_ALPHA;
	else 
	if (upper == "ONE_MINUS_SRC_ALPHA")
		return RenderState::BLEND_ONE_MINUS_SRC_ALPHA;
	else 
	if (upper == "DST_ALPHA")
		return RenderState::BLEND_DST_ALPHA;
	else 
	if (upper == "ONE_MINUS_DST_ALPHA")
		return RenderState::BLEND_ONE_MINUS_DST_ALPHA;
	else 
	if (upper == "CONSTANT_ALPHA")
		return RenderState::BLEND_CONSTANT_ALPHA;
	else 
	if (upper == "ONE_MINUS_CONSTANT_ALPHA")
		return RenderState::BLEND_ONE_MINUS_CONSTANT_ALPHA;
	else 
	if (upper == "SRC_ALPHA_SATURATE")
		return RenderState::BLEND_SRC_ALPHA_SATURATE;
	else
	{
		GP_ERROR("Unsupported blend value (%s). (Will default to BLEND_ONE if errors are treated as warnings)", value);
		return RenderState::BLEND_ONE;
	}
}

void RenderState::StateBlock::setState(const char* name, const char* value) {
	GP_ASSERT(name);

	if (strcmp(name, "blend") == 0)
	{
		setBlend(parseBoolean(value));
	}
	else if (strcmp(name, "blendSrc") == 0 || strcmp(name, "srcBlend") == 0 )   // Leaving srcBlend for backward compat.
	{
		setBlendSrc(parseBlend(value));
	}
	else if (strcmp(name, "blendDst") == 0 || strcmp(name, "dstBlend") == 0)    // // Leaving dstBlend for backward compat.
	{
		setBlendDst(parseBlend(value));
	}
	else if (strcmp(name, "cullFace") == 0)
	{
		setCullFace(parseBoolean(value));
	}
	else if (strcmp(name, "depthTest") == 0)
	{
		setDepthTest(parseBoolean(value));
	}
	else if (strcmp(name, "depthWrite") == 0)
	{
		setDepthWrite(parseBoolean(value));
	}
	else
	{
		GP_ERROR("Unsupported render state string '%s'.", name);
	}
}

void RenderState::StateBlock::setBlend(bool bEnabled) {

	m_bBlendEnabled = bEnabled;
	if(!bEnabled) {
		m_lBits &= ~RS_BLEND;
	}
	else {
		m_lBits |= RS_BLEND;
	}
}

void RenderState::StateBlock::setBlendSrc(Blend blend) {

	m_eBlendSrc = blend;
	if (m_eBlendSrc == BLEND_ONE && m_eBlendDst == BLEND_ZERO) {

		// Default blend func
		m_lBits &= ~RS_BLEND_FUNC;
	}
	else {
		m_lBits |= RS_BLEND_FUNC;
	}
}

void RenderState::StateBlock::setBlendDst(Blend blend) {

	m_eBlendDst = blend;
	if (m_eBlendSrc == BLEND_ONE && m_eBlendDst == BLEND_ZERO) {

		// Default blend func
		m_lBits &= ~RS_BLEND_FUNC;
	}
	else {
		m_lBits |= RS_BLEND_FUNC;
	}
}

void RenderState::StateBlock::setCullFace(bool bEnabled) {

	m_bCullFaceEnabled = bEnabled;
	if (!bEnabled) {
		m_lBits &= ~RS_CULL_FACE;
	}
	else {
		m_lBits |= RS_CULL_FACE;
	}
}

void RenderState::StateBlock::setDepthTest(bool bEnabled) {

	m_bDepthTestEnabled = bEnabled;
	if (!bEnabled) {
		m_lBits &= ~RS_DEPTH_TEST;
	}
	else {
		m_lBits |= RS_DEPTH_TEST;
	}
}

void RenderState::StateBlock::setDepthWrite(bool bEnabled) {

	m_bDepthWriteEnabled = bEnabled;
	if (!bEnabled) {
		m_lBits &= ~RS_DEPTH_WRITE;
	}
	else {
		m_lBits |= RS_DEPTH_WRITE;
	}
}
///////////////////////////////////////////////////////////////////////////
//StateBlock
///////////////////////////////////////////////////////////////////////////