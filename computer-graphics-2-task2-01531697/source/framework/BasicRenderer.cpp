


#include "BasicRenderer.h"


BasicRenderer::BasicRenderer(GL::platform::Window& window, int version_major, int version_minor)
	: context(window.createContext(version_major, version_minor, true)),
	  ctx(context, window)
{
}

void BasicRenderer::swapBuffers()
{
	ctx.swapBuffers();
}
