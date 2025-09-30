#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

float SW = 1600.0f; // Screen Width
float SH = 900.0f; // Screen Height
const double GravConst = 6.674e-3; // Was to small to actual change the planets visually


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
void DrawGrid(int GridSize, int CellSize);
void PhysicsProcess(std::vector<double>& P1_POS, std::vector<double>& P1_VEL, double P1_MASS, std::vector<double>& P2_POS, std::vector<double>& P2_VEL, double P2_MASS, double deltaTime);

int main() {

    GLFWwindow* window = StartGLFW();

    Object Planet1;
    Planet1.radius = 15.0;
    Planet1.mass = 1e6;
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
    int points = 50;


    std::vector<float> position = {400.0f, 600.0f};
    std::vector<float> velocity = {0.0f, 0.0f};
    
    double previousTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = (currentTime - previousTime) * 10.0; // *5 is there to speed up the simulation
        previousTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT);
        DrawGrid(1, 100.0f);

        //DrawCircle(position[0], position[1], radius, points);
        glColor3f(1.0f, 1.0f, 1.0f);
        DrawCircle(Planet1.position[0], Planet1.position[1], Planet1.radius, points);
        DrawCircle(Planet2.position[0], Planet2.position[1], Planet2.radius, points);

        PhysicsProcess(Planet1.position, Planet1.velocity, Planet1.mass, Planet2.position, Planet2.velocity, Planet2.mass, deltaTime);

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

void DrawGrid(int GridSize, int CellSize) {
    glBegin(GL_LINES);
    glColor3f(0.3f, 0.3f, 0.3f); // Set grid color

    // Draw vertical lines
    for (int x = 0; x <= SW; x += CellSize) {
        glVertex2f(x, 0.0f);
        glVertex2f(x, SH);
    }

    // Draw horizontal lines
    for (int y = 0; y <= SH; y += CellSize) {
        glVertex2f(0.0f, y);
        glVertex2f(SW, y);
    }

    glEnd();
}

void PhysicsProcess(std::vector<double>& P1_POS, std::vector<double>& P1_VEL, double P1_MASS, std::vector<double>& P2_POS, std::vector<double>& P2_VEL, double P2_MASS, double deltaTime) {
    double Distance = GetDis(P1_POS, P2_POS);

    double force = (GravConst * (P1_MASS * P2_MASS)) / (Distance * Distance);

    std::vector<double> forceVec{(P2_POS[0] - P1_POS[0]) / Distance, (P2_POS[1] - P1_POS[1]) / Distance};
    forceVec[0] *= force;
    forceVec[1] *= force;

    P1_VEL[0] += (forceVec[0] / P1_MASS) * deltaTime;
    P1_VEL[1] += (forceVec[1] / P1_MASS) * deltaTime;

    P2_VEL[0] += (forceVec[0] / P2_MASS) * deltaTime;
    P2_VEL[1] += (forceVec[1] / P2_MASS) * deltaTime;

    P1_POS[0] += P1_VEL[0] * deltaTime;
    P1_POS[1] += P1_VEL[1] * deltaTime;

    P2_POS[0] += P2_VEL[0] * deltaTime;
    P2_POS[1] += P2_VEL[1] * deltaTime;

}