#include "engine.hpp"

Engine::Engine() : m_isRunning(false) {};
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

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    GLFWwindow *window = glfwCreateWindow(
        mode->width,
        mode->height,
        "CLngine",
        (wantsFullscreen) ? monitor : nullptr,
        nullptr);

    if (window == nullptr)
    {
        std::cout << "GLFW Window initialization failed\n";
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);
    // V-Sync on.

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD initialization error." << std::endl;
        return false;
    }
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Background color.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    globalCamPointer = &m_camera;

    m_window = window;
    glfwSetWindowUserPointer(m_window, this);
    return true;
}

void Engine::createAndLoad()
{
    Mesh ballMesh = MeshGen::CreateCube(1.0f);
    MeshUtils::GLMesh GPUBallMesh = MeshUtils::uploadToGPU(ballMesh.vertices, ballMesh.indices);

    m_meshCount = 10000;
    m_bounds = 50.0f;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> bounds(-m_bounds, m_bounds);
    std::uniform_real_distribution<float> zero_to_one(0.0f, 1.0f);

    // 1. Generate initial random positions / matrices on CPU
    std::vector<glm::mat4> sphereMatrices;
    sphereMatrices.reserve(m_meshCount);
    for (int i = 0; i < m_meshCount; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(bounds(gen), bounds(gen), bounds(gen)));
        sphereMatrices.push_back(model);
    }

    std::vector<glm::vec3> sphereColors;
    sphereColors.reserve(m_meshCount);
    for (int i = 0; i < m_meshCount; ++i)
    {
        glm::vec3 color = glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen));
        sphereColors.push_back(color);
    }

    m_mesh = GPUBallMesh;
    m_colors = std::move(sphereColors);
    m_models = std::move(sphereMatrices);

    // 2. Create the VBO to hold instance data on GPU

    m_colorbuffer.create(BufferUsage::Dynamic);
    m_modelbuffer.create();

    m_colorbuffer.upload(m_colors);
    m_modelbuffer.upload(m_models);

    glBindVertexArray(m_mesh.VAO);

    m_colorbuffer.link_attribute(7, 3, GL_FLOAT);
    m_colorbuffer.set_instanced(7);

    m_modelbuffer.link_attribute(3);
    m_modelbuffer.set_instanced(3);

    glBindVertexArray(0);

    m_camera.fov = 103.0f;
    m_camera.farPlane = 1500.0f;
    m_camera.nearPlane = 0.05f;

    m_shader.load("shader/vertex.glsl", "shader/fragment.glsl");
    // Shaders and camera ready.
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
    MeshUtils::deleteFromGPU(m_mesh);
    glfwTerminate();
}

// Private Methods

void Engine::processInput()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution zero_to_one(0.0f, 1.0f);

    if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
    {

        for (int i = 0; i < m_meshCount; ++i)
        {
            m_colors[i] = glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen));
        }
        m_needsGPUUpdate = true;
    }

    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_window, 1);
    }

    handle_input(m_window);
}

void Engine::update(float DeltaTime)
{
    // TODO: Add physics CPU part here.
    // Ideally, m_world.update(DeltaTime);

    if (m_needsGPUUpdate)
    {
        m_colorbuffer.update_sub_data(m_colors, 0);
        m_modelbuffer.update_sub_data(m_models, 0);

        m_needsGPUUpdate = false;
    }
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

    glm::vec3 lightPos = glm::vec3(10.0f);
    glm::vec3 lightColor = glm::vec3(1.0f); // White
    glm::vec3 viewPos = m_camera.cameraPos;
    m_shader.setVec3("lightPos", lightPos);
    m_shader.setVec3("lightColor", lightColor);
    m_shader.setVec3("viewPos", viewPos);

    glBindVertexArray(m_mesh.VAO);
    glDrawElementsInstanced(
        GL_TRIANGLES,
        m_mesh.indexCount, // Assuming GLMesh stores your index count
        GL_UNSIGNED_INT,
        0,
        m_meshCount);
    glBindVertexArray(0);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
