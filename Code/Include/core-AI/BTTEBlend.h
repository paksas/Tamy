/// @file   core-AI/BTTEBlend.h
/// @brief  this effects performs a blends the poses generated by two states
#pragma once

#include "core-AI\BlendTreeTransitionEffect.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * This effects performs a blends the poses generated by two states.
 */
class BTTEBlend : public BlendTreeTransitionEffect
{
   DECLARE_ALLOCATOR( BTTEBlend, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   float                         m_duration;

public:
   /**
    * Constructor.
    *
    * @param duration
    */
   BTTEBlend( float duration = 1.0f );
   ~BTTEBlend();

   // -------------------------------------------------------------------------
   // BlendTreeTransitionEffect implementation
   // -------------------------------------------------------------------------
   void activateEffect( BlendTreePlayer* player, const BlendTreeNode* sourceNode, const BlendTreeNode* targetNode ) const override;
   void combinePoses( BlendTreePlayer* player, float timeDelta, Transform* generatedPose, Transform& outAccMotion, uint bonesCount ) const override;
};

///////////////////////////////////////////////////////////////////////////////
