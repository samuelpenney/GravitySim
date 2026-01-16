#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

float SW = 1600.0f;
float SH = 900.0f;
const double GravConst = 6.674e-5; // Raised to speed up sim at 5
const double PI = 3.14159265358979323846;
const double LIGHT_SPEED = 299792458.0;
int stacks = 50;
int slices = 50;
bool Collision = false;
bool ifcol = false;
std::vector<std::vector<float>> colorPalette = {
    {0.2f, 0.0f, 0.0f},
    {0.4f, 0.0f, 0.0f},
    {0.6f, 0.0f, 0.0f},
    {0.8f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 0.2f, 0.0f},
    {0.0f, 0.4f, 0.0f},
    {0.0f, 0.6f, 0.0f},
    {0.0f, 0.8f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.2f},
    {0.0f, 0.0f, 0.4f},
    {0.0f, 0.0f, 0.6f},
    {0.0f, 0.0f, 0.8f},
    {0.0f, 0.0f, 1.0f},
    {0.2f, 0.2f, 0.0f},
    {0.4f, 0.4f, 0.0f},
    {0.6f, 0.6f, 0.0f},
    {0.8f, 0.8f, 0.0f},
    {1.0f, 1.0f, 0.0f},
    {0.2f, 0.0f, 0.2f},
    {0.4f, 0.0f, 0.4f},
    {0.6f, 0.0f, 0.6f},
    {0.8f, 0.0f, 0.8f},
    {1.0f, 0.0f, 1.0f},
    {0.0f, 0.2f, 0.2f},
    {0.0f, 0.4f, 0.4f},
    {0.0f, 0.6f, 0.6f},
    {0.0f, 0.8f, 0.8f},
    {0.0f, 1.0f, 1.0f},
    {0.2f, 0.2f, 0.2f},
    {0.4f, 0.4f, 0.4f},
    {0.6f, 0.6f, 0.6f},
    {0.8f, 0.8f, 0.8f},
    {1.0f, 1.0f, 1.0f}
};
const char* vertexShaderSource = R"glsl(
    #version 120
    attribute vec3 aPos;

    uniform mat4 view;
    uniform mat4 projection; // Add projection matrix uniform

    void main() {
        gl_Position = projection * view * vec4(aPos, 1.0); // Apply projection and view transformations
    }
)glsl";

const char* fragmentShaderSource = R"glsl(
    #version 120

    uniform vec3 color; // Add color uniform

    void main() {
        gl_FragColor = vec4(color, 1.0); // Use the color uniform
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

Camera camera(glm::vec3(500.0f, 20.0f, 500.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool paused = false;
bool spacePressedLastFrame = false;
bool cursorInWindow = true;
bool zPressedLastFrame = false;

void processInput(GLFWwindow* window) {
    bool spacePressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    if (spacePressed && !spacePressedLastFrame) {
        paused = !paused;
        std::cout << (paused ? "Simulation paused\n" : "Simulation resumed\n");
    }
    spacePressedLastFrame = spacePressed;

    bool zPressed = (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS);
    if (zPressed && !zPressedLastFrame) {
        cursorInWindow = !cursorInWindow;
        if (cursorInWindow) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            std::cout << "Cursor in window" << std::endl;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            std::cout << "Cursor out of window" << std::endl;
        } 
    }
    zPressedLastFrame = zPressed;

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

    if (!cursorInWindow) {
        firstMouse = true;
        return;
    }

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
    std::vector<std::vector<double>> positionHistory;
    int maxHistorySize = 250;
    int colorIndex = -1;

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

    void drawPastPOS() {
        drawLINE();
    }

    void drawLabel(const glm::mat4& view, const glm::mat4& projection) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(projection));

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(view));

        glRasterPos3f(position[0], position[1] + radius + 2.0, position[2]);

        for (char c : name) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    void updatePositionHistory() {
        std::vector<double> currentPos = {position[0], position[1], position[2]};
        positionHistory.push_back(currentPos);

        if (positionHistory.size() > maxHistorySize) {
            positionHistory.erase(positionHistory.begin());
        }
    }

private:
    void drawLINE() {
        if (positionHistory.size() < 2) return;

        glBegin(GL_LINE_STRIP);
        for (const auto& pos : positionHistory) {
            glVertex3f(pos[0], pos[1], pos[2]);
        }
        glEnd();
    }
};

struct GridVertex {
    float x, y, z;
};

double GetDis(const std::vector<double>& pos1, const std::vector<double>& pos2);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void PhysicsProcess(Object& Object1, Object& Object2, double DT);
void DrawCurvedGrid(int GridSize, GLuint colorLoc, const std::vector<Object*>& objects);
double CalCurve(double potential);
double CalGravPot(double x, double z, const std::vector<Object*>& objects);
bool CollisionDetection(const std::vector<Object*>& objects);
void Process(std::vector<Object*>& objects, GLuint colorLoc, glm::mat4 projection, GLuint shaderProgram, double DT);

int main() {
    GLFWwindow* window = StartGLFW();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);
    /*
    int argc = 0;
    char** argv = nullptr;
    glutInit(&argc, argv);
    */
    Object Planet1;
    Planet1.name = "Planet1";
    Planet1.radius = 1.0;
    Planet1.mass = 1e6;
    Planet1.position = {550.0f, 0.0f, 530.0f};
    Planet1.velocity = {-4.7f, 0.0f, 0.0f};

    Object Planet2;
    Planet2.name = "Planet2";
    Planet2.radius = 2.0;
    Planet2.mass = 5e6;
    Planet2.position = {525.0f, 0.0f, 500.0f};
    Planet2.velocity = {-2.0f, 0.0f, 0.0f};

    Object Planet3;
    Planet3.name = "Planet3";
    Planet3.radius = 3.0;
    Planet3.mass = 1e7;
    Planet3.position = {450.0f, 0.0f, 450.0f};
    Planet3.velocity = {0.5f, 0.0f, 0.0f};

    Object Planet4;
    Planet4.name = "Planet4";
    Planet4.radius = 4.0;
    Planet4.mass = 2e7;
    Planet4.position = {370.0f, 0.0f, 370.0f};

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

        std::vector<Object*> objects = {&Planet1, &Planet2, &Planet3, &Planet4};
        Process(objects, colorLoc, projection, shaderProgram, DT);

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

    // Use OpenGL 2.1 compatibility profile for immediate mode rendering
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow* window = glfwCreateWindow(1600, 900, "Gravity Sim", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window. Exiting..." << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    
    // Get actual framebuffer size (may differ from window size on high DPI displays)
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    SW = framebufferWidth;
    SH = framebufferHeight;
    glViewport(0, 0, framebufferWidth, framebufferHeight);
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
    SW = width;
    SH = height;
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


bool CollisionDetection(std::vector<Object*>& objects) {
    int ObjectAmount = objects.size();
    bool Collision = false;

    for (int i = 0; i < ObjectAmount; i++) {
        for (int j = i + 1; j < ObjectAmount; j++) {
            double Distance = GetDis(objects[i]->position, objects[j]->position);
            if (Distance <= (objects[i]->radius + objects[j]->radius)) {
                if (!ifcol) {
                    std::cout << "Collision between " << objects[i]->name << " and " << objects[j]->name << std::endl;
                    ifcol = true;
                }
                Collision = true;
                break;
            }
        }
    }
    return Collision;
}

double CalGravPot(double x, double z, const std::vector<Object*>& objects){
    double potential = 0.0;

    for (const auto& obj : objects) {
        double dx = x - obj->position[0];
        double dz = z - obj->position[2];
        double distance = std::sqrt(dx * dx + dz * dz);

        double softening = obj->radius * 3.5; // Change last value to increase or decrease curve of grid(less or more pointy)

        double smoothedDist = std::sqrt(distance * distance + softening * softening);

        double phi = -(GravConst * obj->mass) / smoothedDist;
        potential += phi;
    }

    return potential;
}

double CalCurve(double potential) {
    return potential * 0.5;
}

void DrawCurvedGrid(int GridSize, GLuint colorLoc, const std::vector<Object*>& objects) {
    glUniform3f(colorLoc, 0.3f, 0.3f, 0.3f);

    for (int z = 0; z <= 1000; z += GridSize) {
        glBegin(GL_LINE_STRIP);
        for (int x = 0; x <= 1000; x += GridSize / 4) {
            double potential = CalGravPot(x, z, objects);
            double y = CalCurve(potential);
            glVertex3f(x, y, z);
        }
        glEnd();
    }

    for (int x = 0; x <= 1000; x+= GridSize) {
        glBegin(GL_LINE_STRIP);
        for (int z = 0; z <= 1000; z += GridSize / 4) {
            double potential = CalGravPot(x, z, objects);
            double y = CalCurve(potential);
            glVertex3f(x,y,z);
        }
        glEnd();
    }
}

void Process(std::vector<Object*>& objects, GLuint colorLoc, glm::mat4 projection, GLuint shaderProgram, double DT) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::vector<int> availableColors;

    static bool initialized = false;
    if (!initialized) {
        for (int i = 0; i < colorPalette.size(); i++) {
            availableColors.push_back(i);
        }
        initialized = true;
    }

    int ObjectAmount = objects.size();

    for (int i = 0; i < ObjectAmount; i++) {
        if (objects[i]->colorIndex == -1 && !availableColors.empty()) {
            std::uniform_int_distribution<int> dis(0, availableColors.size() - 1);
            int randomIdx = dis(gen);
            objects[i]->colorIndex = availableColors[randomIdx];
            availableColors.erase(availableColors.begin() + randomIdx);
        }
    }

    DrawCurvedGrid(4.0f, colorLoc, objects);

    for (int i = 0; i < ObjectAmount; i++) {
        if (objects[i]->colorIndex != -1) {
            glUniform3f(colorLoc, colorPalette[objects[i]->colorIndex][0], colorPalette[objects[i]->colorIndex][1], colorPalette[objects[i]->colorIndex][2]);

        }
        objects[i]->drawObject();
        objects[i]->drawPastPOS();
    }
    /*
    glUseProgram(0);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (const auto& obj : objects) {
        obj->drawLabel(camera.GetViewMatrix(), projection);
    }
    glUseProgram(shaderProgram);
    */
    Collision = CollisionDetection(objects);

    if (!paused && !Collision) {
        for (int i = 0; i < ObjectAmount; i++) {
            for (int j = i + 1; j < ObjectAmount; j++) {
                PhysicsProcess(*objects[i], *objects[j], DT);
            }
        }
        for (int i = 0; i < ObjectAmount; i++) {
            objects[i]->updatePositionHistory();
        }
    }
}