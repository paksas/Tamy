#include "core-AI\SkeletonMapper.h"
#include "core-AI\Skeleton.h"
#include "core\Transform.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SkeletonMapper );
   PARENT( ReflectionObject );

END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BEGIN_ENUM( BoneLookupMethod );
   ENUM_VAL( Lookup_ByName );
   ENUM_VAL( Lookup_ByDistance );
END_ENUM();

///////////////////////////////////////////////////////////////////////////////

SkeletonMapper::SkeletonMapper()
   : m_sourceSkeleton( NULL )
   , m_targetSkeleton( NULL )
{
   m_sourceChainSkeleton = new Skeleton();
   m_targetChainSkeleton = new Skeleton();
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapper::~SkeletonMapper()
{
   m_sourceSkeleton = NULL;
   m_targetSkeleton = NULL;

   delete m_sourceChainSkeleton;
   m_sourceChainSkeleton = NULL;

   delete m_targetChainSkeleton;
   m_targetChainSkeleton = NULL;

   uint chainsCount = m_sourceChains.size();
   for ( uint i = 0; i < chainsCount; ++i )
   {
      delete m_sourceChains[i];
   }

   chainsCount = m_targetChains.size();
   for ( uint i = 0; i < chainsCount; ++i )
   {
      delete m_targetChains[i];
   }
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapper& SkeletonMapper::defineMapping( const Skeleton* sourceSkeleton, const Skeleton* targetSkeleton )
{
   // clear the old mapping
   m_sourceSkeleton = sourceSkeleton;
   m_targetSkeleton = targetSkeleton;
   m_sourceChainSkeleton->clear();
   m_targetChainSkeleton->clear();

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapper& SkeletonMapper::mapBone( const char* sourceBone, const char* targetBone )
{
   char sourceChainName[128];
   sprintf_s( sourceChainName, "__bone_%d", m_sourceChains.size() );
   addSourceChain( sourceChainName, sourceBone, sourceBone );

   char targetChainName[128];
   sprintf_s( targetChainName, "__bone_%d", m_targetChains.size() );
   addTargetChain( targetChainName, targetBone, targetBone );

   mapChain( sourceChainName, targetChainName );

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapper& SkeletonMapper::addSourceChain( const char* chainName, const char* firstBoneName, const char* lastBoneName )
{
   const int firstBoneIdx = m_sourceSkeleton->getBoneIndex( firstBoneName );
   const int lastBoneIdx = m_sourceSkeleton->getBoneIndex( lastBoneName );

   if ( firstBoneIdx >= 0 && lastBoneIdx >= 0 )
   {
      SkeletonBoneChain* chain = new SkeletonBoneChain( m_sourceSkeleton, firstBoneIdx, lastBoneIdx );
      m_sourceChains.push_back( chain );

      m_sourceChainSkeleton->addBone( chainName, Transform::IDENTITY, -1, 1 );
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapper& SkeletonMapper::addTargetChain( const char* chainName, const char* firstBoneName, const char* lastBoneName )
{
   const int firstBoneIdx = m_targetSkeleton->getBoneIndex( firstBoneName );
   const int lastBoneIdx = m_targetSkeleton->getBoneIndex( lastBoneName );

   if ( firstBoneIdx >= 0 && lastBoneIdx >= 0 )
   {
      SkeletonBoneChain* chain = new SkeletonBoneChain( m_targetSkeleton, firstBoneIdx, lastBoneIdx );
      m_targetChains.push_back( chain );

      m_targetChainSkeleton->addBone( chainName, Transform::IDENTITY, -1, 1 );
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapper& SkeletonMapper::mapChain( const char* sourceChainName, const char* targetChainName )
{
   const int sourceChainIdx = getChainIdx( m_sourceChainSkeleton, sourceChainName );
   const int targetChainIdx = getChainIdx( m_targetChainSkeleton, targetChainName );
   if ( sourceChainIdx < 0 || targetChainIdx < 0 )
   {
      return *this;
   }

   if ( targetChainIdx >= (int)m_chainMappings.size() )
   {
      m_chainMappings.resize( targetChainIdx + 1, -1 );
   }
   
   m_chainMappings[targetChainIdx] = sourceChainIdx;

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

int SkeletonMapper::getChainIdx( const Skeleton* chainSkeleton, const char* chainName ) const
{
   const int count = chainSkeleton->getBoneCount();
   for ( int i = 0; i < count; ++i )
   {
      if ( chainSkeleton->m_boneNames[i] == chainName )
      {
         return i;
      }
   }

   return -1;
}

///////////////////////////////////////////////////////////////////////////////

int SkeletonMapper::findChainByBone( const Array< SkeletonBoneChain* >& chainsCollection, uint baseSkeletonBoneIdx ) const
{
   const int chainsCount = chainsCollection.size();
   for ( int chainIdx = 0; chainIdx < chainsCount; ++chainIdx )
   {
      SkeletonBoneChain* chain = chainsCollection[chainIdx];
      if ( chain->m_firstBoneIdx == baseSkeletonBoneIdx )
      {
         return chainIdx;
      }

      for ( int boneIdx = chain->m_lastBoneIdx; boneIdx != chain->m_firstBoneIdx; boneIdx = chain->m_skeleton->m_boneParentIndices[boneIdx] )
      {
         if ( boneIdx == baseSkeletonBoneIdx )
         {
            return chainIdx;
         }
      }
   }

   return -1;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::buildChain( const Skeleton* baseSkeleton, const Array< SkeletonBoneChain* >& chainBones, Skeleton* outChainSkeleton ) const
{
   Array< Transform > baseModelPose( baseSkeleton->getBoneCount() );
   baseModelPose.resize( baseSkeleton->getBoneCount(), Transform::IDENTITY );
   baseSkeleton->calculateLocalToModel( baseSkeleton->m_boneLocalMatrices.getRaw(), baseModelPose.getRaw() );
  
   // setup the hierarchy and the model space pose, which we'll later on use to calculate the local space transforms
   // for the bones
   const uint chainBonesCount = chainBones.size();
   Array< Transform > chainModelPose( chainBonesCount );
   chainModelPose.resize( chainBonesCount, Transform::IDENTITY );

   for ( uint i = 0; i < chainBonesCount; ++i )
   {
      const SkeletonBoneChain* chain = chainBones[i];
      const int parentBoneIdx = m_sourceSkeleton->m_boneParentIndices[chain->m_firstBoneIdx];
      if ( parentBoneIdx >= 0 )
      {
         outChainSkeleton->m_boneParentIndices[i] = findChainByBone( chainBones, parentBoneIdx );
      }

      chainModelPose[i] = baseModelPose[chain->m_firstBoneIdx];
   }

   // calculate the local pose of the chain skeleton 
   Array< Transform > chainLocalPose( chainBonesCount );
   chainLocalPose.resize( chainBonesCount, Transform::IDENTITY );
   outChainSkeleton->calculateModelToLocal( chainModelPose.getRaw(), chainLocalPose.getRaw() );

   for ( uint i = 0; i < chainBonesCount; ++i )
   {
      chainLocalPose[i].toMatrix( outChainSkeleton->m_boneLocalMatrices[i] );
   }

   // build the skeleton
   outChainSkeleton->buildSkeleton();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::buildMapper()
{
   buildChain( m_sourceSkeleton, m_sourceChains, m_sourceChainSkeleton );
   buildChain( m_targetSkeleton, m_targetChains, m_targetChainSkeleton );

   const uint sourceChainsCount = m_sourceChainSkeleton->getBoneCount();
   const uint targetChainsCount = m_targetChainSkeleton->getBoneCount();
   
   const uint requiredChainMappingsCount = targetChainsCount;
   m_chainMappings.resize( requiredChainMappingsCount, -1 );

   m_tmpSourceBasePose.resize( m_sourceSkeleton->getBoneCount(), Transform::IDENTITY );
   m_tmpSourceChainPose.resize( sourceChainsCount, Transform::IDENTITY );
   m_tmpTargetChainPose.resize( targetChainsCount, Transform::IDENTITY );
   m_tmpTargetBasePose.resize( m_targetSkeleton->getBoneCount(), Transform::IDENTITY );

   // allocate and calculate the transition matrices between the two chain skeletons
   m_targetBindPose.resize( targetChainsCount, Transform::IDENTITY );
   m_targetToSource.resize( targetChainsCount, Transform::IDENTITY );
   m_sourceToTarget.resize( targetChainsCount, Transform::IDENTITY );

   m_sourceChainSkeleton->calculateLocalToModel( m_sourceChainSkeleton->m_boneLocalMatrices.getRaw(), m_tmpSourceChainPose.getRaw() );
   m_targetChainSkeleton->calculateLocalToModel( m_targetChainSkeleton->m_boneLocalMatrices.getRaw(), m_targetBindPose.getRaw() );

   for ( uint targetChainIdx = 0; targetChainIdx< targetChainsCount; ++targetChainIdx )
   {
      const int sourceChainIdx = m_chainMappings[targetChainIdx];
      if ( sourceChainIdx < 0 )
      {
         continue;
      }

      m_targetToSource[targetChainIdx].setMulInverse( m_targetBindPose[targetChainIdx], m_tmpSourceChainPose[sourceChainIdx] );
      m_sourceToTarget[targetChainIdx].setMulInverse( m_tmpSourceChainPose[sourceChainIdx], m_targetBindPose[targetChainIdx] );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::calcPoseLocalSpace( const Transform* sourcePoseLocalSpace, Transform* outTargetPoseLocalSpace ) const
{
   ASSERT_MSG( m_sourceSkeleton->getBoneCount() > 0, "It's likely you forgot to call 'buildMapper'" );
   ASSERT_MSG( m_targetSkeleton->getBoneCount() > 0, "It's likely you forgot to call 'buildMapper'" );
   
   // calculate the model pose of the base skeleton and apply it to the source chain skeleton
   m_sourceSkeleton->calculateLocalToModel( sourcePoseLocalSpace, m_tmpSourceBasePose.getRaw() );
   const uint sourceChainsCount = m_sourceChains.size();
   for ( uint i = 0; i < sourceChainsCount; ++i )
   {
      SkeletonBoneChain* chain = m_sourceChains[i];
      m_tmpSourceChainPose[i] = m_tmpSourceBasePose[chain->m_firstBoneIdx];
   }

   // map the transforms of chains
   const uint mappingsCount = m_chainMappings.size();
   for ( uint targetChainIdx = 0; targetChainIdx < mappingsCount; ++targetChainIdx )
   {
      const int sourceChainIdx = m_chainMappings[targetChainIdx];
      if ( sourceChainIdx < 0 )
      {
         continue;
      }

      // here's the juice: 
      // TODO:, still doesn't work
      
      Transform dtSourceSpace;
      {
         Transform invBindPose, bindPose;
         invBindPose.set( m_sourceChainSkeleton->m_boneInvBindPoseMtx[sourceChainIdx] );
         dtSourceSpace.setMul( invBindPose, m_tmpSourceChainPose[sourceChainIdx] );
      }

      m_tmpTargetChainPose[targetChainIdx].setMul( m_targetBindPose[targetChainIdx], dtSourceSpace );
   }
   
   // map the target chain to the base target skeleton
   const uint targetChainsCount = m_targetChains.size();
   for ( uint i = 0; i < targetChainsCount; ++i )
   {
      SkeletonBoneChain* chain = m_targetChains[i];
      chain->updatePose( m_tmpTargetChainPose[i], m_tmpTargetBasePose.getRaw() );
   }

   // back to the local pose
   m_targetSkeleton->calculateModelToLocal( m_tmpTargetBasePose.getRaw(), outTargetPoseLocalSpace );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SkeletonBoneChain );
   PARENT( ReflectionObject );
   PROPERTY( Skeleton*, m_skeleton );
   PROPERTY( uint, m_firstBoneIdx );
   PROPERTY( uint, m_lastBoneIdx );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SkeletonBoneChain::SkeletonBoneChain()
   : m_skeleton( NULL )
   , m_firstBoneIdx( 0 )
   , m_lastBoneIdx( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////

SkeletonBoneChain::SkeletonBoneChain( const Skeleton* skeleton, uint firstBoneIdx, uint lastBoneIdx )
   : m_skeleton( skeleton )
   , m_firstBoneIdx( firstBoneIdx )
   , m_lastBoneIdx( lastBoneIdx )
{
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonBoneChain::updatePose( const Transform& t, Transform* outPose ) const
{
   // TODO: optimize
   Array< uint > boneIndices;
   for ( uint boneIdx = m_lastBoneIdx; boneIdx != m_firstBoneIdx; boneIdx = m_skeleton->m_boneParentIndices[boneIdx] )
   {
      boneIndices.push_back( boneIdx );
   }

   outPose[m_firstBoneIdx] = t;

   const uint chainBonesCount = boneIndices.size();
   Transform bindPoseT;
   for ( uint i = 0; i < chainBonesCount; ++i )
   {
      const uint boneIdx = boneIndices[i];
      const uint parentBoneIdx = m_skeleton->m_boneParentIndices[boneIdx];

      bindPoseT.set( m_skeleton->m_boneLocalMatrices[boneIdx] );
      outPose[boneIdx].setMul( bindPoseT, outPose[parentBoneIdx] );
   }
}

///////////////////////////////////////////////////////////////////////////////
