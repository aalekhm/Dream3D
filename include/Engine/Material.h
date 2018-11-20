#ifndef MATERIAL_H
#define MATERIAL_H

#include "Engine/RenderState.h"
#include "Engine/Technique.h"

class Material : RenderState {

	friend class Technique;
	friend class Pass;
	friend class RenderState;
	friend class Node;
	friend class Model;

	public:
		static Material* create(const char* url);
		//static Material* create(Properties* materialProperties);
		static Material* create(/*Effect* effect*/);
		//static Material* create(const char* vshPath, const char* fshPath, const char* defines = NULL);

		unsigned int	getTechniqueCount() const;
		Technique*		getTechniqueByIndex(unsigned int index) const;
		Technique*		getTechnique(const char* id) const;
		Technique*		getTechnique() const;
		void			setTechnique(const char* id);
	private:
		Material();
		Material(const Material& m);
		~Material();

		//static bool loadTechnique(Material* material, Properties* techniqueProperties);
		//static bool loadPass(Technique* technique, Properties* passProperites);
		//static void loadRenderState(RenderState* renderState, Properties* properties);
		
		Technique*					m_pCurrentTechnique;
		std::vector<Technique*>		m_vTechniques;
};

#endif