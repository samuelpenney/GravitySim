#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

float SW = 1600.0f; // Screen Width
float SH = 900.0f; // Screen Height
//double GravConst = 6.674e-11;
double GravConst = 6.674e-3; // Was to small to actual change the planets visually


class Object {
    public:
        double radius;
        double mass;
        std::vector<double> position = {0.0f, 0.0f};
        std::vector<double> velocity = {0.0f, 0.0f};

};

GLFWwindow* StartGLFW();
void DrawCircle(float centerX, float centerY, float radius, int points);
double GetDis(const std::vector<double>& pos1, const std::vector<double>& pos2);

int main() {

    GLFWwindow* window = StartGLFW();

    Object Planet1;
    Planet1.radius = 15.0;
    Planet1.mass = 3e6;
    Planet1.position[0] = 1000.0;
    Planet1.position[1] = 100.0;
    Planet1.velocity[0] = 0.0;
    Planet1.velocity[1] = 10.0;

    Object Planet2;
    Planet2.radius = 25.0;
    Planet2.mass = 5e6;
    Planet2.position[0] = 800.0;
    Planet2.position[1] = 50.0;
    Planet2.velocity[0] = 0.0;
    Planet2.velocity[1] = 0.0;

    float centerX = SW / 2.0f;
    float centerY = SH / 2.0f;
    float radius = 50.0f;
    int points = 50;


    std::vector<float> position = {400.0f, 600.0f};
    std::vector<float> velocity = {0.0f, 0.0f};
    
    double previousTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = (currentTime - previousTime) * 10.0; // *5 is there to speed up the simulation
        previousTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT);

        //DrawCircle(position[0], position[1], radius, points);
        DrawCircle(Planet1.position[0], Planet1.position[1], Planet1.radius, points);
        DrawCircle(Planet2.position[0], Planet2.position[1], Planet2.radius, points);

        double Distance = GetDis(Planet1.position, Planet2.position);
        
        double force = (GravConst * (Planet1.mass * Planet2.mass)) / (Distance * Distance);

        std::vector<double> forceVec = {(Planet2.position[0] - Planet1.position[0]) / Distance, (Planet2.position[1] - Planet1.position[1]) / Distance};
        forceVec[0] *= force;
        forceVec[1] *= force;

        Planet1.velocity[0] += ( forceVec[0] / Planet1.mass) * deltaTime;
        Planet1.velocity[1] += ( forceVec[1] / Planet1.mass) * deltaTime;

        Planet2.velocity[0] += (-forceVec[0] / Planet2.mass) * deltaTime;
        Planet2.velocity[1] += (-forceVec[1] / Planet2.mass) * deltaTime;

        Planet1.position[0] += Planet1.velocity[0] * deltaTime;
        Planet1.position[1] += Planet1.velocity[1] * deltaTime;

        Planet2.position[0] += Planet2.velocity[0] * deltaTime;
        Planet2.position[1] += Planet2.velocity[1] * deltaTime;

        
        if (Distance <= (Planet1.radius + Planet2.radius)) {
            std::cout << "Collision detected!\n";
            Planet1.velocity = {0.0, 0.0};
            Planet2.velocity = {0.0, 0.0};
        }

        std::cout << "Planet1 Position: (" << Planet1.position[0] << ", " << Planet1.position[1] << ")\n";
        std::cout << "Planet1 Velocity: (" << Planet1.velocity[0] << ", " << Planet1.velocity[1] << ")\n";
        std::cout << "Planet2 Position: (" << Planet2.position[0] << ", " << Planet2.position[1] << ")\n";
        std::cout << "Planet2 Velocity: (" << Planet2.velocity[0] << ", " << Planet2.velocity[1] << ")\n";
   
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

}

GLFWwindow* StartGLFW(){
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW. Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = glfwCreateWindow(1600, 900, "Gravity Sim", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window. Exiting..." << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glViewport(0, 0, 1600, 900);
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

        int i = 0;
        while (i <= points) {
            float angle = 2.0f * 3.14159265359 * (static_cast<float>(i) / points);
            float x = centerX + std::cos(angle) * radius;
            float y = centerY + std::sin(angle) * radius;
            glVertex2d(x, y);
            i++;
        }
        
        glEnd();
}

double GetDis(const std::vector<double>& pos1, const std::vector<double>& pos2) {
    double dx = pos2[0] - pos1[0];
    double dy = pos2[1] - pos1[1];
    return std::sqrt(dx * dx + dy * dy);
}

