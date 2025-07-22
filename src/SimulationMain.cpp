#include <OpenGLPrj.hpp>
#include <GLFW/glfw3.h>
#include <Camera.hpp>
#include <Shader.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "emps.hpp"
#include <ParticleType.h>
#include "Particle.hpp"

#define PI 3.14159265359f
#define PARTICLE_DISTANCE    0.1

const std::string program_name = ("Colour");

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void illuminate(glm::vec3 lightColor, const Shader &lightingShader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
glm::vec3 getColorForParticleType(enum ParticleType type);

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

  std::string material_shader("material");
  std::string lamp_shader("lamp");

  // build and compile our shader zprogram
  // ------------------------------------
  Shader lightingShader(
      shader_location + material_shader + std::string(".vert"),
      shader_location + material_shader + std::string(".frag"));
  Shader lampShader(shader_location + lamp_shader + std::string(".vert"),
                    shader_location + lamp_shader + std::string(".frag"));

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  //Generate sphere vertices
  int sectorCount=16, stackCount=16;
  float radius = 0.01f;
  Particle particle = Particle(sectorCount, stackCount, radius, PARTICLE_DISTANCE);
  particle.setParticleType(ParticleType::FLUID);

  // first, configure the cube's VAO (and VBO)
  unsigned int VBO, sphereVBO, sphereVAO, EBO;

  // second, configure the light's VAO (VBO stays the same; the vertices are the
  // same for the light object which is also a 3D cube)

  //sphere VAO
  glGenBuffers(1, &sphereVBO);
  glGenBuffers(1, &EBO);
  glGenVertexArrays(1, &sphereVAO);
  //VBO
  glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Particle::sphere_vertices.size(), Particle::sphere_vertices.data(), GL_STATIC_DRAW);
  //EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, Particle::indices.size() * sizeof(int), Particle::indices.data(), GL_STATIC_DRAW);
  //VAO
  glBindVertexArray(sphereVAO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(double),
                        reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);
  //normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(double),
                        reinterpret_cast<void *>(3 * sizeof(double)));
  glEnableVertexAttribArray(1);

  // render loop
  // -----------
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  while (!glfwWindowShouldClose(window)) {
    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    lightPos.x = glm::sin(currentFrame)*3;
    // input
    // -----
    processInput(window);

    // render
    // ------
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // light properties
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    //sphere
    lightingShader.use();
    lightingShader.setVec3("light.position", lightPos);
    lightingShader.setVec3("viewPos", camera.Position);
    lightingShader.setFloat("alpha", 0.4f);
    float shininess = 10.0f;
    glm::vec3 color = getColorForParticleType(particle.type);
    glm::vec3 diffuse = color;
    glm::vec3 ambient = color;
    glm::vec3 specular = color;
    illuminate(lightColor, lightingShader, ambient, diffuse, specular, shininess);
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);
    glm::mat4 model = glm::mat4(1.0f);
    for (int i=0; i<10; i++) {
      float offset = i/10.0f;
      model = glm::translate(model, glm::vec3(0.0f, offset, offset));
      lightingShader.setMat4("model", model);
      //render the sphere
      glBindVertexArray(sphereVAO);
      glDrawElements(GL_TRIANGLES, (unsigned int) Particle::indices.size(), GL_UNSIGNED_INT, Particle::indices.data());
    }
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &sphereVAO);
  glDeleteBuffers(1, &VBO);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void illuminate(glm::vec3 lightColor, const Shader &lightingShader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess) {
  glm::vec3 diffuseColor =
        lightColor * glm::vec3(0.5f); // decrease the influence
  glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
  lightingShader.setVec3("light.ambient", ambientColor);
  lightingShader.setVec3("light.diffuse", diffuseColor);
  lightingShader.setVec3("light.specular", specular);
  // material properties
  lightingShader.setVec3("material.ambient", ambient);
  lightingShader.setVec3("material.diffuse", diffuse);
  lightingShader.setVec3("material.specular", specular);
  // specular lighting doesn't have full effect on this object's material
  lightingShader.setFloat("material.shininess", shininess);
}
glm::vec3 getColorForParticleType(ParticleType type) {
  switch (type) {
    case ParticleType::GHOST: return glm::vec3(0.1f);
    case ParticleType::FLUID: return glm::vec3(0.0f, 0.0f, 1.0f);
    case ParticleType::WALL: return glm::vec3(1.0f, 1.0f, 0.0f);
    default: return glm::vec3(0.0f);
  }
  return glm::vec3(0.0f, 0.0f, 1.0f);
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
