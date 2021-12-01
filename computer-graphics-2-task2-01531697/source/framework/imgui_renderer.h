#ifndef INCLUDED_IMGUI_RENDERER
#define INCLUDED_IMGUI_RENDERER

#pragma once

#include "imgui.h"
#include <GL/gl.h>

class ImGuiRenderer
{
  private:
    static GLuint g_shader_program;
    static GLuint g_AttribLocationTex;
    static GLuint g_AttribLocationProjMtx;
    static GLuint g_VAOHandle;
    static GLuint g_VBOHandle;
    static GLuint g_ElementsHandle;
    static GLuint g_FontTexture;
    static GLuint g_AttribLocationPosition;
    static GLuint g_AttribLocationUV;
    static GLuint g_AttribLocationColor;
    
    static void render(ImDrawData* draw_data);
    bool createDeviceObjects();
    bool createFontsTexture();

  public:
    ImGuiRenderer();
    void init();
    void terminate();

    void newFrame();
    void setDisplaySize(unsigned int viewport_width, unsigned int viewport_height);
};

#endif  // INCLUDED_IMGUI_RENDERER