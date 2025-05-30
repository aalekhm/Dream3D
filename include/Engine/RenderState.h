#ifndef RENDERSTATE_H
#define RENDERSTATE_H

#include "Engine/Base.h"
#include "Common/Matrices.h"

class RenderState {

	friend class Material;
	friend class MaterialParameter;
	friend class Technique;
	friend class Pass;
	friend class Node;

	public:
		enum AutoBinding
		{
			NONE,
			WORLD_MATRIX,
			VIEW_MATRIX,
			PROJECTION_MATRIX,
			WORLD_VIEW_MATRIX,
			VIEW_PROJECTION_MATRIX,
			WORLD_VIEW_PROJECTION_MATRIX,
			INVERSE_TRANSPOSE_WORLD_MATRIX,
			INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX,
			CAMERA_WORLD_POSITION,
			CAMERA_VIEW_POSITION,
			MATRIX_PALETTE,
			SCENE_AMBIENT_COLOR
		};

		enum Blend {
			BLEND_ZERO = GL_ZERO,
			BLEND_ONE = GL_ONE,
			BLEND_SRC_COLOR = GL_SRC_COLOR,
			BLEN_ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR,
			BLEND_DST_COLOR = GL_DST_COLOR,
			BLEND_ONE_MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR,
			BLEND_SRC_ALPHA = GL_SRC_ALPHA,
			BLEND_ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
			BLEND_DST_ALPHA = GL_DST_ALPHA,
			BLEND_ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA,
			BLEND_CONSTANT_ALPHA = GL_CONSTANT_ALPHA,
			BLEND_ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA,
			BLEND_SRC_ALPHA_SATURATE = GL_SRC_ALPHA_SATURATE
		};

		/*
		* Defines a block of fixed-function render states that can be applied to a
		* RenderState object.
		*/
		class StateBlock {
			
			friend class RenderState;
			public:
				static StateBlock*		create();
				void					bind();

				void					setBlend(bool bEnabled);
				void					setBlendSrc(Blend blend);
				void					setBlendDst(Blend blend);

				void					setCullFace(bool bEnabled);

				void					setDepthTest(bool bEnabled);

				void					setDepthWrite(bool bEnabled);

				void					setState(const char* name, const char* value);

			private:
				StateBlock();
				StateBlock(const StateBlock& copy);
				~StateBlock();

				void					bindNoRestore();
				static void				restore(long stateOverrideBits);
				static void				enableDepthWrite();

				// States
				bool					m_bBlendEnabled;
				Blend					m_eBlendSrc;
				Blend					m_eBlendDst;

				bool					m_bCullFaceEnabled;
				bool					m_bDepthTestEnabled;
				bool					m_bDepthWriteEnabled;
				
				long					m_lBits;

				static StateBlock*		m_pDefaultState;
		};

		void							setStateBlock(StateBlock* state);
		StateBlock*						getStateBlock() const;
		void							setNodeBinding(Node* node);
		MaterialParameter*				getParameter(const char* sName) const;

		const char*						autoBindingToString(RenderState::AutoBinding autoBinding);
		void							setParameterAutoBinding(const char* pName, AutoBinding autoBinding);
		void							setParameterAutoBinding(const char* pName, const char* pAutoBinding);

		static void						initialize();
	protected:
		RenderState();
		~RenderState();

		static void						finalize();
		
		void							bind(Pass* pass);
		RenderState*					getTopmost(RenderState* below);
		
	private:
		RenderState(const RenderState& copy);
		RenderState& operator=(const RenderState&);

		void							applyAutoBinding(const char* pUniformName, const char* pAutoBinding);

		const Matrix4&					autoBindingGetWorldMatrix() const;
		const Matrix4&					autoBindingGetViewMatrix() const;
		const Matrix4&					autoBindingGetProjectionMatrix() const;
		const Matrix4&					autoBindingGetWorldViewMatrix() const;
		const Matrix4&					autoBindingGetViewProjectionMatrix() const;
		const Matrix4&					autoBindingGetWorldViewProjectionMatrix() const;
		const Matrix4&					autoBindingGetInverseTransposeWorldMatrix() const;
		const Matrix4&					autoBindingGetInverseTransposeWorldViewMatrix() const;
		Vector3							autoBindingGetCameraWorldPosition() const;
		Vector3							autoBindingGetCameraViewPosition() const;
		const Vector3&					autoBindingGetAmbientColor() const;

		Node*							m_pNodeBinding;
		mutable	StateBlock*				m_pStateBlock;
		RenderState*					m_pParent;

		mutable std::vector<MaterialParameter*> m_vParameters;
		std::map<std::string, std::string>		m_mAutoBindings;
};


#endif