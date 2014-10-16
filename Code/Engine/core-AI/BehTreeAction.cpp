#include "core-AI\BehTreeAction.h"
#include "core-AI\BehaviorTreeListener.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( BehTreeAction );
   PARENT( BehTreeNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BehTreeAction::~BehTreeAction()
{
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeAction::createLayout( BehaviorTreeRunner& runner ) const
{
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeAction::initialize( BehaviorTreeRunner& runner ) const
{
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeAction::deinitialize( BehaviorTreeRunner& runner ) const
{
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeAction::pullStructure( BehaviorTreeListener* listener )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeAction::onHostTreeSet( BehaviorTree* tree )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////
