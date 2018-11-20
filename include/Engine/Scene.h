#ifndef SCENE_H
#define SCENE_H

#include "Engine/Base.h"
#include "Common/CCString.h"

class Node;
class Camera;

class Scene {

	public:
		static Scene*	create();

		const char*		getID();
		void			setID(const char* pID);

		Node*			addNode(const char* pID = NULL);
		void			addNode(Node* pNode);
		void			removeNode(Node* pNode);
		void			removeAllNodes();
		unsigned int	getNodeCount() const;
		Node*			getFirstNode();

		Camera*			getActiveCamera() const;
		void			setActiveCamera(Camera* pCamera);

		void			render();
	private:
		Scene();
		virtual ~Scene();

		CCString		m_sID;
		Camera*			m_pActiveCamera;
		Node*			m_pFirstNode;
		Node*			m_pLastNode;
		unsigned int	m_iNodeCount;
};

#endif