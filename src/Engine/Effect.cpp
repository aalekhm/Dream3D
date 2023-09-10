#include <Engine/Effect.h>
#include <Common/RandomAccessFile.h>

static std::map<std::string, Effect*>	__effectCache;
static Effect*							__currentEffect;

Effect::Effect()
: m_iProgram(0)
{

}

Effect::~Effect() {

	// Remove this effect from the cache.
	__effectCache.erase(m_sProgramID);

	// Free uniforms.
	// Todo

	if (m_iProgram) {

		// If our program object is currently bound, unbind it before we're destroyed.
		if (__currentEffect == this) {

			GL_ASSERT( glUseProgram(0) );
			__currentEffect = NULL;
		}

		GL_ASSERT( glDeleteProgram(m_iProgram) );
		m_iProgram = 0;
	}
}

Effect* Effect::createFromFile(const char* vshPath, const char* fshPath, const char* defines) {

	GP_ASSERT( vshPath );
	GP_ASSERT( fshPath );

	// Search the effect cache for a similar effect that is already loaded.
	std::string sUniqueId = vshPath;
	sUniqueId += ';';
	sUniqueId += fshPath;
	sUniqueId += ';';
	if (defines) {
		sUniqueId += defines;
	}

	std::map<std::string, Effect*>::iterator itr = __effectCache.find(sUniqueId);
	if (itr != __effectCache.end()) {

		// Found an existing effect with this id, increase its ref count and return;
		Effect* pEffect = (Effect*)itr->second;
		GP_ASSERT(pEffect);
		//pEffect->addRef();

		return pEffect;
	}

	// Read source from file
	//CCString sVshSource = "";
	std::string sVshSource = "";
	sVshSource = RandomAccessFile::readAll(vshPath);
	if (sVshSource.c_str() == NULL) {

		GP_ERROR("Failed to read vertex shader from file '%s'.", vshPath);
		return NULL;
	}

	std::string sFshSource = "";
	sFshSource = RandomAccessFile::readAll(fshPath);
	if (sFshSource.c_str() == NULL)
	{
		GP_ERROR("Failed to read fragment shader from file '%s'.", fshPath);
		return NULL;
	}

	Effect* pEffect = createFromSource(vshPath, sVshSource.c_str(), fshPath, sFshSource.c_str(), defines);
	if (pEffect == NULL) {
		GP_ERROR("Failed to create effect from shaders '%s' & '%s'", vshPath, fshPath);
	}
	else {
		// Store this effect in the cache.
		pEffect->m_sProgramID = sUniqueId;
		__effectCache[sUniqueId] = pEffect;
	}

	return pEffect;
}

Effect* Effect::createFromSource(const char* vshSource, const char* fshSource, const char* defines) {

	return createFromSource(NULL, vshSource, NULL, fshSource, defines);
}

static void replaceDefines(const char* sDefines, CCString& sOut) {

	if (sDefines) {
		sOut += "#define ";
		sOut += sDefines;
	}

	// Replace semicolons
	if (sOut.length() > 0) {

		sOut.replace(";", "\n#define ");
	}
}

void Effect::QueryAndStoreVertexAttribsMetaData(Effect* pEffect) {

	// Query and store vertex attribute meta-data from the program.
	// NOTE: Rather than using glBindAttribLocation to explicitly specify our own
	// preferred attribute locations, we're going to query the locations that were
	// automatically bound by the GPU. While it can sometimes be convenient to use
	// glBindAttribLocation, some vendors actually reserve certain attribute indices
	// and therefore using this function can create compatibility issues between
	// different hardware vendors.
	
	GL_ASSERT( pEffect );

	GLuint iProgramID = pEffect->m_iProgram;

	GLint iActiveAttributeCount;
	GL_ASSERT( glGetProgramiv(iProgramID, GL_ACTIVE_ATTRIBUTES, &iActiveAttributeCount) );
	if (iActiveAttributeCount > 0) {

		GLint iLength;
		GL_ASSERT( glGetProgramiv(iProgramID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &iLength) );
		if (iLength > 0) {

			GLchar* sAttribName = new GLchar[iLength];
			GLint	iAttribSize;
			GLenum	eAttribType;
			GLint	iAttribLocation;
			for (int i = 0; i < iActiveAttributeCount; i++) {

				// Query attribute info
				GL_ASSERT( glGetActiveAttrib(iProgramID, i, iLength, NULL, &iAttribSize, &eAttribType, sAttribName) );
				sAttribName[iLength] = '\0';

				// Query the pre-assigned attribute location
				GL_ASSERT( iAttribLocation = glGetAttribLocation(iProgramID, sAttribName) );

				// Assign the Vertex Attribute mapping for the Effect.
				pEffect->m_mVertexAttributes[sAttribName] = iAttribLocation;
			}

			SAFE_DELETE_ARRAY( sAttribName );
		}
	}
}

void Effect::QueryAndStoreUniforms(Effect* pEffect) {

	// Query and store uniforms from the program.
	GL_ASSERT(pEffect);

	GLuint iProgramID = pEffect->m_iProgram;
	GLint iActiveUnifomCount;
	GL_ASSERT( glGetProgramiv(iProgramID, GL_ACTIVE_UNIFORMS, &iActiveUnifomCount) );
	if (iActiveUnifomCount > 0) {

		GLint iLength;
		GL_ASSERT(glGetProgramiv(iProgramID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &iLength));
		if (iLength > 0) {

			GLchar* sUniformName = new GLchar[iLength];
			GLint	iUniformSize;
			GLenum	eUniformType;
			GLint	iUniformLocation;
			unsigned int iSamplerIndex = 0;
			for (int i = 0; i < iActiveUnifomCount; i++) {

				// Query uniform name length
				//GLint iLen;
				//GL_ASSERT(glGetActiveUniformName(iProgramID, i, 1024, &iLen, NULL));

				// Query uniform info.
				GL_ASSERT( glGetActiveUniform(iProgramID, i, iLength, NULL, &iUniformSize, &eUniformType, sUniformName) );
				sUniformName[iLength - 1] = '\0';

				if (iLength > 3) {

					// If this is an array uniform, strip array indexers off it since GL does not
					// seem to be consistent across different drivers/implementations in how it returns
					// array uniforms. On some systems it will return "u_matrixArray", while on others
					// it will return "u_matrixArray[0]".
					char* ch = strrchr(sUniformName, '[');
					if (ch) {
						*ch = '\0';
					}
				}

				// Query the pre-assigned uniform location.
				GL_ASSERT( iUniformLocation = glGetUniformLocation(iProgramID, sUniformName) );

				Uniform* pUniform = new Uniform();
				pUniform->m_pEffect		= pEffect;
				pUniform->m_sName		= sUniformName;
				pUniform->m_iLocation	= iUniformLocation;
				pUniform->m_eType		= eUniformType;

				if (eUniformType == GL_SAMPLER_2D || eUniformType == GL_SAMPLER_CUBE) {

					pUniform->m_iIndex = iSamplerIndex;
					iSamplerIndex += iUniformSize;
				}
				else {
					pUniform->m_iIndex = 0;
				}

				pEffect->m_mUniforms[sUniformName] = pUniform;
			}

			SAFE_DELETE_ARRAY( sUniformName );
		}
	}
}

static void replaceIncludes(const char* pFilePath, const char* pSource, CCString& sOut) {

	// Replace the #include "xxxx.xxx" with the sourced file contents of "pFilepath/xxxx.xxx"
	CCString str = pSource;
	size_t iHeadPos = 0;
	size_t iLastHeadPos = 0;
	size_t iFileLength = str.length();
	while (iHeadPos < iFileLength) {

		iLastHeadPos = iHeadPos;
		iHeadPos = str.indexOf("#include", (int)iLastHeadPos);
		if (iHeadPos != -1) {
			
			sOut += str.substr(iLastHeadPos, iHeadPos);
			size_t iStartQuote = str.indexOf("\"", (int)iHeadPos + 1);
			if (iStartQuote != -1) {

				size_t iEndQuote = str.indexOf("\"", (int)iStartQuote + 1);
				if (iEndQuote != -1) {

					iHeadPos = iEndQuote + 1;
					CCString sFilePath = pFilePath;
					CCString sIncludeFile = str.substr(iStartQuote + 1, iEndQuote)->c_str();
					CCString sDirectoryPath = sFilePath.substr(0, sFilePath.lastIndexOf("/") + 1)->c_str();
					sDirectoryPath += sIncludeFile;

					std::string sSource = RandomAccessFile::readAll(sDirectoryPath.c_str());

					replaceIncludes(sDirectoryPath.c_str(), sSource.c_str(), sOut);
				}
				else {

					// We have started an "#include" but missing the leading quote "
					GP_ERROR("Compile failed for shader '%s' missing leading \".", pFilepath);
					return;
				}
			}
			else {

				// We have started an "#include" but missing the leading quote "
				GP_ERROR("Compile failed for shader '%s' missing leading \".", pFilepath);
				return;
			}
		}
		else {

			sOut += str.substr((int)iLastHeadPos);
		}
	}
}

Effect* Effect::createFromSource(const char* vshPath, const char* vshSource, const char* fshPath, const char* fshSource, const char* defines) {

	GP_ASSERT(vshSource);
	GP_ASSERT(fshSource);

	const unsigned int SHADER_SOURCE_LENGTH = 3;
	const GLchar* sShaderSource[SHADER_SOURCE_LENGTH];
	char* infoLog = NULL;

	GLuint iVertexShaderID;
	GLuint iFragmentShaderID;
	GLuint iProgramID;

	GLint iLength;
	GLint iSuccess;

	// Replace all comma separated definitions with #define prefix and \n suffix
	CCString sDefinesStr = "";
	replaceDefines(defines, sDefinesStr);

	sShaderSource[0] = sDefinesStr.c_str();
	sShaderSource[1] = "\n";

	//////////////////////////////////////// LOAD VERTEX SHADER //////////////////////////////////////////////
	CCString vshSourceStr = "";
	if (vshPath)
	{
		// Replace the #include "xxxxx.xxx" with the sources that come from file paths
		replaceIncludes(vshPath, vshSource, vshSourceStr);
		if (vshSource && strlen(vshSource) != 0)
			vshSourceStr += "\n";
	}
	sShaderSource[2] = vshPath ? vshSourceStr.c_str() : vshSource;
	//sShaderSource[2] = vshSource;

	GL_ASSERT( iVertexShaderID = glCreateShader(GL_VERTEX_SHADER) );
	GL_ASSERT( glShaderSource(iVertexShaderID, SHADER_SOURCE_LENGTH, sShaderSource, NULL) );
	GL_ASSERT( glCompileShader(iVertexShaderID) );

	GL_ASSERT( glGetShaderiv(iVertexShaderID, GL_COMPILE_STATUS, &iSuccess) );
	if (iSuccess != GL_TRUE) {

		GL_ASSERT( glGetShaderiv(iVertexShaderID, GL_INFO_LOG_LENGTH, &iLength) );
		if (iLength == 0)	iLength = 4096;
		if (iLength > 0) {

			infoLog = new char[iLength];
			GL_ASSERT( glGetShaderInfoLog(iVertexShaderID, iLength, NULL, infoLog) );
			infoLog[iLength - 1] = '\0';
		}

		// Write out the expanded shader file.
		if (vshPath) {
			CCString sErrFile = vshPath;
			sErrFile  += ".err";
			RandomAccessFile::writeAll(sErrFile.c_str(), infoLog);
		}

		GP_ERROR("Compile failed for vertex shader '%s' with error '%s'.", vshPath == NULL ? vshSource : vshPath, infoLog == NULL ? "" : infoLog);
		SAFE_DELETE_ARRAY(infoLog);

		// Clean up.
		GL_ASSERT( glDeleteShader(iVertexShaderID) );

		return NULL;
	}

	//////////////////////////////////////// LOAD FRAGMENT SHADER //////////////////////////////////////////////
	CCString fshSourceStr = "";
	if (fshPath)
	{
		// Replace the #include "xxxxx.xxx" with the sources that come from file paths
		replaceIncludes(fshPath, fshSource, fshSourceStr);
		if (fshSource && strlen(fshSource) != 0)
			fshSourceStr += "\n";
	}
	sShaderSource[2] = fshPath ? fshSourceStr.c_str() : fshSource;

	GL_ASSERT( iFragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER) );
	GL_ASSERT( glShaderSource(iFragmentShaderID, SHADER_SOURCE_LENGTH, sShaderSource, NULL) );
	GL_ASSERT( glCompileShader(iFragmentShaderID) );

	GL_ASSERT( glGetShaderiv(iFragmentShaderID, GL_COMPILE_STATUS, &iSuccess) );
	if (iSuccess != GL_TRUE) {

		GL_ASSERT( glGetShaderiv(iFragmentShaderID, GL_INFO_LOG_LENGTH, &iLength) );
		if (iLength == 0) iLength = 4096;
		if (iLength > 0) {

			infoLog = new char[iLength];
			GL_ASSERT( glGetShaderInfoLog(iFragmentShaderID, iLength, NULL, infoLog) );
			infoLog[iLength - 1] = '\0';
		}

		// Write out the expanded shader file.
		if (fshPath) {
			CCString sErrFile = fshPath;
			sErrFile += ".err";
			RandomAccessFile::writeAll(sErrFile.c_str(), infoLog);
		}

		GP_ERROR("Compile failed for fragment shader '%s' with error '%s'.", fshPath == NULL ? fshSource : fshPath, infoLog == NULL ? "" : infoLog);
		SAFE_DELETE_ARRAY(infoLog);

		// Clean up.
		GL_ASSERT(glDeleteShader(iFragmentShaderID));

		return NULL;
	}

	// Link Program
	GL_ASSERT( iProgramID = glCreateProgram() );
	GL_ASSERT( glAttachShader(iProgramID, iVertexShaderID) );
	GL_ASSERT( glAttachShader(iProgramID, iFragmentShaderID) );
	GL_ASSERT( glLinkProgram(iProgramID) ); 
	GL_ASSERT( glGetProgramiv(iProgramID,	GL_LINK_STATUS, &iSuccess) );

	// Delete shaders after linking
	GL_ASSERT( glDeleteShader(iVertexShaderID) );
	GL_ASSERT( glDeleteShader(iFragmentShaderID) );

	if (iSuccess != GL_TRUE) {

		GL_ASSERT( glGetProgramiv(iProgramID, GL_INFO_LOG_LENGTH, &iLength) );
		if (iLength == 0)	iLength = 4096;
		if (iLength > 0) {

			infoLog = new char[iLength];
			GL_ASSERT( glGetProgramInfoLog(iProgramID, iLength, NULL, infoLog) );
			infoLog[iLength - 1] = '\0';
		}

		GP_ERROR("Linking program failed (%s,%s): %s", vshPath == NULL ? "NULL" : vshPath, fshPath == NULL ? "NULL" : fshPath, infoLog == NULL ? "" : infoLog);
		SAFE_DELETE_ARRAY(infoLog);

		// cleanup
		GL_ASSERT( glDeleteShader(iProgramID) ); 
	}

	// Create & return the new Effect;
	Effect* pEffect = new Effect();
	pEffect->m_iProgram = iProgramID;

	QueryAndStoreVertexAttribsMetaData(pEffect);
	QueryAndStoreUniforms(pEffect);

	return pEffect;
}

const char*	Effect::getID() const {

	return m_sProgramID.c_str();
}

VertexAttribute Effect::getVertexAttribute(const char* name) const {

	std::map<std::string, VertexAttribute>::const_iterator itr = m_mVertexAttributes.find(name);
	return (itr == m_mVertexAttributes.end() ? -1 : itr->second);
}

Uniform* Effect::getUniform(const char* sUniformName) const {

	std::map<std::string, Uniform*>::const_iterator itr = m_mUniforms.find(sUniformName);

	if (itr != m_mUniforms.end()) {

		// Return cached uniform variable
		return itr->second;
	}

	GLint iUniformLocation;
	GL_ASSERT( iUniformLocation = glGetUniformLocation(m_iProgram, sUniformName) );
	if (iUniformLocation > -1) {

		// Check for array uniforms ("u_directionalLightColor[0]" -> "u_directionalLightColor")
		char* sParentName = new char[strlen(sUniformName) + 1];
		strcpy(sParentName, sUniformName);
		if (strtok(sParentName, "[") != NULL) {

			std::map<std::string, Uniform*>::const_iterator itr = m_mUniforms.find(sParentName);
			if (itr != m_mUniforms.end()) {

				Uniform* pUniform = itr->second;

				Uniform* pUniformNew = new Uniform();
				pUniformNew->m_pEffect = const_cast<Effect*>(this);
				pUniformNew->m_sName = sUniformName;
				pUniformNew->m_iLocation = iUniformLocation;
				pUniformNew->m_iIndex = 0;
				pUniformNew->m_eType = pUniform->getType();
				m_mUniforms[sUniformName] = pUniformNew;

				SAFE_DELETE_ARRAY(sParentName);
				return pUniformNew;
			}
		}

		SAFE_DELETE_ARRAY(sParentName);
	}

	// No uniform variable found - return NULL
	return NULL;
}

Uniform* Effect::getUniform(unsigned int iIndex) const {

	unsigned int i = 0;
	for (std::map<std::string, Uniform*>::const_iterator itr = m_mUniforms.begin(); itr != m_mUniforms.end(); ++itr, ++i) {
		
		if (i == iIndex) {
			return itr->second;
		}
	}

	return NULL;
}

unsigned int Effect::getUniformCount() const {

	return (unsigned int)m_mUniforms.size();
}

void Effect::setValue(Uniform* pUniform, float value) {

	GP_ASSERT( pUniform );
	GL_ASSERT( glUniform1f(pUniform->m_iLocation, value) );
}

void Effect::setValue(Uniform* pUniform, const float* values, unsigned int count) {

	GP_ASSERT( pUniform );
	GP_ASSERT( values );
	GL_ASSERT( glUniform1fv(pUniform->m_iLocation, count, values) );
}

void Effect::setValue(Uniform* pUniform, int value) {

	GP_ASSERT( pUniform);
	GL_ASSERT( glUniform1i(pUniform->m_iLocation, value) );
}

void Effect::setValue(Uniform* pUniform, const int* values, unsigned int count) {

	GP_ASSERT( pUniform);
	GP_ASSERT( values );
	GL_ASSERT( glUniform1iv(pUniform->m_iLocation, count, values) );
}

void Effect::setValue(Uniform* pUniform, const Matrix4& value) {

	GP_ASSERT( pUniform );
	GL_ASSERT( glUniformMatrix4fv(pUniform->m_iLocation, 1, GL_FALSE, value.m) );
}

void Effect::setValue(Uniform* pUniform, const Matrix4* values, unsigned int count) {

	GP_ASSERT( pUniform);
	GP_ASSERT( values );
	GL_ASSERT( glUniformMatrix4fv(pUniform->m_iLocation, count, GL_FALSE, (GLfloat*)values) );
}

void Effect::setValue(Uniform* pUniform, const Vector2& value) {

	GP_ASSERT( pUniform);
	GL_ASSERT( glUniform2f(pUniform->m_iLocation, value.x, value.y) );
}

void Effect::setValue(Uniform* pUniform, const Vector2* values, unsigned int count) {

	GP_ASSERT( pUniform );
	GP_ASSERT( values );
	GL_ASSERT( glUniform2fv(pUniform->m_iLocation, count, (GLfloat*)values) );
}

void Effect::setValue(Uniform* pUniform, const Vector3& value) {

	GP_ASSERT( pUniform );
	GL_ASSERT( glUniform3f(pUniform->m_iLocation, value.x, value.y, value.z) );
}

void Effect::setValue(Uniform* pUniform, const Vector3* values, unsigned int count) {

	GP_ASSERT(	pUniform );
	GP_ASSERT(	values );
	GL_ASSERT(	glUniform3fv(pUniform->m_iLocation, count, (GLfloat*)values) );
}

void Effect::setValue(Uniform* pUniform, const Vector4& value) {

	GP_ASSERT( pUniform );
	GL_ASSERT( glUniform4f(pUniform->m_iLocation, value.x, value.y, value.z, value.w) );
}

void Effect::setValue(Uniform* pUniform, const Vector4* values, unsigned int count) {

	GP_ASSERT( pUniform );
	GP_ASSERT( values );
	GL_ASSERT( glUniform4fv(pUniform->m_iLocation, count, (GLfloat*)values) );
}

void Effect::setValue(Uniform* pUniform, const Texture::Sampler* sampler) {

	GP_ASSERT( pUniform );
	GP_ASSERT( pUniform->m_eType == GL_SAMPLER_2D || pUniform->m_eType == GL_SAMPLER_CUBE);
	GP_ASSERT( sampler );
	GP_ASSERT(	(sampler->getTexture()->getType() == Texture::TEXTURE_2D && pUniform->m_eType == GL_SAMPLER_2D) 
				||
				(sampler->getTexture()->getType() == Texture::TEXTURE_CUBE && pUniform->m_eType == GL_SAMPLER_CUBE) );

	GL_ASSERT( glActiveTexture(GL_TEXTURE0 + pUniform->m_iIndex) );

	// Bind the sampler - this binds the texture and applies sampler state
	const_cast<Texture::Sampler*>(sampler)->bind();

	GL_ASSERT( glUniform1i(pUniform->m_iLocation, pUniform->m_iIndex) );
}

void Effect::setValue(Uniform* pUniform, const Texture::Sampler** values, unsigned int count) {

	GP_ASSERT(	pUniform );
	GP_ASSERT(	pUniform->m_eType == GL_SAMPLER_2D || pUniform->m_eType == GL_SAMPLER_CUBE );
	GP_ASSERT(	values );

	// Set samplers as active and load texture unit array
	GLint units[32];
	for (unsigned int i = 0; i < count; i++) {

		GP_ASSERT(	(const_cast<Texture::Sampler*>(values[i])->getTexture()->getType() == Texture::TEXTURE_2D && pUniform->m_eType == GL_SAMPLER_2D) 
					||
					(const_cast<Texture::Sampler*>(values[i])->getTexture()->getType() == Texture::TEXTURE_CUBE && pUniform->m_eType == GL_SAMPLER_CUBE));

		GL_ASSERT( glActiveTexture(GL_TEXTURE0 + pUniform->m_iIndex + i) );

		// Bind the sampler - this binds the texture and applies sampler state
		const_cast<Texture::Sampler*>(values[i])->bind();

		units[i] = pUniform->m_iIndex + i;
	}

	// Pass texture unit array to GL
	GL_ASSERT( glUniform1iv(pUniform->m_iLocation, count, units) );
}

void Effect::bind() {

	GL_ASSERT( glUseProgram(m_iProgram) );
	__currentEffect = this;
}

Effect*	Effect::getCurrentEffect() {

	return __currentEffect;
}

Uniform::Uniform()
: m_iLocation(-1)
, m_eType(0)
, m_iIndex(0)
, m_pEffect(NULL)
{
}

Uniform::~Uniform() {
	// hidden
}

