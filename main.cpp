#define _CRT_SECURE_NO_WARNINGS
#define CRES 100 // Circle Resolution = Rezolucija kruga

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source); 
unsigned int createShader(const char* vsSource, const char* fsSource);

void swapFloatArrays(float* array1, float* array2, int size) {
    float* temp = new float[size];

    for (int i = 0; i < size; ++i) {
        temp[i] = array1[i];
    }

    for (int i = 0; i < size; ++i) {
        array1[i] = array2[i];
    }

    for (int i = 0; i < size; ++i) {
        array2[i] = temp[i];
    }

    delete[] temp;
}

static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

int main(void)
{

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ INICIJALIZACIJA ++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if (!glfwInit())
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
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

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ PROMJENLJIVE I BAFERI +++++++++++++++++++++++++++++++++++++++++++++++++

    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");

    unsigned int stride = (2 + 4) * sizeof(float);

    // SUNCE I MESEC
    //=========================================================================================
    float sunAndMoon[CRES * 2 + 4]; // +4 je za x i y koordinate centra kruga, i za x i y od nultog ugla
    float r = 0.1; //poluprecnik

    sunAndMoon[0] = 0; //Centar X0
    sunAndMoon[1] = 0; //Centar Y0
    int i;
    for (i = 0; i <= CRES; i++)
    {

        sunAndMoon[2 + 2 * i] = r * cos((3.141592 / 180) * (i * 360 / CRES)); //Xi
        sunAndMoon[2 + 2 * i + 1] = r * sin((3.141592 / 180) * (i * 360 / CRES)); //Yi
    }
    //Crtali smo od "nultog" ugla ka jednom pravcu, sto nam ostavlja prazno mjesto od poslednjeg tjemena kruznice do prvog,
    //pa da bi ga zatvorili, koristili smo <= umjesto <, sto nam dodaje tjeme (cos(0), sin(0))
    unsigned int sunAndMoonVAO, sunAndMoonVBO;
    glGenVertexArrays(1, &sunAndMoonVAO);
    glGenBuffers(1, &sunAndMoonVBO);

    glBindVertexArray(sunAndMoonVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sunAndMoonVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sunAndMoon), sunAndMoon, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    //=========================================================================================
    
    // OBLACI
    //=========================================================================================
    float cloud[CRES * 2 + 4]; // +4 je za x i y koordinate centra kruga, i za x i y od nultog ugla
    float rCloud = 0.1; //poluprecnik

    cloud[0] = 0; //Centar X0
    cloud[1] = 0; //Centar Y0
    for (int i = 0; i <= CRES; i++)
    {

        cloud[2 + 2 * i] = rCloud * cos((3.141592 / 180) * (i * 360 / CRES)); //Xi
        cloud[2 + 2 * i + 1] = rCloud * sin((3.141592 / 180) * (i * 360 / CRES)); //Yi
    }
    //Crtali smo od "nultog" ugla ka jednom pravcu, sto nam ostavlja prazno mjesto od poslednjeg tjemena kruznice do prvog,
    //pa da bi ga zatvorili, koristili smo <= umjesto <, sto nam dodaje tjeme (cos(0), sin(0))
    unsigned int cloudVAO, cloudVBO;
    glGenVertexArrays(1, &cloudVAO);
    glGenBuffers(1, &cloudVBO);

    glBindVertexArray(cloudVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cloudVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cloud), cloud, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    //=========================================================================================

    // MORE
    //=========================================================================================
    float seaVertices[] = {
        -1.0, -1.0,      0.0, 0.0, 0.4, 1.0, // bottom-left
        -1.0,  0.0,      0.0, 0.0, 0.4, 1.0, // top-left
         1.0, -1.0,      0.0, 0.0, 0.4, 1.0, // bottom-right
         1.0,  0.0,      0.0, 0.0, 0.4, 1.0  // top-right
    };

    unsigned int seaVAO, seaVBO;
    glGenVertexArrays(1, &seaVAO);
    glGenBuffers(1, &seaVBO);

    glBindVertexArray(seaVAO);
    glBindBuffer(GL_ARRAY_BUFFER, seaVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(seaVertices), seaVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //=========================================================================================


    // ostrva
    //=========================================================================================
    const int numIslands = 3;
    float islandVertices[numIslands][18] = {
        // island 1
        {
            -0.95, -0.3, 1.0, 1.0, 0.0, 0.5,
            -0.2, -0.3, 1.0, 1.0, 0.0, 0.5,
            -0.5,  0.0, 1.0, 1.0, 0.0, 0.5
        },
        // island 2
        {
            0.0, -0.7, 1.0, 1.0, 0.0, 0.7,
            0.5, -0.7, 1.0, 1.0, 0.0, 0.7,
            0.25, -0.4, 1.0, 1.0, 0.0, 0.7
        },
        // island 3
        {
            0.5, -0.2, 1.0, 1.0, 0.0, 0.9,
            0.95, -0.2, 1.0, 1.0, 0.0, 0.9,
            0.65, 0.1, 1.0, 1.0, 0.0, 0.9
        }
    };

    unsigned int islandVAOs[numIslands];
    unsigned int islandVBOs[numIslands];
    int islandVertexCounts[numIslands];

    for (int i = 0; i < numIslands; ++i) {
        glGenVertexArrays(1, &islandVAOs[i]);
        glGenBuffers(1, &islandVBOs[i]);

        glBindVertexArray(islandVAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, islandVBOs[i]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(islandVertices[i]), islandVertices[i], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        islandVertexCounts[i] = sizeof(islandVertices[i]) / stride;
    }
    //=========================================================================================

    // AJKULE
    //=========================================================================================
    const int numSharks = 6;

    float sharkVertices[numSharks][18] = {
        // ajkula 1
        {
            -0.55, -0.55, 0.5, 0.5, 0.5, 0.5, // bottom-left
            -0.525, -0.45, 0.5, 0.5, 0.5, 0.5, // top
            -0.5, -0.55, 0.5, 0.5, 0.5, 0.5  // bottom-right
        },
        // ajkula 2
        {
            -0.45, -0.6, 0.5, 0.5, 0.5, 0.5, // bottom-left
            -0.425, -0.5, 0.5, 0.5, 0.5, 0.5, // top
            -0.4, -0.6, 0.5, 0.5, 0.5, 0.5  // bottom-right
        },
        // ajkula 3
        {
            -0.05, -0.15, 0.5, 0.5, 0.5, 0.5, // bottom-left
            -0.025, -0.05, 0.5, 0.5, 0.5, 0.5, // top
            0.0, -0.15, 0.5, 0.5, 0.5, 0.5  // bottom-right
            },
        // ajkula 4
        {
            0.1, -0.2, 0.5, 0.5, 0.5, 0.5, // bottom-left
            0.075, -0.1, 0.5, 0.5, 0.5, 0.5, // top
            0.05, -0.2, 0.5, 0.5, 0.5, 0.5  // bottom-right
        },
        // ajkula 5
        {
            0.8, -0.55, 0.5, 0.5, 0.5, 0.5, // bottom-left
            0.825, -0.45, 0.5, 0.5, 0.5, 0.5, // top
            0.85, -0.55, 0.5, 0.5, 0.5, 0.5  // bottom-right
        },
        // ajkula 6
        {
            0.7, -0.65, 0.5, 0.5, 0.5, 0.5, // bottom-left
            0.725, -0.55, 0.5, 0.5, 0.5, 0.5, // top
            0.75, -0.65, 0.5, 0.5, 0.5, 0.5  // bottom-right
        },
    };
    

    unsigned int sharkVAOs[numSharks];
    unsigned int sharkVBOs[numSharks];
    int sharkVertexCounts[numSharks];

    for (int i = 0; i < numSharks; ++i) {
        glGenVertexArrays(1, &sharkVAOs[i]);
        glGenBuffers(1, &sharkVBOs[i]);

        glBindVertexArray(sharkVAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, sharkVBOs[i]);
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(sharkVertices[i]), sharkVertices[i], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        sharkVertexCounts[i] = sizeof(sharkVertices[i]) / stride;
    }
    //=========================================================================================

    // PALMA
    //========================================================================================= 
    float palmTreeTrunkVertices[] = {
        -0.4, -0.17, 0.5, 0.2, 0.0, 0.0, // bottom-left
        -0.4, 0.15, 0.5, 0.2, 0.0, 0.0, // top-left
        -0.365, -0.17, 0.5, 0.2, 0.0, 0.0, // bottom-right

        -0.365, -0.17, 0.5, 0.2, 0.0, 0.0, // bottom-right
        -0.4, 0.15, 0.5, 0.2, 0.0, 0.0, // top-left
        -0.365, 0.15, 0.5, 0.2, 0.0, 0.0  // top-right
    };

    unsigned int palmTreeTrunkVAO, palmTreeTrunkVBO;
    glGenVertexArrays(1, &palmTreeTrunkVAO);
    glGenBuffers(1, &palmTreeTrunkVBO);

    glBindVertexArray(palmTreeTrunkVAO);
    glBindBuffer(GL_ARRAY_BUFFER, palmTreeTrunkVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(palmTreeTrunkVertices), palmTreeTrunkVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    const int numLeafs = 5;
    float leafVertices[numLeafs][18] = {
        // leaf 1
        {
            -0.38, 0.1, 0.0, 1.0, 0.0, 0.0, // bottom-left
            -0.5, 0.125, 0.0, 1.0, 0.0, 0.0, // top
            -0.38, 0.15, 0.0, 1.0, 0.0, 0.0  // bottom-right
        },
        // leaf 2
        {
            -0.375, 0.16, 0.0, 1.0, 0.0, 0.0, // bottom-left
            -0.45, 0, 0.0, 1.0, 0.0, 0.0, // top
            -0.375, 0.1, 0.0, 1.0, 0.0, 0.0, // bottom-right
        },
        // leaf 3
        {
            -0.365, 0.16, 0.0, 1.0, 0.0, 0.0, // bottom-left
            -0.3, 0, 0.0, 1.0, 0.0, 0.0, // top
            -0.39, 0.1, 0.0, 1.0, 0.0, 0.0,  // bottom-right
        },
        // leaf 4
        {
            -0.4, 0.125, 0.0, 1.0, 0.0, 0.0, // bottom-left
            -0.43, 0.3, 0.0, 1.0, 0.0, 0.0, // top
            -0.36, 0.125, 0.0, 1.0, 0.0, 0.0,  // bottom-right
        },
        // leaf 5
        {
            -0.38, 0.125, 0.0, 1.0, 0.0, 0.0, // bottom-left
            -0.32, 0.25, 0.0, 1.0, 0.0, 0.0, // top
            -0.35, 0.125, 0.0, 1.0, 0.0, 0.0,  // bottom-right
        }
    };

    unsigned int leafVAOs[numLeafs];
    unsigned int leafVBOs[numLeafs];
    int leafVertexCounts[numLeafs];

    for (int i = 0; i < numLeafs; ++i) {
        glGenVertexArrays(1, &leafVAOs[i]);
        glGenBuffers(1, &leafVBOs[i]);

        glBindVertexArray(leafVAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, leafVBOs[i]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(leafVertices[i]), leafVertices[i], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        leafVertexCounts[i] = sizeof(leafVertices[i]) / (2 * sizeof(float));
    }

    //=========================================================================================

    // PLAMEN
    //=========================================================================================
    const int numFlames = 2;
    float flameVertices[numFlames][18] = {
        // glavni plamen
        {
            -0.58, -0.1,    1.0, 0.0, 0.0, 0.0, // bottom-left
            -0.55,  0.0,     1.0, 1.0, 0.0, 0.9, // top
            -0.52, -0.1,    1.0, 0.0, 0.0, 0.0  // bottom-right
        },
        // mali plamen
        {
            -0.56, -0.1,    1.0, 1.0, 0.0, 0.9, // bottom-left
            -0.55, -0.05,   1.0, 1.0, 0.0, 0.9, // top
            -0.54, -0.1,    1.0, 0.0, 0.0, 0.0,  // bottom-right
        },
    };

    unsigned int flameVAOs[numFlames];
    unsigned int flameVBOs[numFlames];
    int flameVertexCounts[numFlames];

    for (int i = 0; i < numFlames; ++i) {
        glGenVertexArrays(1, &flameVAOs[i]);
        glGenBuffers(1, &flameVBOs[i]);

        glBindVertexArray(flameVAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, flameVBOs[i]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(flameVertices[i]), flameVertices[i], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        flameVertexCounts[i] = sizeof(flameVertices[i]) / (2 * sizeof(float));
    }
    //=========================================================================================

    // POTPIS
    //=========================================================================================
    float signatureVertices[] = {
     0.7, -0.7,      0.0, 1.0, // top-left
     0.7, -1.0,      0.0, 0.0, // bottom-left
     1.0,  -0.7,     1.0, 1.0,  // top-right
     1.0, -1.0,      1.0, 0.0, // bottom-right
    };

    unsigned int signatureIndecies[] = {
        0, 1, 2,
        1, 2, 3
    };
    unsigned int signatureVAO, signatureVBO, signatureEBO;
    glGenVertexArrays(1, &signatureVAO);
    glGenBuffers(1, &signatureVBO);
    glGenBuffers(1, &signatureEBO);

    glBindVertexArray(signatureVAO);
    glBindBuffer(GL_ARRAY_BUFFER, signatureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(signatureVertices), signatureVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, signatureEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(signatureIndecies), signatureIndecies, GL_STATIC_DRAW);

    glBindVertexArray(0);

    unsigned int signatureShader = createShader("signature.vert", "signature.frag");
    unsigned signatureTexture = loadImageToTexture("potpis.png");
    glBindTexture(GL_TEXTURE_2D, signatureTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(signatureShader);
    unsigned uTexLoc = glGetUniformLocation(signatureShader, "uTex");
    glUniform1i(uTexLoc, 0);
    glUseProgram(0);
    //=========================================================================================

    // podesavanja nivoa vode
    float waterLevel = -0.1;
    float waterLevelIncrement = 0.0001;
    bool increaseWaterLevel = true;
    int waterLevelLocation = glGetUniformLocation(unifiedShader, "waterLevel");

    // podesavanja kretanja ajkula
    float sharkOffset = 0.00005;
    bool sharkGoLeft = true;
    float sharkPositions[numSharks] = { 0, 0, 0, 0 };
    int sharkpositionslocation = glGetUniformLocation(unifiedShader, "sharkPositions");

    // podesavanje plamena
    unsigned int flameShader = createShader("flame.vert", "flame.frag");
    float flameExpandOffset = 0.00008;
    float flamePosition = 0;
    bool flameExpand = true;
    int flameExpandCounter = 0;
    int flameExpandLimit = 200;
    float flameColor1[] = { 1.0f, 0.0f, 0.0f, 0.0f };
    float flameColor2[] = { 1.0f, 0.5f, 0.0f, 1.0f };
    int flamePositionLocation = glGetUniformLocation(flameShader, "flamePosition");
    GLint flameColor1Location = glGetUniformLocation(flameShader, "flameColor1");
    GLint flameColor2Location = glGetUniformLocation(flameShader, "flameColor2");

    // podesavanja za sunce i mesec
    unsigned int sunAndMoonShader = createShader("sunAndMoon.vert", "sunAndMoon.frag");
    int aspectRatioLocation = glGetUniformLocation(sunAndMoonShader, "aspectRatio");
    GLint sunAndMoonColorLocation = glGetUniformLocation(sunAndMoonShader, "sunAndMoonColor");
    float sunColor[] = { 1.0f, 0.5f, 0.0f, 1.0f };
    float moonColor[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    float skyColor[] = { 0.7, 0.7, 1.0, 1.0, };
    float aspectRatio = static_cast<float>(wHeight) / wWidth;

    unsigned int uPosLocation = glGetUniformLocation(sunAndMoonShader, "uPos");
    float pathR = 0.8;
    float rotationSpeed = 0.5;
    float moonPosition[] = { 0, 0 };

    // podesavavanja za kretanje oblaka
    unsigned int cloudShader = createShader("cloud.vert", "cloud.frag");
    int xOffsetLocation = glGetUniformLocation(cloudShader, "xOffset");
    int yOffsetLocation = glGetUniformLocation(cloudShader, "yOffset");
    GLint cloudColorLocation = glGetUniformLocation(cloudShader, "cloudColor");
    float cloudColor1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float cloudColor2[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    float leftmostVertexX = -1 - rCloud;
    float cloudPosition = 0;
    float cloudIncrement = 0.0003;

    glClearColor(0.7, 0.7, 1.0, 1.0);

    while (!glfwWindowShouldClose(window))
    {
        // Komande
        //=========================================================================================
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glEnable(GL_PROGRAM_POINT_SIZE);
            glPointSize(4);
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
            waterLevelIncrement += 0.00001;
            sharkOffset += 0.000001;
            flameExpandOffset += 0.000001;
            rotationSpeed += 0.01;
            cloudIncrement += 0.00001;
        }
        if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
            if (waterLevelIncrement - 0.00001 > 0.00001) waterLevelIncrement -= 0.00001;
            if (sharkOffset - 0.000001 > 0.00001) sharkOffset -= 0.000001;
            if (flameExpandOffset - 0.000001 > 0.00001) flameExpandOffset -= 0.000001;
            if (rotationSpeed - 0.1 > 0) rotationSpeed -= 0.02;
            if (cloudIncrement - 0.0001 > 0) cloudIncrement -= 0.00001;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            waterLevelIncrement = 0.0001;

            sharkOffset = 0.00005;
            sharkPositions[0] = 0;
            sharkPositions[1] = 0;
            sharkPositions[2] = 0;
            sharkPositions[3] = 0;

            flameExpandOffset = 0.00008;
            flamePosition = 0;

            rotationSpeed = 0.5;
            cloudIncrement = 0.0003;
        }
        //=========================================================================================

        if (moonPosition[1] > 0) {
            glClearColor(0.0, 0.0, 0.2, 1.0);
        }
        else {
            glClearColor(0.7, 0.7, 1.0, 1.0);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        // Crtanje sunca i meseca
        //=========================================================================================
        glUseProgram(sunAndMoonShader);       

        // sunce
        glUniform1f(aspectRatioLocation, aspectRatio);
        glUniform4fv(sunAndMoonColorLocation, 1, sunColor);
        glUniform2f(uPosLocation, pathR * cos(glfwGetTime() * rotationSpeed), pathR * (sin(glfwGetTime() * rotationSpeed)));
        glBindVertexArray(sunAndMoonVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, (CRES + 2));

        // mesec
        glUniform1f(aspectRatioLocation, aspectRatio);
        glUniform4fv(sunAndMoonColorLocation, 1, moonColor);
        float uPosX = -pathR * cos(glfwGetTime() * rotationSpeed);
        float uPosY = -pathR * sin(glfwGetTime() * rotationSpeed);
        glUniform2f(uPosLocation, uPosX, uPosY);
        glBindVertexArray(sunAndMoonVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, (CRES + 2));

        moonPosition[0] = uPosX;
        moonPosition[1] = uPosY;
        //=========================================================================================
        
        // crtanje oblaka
        //=========================================================================================
        glUseProgram(cloudShader);

        if (leftmostVertexX > 1) {
            leftmostVertexX = -1 - rCloud;
            cloudPosition = 0;
        }

        glUniform1f(xOffsetLocation, -1 + cloudPosition);
        glUniform1f(yOffsetLocation, 0.5);
        glUniform4fv(cloudColorLocation, 1, cloudColor1);
        glBindVertexArray(cloudVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, (CRES + 2));

        glUniform1f(xOffsetLocation, -0.95 + cloudPosition);
        glUniform1f(yOffsetLocation, 0.45);
        glUniform4fv(cloudColorLocation, 1, cloudColor2);
        glBindVertexArray(cloudVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, (CRES + 2));

        cloudPosition += cloudIncrement;
        leftmostVertexX += cloudIncrement;

        //=========================================================================================

        glUseProgram(unifiedShader);

        // Crtanje mora
        //=========================================================================================
        if (increaseWaterLevel) {
            waterLevel += waterLevelIncrement;
            if (waterLevel >= 0.1)
            {
                waterLevel = 0.1;
                increaseWaterLevel = !increaseWaterLevel;
            }
        }
        else {
            waterLevel -= waterLevelIncrement;
            if (waterLevel <= -0.1)
            {
                waterLevel = -0.1;
                increaseWaterLevel = !increaseWaterLevel;
            }
        }

        glBindVertexArray(seaVAO);
        glBindBuffer(GL_ARRAY_BUFFER, seaVBO);

        glUniform1f(waterLevelLocation, waterLevel);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(seaVertices) / stride);
        //=========================================================================================

        // crtanje ajkula

        for (int i = 0; i < numSharks; ++i) {
            if (sharkGoLeft) {
                sharkPositions[i] -= sharkOffset;
                if (sharkPositions[i] - sharkOffset <= -0.004)
                {
                    sharkPositions[i] = -0.004;
                    sharkGoLeft = !sharkGoLeft;
                }                    
            }
            else {
                sharkPositions[i] += sharkOffset;
                if (sharkPositions[i] + sharkOffset >= 0.04)
                {
                    sharkPositions[i] = 0.04;
                    sharkGoLeft = !sharkGoLeft;
                }                    
            }
        }

        glUniform1fv(sharkpositionslocation, numSharks, sharkPositions);

        for (int i = 0; i < numSharks; ++i) {
            glBindVertexArray(sharkVAOs[i]);
            glDrawArrays(GL_TRIANGLES, 0, sharkVertexCounts[i]);
        }
        //=========================================================================================

        // crtanje ostrva
        //=========================================================================================
        for (int i = 0; i < numIslands; ++i) {
            glBindVertexArray(islandVAOs[i]);
            glDrawArrays(GL_TRIANGLES, 0, islandVertexCounts[i]);
        }
        //=========================================================================================

        // crtanje palme
        //=========================================================================================
        glBindVertexArray(palmTreeTrunkVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        for (int i = 0; i < numLeafs; ++i) {
            glBindVertexArray(leafVAOs[i]);
            glDrawArrays(GL_TRIANGLES, 0, leafVertexCounts[i]);
        }
        //=========================================================================================

        // crtanje plamena
        //=========================================================================================
        glUseProgram(flameShader);

        if (flameExpand) {
            flamePosition += flameExpandOffset;
            if (flamePosition + flameExpandOffset >= 0.04) {
                flamePosition = 0.04;
                flameExpand = !flameExpand;
            }                
        }
        else {
            flamePosition -= flameExpandOffset;
            if (flamePosition - flameExpandOffset <= -0.001) {
                flamePosition = -0.001;
                flameExpand = !flameExpand;
            }                
        }

        if (flameExpandCounter == 100)
        {
            glUniform4fv(flameColor1Location, 1, flameColor1);
            glUniform4fv(flameColor2Location, 1,flameColor2);
           
            swapFloatArrays(flameColor1, flameColor2, 4);
            flameExpandCounter = 0;
        }

        glUniform1f(flamePositionLocation, flamePosition);

        for (int i = 0; i < numFlames; ++i) {
            glBindVertexArray(flameVAOs[i]);
            glDrawArrays(GL_TRIANGLES, 0, flameVertexCounts[i]);
        }

        flameExpandCounter++;
        //=========================================================================================

        // potpis
        //=========================================================================================
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(signatureShader);
        glBindVertexArray(signatureVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, signatureTexture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0*sizeof(unsigned int)));
        
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        //=========================================================================================

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ POSPREMANJE +++++++++++++++++++++++++++++++++++++++++++++++++

    /*glDeleteBuffers(2, seaVBO);
    glDeleteProgram(unifiedShader);*/

    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
    //Citanje izvornog koda iz fajla
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)
    
    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
    glCompileShader(shader); //Kompajliraj sejder

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

    unsigned int program; //Objedinjeni sejder
    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

    //Zakaci verteks i fragment sejdere za objedinjeni program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
    glValidateProgram(program); //Izvrsi provjeru novopecenog programa

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
