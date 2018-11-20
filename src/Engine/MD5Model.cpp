#include "Engine/MD5Model.h"
#include "Engine/VertexFormat.h"
#include "Engine/Mesh.h"
#include "Engine/MeshPart.h"
#include "Engine/Model.h"
#include "Engine/Node.h"

Node* MD5Model::loadModel(const char* sFileName) {

	GP_ASSERT( sFileName );

	RandomAccessFile* pRafIn = new RandomAccessFile();
	bool bCanOpen = pRafIn->openForRead(sFileName);

	if(bCanOpen) {

		// Now that we have a valid file and it's open, let's read in the info!
		m_Joints.clear();
		m_Meshes.clear();

		CCString singleLine;
		while(!pRafIn->isEOF()) {
			singleLine = (char*)pRafIn->readLine();
			singleLine.trim();

			if(CCString::startsWith(singleLine.c_str(), "//")) {
				continue;
			}
			else
			if(CCString::startsWith(singleLine.c_str(), "MD5Version")) {
				sscanf(singleLine.c_str(), "MD5Version %d", &m_iMD5Version);
			}
			else
			if(CCString::startsWith(singleLine.c_str(), "numJoints")) {
				sscanf(singleLine.c_str(), "numJoints %d", &m_iNumJoints);
			}
			else
			if(CCString::startsWith(singleLine.c_str(), "numMeshes")) {	
				sscanf(singleLine.c_str(), "numMeshes %d", &m_iNumMeshes);
			}
			else
			if(CCString::startsWith(singleLine.c_str(), "joints")) {	

				Joint* joint = NULL;

				// Read all joints
				for(int i = 0; i < m_iNumJoints; i++) {
					singleLine = (char*)pRafIn->readLine();
					singleLine.trim();

					joint = new Joint();
					sscanf(singleLine.c_str(), "%s %d ( %f %f %f ) ( %f %f %f )",		joint->m_sName, &joint->m_iParentID, 
																														&joint->m_Pos.x, &joint->m_Pos.y, &joint->m_Pos.z,
																														&joint->m_Orient._x, &joint->m_Orient._y, &joint->m_Orient._z
																												);
					computeQuatW(joint->m_Orient);
					m_Joints.push_back(joint);
				}
			}
			else
			if(CCString::startsWith(singleLine.c_str(), "mesh")) {

				Mesh_* mesh = new Mesh_();

				while(true) {
					singleLine = (char*)pRafIn->readLine();
					singleLine.trim();

					if(CCString::startsWith(singleLine.c_str(), "//")) {
						continue;
					}
					else
					if(CCString::startsWith(singleLine.c_str(), "shader")) {	
						sscanf(singleLine.c_str(), "shader %s", mesh->m_sShader);
					}
					else
					if(CCString::startsWith(singleLine.c_str(), "numverts")) {
						Vertex* vert = NULL;
						sscanf(singleLine.c_str(), "numverts %d", &mesh->m_iNumVertices);
						for ( int i = 0; i < mesh->m_iNumVertices; i++ ) {
							
							singleLine = (char*)pRafIn->readLine();
							singleLine.trim();
							
							int iVertexID = 0;
							vert = new Vertex();
							sscanf(singleLine.c_str(), "vert %d ( %f %f ) %d %d", &iVertexID, &vert->m_Tex0.x, &vert->m_Tex0.y, &vert->m_iStartWeight, &vert->m_iWeightCount);

							mesh->m_Vertices.push_back( vert );
							mesh->m_Tex2DBuffer.push_back( vert->m_Tex0 );
						}
					}
					else
					if(CCString::startsWith(singleLine.c_str(), "numtris")) {	
						Traingle* tri = NULL;
						sscanf(singleLine.c_str(), "numtris %d", &mesh->m_iNumTriangles);
						for ( int i = 0; i < mesh->m_iNumTriangles; i++ ) {

							singleLine = (char*)pRafIn->readLine();
							singleLine.trim();
							
							int iTriangleID = 0;
							tri = new Traingle();
							sscanf(singleLine.c_str(), "tri %d %d %d %d", &iTriangleID, &tri->m_iIndices[0], &tri->m_iIndices[1], &tri->m_iIndices[2]);

							mesh->m_Triangles.push_back(tri);
							mesh->m_IndexBuffer.push_back( (GLuint)tri->m_iIndices[0] );
							mesh->m_IndexBuffer.push_back( (GLuint)tri->m_iIndices[1] );
							mesh->m_IndexBuffer.push_back( (GLuint)tri->m_iIndices[2] );
						}
					}
					else
					if(CCString::startsWith(singleLine.c_str(), "numweights")) {
						Weight* weight = NULL;
						sscanf(singleLine.c_str(), "numweights %d", &mesh->m_iNumWeights);
						for ( int i = 0; i < mesh->m_iNumWeights; i++ ) {
							singleLine = (char*)pRafIn->readLine();
							singleLine.trim();

							int iWeightID;
							weight = new Weight();
							sscanf(singleLine.c_str(), "weight %d %d %f ( %f %f %f )", &iWeightID, &weight->m_iJointID, &weight->m_fBias, &weight->m_Pos.x, &weight->m_Pos.y, &weight->m_Pos.z);

							mesh->m_Weights.push_back(weight);
						}

						break;
					}
				}

				updateMesh(mesh);
				updateNormals(mesh);

				m_Meshes.push_back(mesh);
			}
		}

		GP_ASSERT( m_Joints.size() == m_iNumJoints );
		GP_ASSERT( m_Meshes.size() == m_iNumMeshes );

		pRafIn->close();
		SAFE_DELETE( pRafIn );

		return createModel();
	}

	pRafIn->close();
	SAFE_DELETE( pRafIn );
		
	return NULL;
}

bool MD5Model::loadAnim(const char* sFileName) {

	GP_ASSERT( sFileName );
	if( m_pMD5Animation->loadAnimation( sFileName ) ) {

		m_bHasAnimation = checkAnimation( m_pMD5Animation );
	}

	return m_bHasAnimation;
}

bool MD5Model::checkAnimation( MD5Animation* pMD5Animation ) {

	if( m_iNumJoints != pMD5Animation->getNumJoints() )
		return false;

	// Check to make sure the joints match up
	for( unsigned int i = 0; i < m_iNumJoints; i++) {

		Joint* pMeshJoint = m_Joints[ i ];
		MD5Animation::JointInfo* pAnimJoint = (MD5Animation::JointInfo*)pMD5Animation->getJointInfo( i );

		if(	strcmp( pMeshJoint->m_sName.c_str(), pAnimJoint->m_sName.c_str() ) != 0 
				||
				pMeshJoint->m_iParentID != pAnimJoint->m_iParentID
		) {
			return false;
		}
	}

	return true;
}

void MD5Model::computeQuatW( Quaternionf& qOrient ) {

	float t = 1.0f - ( qOrient._x * qOrient._x ) - ( qOrient._y * qOrient._y ) - ( qOrient._z * qOrient._z );
	if ( t < 0.0f )
	{
		qOrient._w = 0.0f;
	}
	else
	{
		qOrient._w = -sqrtf(t);
	}
}

bool MD5Model::updateMesh( Mesh_* mesh ) {

	mesh->m_PositionBuffer.clear();
	mesh->m_Tex2DBuffer.clear();

	// Compute vertex positions
	for(unsigned int i = 0; i < mesh->m_iNumVertices; i++) {

		Vector3 vFinalPos;
		Vertex* pVertex = mesh->m_Vertices[ i ];

		pVertex->m_Pos = Vector3(0.0f, 0.0f, 0.0f);

		// Sum the position of the weights
		for(int j = 0; j < pVertex->m_iWeightCount; j++) {

			Weight* pWeight = mesh->m_Weights[pVertex->m_iStartWeight + j];
			Joint* pJoint = m_Joints[pWeight->m_iJointID];

			// Convert the weight position from Joint local space to object space
			Vector3 vRotatedPos = pWeight->m_Pos;
			pJoint->m_Orient.rotate(vRotatedPos);

			pVertex->m_Pos += (pJoint->m_Pos + vRotatedPos) * pWeight->m_fBias;
		}

		mesh->m_PositionBuffer.push_back(pVertex->m_Pos);
		mesh->m_Tex2DBuffer.push_back(pVertex->m_Tex0);
	}

	return true;
}

bool MD5Model::updateMesh( const MD5Animation::FrameSkeleton* pFrameSkeleton ) {

	Mesh* pMesh = m_pModel->getMesh();
	GLvoid* pMapBuffer = pMesh->getMapBuffer();
	float* pVertices = (float*)pMapBuffer;
	int iStride = 5;
	for( unsigned int i = 0, m = 0; i < m_iNumMeshes; i++ ) {

		Mesh_* pMesh = (Mesh_*)m_Meshes[ i ];
		for ( unsigned int j = 0; j < pMesh->m_iNumVertices; j++ ) {

			Vertex* pVertex = pMesh->m_Vertices[ j ];
			Vector3 vPos = Vector3(0.0f, 0.0f, 0.0f);

			for( int k = 0; k < pVertex->m_iWeightCount; k++ ) {

				const Weight* pWeight = pMesh->m_Weights[ pVertex->m_iStartWeight + k ];
				const MD5Animation::SkeletonJoint* pSkeletonJoint = pFrameSkeleton->m_Joints[ pWeight->m_iJointID ];

				Vector3 vRotatedPos = pWeight->m_Pos;
				pSkeletonJoint->m_qOrient.rotate( vRotatedPos );

				vPos += ( pSkeletonJoint->m_vPos + vRotatedPos ) * pWeight->m_fBias;
				pVertices[ 0 + m * iStride ] = vPos.x;
				pVertices[ 1 + m * iStride ] = vPos.y;
				pVertices[ 2 + m * iStride ] = vPos.z;
			}

			m++;
		}
	}
	pMesh->unmapBuffer();

	return true;
}

bool MD5Model::updateNormals( Mesh_* mesh ) {

	return true;
}

Node* MD5Model::createModel() {

	VertexFormat::Element vertexElements[] = 
	{
		VertexFormat::Element(VertexFormat::POSITION, VertexFormat::THREE),
		//VertexFormat::Element(VertexFormat::NORMAL, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::TEXCOORD0, VertexFormat::TWO)
	};

	int iNumOfVertices = 0;
	for(int i = 0; i < m_iNumMeshes; i++) {
		int iSize = ((Mesh_*)m_Meshes[i])->m_iNumVertices;
		iNumOfVertices += iSize;
	}

	unsigned int vertexElementCount = sizeof(vertexElements) / sizeof(VertexFormat::Element);
	Mesh* mesh = Mesh::createMesh(VertexFormat(vertexElements, vertexElementCount), iNumOfVertices, true);
	if(mesh == NULL) {
		return  NULL;
	}
	mesh->setPrimitiveType(Mesh::TRIANGLES);

	int iStride = 0;
	for(int i = 0; i < vertexElementCount; i++) {
		VertexFormat::Element& vElement = vertexElements[i];
		iStride += vElement.size;
	}

	GLvoid* pMapBuffer = mesh->getMapBuffer();
	float* pVertices = (float*)pMapBuffer;
	for(unsigned int i = 0, k = 0; i < m_iNumMeshes; i++) {

		Mesh_* pMesh = (Mesh_*)m_Meshes[i];
		for(unsigned int j = 0; j < pMesh->m_iNumVertices; j++) {

			memcpy(&pVertices[0 + k * iStride], &pMesh->m_PositionBuffer[j], sizeof(float) * 3);
			//pVertices[ 0 + k * iStride ] = pMesh->m_PositionBuffer[j].x;
			//pVertices[ 1 + k * iStride ] = pMesh->m_PositionBuffer[j].y;
			//pVertices[ 2 + k * iStride ] = pMesh->m_PositionBuffer[j].z;

			memcpy(&pVertices[3 + k * iStride], &pMesh->m_Tex2DBuffer[j], sizeof(float) * 2);
			//pVertices[ 3 + k * iStride ] = pMesh->m_Tex2DBuffer[j].x;
			//pVertices[ 4 + k * iStride ] = pMesh->m_Tex2DBuffer[j].y;

			k++;
		}

		//pMesh->m_PositionBuffer.clear();
		//pMesh->m_Tex2DBuffer.clear();
	}
	mesh->unmapBuffer();

	int iPrevMeshPositionBufferSize = 0;
	for(unsigned int i = 0; i < m_iNumMeshes; i++) {

		Mesh_* pMesh = (Mesh_*)m_Meshes[i];
		
		int iIndexCount = pMesh->m_iNumTriangles * 3;
		MeshPart* meshPart = mesh->addMeshPart(Mesh::TRIANGLES, Mesh::INDEX16, iIndexCount, false);
		unsigned short* pIndices = (unsigned short*)meshPart->getMapBuffer();

		if(i > 0) {
			iPrevMeshPositionBufferSize += ((Mesh_*)m_Meshes[i-1])->m_iNumVertices;
		}

		for(int j = 0; j < iIndexCount; j++) {
			pIndices[j] = pMesh->m_IndexBuffer[j];

			if(iPrevMeshPositionBufferSize > 0) {
				pIndices[j] += iPrevMeshPositionBufferSize;
			}
		}
		
		//pMesh->m_IndexBuffer.clear();
		meshPart->unmapBuffer();
	}
	
	Model* pModel = Model::create(mesh);
	Node* pNode = Node::create("MD5Model");
	pNode->setModel(pModel);
	m_pModel = pModel;

	return pNode;
}

void MD5Model::update( float fDeltaTime ) {

	if ( m_bHasAnimation ) {
		m_pMD5Animation->update(fDeltaTime);

		const MD5Animation::FrameSkeleton* pFrameSkeleton = m_pMD5Animation->getSkeletion();
		updateMesh( pFrameSkeleton );
	}
}
