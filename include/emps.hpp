//
// Created by admin on 7/22/2025.
//

#ifndef EMPS_HPP
#define EMPS_HPP

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <Particle.hpp>
#include<ParticleType.h>

#include "Particle2d.h"

/* for two-dimensional simulation */
#define DIM                  2
#define OUTPUT_INTERVAL      20
/* for three-dimensional simulation */
// #define DIM                  3
// #define DT                   0.002
// #define OUTPUT_INTERVAL      10  v
#define ARRAY_SIZE           10000
#define FINISH_TIME          1.5
#define KINEMATIC_VISCOSITY  (1.0E-6)
#define SPEED_OF_SOUND       4.766
#define FLUID_DENSITY        0.3
#define GRAVITY_X  0.0
#define GRAVITY_Y  -9.8
#define GRAVITY_Z  0.0
#define COLLISION_DISTANCE         (0.5*PARTICLE_DISTANCE)
#define COEFFICIENT_OF_RESTITUTION 0.2
#define ON              1
#define OFF             0

class EmpsSingleton {
private:
  static EmpsSingleton *instance;
  float FIRE_TOP, FIRE_BOTTOM, FIRE_LEFT, FIRE_RIGHT, PARTICLE_DISTANCE, PARTICLE_RADIUS;
  EmpsSingleton(){}
public:
  double AccelerationX[ARRAY_SIZE];
  double AccelerationY[ARRAY_SIZE];
  double AccelerationZ[ARRAY_SIZE];
  ParticleType particleType[ARRAY_SIZE];
  double PositionX[ARRAY_SIZE];
  double PositionY[ARRAY_SIZE];
  double PositionZ[ARRAY_SIZE];
  double VelocityX[ARRAY_SIZE];
  double VelocityY[ARRAY_SIZE];
  double VelocityZ[ARRAY_SIZE];
  double Pressure[ARRAY_SIZE];
  double ParticleNumberDensity[ARRAY_SIZE];
  int    BoundaryCondition[ARRAY_SIZE];
  double SourceTerm[ARRAY_SIZE];
  int    FlagForCheckingBoundaryCondition[ARRAY_SIZE];
  double CoefficientMatrix[ARRAY_SIZE * ARRAY_SIZE];
  double MinimumPressure[ARRAY_SIZE];
  int    FileNumber;
  double Time;
  int    NumberOfParticles;
  double Re_forParticleNumberDensity,Re2_forParticleNumberDensity;
  double Re_forGradient,     Re2_forGradient;
  double Re_forLaplacian,    Re2_forLaplacian;
  double N0_forParticleNumberDensity;
  double N0_forGradient;
  double Lambda;
  double collisionDistance,collisionDistance2;
  double FluidDensity;
  Particle *particles;
  double DT;

  static EmpsSingleton* getInstance(float FIRE_TOP, float FIRE_BOTTOM, float FIRE_LEFT, float FIRE_RIGHT, float PARTICLE_DISTANCE, float PARTICLE_RADIUS) {
    if (instance==nullptr) {
      instance = new EmpsSingleton(FIRE_TOP, FIRE_BOTTOM, FIRE_LEFT, FIRE_RIGHT, PARTICLE_DISTANCE, PARTICLE_RADIUS);
    }
    return instance;
  }

  EmpsSingleton(float FIRE_TOP, float FIRE_BOTTOM, float FIRE_LEFT, float FIRE_RIGHT, float PARTICLE_DISTANCE, float PARTICLE_RADIUS) {
    this->FIRE_TOP = FIRE_TOP; this->FIRE_BOTTOM = FIRE_BOTTOM; this->FIRE_LEFT = FIRE_LEFT; this->FIRE_RIGHT = FIRE_RIGHT;
    this->PARTICLE_DISTANCE = PARTICLE_DISTANCE;
    this->PARTICLE_RADIUS = PARTICLE_RADIUS;
  }

  EmpsSingleton(const EmpsSingleton& obj) = delete;

  int simulate( void ) {
    printf("\n*** START EMPS-SIMULATION ***\n");
    // calculateConstantParameter();
    mainLoopOfSimulation();
    printf("*** END ***\n\n");
    return 0;
  }

  void calculateConstantParameter(int index){
    Re_forParticleNumberDensity  = 3.0 * PARTICLE_DISTANCE;
    Re_forGradient       = 3.0 * PARTICLE_DISTANCE;
    Re_forLaplacian      = 3.1 * PARTICLE_DISTANCE;
    Re2_forParticleNumberDensity = Re_forParticleNumberDensity*Re_forParticleNumberDensity;
    Re2_forGradient      = Re_forGradient*Re_forGradient;
    Re2_forLaplacian     = Re_forLaplacian*Re_forLaplacian;
    calculateNZeroAndLambda(index);
    FluidDensity       = FLUID_DENSITY;
    collisionDistance  = COLLISION_DISTANCE;
    collisionDistance2 = collisionDistance*collisionDistance;
    FileNumber=0;
    Time=0.0;
  }

  void calculateNZeroAndLambda(int index) {
    double xj, yj, zj, distance, distance2;
    double xi, yi, zi;
    N0_forParticleNumberDensity = 0.0;
    N0_forGradient      = 0.0;
    double pndTemp=0.0, gradTemp=0.0;
    double minDistance = 10.0;
    int neighborsCount=0;
    xi = PositionX[index];  yi = PositionY[index];  zi = 0.0;
    std::cout<<"Neighbors: "<<std::endl;
    for(int i=0; i<NumberOfParticles; i++) {
        xj = PositionX[i];
        yj = PositionY[i];
        zj = 0.0f;
        distance2 = (xj-xi)*(xj-xi)+(yj-yi)*(yj-yi)+(zj-zi)*(zj-zi);
        distance = sqrt(distance2);
        if (distance!=0.0 && distance<minDistance)
          minDistance = distance;
        pndTemp = weight(distance, Re_forParticleNumberDensity);
        gradTemp = weightGrad(distance, Re_forGradient);
        if(pndTemp!=0.0) {
          neighborsCount++;
          std::cout<<i<<", Weight: "<<pndTemp<<std::endl;;
        }
        N0_forParticleNumberDensity += pndTemp;
        N0_forGradient += gradTemp;
    }
    std::cout<<"N0_forParticleNumberDensity = "<<N0_forParticleNumberDensity<<std::endl;
    std::cout<<"N0_forGradient = "<<N0_forGradient<<std::endl;
  }

  double weight( double distance, double re ){
    double weightIJ;
    if( distance >= re || distance==0.0){
      weightIJ = 0.0;
    }else{
      weightIJ = (re/distance) - 1.0;
    }
    return weightIJ;
  }

  double weightGrad( double distance, double re ) {
    double weightIJ;
    if( distance>=re || distance==0.0){
      weightIJ = 0.0;
    }    else{
      weightIJ = (re/distance) - (distance/re);
    }
    return weightIJ;
  }

  void mainLoopOfSimulation( void ){
    int iTimeStep = 0;
    while(1){
      // calculateGravity();
      // calculateViscosity();
      // moveParticle();
      // collision();
      // calculateParticleNumberDensity();
      // calculatePressure_forExplicitMPS();
      // calculatePressureGradient_forExplicitMPS();
      // moveParticleUsingPressureGradient();
      iTimeStep++;
      Time += DT;
      if( (iTimeStep % OUTPUT_INTERVAL) == 0 ){
        printf("TimeStepNumber: %4d   Time: %lf(s)   NumberOfParticles: %d\n", iTimeStep, Time, NumberOfParticles);
      }
      if( Time >= FINISH_TIME ){break;}
    }
  }

double calculateParticleNumberDensity(int i){
    double xij, yij, zij;
    double distance, distance2;
    double w;
    ParticleNumberDensity[i] = 0.0;
    for(int j=0;j<NumberOfParticles;j++){
      if (i==j)
        continue;
      xij = PositionX[j] - PositionX[i];
      yij = PositionY[j] - PositionY[i];
      zij = PositionZ[j] - PositionZ[i];
      distance2 = (xij*xij) + (yij*yij) + (zij*zij);
      distance = sqrt(distance2);
      w =  weight(distance, Re_forParticleNumberDensity);
      ParticleNumberDensity[i] += w;
    }
    return ParticleNumberDensity[i];
}

  double calculatePressure_forExplicitMPS(int iParticle){
    double n0;
    double ni;
    double c,c2;
    double rho = FLUID_DENSITY;
    c  = SPEED_OF_SOUND;
    c2 = c * c;
    n0 = N0_forParticleNumberDensity;
    // if ( (particleType[iParticle]!=ParticleType::FIRE) ){
    //   Pressure[iParticle] = 0.0;
    // }else{
      ni  = ParticleNumberDensity[iParticle];
       if( ni > n0 ){
        Pressure[iParticle] =  c2 * rho * (ni - n0) / n0;
      }else{
        Pressure[iParticle] =  0.0;
      // }
    }
    return Pressure[iParticle];
  }

  void calculatePressureGradient_forExplicitMPS(int i){
    double gradientX, gradientY, gradientZ;
    double xij, yij, zij;
    double distance, distance2;
    double w,pij;
    double a;
    a =DIM/N0_forGradient;
    if(particleType[i] != ParticleType::FIRE) return;
    gradientX = 0.0;  gradientY = 0.0;  gradientZ = 0.0;
    for(int j=0;j<NumberOfParticles;j++){
      if( j==i ) continue;
      xij = PositionX[j] - PositionX[i];
      yij = PositionY[j] - PositionY[i];
      zij = PositionZ[j] - PositionZ[i];
      distance2 = (xij*xij) + (yij*yij) + (zij*zij);
      distance = sqrt(distance2);
      if(distance<Re_forGradient){
        w =  weightGrad(distance, Re_forGradient);
        pij = (Pressure[j] + Pressure[i])/distance2;
        gradientX += xij*pij*w;
        gradientY += yij*pij*w;
        gradientZ += zij*pij*w;
      }
      gradientX *= a;
      gradientY *= a;
      gradientZ *= a;
      AccelerationX[i]= (-1.0)*gradientX/FluidDensity;
      AccelerationY[i]= (-1.0)*gradientY/FluidDensity;
      AccelerationZ[i]= (-1.0)*gradientZ/FluidDensity;
    }
  }

  void moveParticleUsingPressureGradient(Particle2d &particle, int i) {
    if(particle.particleType == ParticleType::FIRE || particle.particleType == ParticleType::AIR){
      glm::vec3 acceleration = glm::vec3(AccelerationX[i], AccelerationY[i], AccelerationZ[i]);
      glm::vec3 velocity_diff = acceleration * glm::vec3(static_cast<float>(DT));
      glm::vec3 position_diff = velocity_diff * glm::vec3(static_cast<float>(DT));

      VelocityX[i] += velocity_diff.x;
      VelocityY[i] += velocity_diff.y;
      VelocityZ[i] += velocity_diff.z;
      PositionX[i] += position_diff.x;
      PositionY[i] += position_diff.y;
      PositionZ[i] += position_diff.z;

      particle.velocity += velocity_diff;
      particle.position += position_diff;
      particle.acceleration = glm::vec3(0.0f);

      AccelerationX[i]=0.0;
      AccelerationY[i]=0.0;
      AccelerationZ[i]=0.0;
    }
  }
};
#endif //EMPS_HPP
