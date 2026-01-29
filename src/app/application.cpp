#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "application.h"

#include <iostream>
#include <chrono>
#include <cassert>

static void glfwErrorCallback(int code, const char* desc) {
    std::cerr << "[GLFW] (" << code << ") " << desc << std::endl;
}

static void framebufferSizeCallback(GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
}

Application::Application(int width, int height, const std::string& title)
    : m_width(width), m_height(height), m_title(title) {
    initWindow();
    initOpenGL();
    setupCallbacks();
}

Application::~Application() {
    glfwTerminate();
}

void Application::initWindow() {
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // OpenGL 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // vsync
}

void Application::initOpenGL() {
    if (!gladLoadGL(glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    std::cout << "OpenGL Vendor   : " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL Renderer : " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version  : " << glGetString(GL_VERSION) << std::endl;

    // Global GL state
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(
        [](GLenum source, GLenum type, GLuint id, GLenum severity,
           GLsizei, const GLchar* message, const void*) {
            if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
            std::cerr << "[GL DEBUG] " << message << std::endl;
        },
        nullptr
    );
#endif
}

void Application::setupCallbacks() {
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
}

void Application::run() {
    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();

    while (m_running && !glfwWindowShouldClose(m_window)) {
        auto now = clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        glfwPollEvents();
        processInput();
        update(dt);
        render();

        glfwSwapBuffers(m_window);
    }
}

void Application::processInput() {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, true);
    }
}

void Application::update(float /*dt*/) {
    // Terrain updates later (regen, erosion, etc.)
}

void Application::render() {
    glClearColor(0.55f, 0.75f, 0.95f, 1.0f); // sky-ish
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

