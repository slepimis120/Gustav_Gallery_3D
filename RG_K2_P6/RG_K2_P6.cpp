// Autor: Nemanja Simsic
// Opis: Gustav 3D
#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders/shader_m.h"
#include "camera/camera.h"
#include "model.h"

#include <iostream>
#include <filesystem>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, bool& isBatteryLampActive, Shader& lightingShader);
bool isModelInView(const glm::mat4& modelMatrix, const glm::vec3& modelScale, const Camera& camera);
unsigned int loadTexture(const char* path);

const unsigned int SCR_WIDTH = 1366;
const unsigned int SCR_HEIGHT = 768;

//Kamera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float radiansValue = 0;
bool firstMouse = true;

//Timing
float deltaTime = 0.0f;	// Vreme izmedju trenutnog i poslednjeg frame-a
float lastFrame = 0.0f;

//Osvetljenje
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

bool isLine = false;
bool isBatteryLampActive = false;
bool isLightOn = true;
bool isBKeyPressedLastFrame = false;
bool isLKeyPressedLastFrame = false;

int main(void)
{

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ INICIJALIZACIJA ++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //GLFW: Inicijalizacija i konfiguracija
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //GLFW: Kreiranje prozora
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Gustav Projekat Studenta Nemanja Simsic Indeksa SV 68 Kroz 2020", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



    //GLAD: Ucitati sve OpenGL pointere
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD nije mogao da se ucita! :'(\n" << std::endl;
        return -1;
    }

    //Konfigurisanje globalnog OpenGL State-a
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //Buildovanje i kompajliranje shader zprograma
    Shader lightingShader("lighting.vert", "lighting.frag");
    Shader lightCubeShader("light_cube_shader.vert", "light_cube_shader.frag");
    Shader indexShader("index.vert", "index.frag");


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ PROMJENLJIVE I BAFERI +++++++++++++++++++++++++++++++++++++++++++++++++

    //Temena trouglova koja sadrže sve informacije o njemu
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    float indexPictureVertices[] = {
        // Positions   // Texture Coords
        -0.90, -0.60,  0.0, 0.0,
        -0.90, -0.90,  0.0, 1.0,
        -0.30, -0.90,  1.0, 1.0,
        -0.30, -0.60,  1.0, 0.0,
    };

    //Koliko mesta u memoriji izmedju istih komponenti susednih temena
    unsigned int stride = 8 * sizeof(float);

    //VAO
    //Vertex Array Object - Kontejner za VBO i pokazivace na njihove atribute
    unsigned int VAO;
    //Generisi 1 kontejner na adresi od promjenljive "VAO"
    glGenVertexArrays(1, &VAO);
    //Povezi VAO za aktivni kontekst - Sve sto radimo ispod ovoga ce se odnositi na kontejner "VAO"
    glBindVertexArray(VAO);

    //VBO
    //Vertex Buffer Object - Nase figure koje crtamo
    unsigned int VBO;
    //Generisi 1 bafer sa adresom promjenljive "VBO" 
    glGenBuffers(1, &VBO);
    //Povezi "VBO" za aktivni Array Buffer (on se koristi za VBO-eve)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //Objasni gdje su podaci i za koji bafer. Moze se koristiti GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //POZICIONI ATRIBUTI
    //Objasni da su prva tri broja u temenu jedan atribut (u ovom slucaju pozicija)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    //Aktiviraj taj pokazivac i tako intepretiraj podatke
    glEnableVertexAttribArray(0);

    //ATRIBUTI ZA TEKSTURU
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //Konfigurisemo VAO svetla (VBO ostaje isti; ivice su iste za 3D objekat)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //Updateujemo poziciju lampe da bi reflektovao buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int pictureVAO[4];
    glGenVertexArrays(4, pictureVAO);
    unsigned int pictureVBO[4];
    glGenBuffers(4, pictureVBO);

    glBindVertexArray(pictureVAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, pictureVBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indexPictureVertices), indexPictureVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //Ucitavanje tekstura
    unsigned int diffuseTilesMap = loadTexture("../res/tiles.png");
    unsigned int specularTilesMap = loadTexture("../res/tiles_specular.png");
    unsigned int diffuseWallGoldMap = loadTexture("../res/wall_gold.png");
    unsigned int specularWallGoldMap = loadTexture("../res/wall_gold_specular.png");
    unsigned int diffuseWallWoodMap = loadTexture("../res/wall_wood.png");
    unsigned int specularWallWoodMap = loadTexture("../res/wall_wood_specular.png");

    unsigned int image01 = loadTexture("../res/image01.png");
    unsigned int image02 = loadTexture("../res/image02.png");
    unsigned int image03 = loadTexture("../res/image03.png");
    unsigned int indexMap = loadTexture("../res/index.png");

    stbi_set_flip_vertically_on_load(true);

    Model angel("../res/uploads_files_874740_Angel.obj");

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP - PETLJA ZA CRTANJE +++++++++++++++++++++++++++++++++++++++++++++++++

    while (!glfwWindowShouldClose(window))
    {
        // Per-Frame vremenska logika
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, isBatteryLampActive, lightingShader);

        // RENDER
        // Brisanje ekrana
        // RGBA (A - neprovidno)
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // Clear Depth Buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();
        lightingShader.setInt("material.diffuse", 0);
        lightingShader.setInt("material.specular", 1);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);
        lightingShader.setBool("isFlashlightOn", isBatteryLampActive);
        lightingShader.setBool("isLightOn", isLightOn);

        //Aktiviranje shadera
        lightingShader.setVec3("spotLight[0].position", camera.Position);
        lightingShader.setVec3("spotLight[0].direction", camera.Front);
        lightingShader.setVec3("spotLight[0].ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight[0].diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight[0].constant", 1.0f);
        lightingShader.setFloat("spotLight[0].linear", 0.09f);
        lightingShader.setFloat("spotLight[0].quadratic", 0.032f);
        lightingShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(15.0f)));

        float time = glfwGetTime();
        glm::vec3 dynamicColor = glm::vec3(sin(2.0 * 3.14 * 0.5 * time), cos(2.0 * 3.14 * 0.5 * time), sin(2.0 * 3.14 * 0.5 * time));
        
        //Aktiviranje shadera
        lightingShader.setVec3("spotLight[1].position", glm::vec3(0.0f, 0.95f, -3.0f));
        lightingShader.setVec3("spotLight[1].direction", 0.0f, -1.0f, 0.0f);
        lightingShader.setVec3("spotLight[1].ambient", dynamicColor*glm::vec3(0.5, 0.5, 0.5));
        lightingShader.setVec3("spotLight[1].diffuse", dynamicColor);
        lightingShader.setVec3("spotLight[1].specular", dynamicColor);
        lightingShader.setFloat("spotLight[1].constant", 1.0f);
        lightingShader.setFloat("spotLight[1].linear", 0.09f);
        lightingShader.setFloat("spotLight[1].quadratic", 0.032f);
        lightingShader.setFloat("spotLight[1].cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight[1].outerCutOff", glm::cos(glm::radians(15.0f)));

        //Blin-Phong
        lightingShader.setVec3("pointLights[0].position", glm::vec3(0.0f, 0.75f, 0.0f));
        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09f);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032f);

        // KOD ZA CRTANJE
        // Kreiranje transformacija
        //Pod
        glm::mat4 modelFloor = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        modelFloor = glm::scale(modelFloor, glm::vec3(2.0f, 0.05f, 2.0f));
        modelFloor = glm::rotate(modelFloor, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        //View/Projection transformacija
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        //Svet transformacija
        lightingShader.setMat4("model", modelFloor);

        //Bindovanje difuzne mape
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTilesMap);
        //Bindovanje spekularne mape
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularTilesMap);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //HodnikPod
        glm::mat4 hallwayFloor = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -1.5f));
        hallwayFloor = glm::scale(hallwayFloor, glm::vec3(1.0f, 0.05f, 1.0f));
        hallwayFloor = glm::rotate(hallwayFloor, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        lightingShader.setMat4("model", hallwayFloor);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //PodSoba2
        glm::mat4 model2Floor = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -3.0f));
        model2Floor = glm::scale(model2Floor, glm::vec3(2.0f, 0.05f, 2.0f));
        model2Floor = glm::rotate(model2Floor, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        lightingShader.setMat4("model", model2Floor);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Plafon
        glm::mat4 modelRoof = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelRoof = glm::scale(modelRoof, glm::vec3(2.0f, 0.05f, 2.0f));
        lightingShader.setMat4("model", modelRoof);

        //Bindovanje difuzne mape
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseWallGoldMap);
        //Bindovanje spekularne mape
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularWallGoldMap);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        //ZidDesno
        glm::mat4 modelRightWall = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelRightWall = glm::scale(modelRightWall, glm::vec3(0.05f, 2.0f, 2.0f));
        lightingShader.setMat4("model", modelRightWall);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //ZidLevo
        glm::mat4 modelLeftWall = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        modelLeftWall = glm::scale(modelLeftWall, glm::vec3(0.05f, 2.0f, 2.0f));
        modelLeftWall = glm::rotate(modelLeftWall, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightingShader.setMat4("model", modelLeftWall);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //ZidNazad
        glm::mat4 modelBackWall = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelBackWall = glm::scale(modelBackWall, glm::vec3(2.0f, 2.0f, 0.05f));
        modelBackWall = glm::rotate(modelBackWall, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightingShader.setMat4("model", modelBackWall);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //ZidNapredDesno
        glm::mat4 modelFrontWallRight = glm::translate(glm::mat4(1.0f), glm::vec3(1.4f, 0.0f, -1.0f));
        modelFrontWallRight = glm::scale(modelFrontWallRight, glm::vec3(2.0f, 2.0f, 0.05f));
        lightingShader.setMat4("model", modelFrontWallRight);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //ZidNapredLevo
        glm::mat4 modelFrontWallLeft = glm::translate(glm::mat4(1.0f), glm::vec3(-1.4f, 0.0f, -1.0f));
        modelFrontWallLeft = glm::scale(modelFrontWallLeft, glm::vec3(2.0f, 2.0f, 0.05f));
        lightingShader.setMat4("model", modelFrontWallLeft);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //HodnikLevo
        glm::mat4 hallwayLeft = glm::translate(glm::mat4(1.0f), glm::vec3(-0.4f, 0.0f, -1.5f));
        hallwayLeft = glm::scale(hallwayLeft, glm::vec3(0.05f, 2.0f, 1.0f));
        hallwayLeft = glm::rotate(hallwayLeft, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightingShader.setMat4("model", hallwayLeft);
        //Bindovanje difuzne mape
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseWallWoodMap);
        //Bindovanje spekularne mape
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularWallWoodMap);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //HodnikDesno
        glm::mat4 hallwayRight = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, -1.5f));
        hallwayRight = glm::scale(hallwayRight, glm::vec3(0.05f, 2.0f, 1.0f));
        lightingShader.setMat4("model", hallwayRight);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //PlafonHodnik
        glm::mat4 hallwayRoof = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, -1.5f));
        hallwayRoof = glm::scale(hallwayRoof, glm::vec3(1.0f, 0.05f, 1.0f));
        lightingShader.setMat4("model", hallwayRoof);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //PlafonSoba2
        glm::mat4 model2Roof = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, -3.0f));
        model2Roof = glm::scale(model2Roof, glm::vec3(2.0f, 0.05f, 2.0f));
        lightingShader.setMat4("model", model2Roof);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //ZidNapredDesnoSoba2
        glm::mat4 model2FrontWallRight = glm::translate(glm::mat4(1.0f), glm::vec3(1.4f, 0.0f, -2.0f));
        model2FrontWallRight = glm::scale(model2FrontWallRight, glm::vec3(2.0f, 2.0f, 0.05f));
        model2FrontWallRight = glm::rotate(model2FrontWallRight, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightingShader.setMat4("model", model2FrontWallRight);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //ZidNapredLevoSoba2
        glm::mat4 model2FrontWallLeft = glm::translate(glm::mat4(1.0f), glm::vec3(-1.4f, 0.0f, -2.0f));
        model2FrontWallLeft = glm::scale(model2FrontWallLeft, glm::vec3(2.0f, 2.0f, 0.05f));
        model2FrontWallLeft = glm::rotate(model2FrontWallLeft, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightingShader.setMat4("model", model2FrontWallLeft);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //ZidDesnoSoba2
        glm::mat4 model2RightWall = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, -3.0f));
        model2RightWall = glm::scale(model2RightWall, glm::vec3(0.05f, 2.0f, 2.0f));
        lightingShader.setMat4("model", model2RightWall);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //ZidLevoSoba2
        glm::mat4 model2LeftWall = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -3.0f));
        model2LeftWall = glm::scale(model2LeftWall, glm::vec3(0.05f, 2.0f, 2.0f));
        model2LeftWall = glm::rotate(model2LeftWall, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightingShader.setMat4("model", model2LeftWall);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //ZidNazadSoba2
        glm::mat4 model2BackWall = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));
        model2BackWall = glm::scale(model2BackWall, glm::vec3(2.0f, 2.0f, 0.05f));
        lightingShader.setMat4("model", model2BackWall);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Salter
        glm::mat4 modelCounter = glm::translate(glm::mat4(1.0f), glm::vec3(0.35f, -0.3f, -1.5f));
        modelCounter = glm::scale(modelCounter, glm::vec3(0.1f, 0.1f, 0.3f));
        modelCounter = glm::rotate(modelCounter, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        lightingShader.setMat4("model", modelCounter);

        //Bindovanje difuzne mape
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTilesMap);
        //Bindovanje spekularne mape
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularTilesMap);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Bindovanje difuzne mape
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, image01);
        //Bindovanje spekularne mape
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, image01);

        glm::mat4 picture01 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.4f));
        picture01 = glm::rotate(picture01, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
        lightingShader.setMat4("model", picture01);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Bindovanje difuzne mape
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, image02);
        //Bindovanje spekularne mape
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, image02);

        glm::mat4 picture02 = glm::translate(glm::mat4(1.0f), glm::vec3(1.4f, 0.0f, 0.0f));
        picture02 = glm::rotate(picture02, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        lightingShader.setMat4("model", picture02);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Bindovanje difuzne mape
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, image03);
        //Bindovanje spekularne mape
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, image03);

        glm::mat4 picture03 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.4f, 0.0f, 0.0f));
        picture03 = glm::rotate(picture03, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 1.0f));
        lightingShader.setMat4("model", picture03);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Bindovanje difuzne mape
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseWallWoodMap);
        //Bindovanje spekularne mape
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularWallWoodMap);

        glm::mat4 angelModel = glm::mat4(1.0f);
        angelModel = glm::translate(angelModel, glm::vec3(0.0f, -0.90f, -3.0f));
        angelModel = glm::scale(angelModel, glm::vec3(0.02f, 0.02f, 0.02f));

        glm::vec3 cameraToStatue = glm::vec3(0.0f, -0.90f, -3.0f) - camera.Position;

        glm::vec3 frontDirection = glm::normalize(camera.Front);
        glm::vec3 cameraToStatueDirection = glm::normalize(cameraToStatue);

        float dotProduct = glm::dot(frontDirection, cameraToStatueDirection);

        float threshold = 0.70f;

        if (dotProduct < threshold) {
            radiansValue += 2.0;
        }

        angelModel = glm::rotate(angelModel, glm::radians(radiansValue), glm::vec3(0.0f, 1.0f, 0.0f));

        lightingShader.setMat4("model", angelModel);
        angel.Draw(lightingShader);


        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        glBindVertexArray(lightCubeVAO);

        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, glm::vec3(0.0f, 0.95f, 0.0f));
        lightModel = glm::scale(lightModel, glm::vec3(0.05f));
        lightCubeShader.setMat4("model", lightModel);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 lightModel2 = glm::mat4(1.0f);
        lightModel2 = glm::translate(lightModel2, glm::vec3(0.0f, 0.95f, -3.0f));
        lightModel2 = glm::scale(lightModel2, glm::vec3(0.05f));
        lightCubeShader.setMat4("model", lightModel2);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        indexShader.use();
        glBindVertexArray(pictureVAO[3]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, indexMap);
        unsigned uTexLoc = glGetUniformLocation(indexShader.ID, "uTexture1");
        glUniform1i(uTexLoc, 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Zamena vidljivog bafera sa pozadinskim
        glfwSwapBuffers(window);

        // Hvatanje dogadjaja koji se ticu okvira prozora (promjena velicine, pomjeranje itd)
        glfwPollEvents();
    }


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ POSPREMANJE +++++++++++++++++++++++++++++++++++++++++++++++++


    //Brisanje bafera i shadera
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &diffuseTilesMap);
    glDeleteTextures(1, &specularTilesMap);
    glDeleteTextures(1, &diffuseWallGoldMap);
    glDeleteTextures(1, &specularWallGoldMap);
    glDeleteTextures(1, &diffuseWallWoodMap);
    glDeleteTextures(1, &specularWallWoodMap);
    glDeleteTextures(1, &indexMap);
    //Sve OK - batali program
    glfwTerminate();
    return 0;
}

// Procesuj sav input
void processInput(GLFWwindow* window, bool& isBatteryLampActive, Shader& lightingShader)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        isLine = true;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        isLine = false;
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !isBKeyPressedLastFrame)
    {
        isBatteryLampActive = !isBatteryLampActive;
    }


    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !isLKeyPressedLastFrame)
    {
        glm::vec3 cameraPosition = camera.Position;
        if (cameraPosition.x >= -0.3f && cameraPosition.x <= 0.3f &&
            cameraPosition.z >= -2.0f && cameraPosition.z <= -1.0f)
        {
            isLightOn = !isLightOn;
        }

    }

    if (isLine) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    isBKeyPressedLastFrame = (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS);
    isLKeyPressedLastFrame = (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// GLFW: Kadgod se Windows resizeuje, ovo se poziva
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//Ucitavanje 2D tekstura iz fajla
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

bool isModelInView(const glm::mat4& modelMatrix, const glm::vec3& modelScale, const Camera& camera) {
    // Get the model's position from the model matrix
    glm::vec3 modelPosition = glm::vec3(modelMatrix[3]);

    // Adjust the model's position based on its scale
    modelPosition += modelScale * 0.5f; // Assuming the model origin is at the center

    // Calculate the vector from the camera to the model
    glm::vec3 cameraToModel = modelPosition - camera.Position;

    // Normalize both vectors for comparison
    glm::vec3 front = glm::normalize(camera.Front);
    glm::vec3 cameraToModelNormalized = glm::normalize(cameraToModel);

    // Set a threshold for the dot product to determine if the camera is looking at the model
    float dotProductThreshold = 0.5f; // Adjust as needed

    // Check if the camera is looking in the direction of the model
    if (glm::dot(front, cameraToModelNormalized) > dotProductThreshold) {
        return true; // Model is in view, don't rotate
    }

    return false; // Model is not in view, can rotate
}
