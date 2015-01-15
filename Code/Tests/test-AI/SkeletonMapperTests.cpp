#include "core-TestFramework\TestFramework.h"
#include "core-AI\SkeletonMapper.h"
#include "core-AI\SkeletonMapperComponent.h"
#include "core-AI\Skeleton.h"
#include "core-AI\SkeletonPoseTool.h"


///////////////////////////////////////////////////////////////////////////////

#define TEST_BONE( pose, boneName, rotationAxis, angle, translation )  { \
      bool result = pose.testBoneModel( boneName, rotationAxis, angle, translation ); \
      CPPUNIT_NS::Asserter::failIf( !result, CPPUNIT_NS::Message( pose.getErrorMsg() ), CPPUNIT_SOURCELINE() ); \
   }

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, sameBoneRotations )
{
   Skeleton skeletonA, skeletonB;
   SkeletonPoseTool poseA( skeletonA );
   SkeletonPoseTool poseB( skeletonB );
   {
      poseA.startSkeleton( "boneA", Vector_OX, DEG2RAD( 45.0f ), Vector_ZERO ).buildSkeleton();
      poseB.startSkeleton( "boneA", Vector_OX, DEG2RAD( 45.0f ), Vector_ZERO ).buildSkeleton();
   }

   SkeletonMapper mapper;
   mapper.setSkeletons( &skeletonA, &skeletonB )
      .mapBone( "boneA", "boneA" );

   SkeletonMapperComponent runtime;
   runtime.setMapper( &mapper );
   runtime.compileRuntime();

   poseA.start().rotateAndTranslate( "boneA", Vector_OX, DEG2RAD( 45.0f ), Vector( 1.0f, 2.0f, -3.0f ) ).end();
   runtime.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

   TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( 90.0f ), Vector( 1.0f, 2.0f, -3.0f ) );
   TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( 90.0f ), Vector( 1.0f, 2.0f, -3.0f ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, oppositeBoneRotations )
{
   Skeleton skeletonA, skeletonB;
   SkeletonPoseTool poseA( skeletonA );
   SkeletonPoseTool poseB( skeletonB );
   {
      poseA.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO ).buildSkeleton();
      poseB.startSkeleton( "boneA", Vector_OX, DEG2RAD( 180.0f ), Vector( 0.0f, 0.0f, 1.0f ) ).buildSkeleton();
   }

   SkeletonMapper mapper;
   mapper.setSkeletons( &skeletonA, &skeletonB )
      .mapBone( "boneA", "boneA" );

   SkeletonMapperComponent runtime;
   runtime.setMapper( &mapper );
   runtime.compileRuntime();

   // Let's make the bone in skeleton A turn 45 degrees about OX axis.
   poseA.start().rotate( "boneA", Vector_OX, DEG2RAD( -90.0f ) ).end();
   runtime.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

   TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
   TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( 90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, twoChainsWithSameRotation )
{
   Skeleton skeletonA;
   SkeletonPoseTool poseA( skeletonA );
   {
      poseA.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO )
         .bone( "boneB", "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) )
         .buildSkeleton();
   }

   Skeleton skeletonB;
   SkeletonPoseTool poseB( skeletonB );
   {
      poseB.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO )
         .bone( "boneB", "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) )
         .buildSkeleton();
   }

   SkeletonMapper mapper;
   mapper.setSkeletons( &skeletonA, &skeletonB )
      .mapBone( "boneA", "boneA" )
      .mapBone( "boneB", "boneB" );

   SkeletonMapperComponent runtime;
   runtime.setMapper( &mapper );
   runtime.compileRuntime();

   // rotating bone A
   {
      poseA.start().rotate( "boneA", Vector_OX, DEG2RAD( -90.0f ) ).end();
      runtime.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneB", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );

      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
   }

   // rotating bone B
   {
      poseA.start().rotate( "boneB", Vector_OX, DEG2RAD( -90.0f ) ).end();
      runtime.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneB", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 0.0f, 1.0f ) );

      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO );
      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 0.0f, 1.0f ) );
   }

   // rotating bone A & B
   {
      poseA.start().rotate( "boneA", Vector_OX, DEG2RAD( -90.0f ) ).rotate( "boneB", Vector_OX, DEG2RAD( 90.0f ) ).end();
      runtime.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneB", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 1.0f, 0.0f ) );

      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, twoChainsWithOppositeRotations )
{
   Skeleton skeletonA;
   SkeletonPoseTool poseA( skeletonA );
   {
      poseA.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO )
         .bone( "boneB", "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) )
         .buildSkeleton();
   }

   Skeleton skeletonB;
   SkeletonPoseTool poseB( skeletonB );
   {
      poseB.startSkeleton( "boneB", Vector_OX, DEG2RAD( 180.0f ), Vector( 0.0f, 0.0f, 2.0f ) )
         .bone( "boneA", "boneB", Vector_OX, DEG2RAD( 180.0f ), Vector( 0.0f, 0.0f, 1.0f ) )
         .buildSkeleton();
   }

   SkeletonMapper mapper;
   mapper.setSkeletons( &skeletonA, &skeletonB )
      .mapBone( "boneA", "boneA" )
      .mapBone( "boneB", "boneB" );

   SkeletonMapperComponent runtime;
   runtime.setMapper( &mapper );
   runtime.compileRuntime();

   // rotating bone A
   {
      poseA.start().rotate( "boneA", Vector_OX, DEG2RAD( -90.0f ) ).end();
      runtime.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneB", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );

      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD(  90.0f ), Vector( 0.0f, 2.0f, 0.0f ) );
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD(  90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
   }

   // rotating bone B
   {
      poseA.start().rotate( "boneB", Vector_OX, DEG2RAD( -90.0f ) ).end();
      runtime.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD(  0.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneB", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 0.0f, 1.0f ) );

      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD(  90.0f ), Vector( 0.0f, 1.0f, 1.0f ) );
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( 180.0f ), Vector( 0.0f, 0.0f, 1.0f ) );
   }

   // rotating bone A & B
   {
      poseA.start().rotate( "boneA", Vector_OX, DEG2RAD( -90.0f ) ).rotate( "boneB", Vector_OX, DEG2RAD( 90.0f ) ).end();
      runtime.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneB", Vector_OX, DEG2RAD(  0.0f ), Vector( 0.0f, 1.0f, 0.0f ) );

      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD( 180.0f ), Vector( 0.0f, 1.0f, 1.0f ) );
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD(  90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
   }

}

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, oneToManyMappingWithSameRotation )
{
   Skeleton skeletonA;
   SkeletonPoseTool poseA( skeletonA );
   {
      poseA.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO )
         .bone( "boneC", "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 2.0f ) )
      .buildSkeleton();
   }

   Skeleton skeletonB;
   SkeletonPoseTool poseB( skeletonB );
   {
      poseB.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO )
         .bone( "boneB", "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) )
         .bone( "boneC", "boneB", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 2.0f ) )
      .buildSkeleton();
   }

   SkeletonMapper mapper;
   mapper.setSkeletons( &skeletonA, &skeletonB )
      .addSourceChain( "chain1", "boneA", "boneA" )
      .addSourceChain( "chain2", "boneC", "boneC" )
      .addTargetChain( "chain1", "boneA", "boneB" )
      .addTargetChain( "chain2", "boneC", "boneC" )
      .mapChain( "chain1", "chain1" )
      .mapChain( "chain2", "chain2" );

   SkeletonMapperComponent runtime;
   runtime.setMapper( &mapper );
   runtime.compileRuntime();

   // rotating bone A
   {
      poseA.start().rotate( "boneA", Vector_OX, DEG2RAD( -90.0f ) ).end();
      runtime.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // pose A check
      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneC", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 2.0f, 0.0f ) );

      // pose B check:
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
      TEST_BONE( poseB, "boneC", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 2.0f, 0.0f ) );
   }

   // rotating bone C
   {
      poseA.start().rotate( "boneC", Vector_OX, DEG2RAD( -90.0f ) ).end();
      runtime.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // pose A check
      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneC", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 0.0f, 2.0f ) );

      // pose B check - boneA and bone B hasn't moved, and mapping should reflect that
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO );
      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) );
      TEST_BONE( poseB, "boneC", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 0.0f, 2.0f ) );
   }

   // rotating bones A & C
   {
      poseA.start().rotate( "boneA", Vector_OX, DEG2RAD( -90.0f ) ).rotate( "boneC", Vector_OX, DEG2RAD( 90.0f ) ).end();
      runtime.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // pose A check
      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneC", Vector_OX, DEG2RAD(   0.0f ), Vector( 0.0f, 2.0f, 0.0f ) );

      // pose B check - boneA and bone B hasn't moved, and mapping should reflect that
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
      TEST_BONE( poseB, "boneC", Vector_OX, DEG2RAD(   0.0f ), Vector( 0.0f, 2.0f, 0.0f ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, oneToManyMappingWithOppositeRotations )
{
   Skeleton skeletonA;
   SkeletonPoseTool poseA( skeletonA );
   {
      poseA.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO )
         .bone( "boneC", "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 2.0f ) )
         .buildSkeleton();
   }

   Skeleton skeletonB;
   SkeletonPoseTool poseB( skeletonB );
   {
      poseB.startSkeleton( "boneC", Vector_OX, DEG2RAD( 180.0f ), Vector( 0.0f, 0.0f, 3.0f ) )
         .bone( "boneB", "boneC", Vector_OX, DEG2RAD( 180.0f ), Vector( 0.0f, 0.0f, 2.0f ) )
         .bone( "boneA", "boneB", Vector_OX, DEG2RAD( 180.0f ), Vector( 0.0f, 0.0f, 1.0f ) )
         .buildSkeleton();
   }

   SkeletonMapper mapper;
   mapper.setSkeletons( &skeletonA, &skeletonB )
      .addSourceChain( "chain1", "boneA", "boneA" )
      .addSourceChain( "chain2", "boneC", "boneC" )
      .addTargetChain( "chain1", "boneC", "boneC" )
      .addTargetChain( "chain2", "boneB", "boneA" )
      .mapChain( "chain1", "chain2" )
      .mapChain( "chain2", "chain1" );

   SkeletonMapperComponent runtime;
   runtime.setMapper( &mapper );
   runtime.compileRuntime();

   // rotating bone A
   {
      poseA.start().rotate( "boneA", Vector_OX, DEG2RAD( -90.0f ) ).end();
      runtime.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // pose A check
      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneC", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 2.0f, 0.0f ) );

      // pose B check
      TEST_BONE( poseB, "boneC", Vector_OX, DEG2RAD( 90.0f ), Vector( 0.0f, 3.0f, 0.0f ) );
      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD( 90.0f ), Vector( 0.0f, 2.0f, 0.0f ) );
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( 90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
   }

   // rotating bone C
   {
      poseA.start().rotate( "boneC", Vector_OX, DEG2RAD( -90.0f ) ).end();
      runtime.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // pose A check
      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD(   0.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneC", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 0.0f, 2.0f ) );

      // pose B check
      TEST_BONE( poseB, "boneC", Vector_OX, DEG2RAD(  90.0f ), Vector( 0.0f, 1.0f, 2.0f ) );
      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD( 180.0f ), Vector( 0.0f, 0.0f, 2.0f ) );
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( 180.0f ), Vector( 0.0f, 0.0f, 1.0f ) );
   }

   // rotating bones A & C
   {
      poseA.start().rotate( "boneA", Vector_OX, DEG2RAD( -90.0f ) ).rotate( "boneC", Vector_OX, DEG2RAD( 90.0f ) ).end();
      runtime.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // pose A check
      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneC", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 2.0f, 0.0f ) );

      // pose B check - boneA and bone B hasn't moved, and mapping should reflect that
      TEST_BONE( poseB, "boneC", Vector_OX, DEG2RAD( 180.0f ), Vector( 0.0f, 2.0f, 1.0f ) );
      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD(  90.0f ), Vector( 0.0f, 2.0f, 0.0f ) );
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD(  90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
   }
}

///////////////////////////////////////////////////////////////////////////////
