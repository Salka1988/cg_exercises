#include "imgui_renderer.h"

GLuint ImGuiRenderer::g_shader_program = 0;
GLuint ImGuiRenderer::g_AttribLocationTex = 0;
GLuint ImGuiRenderer::g_AttribLocationProjMtx = 0;
GLuint ImGuiRenderer::g_VAOHandle = 0;
GLuint ImGuiRenderer::g_VBOHandle = 0;
GLuint ImGuiRenderer::g_ElementsHandle = 0;
GLuint ImGuiRenderer::g_FontTexture = 0;
GLuint ImGuiRenderer::g_AttribLocationPosition = 0;
GLuint ImGuiRenderer::g_AttribLocationUV = 0;
GLuint ImGuiRenderer::g_AttribLocationColor = 0;


ImGuiRenderer::ImGuiRenderer()
{

}

void ImGuiRenderer::init()
{
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.RenderDrawListsFn = &ImGuiRenderer::render;
  createDeviceObjects();
  ImGui::StyleColorsDark();
}

void ImGuiRenderer::terminate()
{
  if (g_VAOHandle) 
    glDeleteVertexArrays(1, &g_VAOHandle);
  if (g_VBOHandle) 
    glDeleteBuffers(1, &g_VBOHandle);
  if (g_ElementsHandle) 
    glDeleteBuffers(1, &g_ElementsHandle);
  g_VAOHandle = g_VBOHandle = g_ElementsHandle = 0;

  if (g_shader_program) 
    glDeleteProgram(g_shader_program);
  g_shader_program = 0;

  if (g_FontTexture)
  {
    glDeleteTextures(1, &g_FontTexture);
    ImGui::GetIO().Fonts->TexID = 0;
    g_FontTexture = 0;
  }
}

void ImGuiRenderer::setDisplaySize(unsigned int viewport_width, unsigned int viewport_height)
{
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize.x = static_cast<float>(viewport_width);
  io.DisplaySize.y = static_cast<float>(viewport_height);
}

void ImGuiRenderer::newFrame()
{
  ImGuiIO& io = ImGui::GetIO();
  //io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);
}

void ImGuiRenderer::render(ImDrawData* draw_data)
{
  ImGuiIO& io = ImGui::GetIO();
  int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
  int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
  if (fb_width == 0 || fb_height == 0)
    return;

  draw_data->ScaleClipRects(io.DisplayFramebufferScale);
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // setup viewport, orthographic projection matrix
  glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
  const float ortho_projection[4][4] =
  {
    {  2.0f / io.DisplaySize.x, 0.0f,                      0.0f, 0.0f },
    {  0.0f,                    2.0f / -io.DisplaySize.y,  0.0f, 0.0f },
    {  0.0f,                    0.0f,                     -1.0f, 0.0f },
    { -1.0f,                    1.0f,                      0.0f, 1.0f },
  };

  glUseProgram(g_shader_program);
  glUniform1i(g_AttribLocationTex, 0);
  glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
  glBindVertexArray(g_VAOHandle);
  glBindSampler(0, 0); // Rely on combined texture/sampler state.

  for (int n = 0; n < draw_data->CmdListsCount; n++)
  {
    const ImDrawList* cmd_list = draw_data->CmdLists[n];
    const ImDrawIdx* idx_buffer_offset = 0;

    glBindBuffer(GL_ARRAY_BUFFER, g_VBOHandle);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const void*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const void*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

    for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
    {
      const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
      if (pcmd->UserCallback)
      {
        pcmd->UserCallback(cmd_list, pcmd);
      }
      else
      {
        glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
        glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
        glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
      }
      idx_buffer_offset += pcmd->ElemCount;
    }
  }
}

bool ImGuiRenderer::createDeviceObjects()
{
  const GLchar *vertex_shader =
  R"""(
  #version 150
  uniform mat4 ProjMtx;
  in vec2 Position;
  in vec2 UV;
  in vec4 Color;
  out vec2 Frag_UV;
  out vec4 Frag_Color;
  void main()
  {
    Frag_UV = UV;
    Frag_Color = Color;
    gl_Position = ProjMtx * vec4(Position.xy,0,1);
  }
  )""";

  const GLchar* fragment_shader =
  R"""(
  #version 150
  uniform sampler2D Texture;
  in vec2 Frag_UV;
  in vec4 Frag_Color;
  out vec4 Out_Color;
  void main()
  {
    Out_Color = Frag_Color * texture( Texture, Frag_UV.st);
  }
  )""";

  g_shader_program = glCreateProgram();
  GLuint g_vs = glCreateShader(GL_VERTEX_SHADER);
  GLuint g_fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(g_vs, 1, &vertex_shader, 0);
  glShaderSource(g_fs, 1, &fragment_shader, 0);
  glCompileShader(g_vs);
  glCompileShader(g_fs);
  glAttachShader(g_shader_program, g_vs);
  glAttachShader(g_shader_program, g_fs);
  glLinkProgram(g_shader_program);
  glDeleteShader(g_vs);
  glDeleteShader(g_fs);

  g_AttribLocationTex = glGetUniformLocation(g_shader_program, "Texture");
  g_AttribLocationProjMtx = glGetUniformLocation(g_shader_program, "ProjMtx");
  g_AttribLocationPosition = glGetAttribLocation(g_shader_program, "Position");
  g_AttribLocationUV = glGetAttribLocation(g_shader_program, "UV");
  g_AttribLocationColor = glGetAttribLocation(g_shader_program, "Color");

  glGenBuffers(1, &g_VBOHandle);
  glGenBuffers(1, &g_ElementsHandle);

  glGenVertexArrays(1, &g_VAOHandle);
  glBindVertexArray(g_VAOHandle);
  glBindBuffer(GL_ARRAY_BUFFER, g_VBOHandle);
  glEnableVertexAttribArray(g_AttribLocationPosition);
  glEnableVertexAttribArray(g_AttribLocationUV);
  glEnableVertexAttribArray(g_AttribLocationColor);

  glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (void*)IM_OFFSETOF(ImDrawVert, pos));
  glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (void*)IM_OFFSETOF(ImDrawVert, uv));
  glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (void*)IM_OFFSETOF(ImDrawVert, col));

  createFontsTexture();
  return true;
}

bool ImGuiRenderer::createFontsTexture()
{
  // Build texture atlas
  ImGuiIO& io = ImGui::GetIO();
  unsigned char* pixels;
  int width, height;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

  // Upload texture to graphics system
  GLint last_texture;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  glGenTextures(1, &g_FontTexture);
  glBindTexture(GL_TEXTURE_2D, g_FontTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  // Store our identifier
  io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

  // Restore state
  glBindTexture(GL_TEXTURE_2D, last_texture);

  return true;
}