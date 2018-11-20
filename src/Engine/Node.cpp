#include "Engine/Node.h"
#include "Engine/Model.h"
#include "Engine/Camera.h"
#include "Common/Matrices.h"

Node::Node(const char* id)
	:	m_pModel(NULL),
		m_pCamera(NULL),
		m_pParent(NULL),
		m_pFirstChild(NULL),
		m_pLastChild(NULL),
		m_iChildCount(NULL),
		m_pNextSibling(NULL),
		m_pPrevSibling(NULL)
{
	if(id) {
		m_sID = id;
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

Node* Node::getFirstChild() const {
	return m_pFirstChild;
}

Node* Node::getLastChild() const {
	return m_pLastChild;
}

Node* Node::getNextSibling() const {
	return m_pNextSibling;
}

Node* Node::getPrevSibling() const {
	return m_pPrevSibling;
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
		m_pFirstChild->m_pPrevSibling = pChild;
		pChild->m_pNextSibling = m_pFirstChild;
		m_pFirstChild = pChild;
	}
	else 
		m_pFirstChild = pChild;

	pChild->m_pParent = this;
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
		m_pPrevSibling->m_pNextSibling = m_pNextSibling;
	}
	if(m_pNextSibling) {
		m_pNextSibling->m_pPrevSibling = m_pPrevSibling;
	}

	Node* pParent = m_pParent;
	if(pParent) {
		if(this == pParent->m_pFirstChild) {
			pParent->m_pFirstChild = m_pNextSibling;
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
		Matrix4::multiply(parent->getWorldMatrix(), getMatrix(), &m_MatrixWorld);
	}
	else
	{
		m_MatrixWorld = getMatrix();
	}

	return m_MatrixWorld;
}

Matrix4& Node::getWorldViewMatrix() {
	static Matrix4 worldView;

	Matrix4::multiply(getViewMatrix(), getWorldMatrix(), &worldView);

	return worldView;
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

Node::~Node() {
	removeAllChildren();

	if(m_pModel)
		m_pModel->setNode(NULL);
	if(m_pCamera)
		m_pCamera->setNode(NULL);

	SAFE_DELETE( m_pModel );
	SAFE_DELETE( m_pCamera );
}