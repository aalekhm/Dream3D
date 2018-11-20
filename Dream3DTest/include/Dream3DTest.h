#ifndef DREAM3DTEST_H
#define DREAM3DTEST_H

#include "Engine/EngineManager.h"
#include "Engine/Scene.h"
#include "Engine/Camera.h"
#include "Engine/Node.h"
#include "Engine/Light.h"
#include <Common/RandomAccessFile.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#define POINT_LIGHT_COUNT	1
#define SPOT_LIGHT_COUNT	1


class Logger {

	public:
								Logger();
		virtual					~Logger();
		static Logger*			create(const char* pLogFileName);

		void					log(unsigned int iIntVal) const;
		void					log(float fFloatVal) const;
		void					log(const Vector2& vVector2Val) const;
		void					log(const Vector3& vVector3Val) const;
	private:
		RandomAccessFile*		m_pLogFile;
};

class Dream3DTest : EngineManager
{
	public:
								Dream3DTest();
		virtual					~Dream3DTest();
		
		virtual void			initialize();
		virtual void			update(float elapsedTime);
		virtual void			render(float elapsedTime);

		void					render3D(float deltaTimeMs);
		void					render2D(float deltaTimeMs);

		virtual void			keyPressedEx(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void			keyReleasedEx(unsigned int iVirtualKeycode, unsigned short ch);

		virtual void			onMouseDownEx(int mCode, int x, int y);
		virtual void			onMouseMoveEx(int mCode, int x, int y);
		virtual void			onMouseUpEx(int mCode, int x, int y);
		virtual void			onMouseWheelEx(WPARAM wParam, LPARAM lParam);

	private:
		Scene*					m_pScene;

		void					processScene(aiMesh* pAIMesh, const aiScene* pAIScene, Vector3& vPosition, float fScale);
		void					loadSceneUsingAssimp(const char* pFilename, Vector3& vPosition, float fScale);
		void					recurseScene(aiNode* pAINode, const aiScene* pAIScene, Vector3& vPosition, float fScale);

		Logger*					m_pLogger;
		Light*					m_pPointLight[POINT_LIGHT_COUNT];

	private:
		void					moveLight();
};

#endif


