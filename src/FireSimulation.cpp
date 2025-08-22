#include <OpenGLPrj.hpp>
#include <GLFW/glfw3.h>
#include <Camera.hpp>
#include <Shader.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "emps.hpp"
#include <ParticleType.h>
#include <stb_image.h>
#include <thread>
#include "sphere.hpp"
#include "PointParticleGenerator.h"
#define PI 3.14159265359f
#define DT 0.01f

const std::string program_name = ("EMPS method");

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void illuminate(Shader &objectShader, glm::vec3 ambient, int diffuse, glm::vec3 specular, float shininess);
void updateLight(Shader &objectShader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
void generateBuffers(unsigned int *VBO, unsigned int *VAO, std::vector<float> data);
void drawEnvironment(unsigned int VAO);
void generateTextures(unsigned int *texture, const std::string &path);
void illuminateFloor(Shader &objectShader);
void cleanup();

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1000;
// camera
static Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
static float lastX = SCR_WIDTH / 2.0f;
static float lastY = SCR_HEIGHT / 2.0f;
static bool firstMouse = true;
// timing
static float deltaTime = 0.0f;
// lighting
static glm::vec3 lightPos(0.0f, 0.5f, 1.0f);
glm::mat4 *instanceTransformations;
glm::vec3 *instanceColors;
//general
unsigned int VBO, VAO, floorTexture;

int main() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(
      GLFW_OPENGL_FORWARD_COMPAT,
      GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

  // glfw window creation
  // --------------------
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                        program_name.c_str(), nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile our shader program
  // ------------------------------------
  std::string shader_location("../res/shaders/");
  std::string fire_shader("fire");
  std::string object_shader("object");

  // build and compile our shader zprogram
  // ------------------------------------
  Shader fireShader(
      shader_location + fire_shader + std::string(".vert"),
      shader_location + fire_shader + std::string(".frag"));
  Shader objectShader(shader_location + object_shader + std::string(".vert"),
                    shader_location + object_shader + std::string(".frag"));

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  //BLENDING PROPERTIES
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_ONE, GL_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //SET UP ENVIRONMENT
  std::vector<float> floorVertices = {
    //position                //normal           //tex coord
    -100.0f, 3.0f, -100.0f,   0.0f, 1.0f, 0.0f,    0.0f, 100.0f, //top left
    100.0f, 3.0f, -100.0f,    0.0f, 1.0f, 0.0f,    100.0f, 100.0f, //top right
    -100.0f, -5.0, 100.0f,     0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  //bottom left
    100.0f, -5.0f, 100.0f,     0.0f, 1.0f, 0.0f,    100.0f, 0.0f//bottom right
  };
  generateBuffers(&VBO, &VAO, floorVertices);
  generateTextures(&floorTexture, "../res/textures/checkerboard.jpg");
  //GENERATE PARTICLES
  PointParticleGenerator generator = PointParticleGenerator(DT, fireShader, objectShader);
  generator.init(DT);
  //GENERATE AN OBJECT
  Sphere sphere = Sphere(64, 64, 0.5f);
//bug check
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL Error after setup: " << err << std::endl;
  }
  //SET UP PROJECTION AND VIEW MATRICES
  glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),0.1f, 100.0f);
  //fire shader static uniforms
  float particleRadius = PointParticleGenerator::P_RADIUS;
  fireShader.use();
  fireShader.setMat4("projection", projection);
  fireShader.setFloat("billboardSize", particleRadius);
  //object shader static uniforms
  objectShader.use();
  objectShader.setMat4("projection", projection);
  //light color properties
  glm::vec3 light_ambient = glm::vec3(0.2f);
  glm::vec3 light_diffuse = glm::vec3(0.5f);
  glm::vec3 light_specular = glm::vec3(1.0f, 1.0f, 1.0f);
  updateLight(objectShader, light_ambient, light_diffuse, light_specular);
  double lastTime = glfwGetTime();
  //----------------------------------------GAME LOOP-------------------------------------------------
  //----------------------------------------------------------------------------------------------------
  while (!glfwWindowShouldClose(window)) {
    // per-frame time logic
    // --------------------
    double currentTime = glfwGetTime();
    lightPos.x = glm::sin((float)currentTime)*3;
    processInput(window);
    glClearColor(.0f, .0f, .0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //transform fire to world space
    glm::mat4 fireModel = glm::mat4(1.0f);
    fireShader.use();
    fireShader.setMat4("model", fireModel);
    //transform fire to view space
    glm::mat4 view = camera.GetViewMatrix();
    fireShader.setMat4("view", view);
    // manually adjust fps
    deltaTime = currentTime - lastTime;
    if (deltaTime < DT) {
      double sleepingTime = DT - deltaTime;
      std::this_thread::sleep_for(std::chrono::duration<double>(sleepingTime));
    }
    // update
    generator.update();
    // draw fire
    generator.draw();
    currentTime = glfwGetTime();
    lastTime = currentTime;
    //draw floor
    glm::mat4 objectModel = glm::mat4(1.0f);
    // objectModel = glm::scale(objectModel, glm::vec3(100.0f, 0.0f, 100.0f));
    objectShader.use();
    objectShader.setMat4("model", objectModel);
    objectShader.setMat4("view", view);
    illuminateFloor(objectShader);
    drawEnvironment(VAO);
    // // draw object
    // objectModel = glm::translate(objectModel, glm::vec3(0.73f, -0.78f, 0.0f));
    // objectShader.use();
    // objectShader.setMat4("model", objectModel);
    // objectShader.setMat4("view", view);
    // sphere.draw();
    // end of game loop
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  generator.cleanup();
  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}
//-------------------------------------------END OF GAME LOOP-----------------------------------------------
//----------------------------------------------------------------------------------------------------------

//method definitions
//----------------------------------------------------------------------------------------------------------
void illuminate(Shader &objectShader, glm::vec3 ambient, int diffuse, glm::vec3 specular, float shininess) {
  objectShader.use();
  objectShader.setVec3("material.ambient", ambient);
  objectShader.setInt("material.diffuse", diffuse);
  objectShader.setVec3("material.specular", specular);
  objectShader.setFloat("material.shininess", shininess);
}
void illuminateFloor(Shader &objectShader) {
  objectShader.use();
  objectShader.setVec3("material.ambient", glm::vec3(0.1f));
  objectShader.setInt("material.diffuse", 0);
  objectShader.setVec3("material.specular", glm::vec3(0.9f));
  objectShader.setFloat("material.shininess", 16);
}
void updateLight(Shader &objectShader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
  objectShader.use();
  objectShader.setVec3("light.ambient", ambient);
  objectShader.setVec3("light.diffuse", diffuse);
  objectShader.setVec3("light.specular", specular);
}
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.ProcessKeyboard(RIGHT, deltaTime);
  }
}
// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposd, double yposd) {
  float xpos = static_cast<float>(xposd);
  float ypos = static_cast<float>(yposd);
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
void generateBuffers(unsigned int *VBO, unsigned int *VAO, std::vector<float> data) {
  glGenVertexArrays(1, VAO);
  glBindVertexArray(*VAO);
  glGenBuffers(1, VBO);
  //VBO
  glBindBuffer(GL_ARRAY_BUFFER, *VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);
  //VAO
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);
  //normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  //texture attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
}
void drawEnvironment(unsigned int VAO) {
  //draw floor
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, floorTexture);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}
void generateTextures(unsigned int *texture, const std::string &path) {
  // texture 1
  // ---------
  glGenTextures(1, texture);
  glBindTexture(GL_TEXTURE_2D, *texture);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load image, create texture and generate mipmaps
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
  if (data)
  {
    if (nrChannels>3)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    else
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
}
