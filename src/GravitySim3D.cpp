#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


float SW = 1600.0f;
float SH = 900.0f;
double GravConst = 6.674e-5; // Raised to speed up sim

const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    uniform mat4 view;
    uniform mat4 projection; // Add projection matrix uniform

    void main() {
        gl_Position = projection * view * vec4(aPos, 1.0); // Apply projection and view transformations
    }
)glsl";

const char* fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;

    uniform vec3 color; // Add color uniform

    void main() {
        FragColor = vec4(color, 1.0); // Use the color uniform
    }
)glsl";


GLFWwindow* StartGLFW();

GLuint CompileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);


    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint CreateShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;

    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f), MouseSensitivity(0.1f) {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(int direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (direction == GLFW_KEY_W)
            Position += Front * velocity;
        if (direction == GLFW_KEY_S)
            Position -= Front * velocity;
        if (direction == GLFW_KEY_A)
            Position -= Right * velocity;
        if (direction == GLFW_KEY_D)
            Position += Right * velocity;
    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if (constrainPitch) {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
    }

private:
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};

Camera camera(glm::vec3(800.0f, 20.0f, 600.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_W, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_S, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_A, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_D, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_W, (deltaTime * 5));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_A, (deltaTime * 5));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_S, (deltaTime * 5));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_D, (deltaTime * 5));

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = 800.0f, lastY = 450.0f;
    static bool firstMouse = true;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

class Object{
    public:
        double radius;
        double mass;
        std::vector<double> position = {0.0f, 0.0f, 0.0f};
        std::vector<double> velocity = {0.0f, 0.0f, 0.0f};
};


void drawSphere(float centerX, float centerY, float centerZ, float radius, int stacks, int slices);
double GetDis(const std::vector<double>& pos1, const std::vector<double>& pos2);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void DrawGrid(int GridSize, int CellSize, GLuint colorLoc);
void PhysicsProcess(std::vector<double>& pla1POS, std::vector<double>& pla1VEL, std::vector<double>& pla2POS, std::vector<double>& pla2VEL, double pla1MASS, double pla2MASS, double DT);

int main() {
    GLFWwindow* window = StartGLFW();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);

    Object Planet1;
    Planet1.radius = 1.0;
    Planet1.mass = 1e6;
    Planet1.position[0] = 810.0;
    Planet1.position[1] = 0.0;
    Planet1.position[2] = 610.0;
    Planet1.velocity[0] = -5.6;
    Planet1.velocity[1] = 0.0;
    Planet1.velocity[2] = 0.0;

    Object Planet2;
    Planet2.radius = 2.0;
    Planet2.mass = 5e6;
    Planet2.position[0] = 800.0;
    Planet2.position[1] = 0.0;
    Planet2.position[2] = 600.0;
    Planet2.velocity[0] = 0.0;
    Planet2.velocity[1] = 0.0;
    Planet2.velocity[2] = 0.0;

    

    int stacks = 50;
    int slices = 50;

    double prevTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        double CT = glfwGetTime();
        double DT = (CT - prevTime); // *5 to speed up the sim
        prevTime = CT;
        

        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
        GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), SW / SH, 0.1f, 1000.0f); // Change last value for render distance if needed
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        GLuint colorLoc = glGetUniformLocation(shaderProgram, "color");

        glUniform3f(colorLoc, 0.3f, 0.3f, 0.3f);
        DrawGrid(1, 10.0f, colorLoc);
        glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);
        drawSphere(Planet1.position[0], Planet1.position[1], Planet1.position[2], Planet1.radius, stacks, slices);
        glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f);
        drawSphere(Planet2.position[0], Planet2.position[1], Planet2.position[2], Planet2.radius, stacks, slices);

        PhysicsProcess(Planet1.position, Planet1.velocity, Planet2.position, Planet2.velocity, Planet1.mass, Planet2.mass, DT);



        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
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
    gluPerspective(45.0, SW / SH, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW. Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }

    return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)width / (float)height, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void drawSphere(float centerX, float centerY, float centerZ, float radius, int stacks, int slices) {
    for (int i = 0; i <= stacks; ++i) {
        float stackAngle1 = M_PI / 2 - i * M_PI / stacks;
        float stackAngle2 = M_PI / 2 - (i + 1) * M_PI / stacks;

        float xy1 = radius * cos(stackAngle1);
        float z1 = radius * sin(stackAngle1);

        float xy2 = radius * cos(stackAngle2);
        float z2 = radius * sin(stackAngle2);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float sliceAngle = j * 2 * M_PI / slices;

            float x1 = xy1 * cos(sliceAngle);
            float y1 = xy1 * sin(sliceAngle);

            float x2 = xy2 * cos(sliceAngle);
            float y2 = xy2 * sin(sliceAngle);

            glVertex3f(centerX + x1, centerY + y1, centerZ + z1);
            glVertex3f(centerX + x2, centerY + y2, centerZ + z2);
        }
        glEnd();
    }
}

double GetDis(const std::vector<double>& pos1, const std::vector<double>& pos2) {
    double dx = pos2[0] - pos1[0];
    double dy = pos2[1] - pos1[1];
    double dz = pos2[2] - pos1[2];
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

void DrawGrid(int GridSize, int CellSize, GLuint colorLoc) {
    glBegin(GL_LINES);
    glUniform3f(colorLoc, 0.3f, 0.3f, 0.3f);

    for (int x = 0; x <= SW; x += CellSize) {
        glVertex3f(x, 0.0f, 0.0f);
        glVertex3f(x, 0.0f, SH);
    }

    for (int z = 0; z < SH; z += CellSize) {
        glVertex3f(0.0f, 0.0f, z);
        glVertex3f(SW, 0.0f, z);
    }

    glEnd();

}

void PhysicsProcess(std::vector<double>& pla1POS, std::vector<double>& pla1VEL, std::vector<double>& pla2POS, std::vector<double>& pla2VEL, double pla1MASS, double pla2MASS, double DT) {
    double Distance = GetDis(pla1POS, pla2POS);

    double force = (GravConst * (pla1MASS * pla2MASS)) / (Distance * Distance);
    
    std::vector<double> forceVec = {(pla2POS[0] - pla1POS[0]) / Distance, (pla2POS[1] - pla1POS[1]) / Distance, (pla2POS[2] - pla1POS[2]) / Distance};
    forceVec[0] *= force;
    forceVec[1] *= force;
    forceVec[2] *= force;

    pla1VEL[0] += (forceVec[0] / pla1MASS) * DT;
    pla1VEL[1] += (forceVec[1] / pla1MASS) * DT;
    pla1VEL[2] += (forceVec[2] / pla1MASS) * DT;

    pla2VEL[0] -= (forceVec[0] / pla2MASS) * DT;
    pla2VEL[1] -= (forceVec[1] / pla2MASS) * DT;
    pla2VEL[2] -= (forceVec[2] / pla2MASS) * DT;

    pla1POS[0] += pla1VEL[0] * DT;
    pla1POS[1] += pla1VEL[1] * DT;
    pla1POS[2] += pla1VEL[2] * DT;

    pla2POS[0] += pla2VEL[0] * DT;
    pla2POS[1] += pla2VEL[1] * DT;
    pla2POS[2] += pla2VEL[2] * DT;
 
    
}