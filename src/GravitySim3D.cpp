#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float SW = 1600.0f;
float SH = 900.0f;
const double GravConst = 6.674e-5; // Raised to speed up sim
const double PI = 3.14159265358979323846;
int stacks = 50;
int slices = 50;
bool Collision = false;


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

Camera camera(glm::vec3(1500.0f, 20.0f, 1500.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
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
        camera.ProcessKeyboard(GLFW_KEY_W, (deltaTime * 15));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_A, (deltaTime * 15));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_S, (deltaTime * 15));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_D, (deltaTime * 15));

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
    std::string name;
    double radius;
    double mass;
    std::vector<double> position = {0.0f, 0.0f, 0.0f};
    std::vector<double> velocity = {0.0f, 0.0f, 0.0f};

    void drawObject() {
        for (int i = 0; i <= stacks; ++i) {
            float stackAngle1 = PI / 2 - i * PI / stacks;
            float stackAngle2 = PI / 2 - (i + 1) * PI / stacks;

            float xy1 = radius * cos(stackAngle1);
            float z1 = radius * sin(stackAngle1);

            float xy2 = radius * cos(stackAngle2);
            float z2 = radius * sin(stackAngle2);

            glBegin(GL_TRIANGLE_STRIP);
            for (int j = 0; j <= slices; ++j) {
                float sliceAngle = j * 2 * PI / slices;

                float x1 = xy1 * cos(sliceAngle);
                float y1 = xy1 * sin(sliceAngle);
                    
                float x2 = xy2 * cos(sliceAngle);
                float y2 = xy2 * sin(sliceAngle);

                glVertex3f(position[0] + x1, position[1] + y1, position[2] + z1);
                glVertex3f(position[0] + x2, position[1] + y2, position[2] + z2);
            }
            glEnd();
        }
    }

};

double GetDis(const std::vector<double>& pos1, const std::vector<double>& pos2);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void DrawGrid(int GridSize, int CellSize, GLuint colorLoc);
void PhysicsProcess(Object& Object1, Object& Object2, double DT);
bool CollisionDet(Object& Object1, Object& Object2);

int main() {
    GLFWwindow* window = StartGLFW();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);

    Object Planet1;
    Planet1.name = "Planet1";
    Planet1.radius = 1.0;
    Planet1.mass = 1e6;
    Planet1.position[0] = 1520.0;
    Planet1.position[1] = 0.0;
    Planet1.position[2] = 1520.0;
    Planet1.velocity[0] = 0.0;
    Planet1.velocity[1] = 0.0;
    Planet1.velocity[2] = -4.0;

    Object Planet2;
    Planet2.name = "Planet2";
    Planet2.radius = 2.0;
    Planet2.mass = 5e6;
    Planet2.position[0] = 1500.0;
    Planet2.position[1] = 0.0;
    Planet2.position[2] = 1500.0;
    Planet2.velocity[0] = 0.0;
    Planet2.velocity[1] = 0.0;
    Planet2.velocity[2] = -1.0;

    Object Planet3;
    Planet3.name = "Planet3";
    Planet3.radius = 3.0;
    Planet3.mass = 2e7;
    Planet3.position[0] = 1400.0;
    Planet3.position[1] = 0.0;
    Planet3.position[2] = 1400.0;
    Planet3.velocity[0] = 0.0;
    Planet3.velocity[1] = 0.0;
    Planet3.velocity[2] = 0.0;

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
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), SW / SH, 0.1f, 3000.0f); // Change last value for render distance if needed
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        GLuint colorLoc = glGetUniformLocation(shaderProgram, "color");
        glUniform3f(colorLoc, 0.3f, 0.3f, 0.3f);
        DrawGrid(100, 100.0f, colorLoc);

        glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);
        Planet1.drawObject();
        glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f);
        Planet2.drawObject();
        glUniform3f(colorLoc, 1.0f, 1.0f, 0.0f);
        Planet3.drawObject();

        if (CollisionDet(Planet1, Planet2) || CollisionDet(Planet1, Planet3) || CollisionDet(Planet2, Planet3)) {
            Collision = true;
        }

        if (!Collision) {
            PhysicsProcess(Planet1, Planet2, DT);
            PhysicsProcess(Planet1, Planet3, DT);
            PhysicsProcess(Planet2, Planet3, DT);
        } 

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

double GetDis(const std::vector<double>& pos1, const std::vector<double>& pos2) {
    double dx = pos2[0] - pos1[0];
    double dy = pos2[1] - pos1[1];
    double dz = pos2[2] - pos1[2];
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

void DrawGrid(int GridSize, int CellSize, GLuint colorLoc) {
    glBegin(GL_LINES);
    glUniform3f(colorLoc, 0.3f, 0.3f, 0.3f);

    for (int x = 0; x <= 3000; x += CellSize) {
        glVertex3f(x, 0.0f, 0.0f);
        glVertex3f(x, 0.0f, 3000);
    }

    for (int z = 0; z < 3000; z += CellSize) {
        glVertex3f(0.0f, 0.0f, z);
        glVertex3f(3000, 0.0f, z);
    }

    glEnd();

}

void PhysicsProcess(Object& Object1, Object& Object2, double DT) {
    double Distance = GetDis(Object1.position, Object2.position);

    double force = (GravConst * (Object1.mass * Object2.mass)) / (Distance * Distance);
    
    std::vector<double> forceVec = {(Object2.position[0] - Object1.position[0]) / Distance, (Object2.position[1] - Object1.position[1]) / Distance, (Object2.position[2] - Object1.position[2]) / Distance};
    forceVec[0] *= force;
    forceVec[1] *= force;
    forceVec[2] *= force;

    Object1.velocity[0] += (forceVec[0] / Object1.mass) * DT;
    Object1.velocity[1] += (forceVec[1] / Object1.mass) * DT;
    Object1.velocity[2] += (forceVec[2] / Object1.mass) * DT;

    Object2.velocity[0] -= (forceVec[0] / Object2.mass) * DT;
    Object2.velocity[1] -= (forceVec[1] / Object2.mass) * DT;
    Object2.velocity[2] -= (forceVec[2] / Object2.mass) * DT;

    Object1.position[0] += Object1.velocity[0] * DT;
    Object1.position[1] += Object1.velocity[1] * DT;
    Object1.position[2] += Object1.velocity[2] * DT;

    Object2.position[0] += Object2.velocity[0] * DT;
    Object2.position[1] += Object2.velocity[1] * DT;
    Object2.position[2] += Object2.velocity[2] * DT;
}

bool CollisionDet(Object& Object1, Object& Object2) {
    double Distance = GetDis(Object1.position, Object2.position);
    bool Collision = false;

    if (Distance <= (Object1.radius + Object2.radius)) {
        std::cout << "Collision between " << Object1.name << " and " << Object2.name << std::endl;
        Object1.velocity = {0.0f, 0.0f, 0.0f};
        Object2.velocity = {0.0f, 0.0f, 0.0f};
        Collision = true;
    }
    return Collision;
}