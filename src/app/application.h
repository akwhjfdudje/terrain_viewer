#pragma once

#include <string>

struct GLFWwindow;

class Application {
public:
    Application(int width = 1280, int height = 800, const std::string& title = "Terrain Viewer");
    ~Application();

    void run();

private:
    void initWindow();
    void initOpenGL();
    void setupCallbacks();

    void processInput();
    void update(float dt);
    void render();

private:
    int m_width;
    int m_height;
    std::string m_title;

    GLFWwindow* m_window = nullptr;
    bool m_running = true;
};

