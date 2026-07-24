#include "engine.hpp"

Engine::Engine() : m_isRunning(false), m_world(0.0f) {};
Engine::~Engine() {};

bool Engine::initialize()
{
  bool wantsFullscreen = false;
  if (!glfwInit())
  {
    std::cout << "GLFW initialization failed\n";
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  GLFWwindow* window =
      glfwCreateWindow(mode->width, mode->height, "CLngine",
                       (wantsFullscreen) ? monitor : nullptr, nullptr);

  if (window == nullptr)
  {
    std::cout << "GLFW Window initialization failed\n";
    return false;
  }

  m_window = window;

  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSwapInterval(1);
  // V-Sync on.

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "GLAD initialization error." << std::endl;
    return false;
  }
  glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
  // Background color.
  glEnable(GL_DEPTH_TEST);

  return true;
}

void Engine::createAndLoad()
{
  // Constants for user to define(instancing)
  int instance_count = 50;
  float bound = 2.0f;
  float mass = 1.0f;
  float radius = 0.05f;
  // * Redundant as of right now but kept just in case

  m_camera =
      Camera(20.0f, glm::vec3(1.5 * bound, 0.5 * bound, 0.0f)); // Speed(m/s)
  globalCamPtr = &m_camera;

  m_world = Physics(bound, 9.81f, 0.7f, 0.8f);

  Mesh sphere = MeshGen::CreateSphere(1.0f);
  MeshUtils::GLMesh GLSphere =
      MeshUtils::uploadToGPU(sphere.vertices, sphere.indices);
  m_group.initialize(GLSphere, instance_count, bound, mass, radius);

  Mesh cube = MeshGen::CreateCube(2.0f);
  m_cube = MeshUtils::uploadToGPU(cube.vertices, cube.indices);

  m_shader.load("shaders/vertex.glsl", "shaders/fragment.glsl");
  m_indep_shader.load("shaders/indepVert.glsl", "shaders/indepFrag.glsl");
}

void Engine::run()
{
  while (!glfwWindowShouldClose(m_window))
  {
    processInput();
    update();
    render();
  }
}

void Engine::shutdown()
{
  m_group.destroy();
  glfwTerminate();
}

// Private Methods

void Engine::processInput(float deltaTime)
{
  m_camera.handleKeyboard(isPressed(GLFW_KEY_W), isPressed(GLFW_KEY_A),
                          isPressed(GLFW_KEY_S), isPressed(GLFW_KEY_D),
                          deltaTime);
  int key = glfw_getch_nonblocking(m_window);
  switch (key)
  {
  case -1:
    break;
  default:
    break;

  case GLFW_KEY_SPACE:
    m_camera.m_fly = 1 - m_camera.m_fly;
    break;
  case GLFW_KEY_R:
    m_group.rerun();
    break;
  case GLFW_KEY_RIGHT:
    m_world.newBounds *= 1.01f;
    m_world.m_worldDataUpdated = true;
    m_group.m_bounds = m_world.newBounds;
    break;
  case GLFW_KEY_LEFT:
    m_world.newBounds *= 0.99f;
    m_world.m_worldDataUpdated = true;
    m_group.m_bounds = m_world.newBounds;
    break;
  case GLFW_KEY_ENTER:
    m_world.newBounds *= 10.0f;
    m_world.m_worldDataUpdated = true;
    m_group.m_bounds = m_world.newBounds;
    break;
  case GLFW_KEY_ESCAPE:
    shutdown();
    exit(0);
  }
}

void Engine::update(float DeltaTime)
{
  m_world.update(m_group.getObjs());
  m_group.updateMatrices();
  m_group.updateGPU();
}

void Engine::render()
{
  glfwGetFramebufferSize(m_window, &m_width, &m_height);
  glViewport(0, 0, m_width, m_height);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_shader.use();

  float aspectRatio = (float)m_width / (float)m_height;
  glm::mat4 view = m_camera.genView();
  glm::mat4 projection = m_camera.genProj(aspectRatio);

  m_shader.setMat4("view", view);
  m_shader.setMat4("projection", projection);

  m_group.draw();

  // Now the bounds
  m_indep_shader.use();

  glm::mat4 model =
      glm::scale(glm::mat4(1.0f), glm::vec3(m_group.m_bounds, m_group.m_bounds,
                                            m_group.m_bounds));
  m_indep_shader.setMat4("model", model);
  m_indep_shader.setMat4("view", view);
  m_indep_shader.setMat4("projection", projection);
  m_indep_shader.setVec3("uColor", glm::vec3(1.0f));

  glBindVertexArray(m_cube.vao);

  glDrawElements(GL_LINE_LOOP, m_cube.indexCount, GL_UNSIGNED_INT, (void*)0);

  glBindVertexArray(0);

  glfwSwapBuffers(m_window);
  glfwPollEvents();
}

bool Engine::isPressed(int key)
{
  return (glfwGetKey(m_window, key) == GLFW_PRESS);
}