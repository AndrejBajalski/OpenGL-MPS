#include <OpenGLPrj.hpp>
#include <GLFW/glfw3.h>
#include <Camera.hpp>
#include <Shader.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "emps.hpp"
#include <ParticleType.h>
#include <thread>

#include "Particle.hpp"
#include "PointParticleGenerator.h"

#define PI 3.14159265359f
#define DT 0.01f

const std::string program_name = ("EMPS method");

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
// void mouse_callback(GLFWwindow *window, double xpos, double ypos);
// void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void illuminate(glm::vec3 lightColor, const Shader &lightingShader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
void generateInstanceBuffers(int n, unsigned int VAO);
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
  // glfwSetCursorPosCallback(window, mouse_callback);
  // glfwSetScrollCallback(window, scroll_callback);

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
  std::string material_shader("material");
  std::string lamp_shader("lamp");

  // build and compile our shader zprogram
  // ------------------------------------
  Shader lightingShader(
      shader_location + material_shader + std::string(".vert"),
      shader_location + material_shader + std::string(".frag"));
  // Shader lampShader(shader_location + lamp_shader + std::string(".vert"),
  //                   shader_location + lamp_shader + std::string(".frag"));

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  PointParticleGenerator generator = PointParticleGenerator(DT, lightingShader);
  generator.init(DT);
//bug check
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL Error after setup: " << err << std::endl;
  }
//game loop
//----------------------------------------------------------------------------------------------------
  lightingShader.use();
  double lastTime = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    // per-frame time logic
    // --------------------
    double currentTime = glfwGetTime();
    lightPos.x = glm::sin((float)currentTime)*3;
    processInput(window);
    glClearColor(.9f, .93f, .9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // projections
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);
    // manually adjust fps
    double deltaTime = currentTime - lastTime;
    if (deltaTime < DT) {
      double sleepingTime = DT - deltaTime;
      std::this_thread::sleep_for(std::chrono::duration<double>(sleepingTime));
    }
    // update
    generator.update();
    // draw
    generator.draw();
    currentTime = glfwGetTime();
    lastTime = currentTime;
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
//end of game loop
//----------------------------------------------------------------------------------------------------------



//method definitions
//----------------------------------------------------------------------------------------------------------
void illuminate(glm::vec3 lightColor, const Shader &lightingShader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess) {
  // material properties
  // lightingShader.setVec3("material.ambient", ambient);
  // lightingShader.setVec3("material.diffuse", diffuse);
  // lightingShader.setVec3("material.specular", specular);
  // specular lighting doesn't have full effect on this object's material
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
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
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
// void mouse_callback(GLFWwindow *window, double xposd, double yposd) {
//   float xpos = static_cast<float>(xposd);
//   float ypos = static_cast<float>(yposd);
//   if (firstMouse) {
//     lastX = xpos;
//     lastY = ypos;
//     firstMouse = false;
//   }
//
//   float xoffset = xpos - lastX;
//   float yoffset =
//       lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//   lastX = xpos;
//   lastY = ypos;
//
//   camera.ProcessMouseMovement(xoffset, yoffset);
// }

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
// void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
//   camera.ProcessMouseScroll(static_cast<float>(yoffset));
// }