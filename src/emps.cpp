// /*=====================================================================
//   emps.c
//
//     Sample program of the explicit MPS method
//
//     Moving Particle Semi-implicit Method, Academic Press, 2018
//     ISBN: 9780128127797
//
// =======================================================================*/
#include "emps.hpp"

EmpsSingleton* EmpsSingleton::instance = nullptr;
//
// int simulate( void ) {
//
//   printf("\n*** START EMPS-SIMULATION ***\n");
//   if( DIM == 2 ){
//     initializeParticlePositionAndVelocity_for2dim();
//   }else{
//     initializeParticlePositionAndVelocity_for3dim();
//   }
//   calculateConstantParameter();
//   mainLoopOfSimulation();
//   printf("*** END ***\n\n");
//   return 0;
//
// }
//
//
// void initializeParticlePositionAndVelocity_for2dim( void ){
//
//   int iX, iY;
//   int nX, nY;
//   double x, y, z;
//   int i = 0;
//   int flagOfParticleGeneration;
//
//   nX = (int)(1.0/PARTICLE_DISTANCE)+5;
//   nY = (int)(0.6/PARTICLE_DISTANCE)+5;
//   for(iX= -4;iX<nX;iX++){
//     for(iY= -4;iY<nY;iY++){
//       x = PARTICLE_DISTANCE * (double)(iX);
//       y = PARTICLE_DISTANCE * (double)(iY);
//       z = 0.0;
//       flagOfParticleGeneration = OFF;
//
//       if( ((x>-4.0*PARTICLE_DISTANCE+EPS)&&(x<=1.00+4.0*PARTICLE_DISTANCE+EPS))&&( (y>0.0-4.0*PARTICLE_DISTANCE+EPS )&&(y<=0.6+EPS)) ){  /* dummy wall region */
// 	particleType[i]= ParticleType::DUMMY_WALL;
// 	flagOfParticleGeneration = ON;
//       }
//
//       if( ((x>-2.0*PARTICLE_DISTANCE+EPS)&&(x<=1.00+2.0*PARTICLE_DISTANCE+EPS))&&( (y>0.0-2.0*PARTICLE_DISTANCE+EPS )&&(y<=0.6+EPS)) ){ /* wall region */
// 	particleType[i]=ParticleType::WALL;
// 	flagOfParticleGeneration = ON;
//       }
//
//       if( ((x>-4.0*PARTICLE_DISTANCE+EPS)&&(x<=1.00+4.0*PARTICLE_DISTANCE+EPS))&&( (y>0.6-2.0*PARTICLE_DISTANCE+EPS )&&(y<=0.6+EPS)) ){  /* wall region */
// 	particleType[i]=ParticleType::WALL;
// 	flagOfParticleGeneration = ON;
//       }
//
//       if( ((x>0.0+EPS)&&(x<=1.00+EPS))&&( y>0.0+EPS )){  /* empty region */
// 	flagOfParticleGeneration = OFF;
//       }
//
//       if( ((x>0.0+EPS)&&(x<=0.25+EPS)) &&((y>0.0+EPS)&&(y<=0.50+EPS)) ){  /* fluid region */
// 	particleType[i]=ParticleType::FLUID;
// 	flagOfParticleGeneration = ON;
//       }
//
//       if( flagOfParticleGeneration == ON){
// 	PositionX[i]=x; PositionY[i]=y; PositionZ[i]=z;
// 	i++;
//       }
//     }
//   }
//   NumberOfParticles = i;
//   for(i=0;i<NumberOfParticles;i++) { VelocityX[i]=0.0; VelocityY[i]=0.0; VelocityZ[i]=0.0; }
// }
//
//
// void initializeParticlePositionAndVelocity_for3dim( void ){
//   Particle particle = Particle(16, 16, 0.01f, PARTICLE_DISTANCE);
//   int iX, iY, iZ;
//   int nX, nY, nZ;
//   double x, y, z;
//   int i = 0;
//   int flagOfParticleGeneration;
//
//   nX = (int)(0.9/PARTICLE_DISTANCE);
//   nY = (int)(0.7/PARTICLE_DISTANCE);
//   nZ = (int)(0.5/PARTICLE_DISTANCE);
//   int maxSize = nX*nY*nZ*8;
//   particles = new Particle[maxSize];
//   for(iX= -nX;iX<nX;iX++){
//     for(iY= -nY;iY<nY;iY++){
//       for(iZ= -nZ;iZ<nZ;iZ++){
//         x = PARTICLE_DISTANCE * iX;
//         y = PARTICLE_DISTANCE * iY;
//         z = PARTICLE_DISTANCE * iZ;
//         flagOfParticleGeneration = OFF;
//         ParticleType type = ParticleType::GHOST;
//
//         /* dummy wall region */
//         if( (((x>-0.9)&&(x<=0.9))&&( (y>-0.7&&y<=0.7)))&&( (z>-0.5)&&(z<=0.5 ))){
//           type = ParticleType::WALL;
//           flagOfParticleGeneration = OFF;
//         }
//         if( (((x>-0.8&&x<-0.6)||(x>0.6&&x<0.8))&&( (y>-0.6 )&&(y<=0.6)))&&( (z>-0.5)&&(z<=0.5))){
//           type = ParticleType::WALL;
//           flagOfParticleGeneration = ON;
//         }
//         /* wall region */
//         // if( (((x>-16.0*PARTICLE_DISTANCE+EPS)&&(x<=16*PARTICLE_DISTANCE+EPS))&&( (y>0.6-2.0*PARTICLE_DISTANCE+EPS )&&(y<=0.6+EPS)))&&( (z>0.0-4.0*PARTICLE_DISTANCE+EPS)&&(z<=0.3+4.0*PARTICLE_DISTANCE+EPS ))){
//         //   particle.setParticleType(ParticleType::WALL);
//         //   flagOfParticleGeneration = ON;
//         // }
//         /* empty region */
//         // if( (((x>0.0+EPS)&&(x<=1.00+EPS))&&( y>0.0+EPS ))&&( (z>0.0+EPS )&&(z<=0.3+EPS ))){
//         //   flagOfParticleGeneration = OFF;
//         // }
//         /* fluid region */
//         if( (((x>-0.6)&&(x<=0.6))&&( (y>-0.5)&&(y<0.5) ))&&( (z>0.0)&&(z<=0.3))){
//           type = ParticleType::FLUID;
//           flagOfParticleGeneration = ON;
//         }
//         if( flagOfParticleGeneration == ON ){
//           particle.setParticleType(type);
//           particle.setPosition(x, y, z);
//           particle.setVelocity(0.0, 0.0, 0.0);
//           particles[i] = particle;
//           i++;
//         }
//       }
//     }
//   }
//   NumberOfParticles = i;
// }
//
//
// void calculateConstantParameter( void ){
//
//   Re_forParticleNumberDensity  = RADIUS_FOR_NUMBER_DENSITY;
//   Re_forGradient       = RADIUS_FOR_GRADIENT;
//   Re_forLaplacian      = RADIUS_FOR_LAPLACIAN;
//   Re2_forParticleNumberDensity = Re_forParticleNumberDensity*Re_forParticleNumberDensity;
//   Re2_forGradient      = Re_forGradient*Re_forGradient;
//   Re2_forLaplacian     = Re_forLaplacian*Re_forLaplacian;
//   calculateNZeroAndLambda();
//   FluidDensity       = FLUID_DENSITY;
//   collisionDistance  = COLLISION_DISTANCE;
//   collisionDistance2 = collisionDistance*collisionDistance;
//   FileNumber=0;
//   Time=0.0;
// }
//
//
// void calculateNZeroAndLambda( void ){
//   int iX, iY, iZ;
//   int iZ_start, iZ_end;
//   double xj, yj, zj, distance, distance2;
//   double xi, yi, zi;
//
//   if( DIM == 2 ){
//     iZ_start = 0; iZ_end = 1;
//   }else{
//     iZ_start = -4; iZ_end = 5;
//   }
//
//   N0_forParticleNumberDensity = 0.0;
//   N0_forGradient      = 0.0;
//   N0_forLaplacian     = 0.0;
//   Lambda              = 0.0;
//   xi = 0.0;  yi = 0.0;  zi = 0.0;
//
//   for(iX= -4;iX<5;iX++){
//     for(iY= -4;iY<5;iY++){
//       for(iZ= iZ_start;iZ<iZ_end;iZ++){
// 	if( ((iX==0)&&(iY==0)) && (iZ==0) )continue;
// 	xj = PARTICLE_DISTANCE * (double)(iX);
// 	yj = PARTICLE_DISTANCE * (double)(iY);
// 	zj = PARTICLE_DISTANCE * (double)(iZ);
// 	distance2 = (xj-xi)*(xj-xi)+(yj-yi)*(yj-yi)+(zj-zi)*(zj-zi);
// 	distance = sqrt(distance2);
// 	N0_forParticleNumberDensity += weight(distance, Re_forParticleNumberDensity);
// 	N0_forGradient      += weight(distance, Re_forGradient);
// 	N0_forLaplacian     += weight(distance, Re_forLaplacian);
// 	Lambda              += distance2 * weight(distance, Re_forLaplacian);
//       }
//     }
//   }
//   Lambda = Lambda/N0_forLaplacian;
// }
//
//
// double weight( double distance, double re ){
//   double weightIJ;
//
//   if( distance >= re ){
//     weightIJ = 0.0;
//   }else{
//     weightIJ = (re/distance) - 1.0;
//   }
//   return weightIJ;
// }
//
//
// void mainLoopOfSimulation( void ){
//   int iTimeStep = 0;
//
//   writeData_inVtuFormat();
//   writeData_inProfFormat();
//
//   while(1){
//     calculateGravity();
//     calculateViscosity();
//     moveParticle();
//     collision();
//     calculateParticleNumberDensity();
//     calculatePressure_forExplicitMPS();
//     calculatePressureGradient_forExplicitMPS();
//     moveParticleUsingPressureGradient();
//     iTimeStep++;
//     Time += DT;
//     if( (iTimeStep % OUTPUT_INTERVAL) == 0 ){
//       printf("TimeStepNumber: %4d   Time: %lf(s)   NumberOfParticles: %d\n", iTimeStep, Time, NumberOfParticles);
//       writeData_inVtuFormat();
//       writeData_inProfFormat();
//     }
//     if( Time >= FINISH_TIME ){break;}
//   }
// }
//
//
// void calculateGravity( void ){
//   int i;
//
//   for(i=0;i<NumberOfParticles;i++){
//     if(particleType[i] == ParticleType::FLUID){
//       AccelerationX[i]=GRAVITY_X;
//       AccelerationY[i]=GRAVITY_Y;
//       AccelerationZ[i]=GRAVITY_Z;
//     }else{
//       AccelerationX[i]=0.0;
//       AccelerationY[i]=0.0;
//       AccelerationZ[i]=0.0;
//     }
//   }
// }
//
//
// void calculateViscosity( void ){
//   int i,j;
//   double viscosityTermX, viscosityTermY, viscosityTermZ;
//   double distance, distance2;
//   double w;
//   double xij, yij, zij;
//   double a;
//
//   a = (KINEMATIC_VISCOSITY)*(2.0*DIM)/(N0_forLaplacian*Lambda);
//   for(i=0;i<NumberOfParticles;i++){
//     if(particleType[i] != ParticleType::FLUID) continue;
//     viscosityTermX = 0.0;  viscosityTermY = 0.0;  viscosityTermZ = 0.0;
//
//     for(j=0;j<NumberOfParticles;j++){
//       if( (j==i) || (particleType[j]==ParticleType::GHOST) ) continue;
//       xij = PositionX[j] - PositionX[i];
//       yij = PositionY[j] - PositionY[i];
//       zij = PositionZ[j] - PositionZ[i];
//       distance2 = (xij*xij) + (yij*yij) + (zij*zij);
//       distance = sqrt(distance2);
//       if(distance<Re_forLaplacian){
// 	w =  weight(distance, Re_forLaplacian);
// 	viscosityTermX +=(VelocityX[j]-VelocityX[i])*w;
// 	viscosityTermY +=(VelocityY[j]-VelocityY[i])*w;
// 	viscosityTermZ +=(VelocityZ[j]-VelocityZ[i])*w;
//       }
//     }
//     viscosityTermX = viscosityTermX * a;
//     viscosityTermY = viscosityTermY * a;
//     viscosityTermZ = viscosityTermZ * a;
//     AccelerationX[i] += viscosityTermX;
//     AccelerationY[i] += viscosityTermY;
//     AccelerationZ[i] += viscosityTermZ;
//   }
// }
//
//
// void moveParticle( void ){
//   int i;
//
//   for(i=0;i<NumberOfParticles;i++){
//     if(particleType[i] == ParticleType::FLUID){
//       VelocityX[i] += AccelerationX[i]*DT;
//       VelocityY[i] += AccelerationY[i]*DT;
//       VelocityZ[i] += AccelerationZ[i]*DT;
//
//       PositionX[i] += VelocityX[i]*DT;
//       PositionY[i] += VelocityY[i]*DT;
//       PositionZ[i] += VelocityZ[i]*DT;
//     }
//     AccelerationX[i]=0.0;
//     AccelerationY[i]=0.0;
//     AccelerationZ[i]=0.0;
//   }
// }
//
//
// void collision( void ){
//   int    i,j;
//   double xij, yij, zij;
//   double distance,distance2;
//   double forceDT; /* forceDT is the impulse of collision between particles */
//   double mi, mj;
//   double velocity_ix, velocity_iy, velocity_iz;
//   double e = COEFFICIENT_OF_RESTITUTION;
//   double* VelocityAfterCollisionX = (double*)malloc(NumberOfParticles * sizeof(double));
//   double* VelocityAfterCollisionY = (double*)malloc(NumberOfParticles * sizeof(double));
//   double* VelocityAfterCollisionZ = (double*)malloc(NumberOfParticles * sizeof(double));
//
//   for(i=0;i<NumberOfParticles;i++){
//     VelocityAfterCollisionX[i] = VelocityX[i];
//     VelocityAfterCollisionY[i] = VelocityY[i];
//     VelocityAfterCollisionZ[i] = VelocityZ[i];
//   }
//   for(i=0;i<NumberOfParticles;i++){
//     if(particleType[i] == ParticleType::FLUID){
//       mi = FluidDensity;
//       velocity_ix = VelocityX[i];
//       velocity_iy = VelocityY[i];
//       velocity_iz = VelocityZ[i];
//       for(j=0;j<NumberOfParticles;j++){
// 	if( (j==i) || (particleType[j]==ParticleType::GHOST) ) continue;
// 	xij = PositionX[j] - PositionX[i];
// 	yij = PositionY[j] - PositionY[i];
// 	zij = PositionZ[j] - PositionZ[i];
// 	distance2 = (xij*xij) + (yij*yij) + (zij*zij);
// 	if(distance2<collisionDistance2){
// 	  distance = sqrt(distance2);
// 	  forceDT = (velocity_ix-VelocityX[j])*(xij/distance)
// 	           +(velocity_iy-VelocityY[j])*(yij/distance)
// 	           +(velocity_iz-VelocityZ[j])*(zij/distance);
// 	  if(forceDT > 0.0){
// 	    mj = FluidDensity;
// 	    forceDT *= (1.0+e)*mi*mj/(mi+mj);
// 	    velocity_ix -= (forceDT/mi)*(xij/distance);
// 	    velocity_iy -= (forceDT/mi)*(yij/distance);
// 	    velocity_iz -= (forceDT/mi)*(zij/distance);
// 	    /*
// 	    if(j>i){ fprintf(stderr,"WARNING: Collision occured between %d and %d particles.\n",i,j); }
// 	    */
// 	  }
// 	}
//       }
//       VelocityAfterCollisionX[i] = velocity_ix;
//       VelocityAfterCollisionY[i] = velocity_iy;
//       VelocityAfterCollisionZ[i] = velocity_iz;
//     }
//   }
//   for(i=0;i<NumberOfParticles;i++){
//     if(particleType[i] == ParticleType::FLUID){
//       PositionX[i] += (VelocityAfterCollisionX[i]-VelocityX[i])*DT;
//       PositionY[i] += (VelocityAfterCollisionY[i]-VelocityY[i])*DT;
//       PositionZ[i] += (VelocityAfterCollisionZ[i]-VelocityZ[i])*DT;
//       VelocityX[i] = VelocityAfterCollisionX[i];
//       VelocityY[i] = VelocityAfterCollisionY[i];
//       VelocityZ[i] = VelocityAfterCollisionZ[i];
//     }
//   }
//     free(VelocityAfterCollisionX);
//     free(VelocityAfterCollisionY);
//     free(VelocityAfterCollisionZ);
// }
//
// void calculatePressure_forExplicitMPS( void ){
//
//   int    iParticle;
//   double n0;
//   double ni;
//   double c,c2;
//   double rho = FLUID_DENSITY;
//
//   c  = SPEED_OF_SOUND;
//   c2 = c * c;
//   n0 = N0_forParticleNumberDensity;
//
//   for(iParticle=0; iParticle < NumberOfParticles; iParticle++){
//     if ( (particleType[iParticle]==ParticleType::GHOST) || (particleType[iParticle] == ParticleType::DUMMY_WALL) ){
//       Pressure[iParticle] = 0.0;
//     }else{
//       ni  = ParticleNumberDensity[iParticle];
//       if( ni > n0 ){
// 	Pressure[iParticle] =  c2 * rho * (ni - n0) / n0;
//       }else{
// 	Pressure[iParticle] =  0.0;
//       }
//     }
//   }
//
// }
//
//
// void calculateParticleNumberDensity( void ){
//   int    i,j;
//   double xij, yij, zij;
//   double distance, distance2;
//   double w;
//
//   for(i=0;i<NumberOfParticles;i++){
//     ParticleNumberDensity[i] = 0.0;
//     if(particleType[i] == ParticleType::GHOST) continue;
//     for(j=0;j<NumberOfParticles;j++){
//       if( (j==i) || (particleType[j]==ParticleType::GHOST) ) continue;
//       xij = PositionX[j] - PositionX[i];
//       yij = PositionY[j] - PositionY[i];
//       zij = PositionZ[j] - PositionZ[i];
//       distance2 = (xij*xij) + (yij*yij) + (zij*zij);
//       distance = sqrt(distance2);
//       w =  weight(distance, Re_forParticleNumberDensity);
//       ParticleNumberDensity[i] += w;
//     }
//   }
// }
//
//
// void calculatePressureGradient_forExplicitMPS( void ){
//   int    i,j;
//   double gradientX, gradientY, gradientZ;
//   double xij, yij, zij;
//   double distance, distance2;
//   double w,pij;
//   double a;
//
//   a =DIM/N0_forGradient;
//   for(i=0;i<NumberOfParticles;i++){
//     if(particleType[i] != ParticleType::FLUID) continue;
//     gradientX = 0.0;  gradientY = 0.0;  gradientZ = 0.0;
//     for(j=0;j<NumberOfParticles;j++){
//       if( j==i ) continue;
//       if( particleType[j]==ParticleType::GHOST ) continue;
//       if( particleType[j]==ParticleType::DUMMY_WALL ) continue;
//       xij = PositionX[j] - PositionX[i];
//       yij = PositionY[j] - PositionY[i];
//       zij = PositionZ[j] - PositionZ[i];
//       distance2 = (xij*xij) + (yij*yij) + (zij*zij);
//       distance = sqrt(distance2);
//       if(distance<Re_forGradient){
// 	w =  weight(distance, Re_forGradient);
// 	pij = (Pressure[j] + Pressure[i])/distance2;
// 	gradientX += xij*pij*w;
// 	gradientY += yij*pij*w;
// 	gradientZ += zij*pij*w;
//       }
//     }
//     gradientX *= a;
//     gradientY *= a;
//     gradientZ *= a;
//     AccelerationX[i]= (-1.0)*gradientX/FluidDensity;
//     AccelerationY[i]= (-1.0)*gradientY/FluidDensity;
//     AccelerationZ[i]= (-1.0)*gradientZ/FluidDensity;
//   }
// }
//
//
// void moveParticleUsingPressureGradient( void ){
//   int i;
//
//   for(i=0;i<NumberOfParticles;i++){
//     if(particleType[i] == ParticleType::FLUID){
//       VelocityX[i] +=AccelerationX[i]*DT;
//       VelocityY[i] +=AccelerationY[i]*DT;
//       VelocityZ[i] +=AccelerationZ[i]*DT;
//
//       PositionX[i] +=AccelerationX[i]*DT*DT;
//       PositionY[i] +=AccelerationY[i]*DT*DT;
//       PositionZ[i] +=AccelerationZ[i]*DT*DT;
//     }
//     AccelerationX[i]=0.0;
//     AccelerationY[i]=0.0;
//     AccelerationZ[i]=0.0;
//   }
// }
//
//
// void writeData_inProfFormat( void ){
//   int i;
//   FILE *fp;
//   char fileName[256];
//
//   sprintf(fileName, "output_emps/prof/output_%04d.prof",FileNumber);
//   fp = fopen(fileName, "w");
//   fprintf(fp,"%lf\n",Time);
//   fprintf(fp,"%d\n",NumberOfParticles);
//   for(i=0;i<NumberOfParticles;i++) {
//     fprintf(fp,"%d %lf %lf %lf %lf %lf %lf %lf %lf\n"
// 	    ,(int)particleType[i], PositionX[i], PositionY[i], PositionZ[i]
// 	    ,VelocityX[i], VelocityY[i], VelocityZ[i], Pressure[i], ParticleNumberDensity[i]);
//   }
//   fclose(fp);
//   FileNumber++;
// }
//
//
// void writeData_inVtuFormat( void ){
//   int i;
//   double absoluteValueOfVelocity;
//   FILE *fp;
//   char fileName[1024];
//
//   sprintf(fileName, "output_emps/particle_%04d.vtu", FileNumber);
//   fp=fopen(fileName,"w");
//   fprintf(fp,"<?xml version='1.0' encoding='UTF-8'?>\n");
//   fprintf(fp,"<VTKFile xmlns='VTK' byte_order='LittleEndian' version='0.1' type='UnstructuredGrid'>\n");
//   fprintf(fp,"<UnstructuredGrid>\n");
//   fprintf(fp,"<Piece NumberOfCells='%d' NumberOfPoints='%d'>\n",NumberOfParticles,NumberOfParticles);
//   fprintf(fp,"<Points>\n");
//   fprintf(fp,"<DataArray NumberOfComponents='3' type='Float32' Name='Position' format='ascii'>\n");
//   for(i=0;i<NumberOfParticles;i++){
//     fprintf(fp,"%lf %lf %lf\n",PositionX[i],PositionY[i],PositionZ[i]);
//   }
//   fprintf(fp,"</DataArray>\n");
//   fprintf(fp,"</Points>\n");
//   fprintf(fp,"<PointData>\n");
//   fprintf(fp,"<DataArray NumberOfComponents='1' type='Int32' Name='ParticleType' format='ascii'>\n");
//   for(i=0;i<NumberOfParticles;i++){
//     fprintf(fp,"%d\n",(int)particleType[i]);
//   }
//   fprintf(fp,"</DataArray>\n");
//   fprintf(fp,"<DataArray NumberOfComponents='1' type='Float32' Name='Velocity' format='ascii'>\n");
//   for(i=0;i<NumberOfParticles;i++){
//     absoluteValueOfVelocity=
//       sqrt( VelocityX[i]*VelocityX[i] + VelocityY[i]*VelocityY[i] + VelocityZ[i]*VelocityZ[i] );
//     fprintf(fp,"%f\n",(float)absoluteValueOfVelocity);
//   }
//   fprintf(fp,"</DataArray>\n");
//   fprintf(fp,"<DataArray NumberOfComponents='1' type='Float32' Name='Pressure' format='ascii'>\n");
//   for(i=0;i<NumberOfParticles;i++){
//     fprintf(fp,"%f\n",(float)Pressure[i]);
//   }
//   fprintf(fp,"</DataArray>\n");
//   fprintf(fp,"<DataArray NumberOfComponents='1' type='Float32' Name='ParticleNumberDensity' format='ascii'>\n");
//   for(i=0;i<NumberOfParticles;i++){
//     fprintf(fp,"%f\n",(float)ParticleNumberDensity[i]);
//   }
//   fprintf(fp,"</DataArray>\n");
//   fprintf(fp,"</PointData>\n");
//   fprintf(fp,"<Cells>\n");
//   fprintf(fp,"<DataArray type='Int32' Name='connectivity' format='ascii'>\n");
//   for(i=0;i<NumberOfParticles;i++){
//     fprintf(fp,"%d\n",i);
//   }
//   fprintf(fp,"</DataArray>\n");
//   fprintf(fp,"<DataArray type='Int32' Name='offsets' format='ascii'>\n");
//   for(i=0;i<NumberOfParticles;i++){
//     fprintf(fp,"%d\n",i+1);
//   }
//   fprintf(fp,"</DataArray>\n");
//   fprintf(fp,"<DataArray type='UInt8' Name='types' format='ascii'>\n");
//   for(i=0;i<NumberOfParticles;i++){
//     fprintf(fp,"1\n");
//   }
//   fprintf(fp,"</DataArray>\n");
//   fprintf(fp,"</Cells>\n");
//   fprintf(fp,"</Piece>\n");
//   fprintf(fp,"</UnstructuredGrid>\n");
//   fprintf(fp,"</VTKFile>\n");
//   fclose(fp);
// }