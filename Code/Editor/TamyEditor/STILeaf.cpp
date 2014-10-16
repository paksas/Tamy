#include "STILeaf.h"
#include "ext-StoryTeller\StoryNode.h"


///////////////////////////////////////////////////////////////////////////////

STILeaf::STILeaf( StoryNode& leaf )
   : m_leaf( leaf )
{
   updateDescription();
}

///////////////////////////////////////////////////////////////////////////////

StoryNode* STILeaf::getNode()
{
   return &m_leaf;
}

///////////////////////////////////////////////////////////////////////////////

void STILeaf::addNode( StoryNode* newNode )
{
   // not allowed here
}

///////////////////////////////////////////////////////////////////////////////

void STILeaf::insertNode( int insertionIdx, StoryNode* nodeToInsert )
{
   // not allowed here
   ASSERT_MSG( false, "Unsuported operation" );
}

///////////////////////////////////////////////////////////////////////////////

void STILeaf::removeNode( StoryBrowserTreeItem* removedChildItem )
{
   // not allowed here
}

///////////////////////////////////////////////////////////////////////////////

void STILeaf::clear()
{
   // not allowed here
}

///////////////////////////////////////////////////////////////////////////////
