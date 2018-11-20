#ifndef MD5ANIMATION_H	
#define MD5ANIMATION_H

#include "Engine/Base.h"
#include "Common/CCString.h"
#include "Common/Vectors.h"
#include "Common/Quaternion.h"
#include <vector>

class MD5Animation {

	public:
		MD5Animation();
		virtual ~MD5Animation();

		// Load an animation from the animation file
		bool		loadAnimation(const char* sFileName);

		// Update this animation's joint set.
		void		update(float fDeltaTime);

		// Draw the animated skeleton
		void		render();

		// The JointInfo stores the information necessary to build the 
		// skeletons for each frame
		struct JointInfo {
			CCString	m_sName;
			int			m_iParentID;
			int			m_iFlags;
			int			m_iStartIndex;
		};
		typedef std::vector<JointInfo*>		JointInfoList;

		struct Bound {
			Vector3		m_vMin;
			Vector3		m_vMax;
		};
		typedef std::vector<Bound*>			BoundList;

		struct BaseFrame {
			Vector3				m_vPos;
			Quaternionf		m_qOrient;
		};
		typedef std::vector<BaseFrame*>		BaseFrameList;

		struct FrameData {
			int						m_iFrameID;
			std::vector<float>	m_vFrameData;
		};
		typedef std::vector<FrameData*>		FrameDataList;

		// A Skeleton joint is a joint of the skeleton per frame
		struct SkeletonJoint {

			int					m_iParent;
			Vector3				m_vPos;
			Quaternionf		m_qOrient;

			SkeletonJoint() 
				: m_iParent(-1)
				, m_vPos(0.0f, 0.0f, 0.0f)
				, m_qOrient(0.0f, 0.0f, 0.0f, 0.0f)
			{ }

			SkeletonJoint(const BaseFrame& copy) 
				: m_vPos(copy.m_vPos)
				, m_qOrient(copy.m_qOrient)
			{ }
		};
		typedef std::vector<SkeletonJoint*>			SkeletonJointList;

		// A frame skeleton stores the joints of the skeleton for a single frame.
		struct FrameSkeleton {
			SkeletonJointList		m_Joints;
		};
		typedef std::vector<FrameSkeleton*>		FrameSkeletonList;

		const FrameSkeleton*	getSkeletion() const {
			return m_pAnimatedSkeleton;
		}

		int	getNumJoints() const {
			return m_iNumJoints;
		}

		const JointInfo*	getJointInfo(unsigned int iIndex) const {
			GP_ASSERT( iIndex > 0 && iIndex < m_vJointInfos.size() );
			return m_vJointInfos[iIndex];
		}
	protected:

		JointInfoList				m_vJointInfos;
		BoundList					m_vBounds;
		BaseFrameList			m_vBaseFrames;
		FrameDataList			m_vFrames;
		FrameSkeletonList		m_vSkeletons;			// All the skeletons for all the frames

		FrameSkeleton*			m_pAnimatedSkeleton;

		// Build the frame skeleton for a particular frame
		void		buildFrameSkeleton( FrameSkeletonList& skeletons, const JointInfoList& jointInfo, const BaseFrameList& baseFrames, const FrameData* frameData );
		void		interpolateSkeletons( FrameSkeleton* finalSkeleton, const FrameSkeleton* skeleton0, const FrameSkeleton* skeleton1, float fInterpolate );

	private:
		int		m_iMD5Version;
		int		m_iNumFrames;
		int		m_iNumJoints;
		int		m_iFrameRate;
		int		m_iNumAnimatedComponents;

		float		m_fAnimDuration;
		float		m_fFrameDuration;
		float		m_fAnimTime;
};

#endif