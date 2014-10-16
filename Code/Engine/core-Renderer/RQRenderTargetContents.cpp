#include "core-Renderer\RQRenderTargetContents.h"
#include "core-Renderer\Renderer.h"


///////////////////////////////////////////////////////////////////////////////

RQRenderTargetContents::RQRenderTargetContents( SurfaceQuery* query )
   : m_query( query )
{
}

///////////////////////////////////////////////////////////////////////////////

RQRenderTargetContents::~RQRenderTargetContents()
{
}

///////////////////////////////////////////////////////////////////////////////

void RQRenderTargetContents::execute( Renderer& renderer, RenderTarget2D* queryRT )
{
   new ( renderer.rtComm() ) RCGetRenderTargetContents( queryRT, m_query );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RCGetRenderTargetContents::RCGetRenderTargetContents( const RenderTarget2D* renderTarget, SurfaceQuery* query )
   : m_renderTarget( renderTarget )
   , m_query( query )
{
}

///////////////////////////////////////////////////////////////////////////////

RCGetRenderTargetContents::~RCGetRenderTargetContents()
{
}

///////////////////////////////////////////////////////////////////////////////
