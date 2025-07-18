//
// Created by admin on 7/17/2025.
//
#include <OpenGLPrj.hpp>

#include <GLFW/glfw3.h>

#include <Camera.hpp>
#include <Shader.hpp>

#include <iostream>
#include <string>
#include <vector>

/*=====================================================================
  emps.c

    Sample program of the explicit MPS method

    Moving Particle Semi-implicit Method, Academic Press, 2018
    ISBN: 9780128127797

=======================================================================*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "Particle.hpp"
#define PI 3.14159265359f
/* for two-dimensional simulation */
// #define DIM                  2
// #define PARTICLE_DISTANCE    0.025
// #define DT                   0.001
// #define OUTPUT_INTERVAL      20

/* for three-dimensional simulation */
#define DIM                  3
#define PARTICLE_DISTANCE    0.1
#define DT                   0.002
#define OUTPUT_INTERVAL      10

#define ARRAY_SIZE           5000
#define FINISH_TIME          1.5
#define KINEMATIC_VISCOSITY  (1.0E-6)
#define SPEED_OF_SOUND       10.0
#define FLUID_DENSITY        1000.0
#define GRAVITY_X  0.0
#define GRAVITY_Y  -9.8
#define GRAVITY_Z  0.0
#define RADIUS_FOR_NUMBER_DENSITY  (2.1*PARTICLE_DISTANCE)
#define RADIUS_FOR_GRADIENT        (2.1*PARTICLE_DISTANCE)
#define RADIUS_FOR_LAPLACIAN       (2.1*PARTICLE_DISTANCE)
#define COLLISION_DISTANCE         (0.5*PARTICLE_DISTANCE)
#define EPS                        (0.01 * PARTICLE_DISTANCE)
#define COEFFICIENT_OF_RESTITUTION 0.2
#define ON              1
#define OFF             0

std::vector<Particle> particles;
std::vector<double> particlePositions;

int    FileNumber;
double Time;
int    NumberOfParticles;
double Re_forParticleNumberDensity,Re2_forParticleNumberDensity;
double Re_forGradient,     Re2_forGradient;
double Re_forLaplacian,    Re2_forLaplacian;
double N0_forParticleNumberDensity;
double N0_forGradient;
double N0_forLaplacian;
double Lambda;
double collisionDistance,collisionDistance2;
double FluidDensity;

void initializeParticlePositionAndVelocity_for2dim( void );
void initializeParticlePositionAndVelocity_for3dim( void );
void calculateConstantParameter( void );
void calculateNZeroAndLambda( void );
double weight( double distance, double re );
void mainLoopOfSimulation( void );
void calculateGravity( void );
void calculateViscosity( void );
void moveParticle( void );
void collision( void );
void calculatePressure_forExplicitMPS( void );
void calculateParticleNumberDensity( void );
void calculatePressureGradient_forExplicitMPS( void );
void moveParticleUsingPressureGradient( void );
const std::string program_name = ("Colour");
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void illuminate(glm::vec3 lightColor, const Shader &lightingShader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);


// settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 800;
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
  initializeParticlePositionAndVelocity_for3dim();
  // calculateConstantParameter();
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

  // build and compile our shader zprogram
  // ------------------------------------
  Shader particleShader(
      shader_location + material_shader + std::string(".vert"),
      shader_location + material_shader + std::string(".frag"));


  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  // first, configure the cube's VAO (and VBO)
  unsigned int sphereVBO, sphereVAO, EBO;
  // second, configure the light's VAO (VBO stays the same; the vertices are the
  // same for the light object which is also a 3D cube)

  //sphere VAO
  glGenBuffers(1, &sphereVBO);
  glGenBuffers(1, &EBO);
  glGenVertexArrays(1, &sphereVAO);
  //VBO
  glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particles[338].sphere_vertices.size(), particles[338].sphere_vertices.data(), GL_STATIC_DRAW);
  //EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, particles[338].indices.size() * sizeof(int), particles[338].indices.data(), GL_STATIC_DRAW);
  //VAO
  glBindVertexArray(sphereVAO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);
  //normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // render loop
  // -----------
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  double startTimeFrame = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
  /*---------------------------GAME LOOP----------------------------*/
    double currentFrame = glfwGetTime();
    lightPos.x = glm::sin(currentFrame)*3;
    // input
    // -----
    processInput(window);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // light properties
    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    particleShader.use();
    particleShader.setMat4("projection", projection);
    particleShader.setMat4("view", view);
    particleShader.setInt("particleType", 0);
    glm::mat4 model = glm::mat4(1.0f);
    Particle particle = particles[0];
    model = glm::translate(model, glm::vec3(particle.PositionX, particle.PositionY, particle.PositionZ));
    particleShader.use();
    particleShader.setMat4("model", model);
    printf(" Position: %.4f, %.4f, %.4f\n", particle.PositionX, particle.PositionY, particle.PositionZ);
    printf("Camera position: %.4f, %.4f, %.4f", camera.Position.x, camera.Position.y, camera.Position.z);
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int) particles[0].indices.size(), GL_UNSIGNED_INT, particles[0].indices.data());
    //sphere
    // for (int i=0; i<particles.size(); i++) {
    //   glm::mat4 model = glm::mat4(1.0f);
    //   Particle particle = particles[i];
    //   model = glm::translate(model, glm::vec3(particle.PositionX, particle.PositionY, particle.PositionZ));
    //   particleShader.use();
    //   particleShader.setMat4("model", model);
    //   //coloring
    //   int type = particle.type;
    //   particleShader.setInt("particleType", type);
    //   std::cout<<"Particle Number "<<i<<" Particle Type: "<<particle.type;
    //   printf(" Position: %.4f, %.4f, %.4f\n", particle.PositionX, particle.PositionY, particle.PositionZ);
    //   //render the sphere
    //   glBindVertexArray(sphereVAO);
    //   glDrawElements(GL_TRIANGLES, (unsigned int) particles[0].indices.size(), GL_UNSIGNED_INT, particles[0].indices.data());
    // }
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, sectorCount*stackCount);
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    //   break;
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
    // if (currentFrame - startTimeFrame > 5.0)
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &sphereVAO);
  glDeleteBuffers(1, &sphereVBO);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

void initializeParticleSettings() {

}

/*--------------------------------------EMPS IMPLEMENTATION-------------------------------------------------*/
void initializeParticlePositionAndVelocity_for3dim( void ){
  Particle particle = Particle(8, 8, 0.005, PARTICLE_DISTANCE);
  int iX, iY, iZ;
  int nX, nY, nZ;
  double x, y, z;
  int i = 0;
  int flagOfParticleGeneration;

  nX = (int)(0.9/PARTICLE_DISTANCE);
  nY = (int)(0.7/PARTICLE_DISTANCE);
  nZ = (int)(0.5/PARTICLE_DISTANCE);
  for(iX= -nX;iX<nX;iX++){
    for(iY= -nY;iY<nY;iY++){
      for(iZ= -nZ;iZ<nZ;iZ++){
        x = PARTICLE_DISTANCE * iX;
        y = PARTICLE_DISTANCE * iY;
        z = PARTICLE_DISTANCE * iZ;
        flagOfParticleGeneration = OFF;
        int type = 999;

        /* dummy wall region */
        if( (((x>-0.9)&&(x<=0.9))&&( (y>-0.7&&y<=0.7)))&&( (z>-0.5)&&(z<=0.5 ))){
          type = DUMMY_WALL;
          flagOfParticleGeneration = OFF;
        }
        if( (((x>-0.8&&x<-0.6)||(x>0.6&&x<0.8))&&( (y>-0.6 )&&(y<=0.6)))&&( (z>-0.5)&&(z<=0.5))){
          type = WALL;
          flagOfParticleGeneration = ON;
        }
        /* wall region */
        // if( (((x>-16.0*PARTICLE_DISTANCE+EPS)&&(x<=16*PARTICLE_DISTANCE+EPS))&&( (y>0.6-2.0*PARTICLE_DISTANCE+EPS )&&(y<=0.6+EPS)))&&( (z>0.0-4.0*PARTICLE_DISTANCE+EPS)&&(z<=0.3+4.0*PARTICLE_DISTANCE+EPS ))){
        //   particle.setParticleType(WALL);
        //   flagOfParticleGeneration = ON;
        // }
        /* empty region */
        // if( (((x>0.0+EPS)&&(x<=1.00+EPS))&&( y>0.0+EPS ))&&( (z>0.0+EPS )&&(z<=0.3+EPS ))){
        //   flagOfParticleGeneration = OFF;
        // }
        /* fluid region */
        if( (((x>-0.6)&&(x<=0.6))&&( (y>-0.5)&&(y<0.5) ))&&( (z>0.0)&&(z<=0.3))){
          type = FLUID;
          flagOfParticleGeneration = ON;
        }
        if( flagOfParticleGeneration == ON ){
          particle.setParticleType(static_cast<PARTICLE_TYPE>(type));
          particle.setPosition(x, y, z);
          particle.setVelocity(0.0, 0.0, 0.0);
          particles.push_back(particle);
          i++;
        }
      }
    }
  }
  NumberOfParticles = i;
  // for(i=0;i<particles.size();i++) { particle.VelocityX=0.0; particle.VelocityY=0.0; particle.VelocityZ=0.0; }
}

void calculateConstantParameter( void ){

  Re_forParticleNumberDensity  = RADIUS_FOR_NUMBER_DENSITY;
  Re_forGradient       = RADIUS_FOR_GRADIENT;
  Re_forLaplacian      = RADIUS_FOR_LAPLACIAN;
  Re2_forParticleNumberDensity = Re_forParticleNumberDensity*Re_forParticleNumberDensity;
  Re2_forGradient      = Re_forGradient*Re_forGradient;
  Re2_forLaplacian     = Re_forLaplacian*Re_forLaplacian;
  calculateNZeroAndLambda();
  FluidDensity       = FLUID_DENSITY;
  collisionDistance  = COLLISION_DISTANCE;
  collisionDistance2 = collisionDistance*collisionDistance;
  FileNumber=0;
  Time=0.0;
}

void calculateNZeroAndLambda( void ){
  int iX, iY, iZ;
  int iZ_start, iZ_end;
  double xj, yj, zj, distance, distance2;
  double xi, yi, zi;

  if( DIM == 2 ){
    iZ_start = 0; iZ_end = 1;
  }else{
    iZ_start = -4; iZ_end = 5;
  }

  N0_forParticleNumberDensity = 0.0;
  N0_forGradient      = 0.0;
  N0_forLaplacian     = 0.0;
  Lambda              = 0.0;
  xi = 0.0;  yi = 0.0;  zi = 0.0;

  for(iX= -4;iX<5;iX++){
    for(iY= -4;iY<5;iY++){
      for(iZ= iZ_start;iZ<iZ_end;iZ++){
        if( ((iX==0)&&(iY==0)) && (iZ==0) )continue;
        xj = PARTICLE_DISTANCE * (double)(iX);
        yj = PARTICLE_DISTANCE * (double)(iY);
        zj = PARTICLE_DISTANCE * (double)(iZ);
        distance2 = (xj-xi)*(xj-xi)+(yj-yi)*(yj-yi)+(zj-zi)*(zj-zi);
        distance = sqrt(distance2);
        N0_forParticleNumberDensity += weight(distance, Re_forParticleNumberDensity);
        N0_forGradient      += weight(distance, Re_forGradient);
        N0_forLaplacian     += weight(distance, Re_forLaplacian);
        Lambda              += distance2 * weight(distance, Re_forLaplacian);
      }
    }
  }
  Lambda = Lambda/N0_forLaplacian;
}


double weight( double distance, double re ){
  double weightIJ;

  if( distance >= re ){
    weightIJ = 0.0;
  }else{
    weightIJ = (re/distance) - 1.0;
  }
  return weightIJ;
}





/*--------------------------------------END OF EMPS IMPLEMENTATION METHODS----------------------------------*/



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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}