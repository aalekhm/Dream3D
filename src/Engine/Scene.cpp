#include "Engine/Scene.h"
#include "Engine/Node.h"
#include "Engine/Camera.h"

Scene::Scene()
	:	m_sID(""),
		m_pActiveCamera(NULL),
		m_pFirstNode(NULL),
		m_pLastNode(NULL),
		m_iNodeCount(0)
{

}

Scene* Scene::create() {
	return new Scene();
}

const char*	Scene::getID() {
	return (const char*)m_sID.c_str();
}

void Scene::setID(const char* pID) {
	if(pID) {
		m_sID = pID;
	}
}

Node* Scene::addNode(const char* pID) {
	Node* pNode = Node::create(pID);
	GP_ASSERT( pNode );

	addNode(pNode);

	return pNode;
}

void Scene::addNode(Node* pNode) {
	GP_ASSERT( pNode );

	Scene* pScene = pNode->getScene();

	// The node is already a member of this scene, return.
	if(pScene == this)
		return;	

	// If the node is part of another scene, remove it.
	if(pScene && pScene != this) {
		pScene->removeNode(pNode);
	}

	// If the node is part of another node hierarchy, remove it.
	if(pNode->getParent()) {
		pNode->getParent()->removeChild(pNode);
	}

	// Link the new node into our list.
	if(m_pLastNode) {
		m_pLastNode->setNextSibling(pNode);
		pNode->setPrevSibling(m_pLastNode);
		m_pLastNode = pNode;
	}
	else {
		m_pFirstNode = m_pLastNode = pNode;
	}

	pNode->setScene(this);

	++m_iNodeCount;

	// If we don't have an active camera set, then check for one and set it.
	if(m_pActiveCamera == NULL) {
		Camera* pCamera = pNode->getCamera();
		if(pCamera) {
			setActiveCamera(pCamera);
		}
	}
}

void Scene::removeNode(Node* pNode) {
	GP_ASSERT( pNode );

	Scene* pScene = pNode->getScene();

	if(pScene != this)
		return;

	if(pNode == m_pFirstNode) {
		m_pFirstNode = pNode->getNextSibling();
	}

	if(pNode == m_pLastNode) {
		m_pLastNode = pNode->getPrevSibling();
	}

	pNode->remove();
	pNode->setScene(NULL);

	SAFE_DELETE( pNode );

	--m_iNodeCount;
}

void Scene::removeAllNodes() {
	while(m_pLastNode) {
		removeNode(m_pLastNode);
	}
}

unsigned int Scene::getNodeCount() const {
	return m_iNodeCount;
}

Node* Scene::getFirstNode() {
	return m_pFirstNode;
}

Camera* Scene::getActiveCamera() const {
	return m_pActiveCamera;
}

void Scene::setActiveCamera(Camera* pCamera) {
	GP_ASSERT( pCamera );

	if(pCamera != m_pActiveCamera) {

		if(m_pActiveCamera) {
			SAFE_DELETE( m_pActiveCamera );
		}

		m_pActiveCamera = pCamera;
	}
}

void Scene::render() {

	for(Node* node = m_pFirstNode; node != NULL; node = node->getNextSibling()) {
		node->render(!true);
	}
}

Scene::~Scene() {
	if(m_pActiveCamera) {
		SAFE_DELETE( m_pActiveCamera );
	}

	removeAllNodes();
}
