#include "Engine/Base.h"
#include "Engine/Material.h"
//#include "Effect.h"
#include "Engine/Technique.h"
#include "Engine/Pass.h"
#include "Engine/Node.h"
#include "Engine/Properties.h"
#include "Engine/MaterialReader.h"
#include "Engine/Texture.h"
#include "Engine/MaterialParameter.h"

void resolveFilenameAndMaterialNamespace(CCString& sUrl, CCString& sFileString, CCString& sNamespace) {

	// Find pos of "#".
	int iHashPos = sUrl.lastIndexOf("#");
	if (iHashPos >= 0)
	{
		sFileString = sUrl.substr(0, iHashPos);
		sNamespace = sUrl.substr(iHashPos + 1);
	}
	else {
		sFileString = sUrl;
	}
}

Material::Material() 
	:	m_pCurrentTechnique(NULL)
{

}

Material::~Material() {

	// Destroy all the techniques.
	for(size_t i = 0; i < m_vTechniques.size(); i++) {

		Technique* technique = m_vTechniques[i];
		SAFE_DELETE( technique );
	}
}

Material* Material::create(const char* url) {

	// Create Material from the string which has a following format:
	// res/sample.material#box
	// Todo: eg: res/sample.material#box/technique/pass/sampler
	CCString sUrlString = url;
	CCString sFilenameString;
	CCString sNamespace;
	resolveFilenameAndMaterialNamespace(sUrlString, sFilenameString, sNamespace);

	// Load the material properties from file.
	MaterialReader* pMatReader = new MaterialReader();
	Properties* pProperties = pMatReader->read(sFilenameString.c_str());
	if(pProperties == NULL) {
		GP_ERROR("Failed to create material from file");
		return NULL;
	}

	Material* pMaterial = NULL;
	Properties* pMaterialNamespace = NULL;

	if (strcmp(sNamespace.c_str(), "") != 0)
	{
		pMaterialNamespace = pProperties->getNamespace(sNamespace.c_str());
		if (pMaterialNamespace != NULL) {

			pMaterial = create(pMaterialNamespace);
		}
	}
	else // If no material namespace is specified in the url (i.e "res/sample.material"), then load the first material we encounter. 
	{ 
		pMaterial = create(pProperties->getNextNamespace());
	}

	GP_ASSERT( pMaterial );

	SAFE_DELETE( pProperties );
	SAFE_DELETE( pMatReader );

	return pMaterial;
}

Material* Material::create(Properties* pMaterialProperties) {

	// Check if the Properties is valid and has a valid namespace.
	if( !pMaterialProperties || ( !strcmp(pMaterialProperties->getNamespaceType(), "material") == 0 ) ) {

		GP_ERROR("Properties object must be non-null and have namespace equal to 'material'.");
		return NULL;
	}

	// Create new material from the file passed in.
	Material* pMaterial = new Material();

	// Go through all the material properties and create techniques under this material.
	Properties* pTechniqueProperties = NULL;
	while( pTechniqueProperties = pMaterialProperties->getNextNamespace() ) {

		if( strcmp(pTechniqueProperties->getNamespaceType(), "technique") == 0 ) {

			if( !loadTechnique(pMaterial, pTechniqueProperties) ) {

				GP_ERROR("Failed to load technique for material.");
				SAFE_DELETE( pMaterial );
				return NULL;
			}
		}
	}

	// Load uniform value parameters for this material.
	loadRenderState(pMaterial, pMaterialProperties);

	// Set the current technique to the first found technique.
	if(pMaterial->getTechniqueCount() > 0) {

		Technique* t = pMaterial->getTechniqueByIndex(0);
		if(t) {
			pMaterial->m_pCurrentTechnique = t;
		}
	}

	return pMaterial;
}

Material* Material::create(Effect* pEffect) {

	GP_ASSERT(pEffect);

	// Create a new material with a single technique and pass for the given effect.
	Material* material = new Material();

	Technique* technique = new Technique("", material);
	material->m_vTechniques.push_back(technique);

	Pass* pPass = new Pass(NULL, technique);
	pPass->m_pEffect = pEffect;
	technique->m_vPasses.push_back(pPass);
	//pEffect->addRef();

	material->m_pCurrentTechnique = technique;

	return material;
}


bool Material::loadTechnique(Material* pMaterial, Properties* pTechniqueProperties) {
	
	GP_ASSERT( pMaterial );
	GP_ASSERT( pTechniqueProperties );

	// Create a new technique.
	Technique* pTechnique = new Technique(pTechniqueProperties->getID(), pMaterial);

	// Go through all the properties and create passes under this technique.
	pTechniqueProperties->rewind();
	Properties* pPassProperties = NULL;
	while( pPassProperties = pTechniqueProperties->getNextNamespace() ) {

		if( strcmp(pPassProperties->getNamespaceType(), "pass") == 0 ) {

			// Create and load passes.
			if( !loadPass(pTechnique, pPassProperties) ) {

				GP_ERROR("Failed to create pass for technique.");
				SAFE_DELETE(pTechnique);
				return false;
			}
		}
	}

	// Load uniform value parameters for this technique.
	loadRenderState(pTechnique, pTechniqueProperties);

	// Add the new technique to the material.
	pMaterial->m_vTechniques.push_back(pTechnique);

	return true;
}

bool Material::loadPass(Technique* pTechnique, Properties* pPassProperties) {

	GP_ASSERT( pTechnique );
	GP_ASSERT( pPassProperties );
	
	// Fetch shader info required to create the effect of this technique.
	const char* sVertexSahaderPath = pPassProperties->getString("vertexShader");
	GP_ASSERT( sVertexSahaderPath );

	const char* sFragmentShaderPath = pPassProperties->getString("fragmentShader");
	GP_ASSERT( sFragmentShaderPath );

	const char* sDefines = pPassProperties->getString("defines");

	// Create the pass.
	Pass* pPass = new Pass(pTechnique->getId(), pTechnique);

	// Load render state.
	loadRenderState(pPass, pPassProperties);

	if (!pPass->initialize(sVertexSahaderPath, sFragmentShaderPath, sDefines)) {

		GP_ERROR("Failed to create pass for technique.");
		SAFE_DELETE(pPass);
		return false;
	}

	// Add the new pass to the technique.
	pTechnique->m_vPasses.push_back(pPass);

	return true;
}

static Texture::Wrap parseTextureWrapMode(const char* str, Texture::Wrap defaultValue) {

	if(str == NULL || strlen(str) == 0) {
		GP_ERROR("Texture wrap mode string must be non-null and non-empty.");
		return defaultValue;
	}
	else
	if(strcmp(str, "REPEAT") == 0)
		return Texture::REPEAT;
	else
	if(strcmp(str, "CLAMP") == 0)
		return Texture::CLAMP;
	else
	{
		GP_ERROR("Unsupported texture wrap mode string ('%s').", str);
		return defaultValue;
	}
}

static Texture::Filter parseTextureFilterMode(const char* str, Texture::Filter defaultValue) {

	if(str == NULL || strlen(str) == 0) {
		GP_ERROR("Texture filter mode string must be non-null and non-empty.");
		return defaultValue;
	}
	else
	if(strcmp(str, "NEAREST") == 0)
		return Texture::NEAREST;
	else
	if(strcmp(str, "LINEAR") == 0)
		return Texture::LINEAR;
	else
	if(strcmp(str, "NEAREST_MIPMAP_NEAREST") == 0)
		return Texture::NEAREST_MIPMAP_NEAREST;
	else
	if(strcmp(str, "LINEAR_MIPMAP_NEAREST") == 0)
		return Texture::LINEAR_MIPMAP_NEAREST;
	else
	if(strcmp(str, "NEAREST_MIPMAP_LINEAR") == 0)
		return Texture::NEAREST_MIPMAP_LINEAR;
	else
	if(strcmp(str, "LINEAR_MIPMAP_LINEAR") == 0)
		return Texture::LINEAR_MIPMAP_LINEAR;
	else {
		GP_ERROR("Unsupported texture filter mode string ('%s').", str);
		return defaultValue;
	}
}

static bool isMaterialKeyword(const char* sKeyword) {

	GP_ASSERT( sKeyword );

	#define MATERIAL_KEYWORD_COUNT 3
	static const char* reservedKeywords[MATERIAL_KEYWORD_COUNT] =
	{
		"vertexShader",
		"fragmentShader",
		"defines"
	};

	for (unsigned int i = 0; i < MATERIAL_KEYWORD_COUNT; i++) {
		if (strcmp(sKeyword, reservedKeywords[i]) == 0) {
			return true;
		}
	}

	return false;
}

void Material::loadRenderState(RenderState* pRenderState, Properties* pProperties) {

	GP_ASSERT( pRenderState );
	GP_ASSERT( pProperties );

	// Rewind the properties to start reading from the start.
	pProperties->rewind();

	const char* sName;
	// 1.
	while ((sName = pProperties->getNextProperty())) {

		// Skip the keywords
		if (isMaterialKeyword(sName))
			continue;

		MaterialParameter* pMaterialParameter = pRenderState->getParameter(sName);

		switch (pProperties->getType(sName)) {

			case Properties::NUMBER:
				GL_ASSERT(pMaterialParameter);
				pMaterialParameter->setValue(pProperties->getFloat(sName));
			break;
			case Properties::VECTOR2:
			{
				Vector2 vector2;
				if (pProperties->getVector2(sName, &vector2)) {
					pMaterialParameter->setValue(vector2);
				}
			}
			break;				
			case Properties::VECTOR3:
			{
				Vector3 vector3;
				if (pProperties->getVector3(sName, &vector3)) {
					pMaterialParameter->setValue(vector3);
				}
			}
			break;				
			case Properties::VECTOR4:
			{
				Vector4 vector4;
				if (pProperties->getVector4(sName, &vector4)) {
					pMaterialParameter->setValue(vector4);
				}
			}
			break;				
			case Properties::MATRIX:
			{
				Matrix4 mat4;
				if (pProperties->getMatrix(sName, &mat4)) {
					pMaterialParameter->setValue(mat4);
				}
			}
			break;
			default:
            {
                // Assume this is a parameter auto-binding.
				pRenderState->setParameterAutoBinding(sName, pProperties->getString(sName));
            }
            break;
		}
	}

	// 2. Iterate through all child namespaces searching for samplers and render state blocks.
	Properties* ns;
	while( ns = pProperties->getNextNamespace()) {

		if( strcmp(ns->getNamespaceType(), "sampler") == 0 ) {

			// Read the texture uniform name.
			sName = ns->getID();
			if(strlen(sName) == 0) {
				GP_ERROR("Texture sampler is missing required uniform name.");
				continue;
			}

			// Get the texture path.
			const char* sPath = ns->getString("path");
			if(sPath == NULL || strlen(sPath) == 0) {
				GP_ERROR("Texture sampler '%s' is missing required image file path.", name);
				continue;
			}

			// Read texture state (booleans default to 'false' if not present).
			const char* sValue;
			sValue = ns->getString("mipmap");
			bool bMipmap = ((strcmp( sValue, "true") == 0)?true:false);

			Texture::Wrap wrapS = parseTextureWrapMode(ns->getString("wrapS"), Texture::REPEAT);
			Texture::Wrap wrapT = parseTextureWrapMode(ns->getString("wrapT"), Texture::REPEAT);

			Texture::Filter minFilter = parseTextureFilterMode(ns->getString("minFilter"), bMipmap ? Texture::NEAREST_MIPMAP_LINEAR: Texture::LINEAR);
			Texture::Filter magFilter = parseTextureFilterMode(ns->getString("magFilter"), Texture::LINEAR);

			// Set the sampler parameter.
			MaterialParameter* pMaterialParameter = pRenderState->getParameter(sName);
			GP_ASSERT( pMaterialParameter );
			Texture::Sampler* pSampler = pMaterialParameter->setValue(sPath, bMipmap);
			GP_ASSERT( pSampler );
			if(pSampler) {
				pSampler->setWrapMode(wrapS, wrapT);
				pSampler->setFilterMode(minFilter, magFilter);
			}
		}
		else 
		if( strcmp(ns->getNamespaceType(), "renderState") == 0 ) {

			while ((sName = ns->getNextProperty())) {
				GP_ASSERT(pRenderState->getStateBlock());
				pRenderState->getStateBlock()->setState(sName, ns->getString(sName));
			}
		}
	}
}

unsigned int Material::getTechniqueCount() const {
	
	return (unsigned int)m_vTechniques.size();
}

Technique* Material::getTechniqueByIndex(unsigned int index) const {

	GP_ASSERT( index < m_vTechniques.size() && index >= 0 );
	return m_vTechniques[index];
}

Technique* Material::getTechnique(const char* id) const {

	GP_ASSERT( id );
	for(size_t i = 0, count = m_vTechniques.size(); i < count; i++) {

		Technique* technique = m_vTechniques[i];
		GP_ASSERT( technique );
		if(strcmp(technique->getId(), id) == 0) {
			return technique;
		}
	}

	return NULL;
}

Technique* Material::getTechnique() const {

	return m_pCurrentTechnique;
}

void Material::setTechnique(const char* id) {

	Technique* technique = getTechnique(id);
	if(technique) {
		m_pCurrentTechnique = technique;
	}
}

void Material::setNodeBinding(Node* node) {

	RenderState::setNodeBinding(node);

	for (size_t i = 0, count = m_vTechniques.size(); i < count; ++i) {

		m_vTechniques[i]->setNodeBinding(node);
	}
}
