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

void processInput(GLFWwindow* window);
void useShader(Shader shader, glm::mat4 projection, glm::mat4 view);

float currentTime = 0.0f;

#pragma region Camera properties
glm::vec3 cameraPos = glm::vec3(5.0f, 5.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(-5.0f, -5.0f, -5.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = 195.0f;
float pitch = -57.0f;
#pragma endregion

#pragma region Directional light parametars
glm::vec3 lerpedDirLightColor;
glm::vec3 softYellow = glm::vec3(1.0, 1.0, 0.6);
glm::vec3 softGrey = glm::vec3(0.7529, 0.7529, 0.7529);

glm::vec3 lerpedBackroundColor;
glm::vec3 lightBlue = glm::vec3(0.7, 0.7, 1.0);
glm::vec3 darkBlue = glm::vec3(0.1176, 0.1569, 0.2745);

glm::vec3 dirLightIntensityStart = glm::vec3(1.0);
glm::vec3 dirLightIntensityEnd = glm::vec3(0.5);
glm::vec3 lerpedDirLightIntensity;
#pragma endregion

#pragma region Point light parametars
glm::vec3 firePos = glm::vec3();

glm::vec3 softRed = glm::vec3(1.0, 0.4, 0.4);
glm::vec3 softOrange = glm::vec3(1.0, 0.6, 0.4);
glm::vec3 fireColor = softRed;

glm::vec3 fireIntensityStart = glm::vec3(0.7, 0.7, 0.7);
glm::vec3 fireIntensityEnd = glm::vec3(0.4, 0.4, 0.4);
glm::vec3 fireIntensity = fireIntensityStart;

glm::vec3 fireScaleStart = glm::vec3(1.0, 1.0, 1.0);
glm::vec3 fireScaleEnd = glm::vec3(0.8, 0.8, 0.8);
glm::vec3 fireScale = fireScaleStart;
glm::mat4 oreginalFireModel;
#pragma endregion

#pragma region Spotlight parametars
glm::vec3 spotlightPos = glm::vec3(0.0, 1.0, -0.2);
glm::vec3 spotlightDir = glm::vec3(-0.0, 0.0, -0.01);
glm::vec3 purple = glm::vec3(0.7, 0.0, 1.0);
#pragma endregion

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }
    #pragma endregion

    float aspectRatio = static_cast<float>(wHeight) / wWidth;

    #pragma region Objects

    #pragma region Ocean
    Model ocean("res/Ocean/Ocean.obj");
    glm::mat4 oceanModel = glm::mat4(1.0f);
    oceanModel = glm::scale(oceanModel, glm::vec3(15.0f, 3.0f, 15.0f));
    glm::mat4 oreginalOceanModel = oceanModel;
    #pragma endregion

    #pragma region Clouds
    Model clouds("res/cloud/CloudCollection.obj");
    glm::mat4 cloudsModel = glm::mat4(1.0f);
    cloudsModel = glm::scale(cloudsModel, glm::vec3(0.2f, 0.2f, 0.2f));
    cloudsModel = glm::translate(cloudsModel, glm::vec3(50.0f, 15.0f, 0.0f));
    cloudsModel = glm::rotate(cloudsModel, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    float currentCloudPos = 0.0f;
    #pragma endregion

    #pragma region Islands
    // Island 1
    Model island1("res/Island/island.obj");
    glm::mat4 island1Model = glm::mat4(1.0f);
    island1Model = glm::scale(island1Model, glm::vec3(1.5f, 0.8f, 1.5f));
    island1Model = glm::translate(island1Model, glm::vec3(0.0f, -0.7f, 0.0f));
    glm::vec3 island1Centre = glm::vec3(0.0, 0.0, 0.0);

    // Island 2
    Model island2("res/Island/island.obj");
    glm::mat4 island2Model = glm::mat4(1.0f);
    island2Model = glm::scale(island2Model, glm::vec3(1.0f, 0.5f, 1.0f));
    island2Model = glm::translate(island2Model, glm::vec3(2.0f, -0.7f, 4.0f));
    glm::vec3 island2Centre = glm::vec3(-2.0f, 0.0f, 2.0f);

    // Island 3
    Model island3("res/Island/island.obj");
    glm::mat4 island3Model = glm::mat4(1.0f);
    island3Model = glm::scale(island3Model, glm::vec3(0.8f, 0.4f, 0.8f));
    island3Model = glm::translate(island3Model, glm::vec3(-2.5f, 0.0f, 2.5f));
    glm::vec3 island3Centre = glm::vec3(2.0f, 0.0f, 4.0f);
    #pragma endregion

    #pragma region Palm tree
    Model palmTree("res/palm/OBJ/CoconutPalm.obj");
    glm::mat4 palmTreeModel = glm::mat4(1.0f);
    palmTreeModel = glm::scale(palmTreeModel, glm::vec3(0.01f, 0.01f, 0.01f));
    palmTreeModel = glm::translate(palmTreeModel, glm::vec3(-5.0f, 0.0f, 7.0f));
    palmTreeModel = glm::rotate(palmTreeModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    #pragma endregion

    #pragma region Fire
    Model fire("res/fire/uploads_files_2336673_Fire.obj");
    glm::mat4 fireModel = glm::mat4(1.0f);
    fireModel = glm::scale(fireModel, glm::vec3(0.1f, 0.1f, 0.1f));
    fireModel = glm::translate(fireModel, glm::vec3(1.5f, 0.0f, 0.5f));
    firePos = glm::vec3(0.3f, 0.7f, 0.0f);
    oreginalFireModel = fireModel;
    #pragma endregion

    #pragma region Sharks
    // Shark 1
    Model shark1("res/shark/SHARK.obj");
    glm::mat4 shark1Model = glm::mat4(1.0f);
    shark1Model = glm::translate(shark1Model, glm::vec3(0.0f, -0.1f, 0.3f));
    glm::vec3 shark1CurrentPosition = glm::vec3(shark1Model[3]); //glm::vec3(island2Model[3]) / island2Model[0][0];

    // Shark 2
    Model shark2("res/shark/SHARK.obj");
    glm::mat4 shark2Model = glm::mat4(1.0f);
    shark2Model = glm::translate(shark2Model, glm::vec3(-2.0f, -0.1f, 2.0f));

    // Shark 3
    Model shark3("res/shark/SHARK.obj");
    glm::mat4 shark3Model = glm::mat4(1.0f);
    shark3Model = glm::translate(shark3Model, glm::vec3(2.0f, -0.2f, 4.0f));
    #pragma endregion

    #pragma endregion

    #pragma region Projection
    glm::mat4 currentProjection;
    glm::mat4 projectionPerspective = glm::perspective(glm::radians(45.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);
    glm::mat4 projectionOrtho = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    currentProjection = projectionOrtho;
    #pragma endregion

    // View
    glm::mat4 view;

    // shaders
    Shader shader("shader.vert", "shader.frag");
    Shader sharkShader("shark.vert", "shader.frag");

    #pragma region Settings
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.7, 0.7, 1.0, 1.0);
    #pragma endregion

    while (!glfwWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
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
        
        #pragma region Directional light render

        float t = (sin(currentTime / 2) + 1.0f) / 2.0f;
        
        float dirLightPosX = sin(currentTime) * 5.0f;
        float dirLightPosY = cos(currentTime) * 5.0f;
        float dirLightPosZ = 0.0f;

        lerpedDirLightColor = mix(softYellow, softGrey, t);
        lerpedBackroundColor = mix(lightBlue, darkBlue, t);
        lerpedDirLightIntensity = mix(dirLightIntensityStart, dirLightIntensityEnd, t);

        shader.setVec3("dirLight.direction", dirLightPosX, dirLightPosY, dirLightPosZ);
        shader.setVec3("dirLight.color", lerpedDirLightColor);
        shader.setVec3("dirLight.intensity", lerpedDirLightIntensity);
        glClearColor(lerpedBackroundColor.x, lerpedBackroundColor.y, lerpedBackroundColor.z, 1.0);
        #pragma endregion

        #pragma region Point light render
        fireColor = mix(softRed, softOrange, sin(2* currentTime));
        fireIntensity = mix(fireIntensityStart, fireIntensityEnd, sin(2* currentTime));
        #pragma endregion

        #pragma region Ocean render
        glm::vec3 waterLevel = mix(glm::vec3(0.0, -0.007, 0.0), glm::vec3(0.0, 0.007, 0.0), sin(1.5 * currentTime));
        oceanModel = glm::translate(oreginalOceanModel, waterLevel);
        shader.setMat4("model", oceanModel);
        shader.setInt("fragType", 0);
        ocean.Draw(shader);
        #pragma endregion

        #pragma region Clouds render
        shader.setInt("fragType", 1);
        cloudsModel = glm::translate(cloudsModel, glm::vec3(0.0f, 0.1f, 0.0f));
        currentCloudPos += 0.1f;
        if (currentCloudPos >= 80.0f) 
        {
            cloudsModel = glm::mat4(1.0f);
            cloudsModel = glm::scale(cloudsModel, glm::vec3(0.2f, 0.2f, 0.2f));
            cloudsModel = glm::translate(cloudsModel, glm::vec3(20.0f, 15.0f, 0.0f));
            cloudsModel = glm::rotate(cloudsModel, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            currentCloudPos = 0.0f;
        }

        shader.setMat4("model", cloudsModel);
        clouds.Draw(shader);
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

        fireScale = mix(fireScaleStart, fireScaleEnd, sin(2 * currentTime));
        fireModel = glm::scale(oreginalFireModel, fireScale);
        fireModel = glm::translate(fireModel, glm::vec3(1.3f, 0.0f, 0.5f));

        shader.setMat4("model", fireModel);
        fire.Draw(shader);
        #pragma endregion

        #pragma region Palm tree render
        shader.setInt("fragType", 4);
        shader.setMat4("model", palmTreeModel);
        palmTree.Draw(shader);
        #pragma endregion

        #pragma region Sharks render
        useShader(sharkShader, currentProjection, view);
        sharkShader.setFloat("time", currentTime);
        sharkShader.setFloat("rotationSpeed", 1.0f);
        

        sharkShader.setInt("fragType", 5);
        /*sharkShader.setInt("material.diffuse", 0);
        sharkShader.setInt("material.specular", 1);
        sharkShader.setFloat("material.shininess", 32.0f);*/

        sharkShader.setFloat("waterLevel", waterLevel.y);
        sharkShader.setMat4("model", shark1Model);
        sharkShader.setVec3("rotationPoint", island1Centre);

        float spotlightRadius = 10;
        spotlightDir = glm::vec3(-sin(currentTime * 1.0) * spotlightRadius, -5.0, cos(currentTime * 1.0) * spotlightRadius);
        sharkShader.setVec3("spotLight.direction", spotlightDir);

        shark1.Draw(sharkShader);

        sharkShader.setMat4("model", shark2Model);
        sharkShader.setVec3("rotationPoint", island2Centre);
        shark2.Draw(sharkShader);

        sharkShader.setMat4("model", shark3Model);
        sharkShader.setVec3("rotationPoint", island3Centre);
        shark3.Draw(sharkShader);
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

    if (pitch >= 89.0f)
        pitch = 89.0f;
    if (pitch <= -89.0f)
        pitch = -89.0f;

    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void useShader(Shader shader, glm::mat4 projection, glm::mat4 view)
{
    shader.use();

    // vertex shader
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    // material properties
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);
    shader.setFloat("material.shininess", 32.0f);

    // camera position
    shader.setVec3("viewPos", cameraPos);

    // directional light properties
    shader.setVec3("dirLight.direction", 0.0f, 5.0f, 0.0f);
    shader.setVec3("dirLight.color", softYellow);
    shader.setVec3("dirLight.intensity", dirLightIntensityStart);

    shader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
    shader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    shader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

    // point light properties
    shader.setVec3("pointLight.position", firePos);
    shader.setVec3("pointLight.color", fireColor);
    shader.setVec3("pointLight.intensity", fireIntensity);

    shader.setVec3("pointLight.ambient", 0.07f, 0.07f, 0.07f);
    shader.setVec3("pointLight.diffuse", 2.8f, 2.8f, 2.8f);
    shader.setVec3("pointLight.specular", 4.0f, 4.0f, 4.0f);

    shader.setFloat("pointLight.constant", 1.0f);
    shader.setFloat("pointLight.linear", 0.09f);
    shader.setFloat("pointLight.quadratic", 0.032f);

    // spotlight properties
    shader.setVec3("spotLight.position", spotlightPos);
    shader.setVec3("spotLight.direction", spotlightDir);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(20.0f)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(25.0f)));
    shader.setVec3("spotLight.color", purple);

    shader.setFloat("spotLight.constant", 1.0f);
    shader.setFloat("spotLight.linear", 0.09f);
    shader.setFloat("spotLight.quadratic", 0.032f);

    shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
}

