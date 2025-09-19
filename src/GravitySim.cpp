#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

float SW = 800.0f;
float SH = 600.0f;

GLFWwindow* StartGLFW();
void DrawCircle(float centerX, float centerY, float radius, int points);

int main() {

    GLFWwindow* window = StartGLFW();


    float centerX = SW / 2.0f;
    float centerY = SH / 2.0f;
    float radius = 50.0f;
    int points = 50;

    std::vector<float> pos = {400.0f, 600.0f};
    std::vector<float> velocity = {0.0f, 0.0f};
    
    double previousTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = (currentTime - previousTime) * 5.0; // *5 is there to speed up the simulation
        previousTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT);

        DrawCircle(pos[0], pos[1], radius, points);

   
        pos[0] += velocity[0] * deltaTime;
        pos[1] += velocity[1] * deltaTime;
        velocity[1] += -9.81 * deltaTime;

  
        if (pos[1] - radius <= 0.0f || pos[1] + radius >= SH) {
            velocity[1] = -velocity[1] * 0.8f;
        }
        if (pos[0] - radius <= 0.0f || pos[0] + radius >= SW) {
            velocity[0] = -velocity[0] * 0.8f;
        }
        if (pos[1] - radius < 0.0f) pos[1] = radius;
        if (pos[1] + radius > SH) pos[1] = SH - radius;
        if (pos[0] - radius < 0.0f) pos[0] = radius;
        if (pos[0] + radius > SW) pos[0] = SW - radius;


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

}

GLFWwindow* StartGLFW(){
    if(!glfwInit()){
        std::cerr<<"Failed to initialize glfw"<<std::endl;
    }
    GLFWwindow* window = glfwCreateWindow(800, 600, "Gravity Sim", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glViewport(0, 0, 800, 600);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, SW, 0.0, SH, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    return window;
}

void DrawCircle(float centerX, float centerY, float radius, int points){
    
        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(centerX, centerY);

        for (int i = 0; i <= points; i++) {
            float angle = 2.0f * 3.14159265359 * (static_cast<float>(i) / points);
            float x = centerX + std::cos(angle) * radius;
            float y = centerY + std::sin(angle) * radius;
            glVertex2d(x, y);
        }
        
        glEnd();
}