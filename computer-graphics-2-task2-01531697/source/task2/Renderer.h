#ifndef INCLUDED_RENDERER
#define INCLUDED_RENDERER

#pragma once

#include "Camera.h"
#include "Cloth.h"
#include "SceneConfig.h"
#include "framework/BasicRenderer.h"
#include "framework/imgui_renderer.h"
#include "math/vector.h"
#include <GL/gl.h>
#include <GL/shader.h>
#include <chrono>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>

class Renderer : public BasicRenderer
{
public:
  enum class WindType : int
  {
    NoWind = 0,
    LowSide = 1,
    StrongSide = 2,
    LowBottom = 3,
    StrongBottom = 4,
    MovingSide = 5
  };

private:
  ImGuiRenderer imgui_renderer_;

  int viewport_width_;
  int viewport_height_;

  Cloth cloth_;
  Camera& camera_;

  int scene_ = 0;
  bool reset_scene_ = false;

  SceneConfig scene_config_;
  std::string scene_config_file_;

  size_t mod_frames_write_ = 0;
  size_t max_frames_write_ = 0;
  size_t frames_written_ = 0;
  bool write_mode_ = false;
  bool write_images_ = false;
  size_t current_simulation_frame_ = 0;
  std::string write_path_;

  GL::VertexArray vao_sphere_;
  GL::Buffer vbo_sphere_;
  GL::Buffer ebo_sphere_;
  uint32_t num_sphere_indices_;

  WindType wind_type_{ WindType::NoWind };

  GL::VertexShader cloth_vs_;
  GL::FragmentShader cloth_fs_;
  GL::Program cloth_prog_;

  GL::VertexShader sphere_vs_;
  GL::FragmentShader sphere_fs_;
  GL::Program sphere_prog_;

  bool wireframe_ = false;

  std::chrono::high_resolution_clock::time_point last_simulation_time_point_;
  float simulation_time_ = 0.f;
  float time_accumulator_ = 0.f;

  // Simulation worker
  std::thread sim_thread_;
  std::condition_variable cv_render_;
  std::mutex mutex_render_;
  bool quit_ = false;
  bool paused_ = false;
  bool request_render_ = false;
  void simulation_loop();

  bool pause_at_frame_ = false;
  int frame_num_to_pause_ = 1;

  void applyWind();
  void renderScene();

public:
  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;

  Renderer(GL::platform::Window& window, Camera& camera, int scene, const std::string& scene_config_file,
    int2 grid_size, float2 cloth_size, float cloth_mass, float cloth_y_offset, size_t mod_frames_write, size_t max_frames_write, const std::string& write_path, bool write_images);
  ~Renderer();

  void resize(int width, int height);
  void render();
  void renderObstacles();
  void setupWind();
  void tooglePause() { paused_ = !paused_; }

  void doSimulationStep();
  void setupScene(int scene);
  void setScene(int scene) { scene_ = scene; reset_scene_ = true; }
  void setWind(WindType type) { wind_type_ = type; }

  void screenshot(const std::string& path);
};

#endif // INCLUDED_RENDERER
