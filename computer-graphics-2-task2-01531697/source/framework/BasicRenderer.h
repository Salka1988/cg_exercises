


#ifndef INCLUDED_FRAMEWORK_BASIC_RENDERER
#define INCLUDED_FRAMEWORK_BASIC_RENDERER

#pragma once

#include <GL/platform/Renderer.h>
#include <GL/platform/Context.h>
#include <GL/platform/Window.h>
#include <GL/platform/DefaultDisplayHandler.h>


class BasicRenderer : public GL::platform::Renderer, public GL::platform::DefaultDisplayHandler
{
private:
	GL::platform::Context context;
	GL::platform::context_scope<GL::platform::Window> ctx;

protected:
	void swapBuffers();

public:
	BasicRenderer(const BasicRenderer&) = delete;
	BasicRenderer& operator =(const BasicRenderer&) = delete;

	BasicRenderer(GL::platform::Window& window, int version_major=3, int version_minor=3);
};

#endif  // INCLUDED_FRAMEWORK_BASIC_RENDERER
