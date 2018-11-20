#ifndef NODE_H
#define NODE_H

#include "Base.h"
#include "Engine/Transform.h"
#include <Common/CCString.h>

class Camera;
class Model;
struct Matrix4;

class Node : public Transform {

	public:
		Node(const char* id);

		static Node*	create(const char* id);
		void			setID(const char* id);
		const char*		getID();

		Model*			getModel() const;
		void			setModel(Model* pModel);

		Camera*			getCamera() const;
		void			setCamera(Camera* pCamera);

		unsigned int	getChildCount() const;
		Node*			getParent() const;
		Node*			getFirstChild() const;
		Node*			getLastChild() const;
		Node*			getNextSibling() const;
		Node*			getPrevSibling() const;
		Node*			getRootNode() const;

		void			addChild(Node* pChild);
		void			removeChild(Node* pChild);
		void			remove();
		void			removeAllChildren();

		Matrix4&		getViewMatrix() const;
		Matrix4&		getProjectionMatrix() const;
		Matrix4&		getWorldMatrix();
		Matrix4&		getWorldViewMatrix();

		void			render(bool bWireframe);
	private:
		virtual ~Node();

		CCString		m_sID;

		Model*			m_pModel;
		Camera*			m_pCamera;

		Node*			m_pParent;
		Node*			m_pFirstChild;
		Node*			m_pLastChild;
		unsigned int	m_iChildCount;

		Node*			m_pNextSibling;
		Node*			m_pPrevSibling;

		Matrix4			m_MatrixWorld;
};

#endif
