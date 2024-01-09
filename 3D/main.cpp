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
glm::mat4 rotate(
    glm::mat4 model, 
    float angle, 
    glm::vec3 axis, 
    glm::vec3 currentPosition,
    glm::vec3 rotationPoint,
    float R);

void useShader(Shader shader, glm::mat4 projection, glm::mat4 view);


glm::vec3 cameraPos = glm::vec3(5.0f, 5.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(-5.0f, -5.0f, -5.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = 195.0f;
float pitch = -57.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

int main()
{
#pragma region Setup
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
#pragma endregion

    float aspectRatio = static_cast<float>(wHeight) / wWidth;

    #pragma region Objects
    // Objects

    // Ocean
    Model ocean("res/Ocean/Ocean.obj");
    glm::mat4 oceanModel = glm::mat4(1.0f);
    oceanModel = glm::scale(oceanModel, glm::vec3(20.0f, 1.0f, 20.0f));

    #pragma region Clouds
    Model cloud1("res/cloud/CloudCollection.obj");
    glm::mat4 cloud1Model = glm::mat4(1.0f);
    cloud1Model = glm::scale(cloud1Model, glm::vec3(0.2f, 0.2f, 0.2f));
    cloud1Model = glm::translate(cloud1Model, glm::vec3(0.0f, 15.0f, 0.0f));
    cloud1Model = glm::rotate(cloud1Model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
#pragma endregion

    #pragma region Islands
    // Island 1
    Model island1("res/sphere/sphere.obj");
    glm::mat4 island1Model = glm::mat4(1.0f);
    island1Model = glm::scale(island1Model, glm::vec3(0.5f, 0.3f, 0.5f));
    glm::vec3 island1Centre = glm::vec3(island1Model[3]);

    // Island 2
    Model island2("res/sphere/sphere.obj");
    glm::mat4 island2Model = glm::mat4(1.0f);
    island2Model = glm::scale(island2Model, glm::vec3(0.4f, 0.2f, 0.4f));
    island2Model = glm::translate(island2Model, glm::vec3(5.0f, 0.0f, 10.0f));
    glm::vec3 island2Centre = glm::vec3(0.0f, 0.0f, 3.0f); //glm::vec3(island2Model[3]) / island2Model[0][0];

    // Island 3
    Model island3("res/sphere/sphere.obj");
    glm::mat4 island3Model = glm::mat4(1.0f);
    island3Model = glm::scale(island3Model, glm::vec3(0.3f, 0.2f, 0.3f));
    island3Model = glm::translate(island3Model, glm::vec3(-7.0f, 0.0f, 7.0f));
    glm::vec3 island3Centre = glm::vec3(-2.0f, 0.0f, 2.0f);
#pragma endregion

    #pragma region Palm tree
    Model palmTree("res/palm/OBJ/CoconutPalm.obj");
    glm::mat4 palmTreeModel = glm::mat4(1.0f);
    palmTreeModel = glm::scale(palmTreeModel, glm::vec3(0.01f, 0.01f, 0.01f));
    palmTreeModel = glm::translate(palmTreeModel, glm::vec3(-5.0f, 0.0f, 7.0f));
    palmTreeModel = glm::rotate(palmTreeModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));;
#pragma endregion

    #pragma region Fire
    Model fire("res/fire/uploads_files_2336673_Fire.obj");
    glm::mat4 fireModel = glm::mat4(1.0f);
    fireModel = glm::scale(fireModel, glm::vec3(0.1f, 0.1f, 0.1f));
    fireModel = glm::translate(fireModel, glm::vec3(1.5f, 0.0f, 0.5f));
#pragma endregion

    #pragma region Sharks
    // Shark 1
    Model shark1("res/shark/SHARK.obj");
    glm::mat4 shark1Model = glm::mat4(1.0f);
    shark1Model = glm::scale(shark1Model, glm::vec3(0.3f, 0.3f, 0.3f));
    shark1Model = glm::translate(shark1Model, glm::vec3(0.0f, -0.4f, 3.0f));
    shark1Model = glm::rotate(shark1Model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 shark1CurrentPosition = glm::vec3(shark1Model[3]);

    // Shark 2
    Model shark2("res/shark/SHARK.obj");
    glm::mat4 shark2Model = glm::mat4(1.0f);
    shark2Model = glm::scale(shark2Model, glm::vec3(0.3f, 0.3f, 0.3f));
    shark2Model = glm::translate(shark2Model, glm::vec3(5.0f, -0.5f, 15.0f));
    shark2Model = glm::rotate(shark2Model, glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 shark2CurrentPosition = glm::vec3(shark2Model[3]);

    // Shark 3
    Model shark3("res/shark/SHARK.obj");
    glm::mat4 shark3Model = glm::mat4(1.0f);
    shark3Model = glm::scale(shark3Model, glm::vec3(0.3f, 0.3f, 0.3f));
    shark3Model = glm::translate(shark3Model, glm::vec3(-9.0f, -0.5f, 10.0f));
    shark3Model = glm::rotate(shark3Model, glm::radians(140.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 shark3CurrentPosition = glm::vec3(shark3Model[3]);

#pragma endregion


    #pragma endregion


    #pragma region Projection
    // Projection 
    glm::mat4 currentProjection;
    glm::mat4 projectionPerspective = glm::perspective(glm::radians(45.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);
    glm::mat4 projectionOrtho = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    currentProjection = projectionOrtho;
    #pragma endregion

    // View
    glm::mat4 view;

    // shaders
    Shader shader("shader.vert", "shader.frag");

    /*Shader oceanShader("ocean.vert", "ocean.frag");
    Shader islandShader("island.vert", "island.frag");
    Shader sharkShader("shark.vert", "shark.frag");*/

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

        #pragma region Projection controls
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            cameraPos = glm::vec3(5.0f, 5.0f, 5.0f);
            yaw = 210.0f;
            pitch = -45.0f;

            currentProjection = projectionPerspective;     
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            cameraPos = glm::vec3(5.0f, 5.0f, 5.0f);
            yaw = 195.0f;
            pitch = -57.0f;

            currentProjection = projectionOrtho;
        }
#pragma endregion

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        useShader(shader, currentProjection, view);

        #pragma region Ocean render
        shader.setMat4("model", oceanModel);
        shader.setInt("fragType", 0);
        ocean.Draw(shader);
        #pragma endregion

        #pragma region Clouds render
        shader.setInt("fragType", 1);
        shader.setMat4("model", cloud1Model);
        cloud1.Draw(shader);
        #pragma endregion

        #pragma region Island render
        shader.setInt("fragType", 2);

        shader.setMat4("model", island1Model);
        island1.Draw(shader);

        shader.setMat4("model", island2Model);
        island2.Draw(shader);

        shader.setMat4("model", island3Model);
        island3.Draw(shader);
        #pragma endregion

        #pragma region Fire render
        shader.setInt("fragType", 3);
        shader.setMat4("model", fireModel);
        fire.Draw(shader);
        #pragma endregion

        #pragma region Palm tree render
        shader.setInt("fragType", 4);
        shader.setMat4("model", palmTreeModel);
        palmTree.Draw(shader);
        #pragma endregion

        #pragma region Sharks render
        shader.setInt("fragType", 5);

        shark1Model = rotate(
            shark1Model, 
            glm::radians(0.1f), 
            glm::vec3(0.0f, 1.0f, 0.0f), 
            glm::vec3(shark1Model[3]),
            island1Centre,
            0.01f);
        shader.setMat4("model", shark1Model);
        shark1.Draw(shader);

        shark2Model = rotate(
            shark2Model,
            glm::radians(0.1f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            shark2CurrentPosition,
            island2Centre,
            0.01f);
        shader.setMat4("model", shark2Model);
        shark2.Draw(shader);

        shark3Model = rotate(
            shark3Model,
            glm::radians(0.1f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            shark3CurrentPosition,
            island3Centre,
            0.01f);
        shader.setMat4("model", shark3Model);
        shark3.Draw(shader);
#pragma endregion

        
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

    float offset = 0.05f; //0.005f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += glm::vec3(0.0f, offset, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= glm::vec3(0.0f, offset, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::vec3(offset, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::vec3(offset, 0.0f, 0.0f);

    glm::vec3 front;
    float angle = 1.0f; //0.1f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) 
        pitch += angle;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pitch -= angle;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        yaw += angle;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        yaw -= angle;
    fixPitch();
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

glm::mat4 rotate(
    glm::mat4 model, 
    float angle, 
    glm::vec3 axis, 
    glm::vec3 currentPosition,
    glm::vec3 rotationPoint,
    float R)
{
    model = glm::translate(model, -rotationPoint);
    model = glm::rotate(model, angle, axis);

    glm::vec3 newPosition;
    if (axis.y != 0) 
    {
        newPosition.x = rotationPoint.x - R * cos(angle * deltaTime);
        newPosition.y = rotationPoint.y;
        newPosition.z = rotationPoint.z - R * sin(angle * deltaTime);
    }
    else {
        newPosition.x = rotationPoint.x;
        newPosition.y = rotationPoint.y + R * sin(angle * deltaTime);
        newPosition.z = rotationPoint.z + R * cos(angle * deltaTime);
    }

    return glm::translate(model, newPosition);
}

