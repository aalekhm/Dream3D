#include "Engine/MD5Animation.h"
#include "Common/RandomAccessFile.h"
#include "Engine/MD5Model.h"

MD5Animation::MD5Animation()
	: m_iMD5Version(0)
	,  m_iNumFrames(0)
	,  m_iNumJoints(0)
	,  m_iFrameRate(0)
	,  m_iNumAnimatedComponents(0)
	,  m_fAnimDuration(0.0f)
	,  m_fFrameDuration(0.0f)
	,  m_fAnimTime(0.0f)
{
	m_pAnimatedSkeleton = new FrameSkeleton();
}

MD5Animation::~MD5Animation() {}

bool MD5Animation::loadAnimation(const char* sFileName) {

	GP_ASSERT( sFileName );

	RandomAccessFile* pRafIn = new RandomAccessFile();
	bool bCanOpen = pRafIn->openForRead(sFileName);

	if(bCanOpen) {
		m_vJointInfos.clear();
		m_vBounds.clear();
		m_vBaseFrames.clear();
		m_vFrames.clear();
		m_pAnimatedSkeleton->m_Joints.clear();
		m_iNumFrames = 0;

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
			if(CCString::startsWith(singleLine.c_str(), "numFrames")) {
				sscanf(singleLine.c_str(), "numFrames %d", &m_iNumFrames);
			}
			else
			if(CCString::startsWith(singleLine.c_str(), "numJoints")) {
				sscanf(singleLine.c_str(), "numJoints %d", &m_iNumJoints);
			}
			else
			if(CCString::startsWith(singleLine.c_str(), "frameRate")) {
				sscanf(singleLine.c_str(), "frameRate %d", &m_iFrameRate);
			}
			else
			if(CCString::startsWith(singleLine.c_str(), "numAnimatedComponents")) {
				sscanf(singleLine.c_str(), "numAnimatedComponents %d", &m_iNumAnimatedComponents);
			}
			else
			if(CCString::startsWith(singleLine.c_str(), "hierarchy")) {

				JointInfo* pJointInfo = NULL;
				for(int i = 0; i < m_iNumJoints; i++) {
					singleLine = (char*)pRafIn->readLine();
					singleLine.trim();
					
					pJointInfo = new JointInfo();
					sscanf(singleLine.c_str(), "%s %d %d %d", pJointInfo->m_sName, &pJointInfo->m_iParentID, &pJointInfo->m_iFlags, &pJointInfo->m_iStartIndex);

					m_vJointInfos.push_back(pJointInfo);
				}
			}
			else
			if(CCString::startsWith(singleLine.c_str(), "bounds")) {

				Bound* pBound = NULL;
				for(int i = 0; i < m_iNumFrames; i++) {
					singleLine = (char*)pRafIn->readLine();
					singleLine.trim();

					pBound = new Bound();
					sscanf(singleLine.c_str(), "( %f %f %f ) ( %f %f %f )",		&pBound->m_vMin.x, 
																										&pBound->m_vMin.y, 
																										&pBound->m_vMin.z, 
																										&pBound->m_vMax.x, 
																										&pBound->m_vMax.y, 
																										&pBound->m_vMax.z);

					m_vBounds.push_back(pBound);
				}
			}
			else
			if(CCString::startsWith(singleLine.c_str(), "baseframe")) {

				BaseFrame* pBaseFrame = NULL;
				for(int i = 0; i < m_iNumJoints; i++) {
					singleLine = (char*)pRafIn->readLine();
					singleLine.trim();
					
					pBaseFrame = new BaseFrame();
					sscanf(singleLine.c_str(), "( %f %f %f ) ( %f %f %f )",		&pBaseFrame->m_vPos.x, 
																										&pBaseFrame->m_vPos.y, 
																										&pBaseFrame->m_vPos.z, 
																										&pBaseFrame->m_qOrient._x, 
																										&pBaseFrame->m_qOrient._y, 
																										&pBaseFrame->m_qOrient._z);

					m_vBaseFrames.push_back(pBaseFrame);
				}
			}
			else
			if(CCString::startsWith(singleLine.c_str(), "frame")) {

				FrameData* pFrameData = new FrameData();
				sscanf(singleLine.c_str(), "frame %d", &pFrameData->m_iFrameID);
				
				//for(int i = 0; i < m_iNumAnimatedComponents; i++) 
				int k = 0;
				while(true) {
					singleLine = (char*)pRafIn->readLine();
					singleLine.trim();
					
					char* strTok = strtok(singleLine.c_str(), " ");
					while(strTok != NULL) {
						pFrameData->m_vFrameData.push_back(atof(strTok));
						strTok = strtok(NULL, " ");
						k++;
					}

					if(k >= m_iNumAnimatedComponents)
						break;
				}

				m_vFrames.push_back(pFrameData);
	
				// Build a skeleton for this frame
				buildFrameSkeleton( m_vSkeletons, m_vJointInfos, m_vBaseFrames, pFrameData );
			}
		}
	}

	// Make sure there are enough joints for the animated skeleton.
	m_pAnimatedSkeleton = m_vSkeletons[0];

	m_fFrameDuration = 1.0f / (float)m_iFrameRate;
	m_fAnimDuration = m_fFrameDuration * (float)m_iNumFrames;
	m_fAnimTime = 0.0f;

	pRafIn->close();
	SAFE_DELETE( pRafIn );

	return true;
}

void MD5Animation::buildFrameSkeleton( FrameSkeletonList& skeletons, const JointInfoList& jointInfos, const BaseFrameList& baseFrames, const FrameData* pFrameData ) {

	// Stores all the joints (pos & orient) for a single frame. 
	FrameSkeleton* pSkeleton = new FrameSkeleton();

	for ( unsigned int i = 0; i < jointInfos.size(); ++i )
	{
		unsigned int j = 0;

		const JointInfo* pJointInfo = jointInfos[i];

		// Start with the base frame position and orientation.
		SkeletonJoint* pAnimatedJoint = new SkeletonJoint();
		pAnimatedJoint->m_iParent = pJointInfo->m_iParentID;
		pAnimatedJoint->m_vPos = ((BaseFrame*)baseFrames[i])->m_vPos;
		pAnimatedJoint->m_qOrient = ((BaseFrame*)baseFrames[i])->m_qOrient;

		if ( pJointInfo->m_iFlags & 1 ) // Pos.x
		{
			pAnimatedJoint->m_vPos.x = pFrameData->m_vFrameData[ pJointInfo->m_iStartIndex + j++ ];
		}
		if ( pJointInfo->m_iFlags & 2 ) // Pos.y
		{
			pAnimatedJoint->m_vPos.y = pFrameData->m_vFrameData[ pJointInfo->m_iStartIndex + j++ ];
		}
		if ( pJointInfo->m_iFlags & 4 ) // Pos.x
		{
			pAnimatedJoint->m_vPos.z  = pFrameData->m_vFrameData[ pJointInfo->m_iStartIndex + j++ ];
		}
		if ( pJointInfo->m_iFlags & 8 ) // Orient.x
		{
			pAnimatedJoint->m_qOrient._x = pFrameData->m_vFrameData[ pJointInfo->m_iStartIndex + j++ ];
		}
		if ( pJointInfo->m_iFlags & 16 ) // Orient.y
		{
			pAnimatedJoint->m_qOrient._y = pFrameData->m_vFrameData[ pJointInfo->m_iStartIndex + j++ ];
		}
		if ( pJointInfo->m_iFlags & 32 ) // Orient.z
		{
			pAnimatedJoint->m_qOrient._z = pFrameData->m_vFrameData[ pJointInfo->m_iStartIndex + j++ ];
		}

		MD5Model::computeQuatW( pAnimatedJoint->m_qOrient );

		if ( pAnimatedJoint->m_iParent >= 0 ) // Has a parent joint
		{
			SkeletonJoint* pParentJoint = pSkeleton->m_Joints[pAnimatedJoint->m_iParent];

			Vector3 vRotatedPos = pAnimatedJoint->m_vPos;
			pParentJoint->m_qOrient.rotate(vRotatedPos);

			pAnimatedJoint->m_vPos = pParentJoint->m_vPos + vRotatedPos;
			pAnimatedJoint->m_qOrient = pParentJoint->m_qOrient * pAnimatedJoint->m_qOrient;
			pAnimatedJoint->m_qOrient.normalize();
		}

		pSkeleton->m_Joints.push_back(pAnimatedJoint);
	}

	skeletons.push_back(pSkeleton);
}

void MD5Animation::update(float fDeltaTime) {

	if( m_iNumFrames < 1) 
		return;

	m_fAnimTime += fDeltaTime / 1000.0f;
	while( m_fAnimTime > m_fAnimDuration ) m_fAnimTime -= m_fAnimDuration;
	while( m_fAnimTime < 0.0f ) m_fAnimTime += m_fAnimDuration;

	//Figure out which frame we're on
	float fFrameNum = m_fAnimTime * (float)m_iFrameRate;
	int iFrame0 = (int)floorf(fFrameNum);
	int iFrame1 = (int)ceilf(fFrameNum);

	iFrame0 = iFrame0 % m_iNumFrames;
	iFrame1 = iFrame1 % m_iNumFrames;

	float fInterpolate = fmodf( m_fAnimTime, m_fFrameDuration ) / m_fFrameDuration;
	interpolateSkeletons( m_pAnimatedSkeleton, m_vSkeletons[iFrame0], m_vSkeletons[iFrame1], fInterpolate );
}

void MD5Animation::interpolateSkeletons( FrameSkeleton* pFinalSkeleton, const FrameSkeleton* pSkeleton0, const FrameSkeleton* pSkeleton1, float fInterpolate ) {

	for ( int i = 0; i < m_iNumJoints; i++ ) {

		SkeletonJoint* pFinalJoint = pFinalSkeleton->m_Joints[i];
		const SkeletonJoint* pJoint0 = pSkeleton0->m_Joints[i]; 
		const SkeletonJoint* pJoint1 = pSkeleton1->m_Joints[i];

		pFinalJoint->m_iParent = pJoint0->m_iParent;

		pFinalJoint->m_vPos = Lerp( pJoint0->m_vPos, pJoint1->m_vPos, fInterpolate );
		pFinalJoint->m_qOrient = Slerp( pJoint0->m_qOrient, pJoint1->m_qOrient, fInterpolate );
		//The following is "Not" the way to go.
		//pFinalJoint->m_qOrient._x =  ( pJoint1->m_qOrient._x * fInterpolate ) + ( pJoint0->m_qOrient._x * (1-fInterpolate) );
		//pFinalJoint->m_qOrient._y =  ( pJoint1->m_qOrient._y * fInterpolate ) + ( pJoint0->m_qOrient._y * (1-fInterpolate) );
		//pFinalJoint->m_qOrient._z =  ( pJoint1->m_qOrient._z * fInterpolate ) + ( pJoint0->m_qOrient._z * (1-fInterpolate) );
		//pFinalJoint->m_qOrient._w =  ( pJoint1->m_qOrient._w * fInterpolate ) + ( pJoint0->m_qOrient._w * (1-fInterpolate) );
	}
}