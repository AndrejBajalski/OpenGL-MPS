//
// Created by admin on 7/22/2025.
//

#ifndef EMPS_HPP
#define EMPS_HPP

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <Particle.hpp>
#include<ParticleType.h>

#include "Particle2d.h"

/* for two-dimensional simulation */
#define DIM                  2
#define PARTICLE_DISTANCE    0.025
#define OUTPUT_INTERVAL      20

/* for three-dimensional simulation */
// #define DIM                  3
// #define PARTICLE_DISTANCE    0.042
// #define DT                   0.002
// #define OUTPUT_INTERVAL      10

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

#define FIRE_LEFT (-0.3f)
#define FIRE_RIGHT 0.3f
#define FIRE_BOTTOM (-0.5f)
#define FIRE_TOP (0.5f)

class EmpsSingleton {
private:
  static EmpsSingleton *instance;
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
  double N0_forLaplacian;
  double Lambda;
  double collisionDistance,collisionDistance2;
  double FluidDensity;
  Particle *particles;
  double DT;

  static EmpsSingleton* getInstance() {
    if (instance==nullptr) {
      instance = new EmpsSingleton();
    }
    return instance;
  }
  EmpsSingleton(const EmpsSingleton& obj) = delete;
  int simulate( void ) {

    printf("\n*** START EMPS-SIMULATION ***\n");
    // calculateConstantParameter();
    mainLoopOfSimulation();
    printf("*** END ***\n\n");
    return 0;

  }

  void calculateConstantParameter(int nX, int nY, int nZ ){
    Re_forParticleNumberDensity  = RADIUS_FOR_NUMBER_DENSITY;
    Re_forGradient       = RADIUS_FOR_GRADIENT;
    Re_forLaplacian      = RADIUS_FOR_LAPLACIAN;
    Re2_forParticleNumberDensity = Re_forParticleNumberDensity*Re_forParticleNumberDensity;
    Re2_forGradient      = Re_forGradient*Re_forGradient;
    Re2_forLaplacian     = Re_forLaplacian*Re_forLaplacian;
    calculateNZeroAndLambda(nX, nY, nZ);
    FluidDensity       = FLUID_DENSITY;
    collisionDistance  = COLLISION_DISTANCE;
    collisionDistance2 = collisionDistance*collisionDistance;
    FileNumber=0;
    Time=0.0;
  }


  void calculateNZeroAndLambda(int nX, int nY, int nZ) {
    int iX, iY, iZ;
    int iZ_start, iZ_end;
    double xj, yj, zj, distance, distance2;
    double xi, yi, zi;
    N0_forParticleNumberDensity = 0.0;
    N0_forGradient      = 0.0;
    N0_forLaplacian     = 0.0;
    Lambda              = 0.0;
    xi = 0.0;  yi = 0.0;  zi = 0.0;
    for(iX=0;iX<nX;iX++){
      for(iY=0;iY<nY;iY++){
        for(iZ=0;iZ<nZ;iZ++){
          // if( ((iX==0)&&(iY==0)) && (iZ==0) )continue;
          double xj = FIRE_LEFT + iX * PARTICLE_DISTANCE;
          double yj = FIRE_BOTTOM + iY * PARTICLE_DISTANCE;
          double zj = 0.0f;
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

  void calculatePressure_forExplicitMPS(int iParticle){
    double n0;
    double ni;
    double c,c2;
    double rho = FLUID_DENSITY;

    c  = SPEED_OF_SOUND;
    c2 = c * c;
    n0 = N0_forParticleNumberDensity;

    if ( (particleType[iParticle]!=ParticleType::FIRE) ){
      Pressure[iParticle] = 0.0;
    }else{
      ni  = ParticleNumberDensity[iParticle];
      if( ni > n0 ){
        Pressure[iParticle] =  c2 * rho * (ni - n0) / n0;
      }else{
        Pressure[iParticle] =  0.0;
      }
    }
  }

void calculateParticleNumberDensity(int i){
    double xij, yij, zij;
    double distance, distance2;
    double w;
    ParticleNumberDensity[i] = 0.0;
    for(int j=0;j<NumberOfParticles;j++){
      if( (j==i) || (particleType[j]==ParticleType::GHOST) ) continue;
      xij = PositionX[j] - PositionX[i];
      yij = PositionY[j] - PositionY[i];
      zij = PositionZ[j] - PositionZ[i];
      distance2 = (xij*xij) + (yij*yij) + (zij*zij);
      distance = sqrt(distance2);
      w =  weight(distance, Re_forParticleNumberDensity);
      ParticleNumberDensity[i] += w;
    }
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
      if( particleType[j]==ParticleType::GHOST ) continue;
      if( particleType[j]==ParticleType::DUMMY_WALL ) continue;
      xij = PositionX[j] - PositionX[i];
      yij = PositionY[j] - PositionY[i];
      zij = PositionZ[j] - PositionZ[i];
      distance2 = (xij*xij) + (yij*yij) + (zij*zij);
      distance = sqrt(distance2);
      if(distance<Re_forGradient){
        w =  weight(distance, Re_forGradient);
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


  void moveParticleUsingPressureGradient(Particle2d &particle, int i){
    if(particle.particleType == ParticleType::FIRE){
      double accelerationX = AccelerationX[i];
      double accelerationY = AccelerationY[i];
      double accelerationZ = AccelerationZ[i];

      VelocityX[i] +=accelerationX*DT;
      VelocityY[i] +=accelerationY*DT;
      VelocityZ[i] +=accelerationZ*DT;

      PositionX[i] +=accelerationX*DT*DT;
      PositionY[i] +=accelerationY*DT*DT;
      PositionZ[i] +=accelerationZ*DT*DT;
    }
    // AccelerationX[i]=0.0;
    // AccelerationY[i]=0.0;
    // AccelerationZ[i]=0.0;

    particle.velocity.x = VelocityX[i];
    particle.velocity.y = VelocityY[i];
    particle.velocity.z = VelocityZ[i];
    particle.acceleration.x = AccelerationX[i];
    particle.acceleration.y = AccelerationY[i];
    particle.acceleration.z = AccelerationZ[i];
    particle.position.x = PositionX[i];
    particle.position.y = PositionY[i];
    particle.position.z = PositionZ[i];
  }
};
#endif //EMPS_HPP
