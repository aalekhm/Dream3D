#ifndef NODE_H
#define NODE_H

#include "Engine/Base.h"
#include "Engine/Transform.h"
#include <Common/CCString.h>

class Scene;
class Camera;
class Model;
struct Matrix4;

class Node : public Transform {

	public:
		Node(const char* id);
		virtual ~Node();

		static Node*	create(const char* id);
		void			setID(const char* id);
		const char*		getID();

		Scene*			getScene() const;
		void			setScene(Scene* pScene);

		Model*			getModel() const;
		void			setModel(Model* pModel);

		Camera*			getCamera() const;
		void			setCamera(Camera* pCamera);

		unsigned int	getChildCount() const;
		Node*			getParent() const;
		void			setParent(Node* pParent);
		Node*			getFirstChild() const;
		void			setFirstChild(Node* pNode);
		Node*			getLastChild() const;
		void			setLastChild(Node* pNode);
		Node*			getNextSibling() const;
		void			setNextSibling(Node* pNode);
		Node*			getPrevSibling() const;
		void			setPrevSibling(Node* pNode);
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

	protected:
		Scene*			m_pScene;
		Node*			m_pNextSibling;
		Node*			m_pPrevSibling;
	private:
		CCString		m_sID;

		Model*			m_pModel;
		Camera*			m_pCamera;

		Node*			m_pParent;
		Node*			m_pFirstChild;
		Node*			m_pLastChild;
		unsigned int	m_iChildCount;

		Matrix4			m_MatrixWorld;
};

#endif
