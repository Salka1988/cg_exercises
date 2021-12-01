#include "Renderer.h"
#include "framework/sphere.h"
#include "framework/png.h"
#include "GL/gl.h"
#include "GL/error.h"
#include "GL/shader.h"
#include "GL/platform/Application.h"
#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <algorithm>
#include <limits>
#include <random>
#include <stdint.h>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>


Renderer::Renderer(GL::platform::Window& window, Camera& camera, int scene, const std::string& scene_config_file,
  int2 grid_size, float2 cloth_size, float cloth_mass, float cloth_y_offset, size_t mod_frames_write, size_t max_frames_write, const std::string& write_path, bool write_images)
  : BasicRenderer(window), camera_(camera), scene_(scene), scene_config_file_(scene_config_file),
  mod_frames_write_(mod_frames_write), max_frames_write_(max_frames_write), write_path_(write_path)
{
  scene_config_.grid_size = grid_size;
  scene_config_.cloth_size = cloth_size;
  scene_config_.cloth_mass = cloth_mass;
  scene_config_.cloth_y_offset = cloth_y_offset;

  // init openGL
  glClearColor(0.3f, 0.5f, 1.0f, 1.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  GL::checkError();

  // Setup shader program
  const char* src_cloth_vs =
  {
  R"""(
  #version 330

  layout(location = 0) in vec3 v_position;
  layout(location = 1) in vec3 v_normal;
  layout(location = 2) in vec4 v_color;

  uniform mat4 ProjectionView;

  out vec3 position;
  out vec3 normal;
  out vec4 color;

  void main()
  {
    gl_Position = ProjectionView * vec4(v_position, 1.f);
    normal = v_normal;
    color = v_color;
  }

  )"""
  };

  const char* src_cloth_fs =
  {
  R"""(
  #version 330

  uniform vec3 camera_position;
  uniform vec3 camera_direction;

  in vec3 position;
  in vec3 normal;
  in vec4 color;

  layout(location = 0) out vec4 fragment_color;

  void main()
  {
    vec3 lightDir = -camera_direction;
    float diffuse = abs(dot(lightDir, normal));
    fragment_color = vec4(color.xyz * diffuse, color.w);
  }
  )"""
  };

  const char* src_sphere_vs =
  {
  R"""(
  #version 330

  layout(location = 0) in vec3 v_position;

  uniform mat4 ProjectionView;
  uniform vec3 pos;
  uniform float radius;

  out vec3 normal;
  out vec4 color;

  void main()
  {
    gl_Position = ProjectionView * vec4(pos + radius * v_position, 1.f);
    normal = normalize(v_position);
    color = vec4(0.7f, 0.7f, 0.8f, 1.f);
  }
  )"""
  };

  const char* src_sphere_fs =
  {
  R"""(
  #version 330

  uniform vec3 camera_position;
  uniform vec3 camera_direction;

  in vec3 position;
  in vec3 normal;
  in vec4 color;

  layout(location = 0) out vec4 fragment_color;

  void main()
  {
    vec3 lightDir = -camera_direction;
    float diffuse = abs(dot(lightDir, normalize(normal)));
    fragment_color = vec4(color.xyz * diffuse, color.w);
  }
  )"""
  };

  cloth_vs_ = GL::compileVertexShader(src_cloth_vs);
  cloth_fs_ = GL::compileFragmentShader(src_cloth_fs);

  glAttachShader(cloth_prog_, cloth_vs_);
  glAttachShader(cloth_prog_, cloth_fs_);
  GL::linkProgram(cloth_prog_);

  sphere_vs_ = GL::compileVertexShader(src_sphere_vs);
  sphere_fs_ = GL::compileFragmentShader(src_sphere_fs);

  glAttachShader(sphere_prog_, sphere_vs_);
  glAttachShader(sphere_prog_, sphere_fs_);
  GL::linkProgram(sphere_prog_);

  GL::checkError();

  // Setup sphere
  IcoSphere sphere;
  sphere.Create(3);
  num_sphere_indices_ = static_cast<uint32_t> (sphere.indices.size());

  glBindVertexArray(vao_sphere_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_);
  glBufferData(GL_ARRAY_BUFFER, sphere.vertices.size() * 3 * 4, sphere.vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_sphere_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(uint32_t) * num_sphere_indices_), sphere.indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0U);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float3), reinterpret_cast<void*> (NULL));
  glBindVertexArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  GL::checkError();

  // Setup ImGui
  imgui_renderer_.init();
  imgui_renderer_.setDisplaySize(camera.getViewport().x, camera.getViewport().y);
  window.attach(this);

  viewport_height_ = camera.getViewport().y;
  viewport_width_ = camera.getViewport().x;

  // Init scene
  if (!scene_config_file.empty())
    scene_config_ = SceneConfig::read(scene_config_file);

  setupScene(scene_);

  last_simulation_time_point_ = std::chrono::high_resolution_clock::now();

  // Check if write mode is active
  write_mode_ = max_frames_write_ > 0;
  write_images_ = write_images;

  // Init simulation thread
  if (!write_mode_)
    sim_thread_ = std::thread(&Renderer::simulation_loop, this);
}

Renderer::~Renderer()
{
  quit_ = true;
  if (sim_thread_.joinable())
    sim_thread_.join();
  imgui_renderer_.terminate();
  ImGui::DestroyContext();
}

void Renderer::resize(int width, int height)
{
  viewport_width_ = width;
  viewport_height_ = height;
  camera_.setViewport(width, height);
  imgui_renderer_.setDisplaySize(width, height);
}

void Renderer::simulation_loop()
{
  while (!quit_)
  {
    if (request_render_)
    {
      std::unique_lock<std::mutex> lock(mutex_render_);
      cv_render_.notify_all();
      cv_render_.wait(lock);
    }

    if (!paused_)
    {
      auto next_time = std::chrono::high_resolution_clock::now();
      auto elapsed_time = std::chrono::duration<float>(next_time - last_simulation_time_point_).count();
      last_simulation_time_point_ = next_time;
      time_accumulator_ += elapsed_time;

      if (time_accumulator_ > scene_config_.dt)
      {
        last_simulation_time_point_ = std::chrono::high_resolution_clock::now();
        doSimulationStep();
        auto now = std::chrono::high_resolution_clock::now();
        simulation_time_ = std::chrono::duration<float>(now - last_simulation_time_point_).count();
        time_accumulator_ = 0.f;

        if (pause_at_frame_ && (current_simulation_frame_ % frame_num_to_pause_ == 0))
          paused_ = true;
      }
    }
  }
}

void Renderer::render()
{
  // apply opengl settings and clear default framebuffer
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_SCISSOR_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, wireframe_ ? GL_LINE : GL_FILL);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GL::checkError();

  // Set program and write uniforms
  float4x4 PV = transpose(camera_.getProjection() * camera_.getView());
  auto camera_direction = camera_.getDirection(); //camear_.getDirection() returns a temporary object, cannot get address of that
  glUseProgram(sphere_prog_);
  glUniform3fv(glGetUniformLocation(sphere_prog_, "camera_position"), 1, reinterpret_cast<const float*>(&camera_.getPosition()));
  glUniform3fv(glGetUniformLocation(sphere_prog_, "camera_direction"), 1, reinterpret_cast<const float*>(&camera_direction));
  glUniformMatrix4fv(glGetUniformLocation(sphere_prog_, "ProjectionView"), 1, GL_FALSE, PV._m);
  glUseProgram(cloth_prog_);
  glUniform3fv(glGetUniformLocation(cloth_prog_, "camera_position"), 1, reinterpret_cast<const float*>(&camera_.getPosition()));
  glUniform3fv(glGetUniformLocation(cloth_prog_, "camera_direction"), 1, reinterpret_cast<const float*>(&camera_direction));
  glUniformMatrix4fv(glGetUniformLocation(cloth_prog_, "ProjectionView"), 1, GL_FALSE, PV._m);
  GL::checkError();

  // Interactive mode
  if (!write_mode_)
  {
    // gui calls
    ImGui::NewFrame();

    {
      std::unique_lock<std::mutex> lock(mutex_render_);
      request_render_ = true;
      cv_render_.wait(lock);
    }

    // Check for scene reset request
    if (reset_scene_)
    {
      setupScene(scene_);
      reset_scene_ = false;
    }

    // GUI
    ImGui::SetNextWindowPos(ImVec2(5.f, 5.f), ImGuiCond_FirstUseEver);
    ImGui::Begin(("Simulation: " + std::to_string(simulation_time_) + "s###sim").c_str(), NULL, ImVec2(330, 440));

    ImGui::Text("Scene: %d", scene_);
    if (ImGui::Button("Do step") && paused_)
      doSimulationStep();
    ImGui::SameLine();
    if (ImGui::Button("Reset Simulation"))
      setupScene(scene_);
    ImGui::SameLine();
    if (ImGui::Button("Reset Camera"))
      camera_.getNavigator().reset();

    ImGui::Text("Simulation frame: %zu", current_simulation_frame_);
    ImGui::Checkbox("Autopause at sim-frame mod", &pause_at_frame_); 
    ImGui::SameLine();
    ImGui::PushItemWidth(100);
    ImGui::DragInt("###frame_num", &frame_num_to_pause_, 1.f, 1, std::numeric_limits<int>::max());
    ImGui::PopItemWidth();
    ImGui::Checkbox("Paused", &paused_);
    ImGui::Checkbox("Wireframe", &wireframe_);
    ImGui::Separator();
    ImGui::PushItemWidth(150);
    ImGui::DragFloat("Timestep", &scene_config_.dt, 0.0001f, 0.0001f, 0.1f, "%.4f");
    ImGui::DragFloat("Dampening", &scene_config_.damp, 0.001f, 0.f, 1.f);
    ImGui::DragFloat("Fixup %", &scene_config_.fixup_percent, 0.001f, 0.01f, 1.f);
    ImGui::DragInt("Fixup iterations", &scene_config_.fixup_iterations, 0.5f, 1, 100);
    ImGui::DragFloat3("Gravity", &scene_config_.gravity.x, 0.1f, -1000.f, 1000.f, "%.1f");
    ImGui::PopItemWidth();

    if (ImGui::Combo(" ", reinterpret_cast<int*> (&wind_type_), "NoWind\0LowSide\0StrongSide\0LowBottom\0StrongBottom\0MovingSide\0\0"))
      setupWind();
    ImGui::DragFloat("WindStrength", &scene_config_.wind_multiplier, 0.1f, 0.f, 100.f);

    ImGui::Separator();

    static float2 new_cloth_size = scene_config_.cloth_size;
    static int2 new_grid_size = scene_config_.grid_size;
    static float new_cloth_mass = scene_config_.cloth_mass;
    ImGui::DragFloat2("ClothSize", &new_cloth_size.x, 0.1f, 0.1f, 1000.f, "%.1f");
    ImGui::DragInt2("GridSize", &new_grid_size.x, 1.f, 2, 1000);
    ImGui::DragFloat("Cloth mass", &new_cloth_mass, 0.1f, 0.1f, 100.f);
    if (ImGui::Button("Apply cloth"))
    {
      scene_config_.cloth_size = new_cloth_size;
      scene_config_.grid_size = new_grid_size;
      scene_config_.cloth_mass = new_cloth_mass;
      setupScene(scene_);
    }
    if (ImGui::Button("Write scene"))
      SceneConfig::write(scene_config_, "scene_config.json");
    ImGui::End();

    // Update gpu buffers
    cloth_.updateBuffers();

    {
      std::unique_lock<std::mutex> lock(mutex_render_);
      request_render_ = false;
      cv_render_.notify_all();
    }

    // Render scene and gui
    renderScene();
    ImGui::Render();
  }
  else // Write mode
  {
    // Update gpu buffers and render scene
    cloth_.updateBuffers();
    renderScene();

    // Take screenshot and store on disk
    if (write_images_)
      screenshot(write_path_ + "/frame_" + std::to_string(current_simulation_frame_));
    frames_written_++;

    // Advance simulation frames till modulo criterium is met
    do
      doSimulationStep();
    while (current_simulation_frame_ % mod_frames_write_ != 0);
  }
  swapBuffers();

  if (write_mode_ && frames_written_ >= max_frames_write_)
    GL::platform::quit();
}

void Renderer::renderScene()
{
  cloth_.render();
  renderObstacles();
}

void Renderer::doSimulationStep()
{
  applyWind();
  cloth_.doSimStep(scene_config_.gravity, scene_config_.wind_multiplier * scene_config_.wind, scene_config_.damp,
    scene_config_.fixup_percent, scene_config_.fixup_iterations, scene_config_.obstacles, scene_config_.dt, scene_config_.apply_structural_fixup,
    scene_config_.apply_shear_fixup, scene_config_.apply_flexion_fixup);
  current_simulation_frame_++;
}

void Renderer::renderObstacles()
{
  glUseProgram(sphere_prog_);
  glBindVertexArray(vao_sphere_);

  for (auto& sphere : scene_config_.obstacles)
  {
    glUniform3fv(glGetUniformLocation(sphere_prog_, "pos"), 1, reinterpret_cast<const float*>(&sphere.position.x));
    float r = sphere.radius - 0.01f;
    glUniform1fv(glGetUniformLocation(sphere_prog_, "radius"), 1, reinterpret_cast<const float*>(&r));
    glDrawElements(GL_TRIANGLES, num_sphere_indices_, GL_UNSIGNED_INT, nullptr);
  }
  GL::checkError();
}

void Renderer::setupScene(int scene)
{
  current_simulation_frame_ = 0;
  float3 cloth_offset = { 0.f, scene_config_.cloth_y_offset, 0.f };

  if (!scene_config_file_.empty())
  {
    cloth_.initGrid(cloth_offset, scene_config_.grid_size, scene_config_.cloth_size, scene_config_.cloth_mass, scene_config_.fixed_particles);
    return;
  }

  scene_ = scene;
  scene_config_.fixed_particles.clear();
  scene_config_.obstacles.clear();

  setupWind();

  switch (scene)
  {
    case 0:
    {
      scene_config_.fixed_particles.push_back({ 0,0 });
      scene_config_.fixed_particles.push_back({ scene_config_.grid_size.x - 1 ,0 });
      cloth_.initGrid(cloth_offset, scene_config_.grid_size, scene_config_.cloth_size, scene_config_.cloth_mass, scene_config_.fixed_particles);
      break;
    }
    case 1:
    {
      scene_config_.fixed_particles.push_back({ 0,0 });
      cloth_.initGrid(cloth_offset, scene_config_.grid_size, scene_config_.cloth_size, scene_config_.cloth_mass, scene_config_.fixed_particles);
      break;
    }
    case 2:
    {
      scene_config_.fixed_particles.push_back({ 0,0 });
      scene_config_.fixed_particles.push_back({ scene_config_.grid_size.x - 1 ,0 });
      scene_config_.fixed_particles.push_back({ scene_config_.grid_size.x - 1 , scene_config_.grid_size.y - 1 });
      cloth_.initGrid(cloth_offset, scene_config_.grid_size, scene_config_.cloth_size, scene_config_.cloth_mass, scene_config_.fixed_particles);
      break;
    }
    case 3:
    {
      scene_config_.fixed_particles.push_back({ 0,0 });
      scene_config_.fixed_particles.push_back({ scene_config_.grid_size.x - 1 ,0 });
      scene_config_.obstacles.push_back(Sphere({ {0.f, 0.f, -0.1f}, 0.1f }));
      cloth_.initGrid(cloth_offset, scene_config_.grid_size, scene_config_.cloth_size, scene_config_.cloth_mass, scene_config_.fixed_particles);
      break;
    }
    case 4:
    {
      scene_config_.fixed_particles.push_back({ 0,0 });
      scene_config_.fixed_particles.push_back({ scene_config_.grid_size.x - 1 ,0 });
      scene_config_.obstacles.push_back(Sphere({ {-0.2f, 0.f, -0.1f}, 0.1f }));
      scene_config_.obstacles.push_back(Sphere({ {0.3f, 0.f, -0.1f}, 0.2f }));
      cloth_.initGrid(cloth_offset, scene_config_.grid_size, scene_config_.cloth_size, scene_config_.cloth_mass, scene_config_.fixed_particles);
      break;
    }
    case 5:
    {
      scene_config_.obstacles.push_back(Sphere({ {0.f, 0.f, 0.f}, 0.1f }));
      cloth_.initGrid(cloth_offset, scene_config_.grid_size, scene_config_.cloth_size, scene_config_.cloth_mass, scene_config_.fixed_particles);
      break;
    }
    case 6:
    {
      float d = 0.2f;
      scene_config_.obstacles.push_back(Sphere({ {d, 0.f, d}, 0.1f }));
      scene_config_.obstacles.push_back(Sphere({ {d, 0.f, -d}, 0.1f }));
      scene_config_.obstacles.push_back(Sphere({ {-d, 0.f, d}, 0.1f }));
      scene_config_.obstacles.push_back(Sphere({ {-d, 0.f, -d}, 0.1f }));
      cloth_.initGrid(cloth_offset, scene_config_.grid_size, scene_config_.cloth_size, scene_config_.cloth_mass, scene_config_.fixed_particles);
      break;
    }
  }
}

void Renderer::setupWind()
{
  float wind_per_particle_factor = 20.f / (scene_config_.grid_size.x * scene_config_.grid_size.y);
  float3 wind;
  switch (wind_type_)
  {
  case WindType::LowSide:
    wind.y = 0.f;
    wind.x = 0.5f * wind_per_particle_factor;
    wind.z = 1.2f * wind_per_particle_factor;
    break;
  case WindType::StrongSide:
    wind.y = 0.f;
    wind.x = 1.2f * wind_per_particle_factor;
    wind.z = 2.5f * wind_per_particle_factor;
    break;
  case WindType::LowBottom:
    wind.y = 0.5f * wind_per_particle_factor;
    wind.x = 0.f;
    wind.z = 0.f;
    break;
  case WindType::StrongBottom:
    wind.y = 1.5f * wind_per_particle_factor;
    wind.x = 0.f;
    wind.z = 0.f;
    break;
  case WindType::MovingSide:
    wind.y = 0.f;
    wind.x = 0.25f * wind_per_particle_factor;
    wind.z = 1.5f * wind_per_particle_factor;
    break;
  case WindType::NoWind:
    wind.x = wind.y = wind.z = 0.f;
    break;
  }

  scene_config_.wind_per_particle_factor = wind_per_particle_factor;
  scene_config_.wind = wind;
}

void Renderer::applyWind()
{
  // Update wind forces
  if (wind_type_ == WindType::MovingSide)
  {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(
      -0.5f * scene_config_.wind_per_particle_factor,
      0.5f * scene_config_.wind_per_particle_factor);
    scene_config_.wind.x += dist(gen);
    scene_config_.wind.z += dist(gen);
    float maxwind = 2.0f * scene_config_.wind_per_particle_factor;
    scene_config_.wind.x = std::min(maxwind, std::max(-maxwind, scene_config_.wind.x));
    scene_config_.wind.z = std::min(maxwind, std::max(-maxwind, scene_config_.wind.z));
  }
}

void Renderer::screenshot(const std::string& path)
{
  image<uint32_t> buffer(viewport_width_, viewport_height_);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glReadBuffer(GL_BACK);
  glReadPixels(0, 0, viewport_width_, viewport_height_, GL_RGBA, GL_UNSIGNED_BYTE, data(buffer));
  GL::checkError();

  std::string output_file = path + ".png";
  std::cout << "Creating " << viewport_width_ << "x" << viewport_height_ << " image '" << output_file << "'" << std::endl;

  for (int y = 0; y < height(buffer) / 2; ++y)
    for (int x = 0; x < width(buffer); ++x)
      std::swap(buffer(x, y), buffer(x, height(buffer) - y - 1));

  PNG::saveImage(output_file.c_str(), buffer);
}
