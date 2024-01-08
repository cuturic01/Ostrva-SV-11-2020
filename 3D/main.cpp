//Opis: Primjer ucitavanja modela upotrebom ASSIMP biblioteke
//Preuzeto sa learnOpenGL

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "model.hpp"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void fixPitch();

void useShader(Shader shader, glm::mat4 projection, glm::mat4 view);


glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	
float pitch = 0.0f;

float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
{

    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    unsigned int wWidth = mode->width;
    unsigned int wHeight = mode->height;
    

    const char wTitle[] = "Ostrva SV11/2020";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, glfwGetPrimaryMonitor(), NULL);
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }
    glfwMakeContextCurrent(window);
    glViewport(0, 0, wWidth, wHeight);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    // Objects

    // Ocean
    Model ocean("res/Ocean/Ocean.obj");
    glm::mat4 oceanModel = glm::mat4(1.0f);
    oceanModel = glm::scale(oceanModel, glm::vec3(20.0f, 1.0f, 20.0f));

    // Island 1
    Model island1("res/sphere/sphere.obj");
    glm::mat4 island1Model = glm::mat4(1.0f);
    island1Model = glm::scale(island1Model, glm::vec3(0.5f, 0.3f, 0.5f));

    // Island 2
    Model island2("res/sphere/sphere.obj");
    glm::mat4 island2Model = glm::mat4(1.0f);
    island2Model = glm::scale(island2Model, glm::vec3(0.4f, 0.2f, 0.4f));
    island2Model = glm::translate(island2Model, glm::vec3(5.0f, 0.0f, 10.0f));

    // Island 3
    Model island3("res/sphere/sphere.obj");
    glm::mat4 island3Model = glm::mat4(1.0f);
    island3Model = glm::scale(island3Model, glm::vec3(0.3f, 0.2f, 0.3f));
    island3Model = glm::translate(island3Model, glm::vec3(-7.0f, 0.0f, 8.0f));

    // Shark 1
    Model shark1("res/shark/SHARK.obj");
    glm::mat4 shark1Model = glm::mat4(1.0f);
    shark1Model = glm::scale(shark1Model, glm::vec3(0.1f, 0.1f, 0.1f));
    shark1Model = glm::translate(shark1Model, glm::vec3(-7.0f, -0.3f, 8.0f));

    // Projection 
    glm::mat4 currentProjection;
    glm::mat4 projectionPerspective = glm::perspective(glm::radians(45.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);
    glm::mat4 projectionOrtho = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    currentProjection = projectionOrtho;

    // View
    glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);

    // shaders
    Shader currentShader("ocean.vert", "ocean.frag");
    Shader oceanShader("ocean.vert", "ocean.frag");
    Shader islandShader("island.vert", "island.frag");
    Shader sharkShader("shark.vert", "shark.frag");

    // Settings
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glClearColor(0.7, 0.7, 1.0, 1.0);

    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Projection controls
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
            cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
            yaw = -90.0f;
            pitch = 0.0f;
            currentProjection = projectionPerspective;
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
            cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
            yaw = -90.0f;
            pitch = 0.0f;
            currentProjection = projectionOrtho;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*model = glm::rotate(model, glm::radians(0.1f), glm::vec3(0.0f, 1.0f, 0.0f));*/

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // ocean
        useShader(oceanShader, currentProjection, view);
        currentShader = oceanShader;
        currentShader.setMat4("model", oceanModel);
        ocean.Draw(currentShader);

        // islands
        useShader(islandShader, currentProjection, view);
        currentShader = islandShader;
        currentShader.setMat4("model", island1Model);
        island1.Draw(currentShader);

        currentShader.setMat4("model", island2Model);
        island2.Draw(currentShader);

        currentShader.setMat4("model", island3Model);
        island3.Draw(currentShader);

        // sharks
        useShader(sharkShader, currentProjection, view);
        currentShader = sharkShader;
        sharkShader.setMat4("model", shark1Model);
        shark1.Draw(sharkShader);

        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glPointSize(4);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += glm::vec3(0.0f, 0.01f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= glm::vec3(0.0f, 0.01f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::vec3(0.01f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::vec3(0.01f, 0.0f, 0.0f);

    glm::vec3 front;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) 
        pitch += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pitch -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        yaw += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        yaw -= 1.0f;
    fixPitch();
    //fixYaw();
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = cameraFront.x;
        lastY = cameraFront.y;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    fixPitch();

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void fixPitch()
{
    if (pitch >= 90.0f)
        pitch = 90.0f;
    if (pitch <= -90.0f)
        pitch = -90.0f;
}

void useShader(Shader shader, glm::mat4 projection, glm::mat4 view)
{
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    shader.setVec3("uLightPos", 0, 1, 3);
    shader.setVec3("uViewPos", 0, 0, 5);
    shader.setVec3("uLightColor", 1, 1, 1);
}

