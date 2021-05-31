//
// Created by xenon on 28.05.2021.
//

#include "Game.h"
#include "GameContext.h"

Game* Game::instance = nullptr;

namespace {

void PauseKeyCallback(GLFWwindow* window, int key, int scancode, int action,
                      int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    Game::GetInstance()->PopState();
  }
}

void GameKeyCallback(GLFWwindow* window, int key, int scancode, int action,
                     int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    Game::GetInstance()->PushState(EGameStatePause);
    return;
  }

  GameContext* ctx =
      reinterpret_cast<GameContext*>(Game::GetInstance()->GetContext().get());

  if (action == GLFW_PRESS) {
    switch (key) {
    case GLFW_KEY_UP:
      ctx->GoForward();
      break;
    case GLFW_KEY_DOWN:
      ctx->GoBackward();
      break;
    case GLFW_KEY_LEFT:
      ctx->TurnLeft();
      break;
    case GLFW_KEY_RIGHT:
      ctx->TurnRight();
      break;
    case GLFW_KEY_V:
      ctx->SwitchView();
      break;
    case GLFW_KEY_SPACE:
      ctx->GetCharacter().StartJumping();
      break;
    }
  } else if (action == GLFW_RELEASE) {
    switch (key) {
    case GLFW_KEY_UP:
    case GLFW_KEY_DOWN:
      ctx->StopGo();
      break;
    case GLFW_KEY_LEFT:
    case GLFW_KEY_RIGHT:
      ctx->StopTurn();
      break;
    case GLFW_KEY_SPACE:
      ctx->GetCharacter().StopJumping();
      break;
    }
  }
}

void WindowSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);

  auto& scene        = Game::GetInstance()->GetScene();
  float const aspect = width / static_cast<float>(height);
  float const fov    = 50.0f * M_PI / 180.0f;
  float const z_min  = 0.1f;
  float const z_max  = 100.0f;
  scene.projection   = vcl::projection_perspective(fov, aspect, z_min, z_max);
  scene.projection_inverse =
      vcl::projection_perspective_inverse(fov, aspect, z_min, z_max);
}

const StateCallbacks kStateCallbacks[] = {
    {nullptr, nullptr, nullptr}, // EGameStateMain
    {nullptr, GameKeyCallback,
     []() -> SceneContext* { return new GameContext(); }}, // EGameStateGame
    {nullptr, PauseKeyCallback, nullptr},                  // EGameStatePause
    {nullptr, nullptr, nullptr},                           // EGameStateScores
};

} // namespace
Game::Game(int width, int height)
    : window_(vcl::create_window(width, height)) {
  if (instance != nullptr) {
    throw std::runtime_error("class Game() is a singleton!");
  }
  instance = this;

  std::cerr << vcl::opengl_info_display() << std::endl;
  vcl::imgui_init(window_);
  glfwSetWindowSizeCallback(window_, WindowSizeCallback);
  WindowSizeCallback(window_, width, height);

  InitVCL();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  PushState(EGameStateMain);
}

Game::~Game() {
  while (GetState() != EGameStateMain) {
    PopState();
  }
  vcl::imgui_cleanup();
  glfwDestroyWindow(window_);
  glfwTerminate();
  instance = nullptr;
}

void Game::run() {
  glEnable(GL_DEPTH_TEST);
  while (!glfwWindowShouldClose(window_)) {
    scene_.light = scene_.camera.position();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    RenderScene();
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

void Game::RenderMainScene() {
  glClearColor(1.0f, 0.5f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

  vcl::imgui_create_frame();
  ImGui::Begin("Main", NULL, ImGuiWindowFlags_AlwaysAutoResize);
  bool btn_pressed = false;
  if (ImGui::Button("Play", {100, 100})) {
    ImGui::Text("Loading...");
    btn_pressed = true;
  }
  ImGui::Text("");
  ImGui::End();

  vcl::imgui_render_frame(window_);

  if (btn_pressed) {
    PushState(EGameStateGame);
  }
}

void Game::RenderScene() {
  switch (GetState()) {
  case EGameStateMain:
    RenderMainScene();
    break;
  case EGameStateGame:
    RenderGameScene();
    break;
  case EGameStatePause:
    RenderPauseScene();
    break;
  case EGameStateScores:
    RenderScoreScene();
    break;
  default:
    throw std::runtime_error("Bad game state");
  }
}

void Game::RenderGameScene() {
  glClearColor(0.4f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

  vcl::imgui_create_frame();
  GameContext* ctx = reinterpret_cast<GameContext*>(GetContext().get());
  ctx->Update();
  ctx->Render();
  vcl::imgui_render_frame(window_);
}

void Game::RenderPauseScene() {
  glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

  vcl::imgui_create_frame();
  ImGui::Begin("Pause", NULL, ImGuiWindowFlags_AlwaysAutoResize);
  if (ImGui::Button("Exit", {100, 100})) {
    while (GetState() != EGameStateMain) {
      PopState();
    }
  }
  ImGui::End();

  vcl::imgui_render_frame(window_);
}

void Game::RenderScoreScene() {}

EGameState Game::GetState() const { return state_stack_.back().state; }

void Game::PushState(EGameState state) {
  const auto& callbacks = GetStateCallbacks(state);
  UpdateCallbacks(callbacks);

  std::shared_ptr<SceneContext> ctx = nullptr;
  if (callbacks.BuildContext) {
    ctx.reset(callbacks.BuildContext());
  }
  state_stack_.push_back({state, std::move(ctx)});
}

void Game::PopState() {
  state_stack_.pop_back();
  const auto& callbacks = GetStateCallbacks(GetState());
  UpdateCallbacks(callbacks);
}

StateCallbacks Game::GetStateCallbacks(EGameState state) {
  return kStateCallbacks[state];
}
Game* Game::GetInstance() { return instance; }

void Game::UpdateCallbacks(const StateCallbacks& callbacks) {
  glfwSetCursorPosCallback(window_, callbacks.OnMouseMove);
  glfwSetKeyCallback(window_, callbacks.OnKey);
}
Game::Scene& Game::GetScene() { return scene_; }

const std::shared_ptr<SceneContext>& Game::GetContext() const {
  return state_stack_.back().context;
}
void Game::InitVCL() {
  using vcl::opengl_create_shader_program;
  using vcl::opengl_shader_preset;

  GLuint const shader_mesh =
      opengl_create_shader_program(opengl_shader_preset("mesh_vertex"),
                                   opengl_shader_preset("mesh_fragment"));
  GLuint const shader_uniform_color = opengl_create_shader_program(
      opengl_shader_preset("single_color_vertex"),
      opengl_shader_preset("single_color_fragment"));
  GLuint const texture_white = opengl_texture_to_gpu(
      vcl::image_raw{1, 1, vcl::image_color_type::rgba, {255, 255, 255, 255}});
  vcl::mesh_drawable::default_shader     = shader_mesh;
  vcl::mesh_drawable::default_texture    = texture_white;
  vcl::curve_drawable::default_shader    = shader_uniform_color;
  vcl::segments_drawable::default_shader = shader_uniform_color;
}
