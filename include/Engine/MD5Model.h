#ifndef MD5MODEL_H
#define MD5MODEL_H

#include "Engine/Base.h"
#include "Common/Vectors.h"
#include "Common/Matrices.h"
#include "Common/Quaternion.h"
#include "Common/CCString.h"
#include "Common/RandomAccessFile.h"
#include "Engine/MD5Animation.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
class Node;
class Mesh;
class Model;

class MD5Model {

	public:
		MD5Model()
			: m_iMD5Version(10)
			, m_iNumJoints(0)
			, m_iNumMeshes(0)
			, m_bHasAnimation(false)
			, m_LocalToWorldMatrix()
			, m_pMD5Animation(NULL)
			, m_pModel(NULL)
			, m_iStride(0)
		{
			m_Joints.clear();
			m_Meshes.clear();

			m_pMD5Animation = new MD5Animation();
		};

		~MD5Model();

		Node*			loadModel( const char* sFileName );
		bool				loadAnim( const char* sFileName );
		bool				checkAnimation( MD5Animation* pMD5Animation );
		void				update( float fDeltaTime );
		void				render();

		static void		computeQuatW( Quaternionf& qOrient );
	protected:
		typedef std::vector<Vector3>		PositionBuffer;
		typedef std::vector<Vector3>		NormalBuffer;
		typedef std::vector<Vector2>		Tex2DBuffer;
		typedef std::vector<GLuint>			IndexBuffer;

		struct Vertex {
			Vertex() 
				: m_Pos(0.0f, 0.0f, 0.0f)
				, m_Normal(0.0f, 0.0f, 0.0f)
				, m_Tex0(0.0f, 0.0f)
				, m_iStartWeight(0)
				, m_iWeightCount(0) 
			{ }

			Vector3		m_Pos;
			Vector3		m_Normal;
			Vector2		m_Tex0;
			int			m_iStartWeight;
			int			m_iWeightCount;
		};
		typedef std::vector<Vertex*>		VertexList;

		struct Traingle {
			int			m_iIndices[3];
		};
		typedef std::vector<Traingle*>	TriangleList;
		
		struct Weight {
			Weight() 
				: m_iJointID(0)
				, m_fBias(0.0f)
				, m_Pos(0.0f, 0.0f, 0.0f)
			{ }

			int			m_iJointID;
			float			m_fBias;
			Vector3		m_Pos;
		};
		typedef std::vector<Weight*>		WeightList;

		struct Joint {
			Joint()
				: m_sName("")
				, m_iParentID(0)
				, m_Pos(0.0f, 0.0f, 0.0f)
				, m_Orient(0.0f, 0.0f, 0.0f, 0.0f)
			{ }

			CCString			m_sName;
			int					m_iParentID;
			Vector3				m_Pos;
			Quaternionf		m_Orient;
		};
		typedef std::vector<Joint*>		JointList;

		struct Mesh_ {
			Mesh_()
				: m_sShader("")
				, m_iTexID(0)
				, m_iNumVertices(0)
				, m_iNumTriangles(0)
				, m_iNumWeights(0)
			{
				m_Vertices.clear();
				m_Triangles.clear();
				m_Weights.clear();

				m_PositionBuffer.clear();
				m_NormalBuffer.clear();
				m_Tex2DBuffer.clear();
				m_IndexBuffer.clear();
			}

			CCString			m_sShader;

			// This vertex list stores the vertices in the bind pose.
			VertexList			m_Vertices;
			TriangleList		m_Triangles;
			WeightList			m_Weights;

			// A texture ID for the material
			GLuint				m_iTexID;

			unsigned int		m_iNumVertices;
			unsigned int		m_iNumTriangles;
			unsigned int		m_iNumWeights;

			// These buffers are used for rendering the animated mesh
			PositionBuffer	m_PositionBuffer;	// Vertex position stream
			NormalBuffer	m_NormalBuffer;	// Vertex normals stream
			Tex2DBuffer		m_Tex2DBuffer;		// Texture coordinate set
			IndexBuffer		m_IndexBuffer;		// Vertex index buffer
		};
		typedef std::vector<Mesh_*>	MeshList;
	
		// Prepare the mesh for rendering
		// Compute vertex positions and normals
		bool		updateMesh( Mesh_* mesh );
		bool		updateMesh( const MD5Animation::FrameSkeleton* pFrameSkeleton );
		bool		updateNormals( Mesh_* mesh );

		Node*	createModel();
	private:
		int					m_iMD5Version;
		int					m_iNumJoints;
		int					m_iNumMeshes;

		bool					m_bHasAnimation;

		JointList			m_Joints;
		MeshList			m_Meshes;

		MD5Animation*	m_pMD5Animation;
		Model*				m_pModel;

		Matrix4				m_LocalToWorldMatrix;			

		int					m_iStride;
};

#endif