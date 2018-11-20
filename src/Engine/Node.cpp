#include "Engine/Scene.h"
#include "Engine/Node.h"
#include "Engine/Model.h"
#include "Engine/Camera.h"
#include "Common/Matrices.h"

Node::Node(const char* id)
	:	m_pScene(NULL),
		m_pModel(NULL),
		m_pCamera(NULL),
		m_pParent(NULL),
		m_pFirstChild(NULL),
		m_pLastChild(NULL),
		m_iChildCount(NULL),
		m_pNextSibling(NULL),
		m_pPrevSibling(NULL),

		m_MatrixWorld()
{
	if(id) {
		setID(id);
	}
}

Node* Node::create(const char* id) {
	return new Node(id);
}

void Node::setID(const char* id) {
	if(id) {
		m_sID = id;
	}
}

const char* Node::getID() {
	return m_sID.c_str();
}

unsigned int Node::getChildCount() const {
	return m_iChildCount;
}

Node* Node::getParent() const{
	return m_pParent;
}

void Node::setParent(Node* pParent) {
	m_pParent = pParent;

	Scene* pScene = pParent->getScene();
	setScene(pScene);
}

Node* Node::getFirstChild() const {
	return m_pFirstChild;
}

void Node::setFirstChild(Node* pNode) {
	m_pFirstChild = pNode;
}

Node* Node::getLastChild() const {
	return m_pLastChild;
}

void Node::setLastChild(Node* pNode) {
	m_pLastChild = pNode;
}

Node* Node::getNextSibling() const {
	return m_pNextSibling;
}

void Node::setNextSibling(Node* pNode) {
	m_pNextSibling = pNode;;
}

Node* Node::getPrevSibling() const {
	return m_pPrevSibling;
}

void Node::setPrevSibling(Node* pNode) {
	m_pPrevSibling = pNode;
}

Node* Node::getRootNode() const {
	Node* n = const_cast<Node*>(this);
	while(n->getParent()) {
		n = n->getParent();
	}

	return n;
}

void Node::addChild(Node* pChild) {
	GP_ASSERT( pChild );

	if(pChild->m_pParent == this)
		return;

	if(pChild->m_pParent) {
		pChild->m_pParent->removeChild(pChild);
	}

	if(m_pFirstChild) {
		m_pFirstChild->setPrevSibling(pChild);
		pChild->setNextSibling(m_pFirstChild);
		setFirstChild(pChild);
	}
	else 
		setFirstChild(pChild);

	pChild->setParent(this);
	m_iChildCount++;
}

void Node::removeChild(Node* pChild) {
	if(pChild == NULL || pChild->m_pParent != this)
		return;

	pChild->remove();

	SAFE_DELETE( pChild );
}

void Node::remove() {
	if(m_pPrevSibling) {
		m_pPrevSibling->setNextSibling(m_pNextSibling);
	}
	if(m_pNextSibling) {
		m_pNextSibling->setPrevSibling(m_pPrevSibling);
	}

	Node* pParent = m_pParent;
	if(pParent) {
		if(this == pParent->m_pFirstChild) {
			pParent->setFirstChild(m_pNextSibling);
		}

		--pParent->m_iChildCount;
	}

	m_pNextSibling = NULL;
	m_pPrevSibling = NULL;

	m_pParent = NULL;
}

void Node::removeAllChildren() {
	while(m_pFirstChild) {
		removeChild(m_pFirstChild);
	}
}

Matrix4& Node::getViewMatrix() const {
	//Get Scene Camera here...

	if(m_pCamera) {
		return m_pCamera->getViewMatrix();
	}
	else {
		return Matrix4::identity();
	}
}

Matrix4& Node::getProjectionMatrix() const {

	if(m_pCamera) {
		return m_pCamera->getProjectionMatrix();
	}
	else {
		return Matrix4::identity();
	}
}

Matrix4& Node::getWorldMatrix() {

	// If we have a parent, multiply our parent world transform by our local
	// transform to obtain our final resolved world transform.
	Node* parent = getParent();
	if (parent) {
		Matrix4::multiply(	(m_pCamera)?getTransformedViewMatrix():getTransformedModelMatrix(),
									parent->getWorldMatrix(),
									&m_MatrixWorld);

		//Matrix4::multiply(parent->getWorldMatrix(), getTransformedModelMatrix(), &m_MatrixWorld);
	}
	else
	{
		m_MatrixWorld = (m_pCamera)?getTransformedViewMatrix():getTransformedModelMatrix();
	}

	return m_MatrixWorld;
}

Matrix4& Node::getWorldViewMatrix() {
	static Matrix4 worldView;

	Matrix4::multiply(getViewMatrix(), getWorldMatrix(), &worldView);

	return worldView;
}

Scene* Node::getScene() const {
	return m_pScene;
}

void Node::setScene(Scene* pScene) {
	GP_ASSERT( pScene );
	
	m_pScene = pScene;
}

Model* Node::getModel() const {
	return m_pModel;
}

void Node::setModel(Model* pModel) {
	if(pModel != m_pModel) {
		if(m_pModel) {
			m_pModel->setNode(NULL);
			SAFE_DELETE( m_pModel );
		}
		
		m_pModel = pModel;

		if(m_pModel) {
			m_pModel->setNode(this);
		}
	}
}

Camera*	Node::getCamera() const {
	return m_pCamera;
}

void Node::setCamera(Camera* pCamera) {
	if(pCamera != m_pCamera) {
		if(m_pCamera) {
			m_pCamera->setNode(NULL);
			SAFE_DELETE( m_pCamera );
		}

		m_pCamera = pCamera;

		if(m_pCamera) {
			m_pCamera->setNode(this);
		}
	}
}

void Node::render(bool bWireframe) {
	if(m_pModel) {
		Scene* pScene = getScene();
		Node* pCameraNode = pScene->getActiveCamera()->getNode();

		Matrix4 worldView = Matrix4::identity();
		worldView = pCameraNode->getViewMatrix() * getWorldMatrix();
		glLoadMatrixf(worldView.getTranspose());

		m_pModel->draw(bWireframe);

		for(Node* node = m_pFirstChild; node != NULL; node = node->getNextSibling()) {
			node->render(bWireframe);
		}
	}
}

Node::~Node() {
	removeAllChildren();

	if(m_pModel)
		m_pModel->setNode(NULL);
	if(m_pCamera)
		m_pCamera->setNode(NULL);

	SAFE_DELETE( m_pModel );
	SAFE_DELETE( m_pCamera );
}