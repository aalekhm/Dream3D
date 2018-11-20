#ifndef MATERIAL_H
#define MATERIAL_H

#include "Engine/RenderState.h"

class Technique;
class Pass;
class Properties;

class Material : public RenderState {
	public:
		~Material();

		static Material* create(const char* url);
		static Material* create(Properties* pMaterialProperties);
		static Material* create(/*Effect* effect*/);
		//static Material* create(const char* vshPath, const char* fshPath, const char* defines = NULL);

		unsigned int	getTechniqueCount() const;
		Technique*		getTechniqueByIndex(unsigned int index) const;
		Technique*		getTechnique(const char* id) const;
		Technique*		getTechnique() const;
		void				setTechnique(const char* id);
	private:
		Material();
		Material(const Material& m);

		static bool		loadTechnique(Material* pMaterial, Properties* pTechniqueProperties);
		static bool		loadPass(Technique* pTechnique, Properties* pPassProperties);
		static void		loadRenderState(RenderState* pRenderState, Properties* pProperties);
		
		Technique*							m_pCurrentTechnique;
		std::vector<Technique*>		m_vTechniques;
};

#endif