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

/* for two-dimensional simulation */
// #define DIM                  2
// #define PARTICLE_DISTANCE    0.025
// #define DT                   0.001
// #define OUTPUT_INTERVAL      20

/* for three-dimensional simulation */
#define DIM                  3
#define PARTICLE_DISTANCE    0.042
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
// #define GHOST  -1
// #define FLUID   0
// #define WALL    2
// #define DUMMY_WALL  3
// #define GHOST_OR_DUMMY  -1


void initializeParticlePositionAndVelocity_for2dim( void );
static void initializeParticlePositionAndVelocity_for3dim( void );
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
void writeData_inProfFormat( void );
void writeData_inVtuFormat( void );
static double AccelerationX[ARRAY_SIZE];
static double AccelerationY[ARRAY_SIZE];
static double AccelerationZ[ARRAY_SIZE];
static ParticleType particleType[ARRAY_SIZE];
static double PositionX[ARRAY_SIZE];
static double PositionY[ARRAY_SIZE];
static double PositionZ[ARRAY_SIZE];
static double VelocityX[ARRAY_SIZE];
static double VelocityY[ARRAY_SIZE];
static double VelocityZ[ARRAY_SIZE];
static double Pressure[ARRAY_SIZE];
static double ParticleNumberDensity[ARRAY_SIZE];
static int    BoundaryCondition[ARRAY_SIZE];
static double SourceTerm[ARRAY_SIZE];
static int    FlagForCheckingBoundaryCondition[ARRAY_SIZE];
static double CoefficientMatrix[ARRAY_SIZE * ARRAY_SIZE];
static double MinimumPressure[ARRAY_SIZE];
static int    FileNumber;
static double Time;
static int    NumberOfParticles;
static double Re_forParticleNumberDensity,Re2_forParticleNumberDensity;
static double Re_forGradient,     Re2_forGradient;
static double Re_forLaplacian,    Re2_forLaplacian;
static double N0_forParticleNumberDensity;
static double N0_forGradient;
static double N0_forLaplacian;
static double Lambda;
static double collisionDistance,collisionDistance2;
static double FluidDensity;
static Particle particles[ARRAY_SIZE];
#endif //EMPS_HPP
