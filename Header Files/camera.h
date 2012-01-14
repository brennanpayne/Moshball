//***************************************************************************
//Code and Ideas taken from: 
//
//
// Advanced CodeColony Camera
// Philipp Crocoll, 2003
// http://www.codecolony.de/opengl.htm
//
// Changed by Nick to use our Vec3 class, so it interfaces more easily with 
// everything else
//***************************************************************************

#ifndef CAMERA_H
#define CAMERA_H
// Need to include it here because the GL* types are required //NICK- changed to freeglut instead of glut
#include <GL/freeglut.h>		
#include "vec3.h"

#define PI 3.1415926535897932384626433832795 
#define PIdiv180 (PI/180.0)

/////////////////////////////////
//Note: All angles in degrees  //
/////////////////////////////////

class CCamera
{
private:
	
	Vec3 ViewDir;
	Vec3 RightVector;
	Vec3 UpVector;
	Vec3 Position;
	Vec3 Velocity;

	double  RotatedY, RotatedX, RotatedZ;	
	
public:
	CCamera();				//inits the values (Position: (0|0|0) Target: (0|0|-1) )
	void Render ( void );	//executes some glRotates and a glTranslate command
							//Note: You should call glLoadIdentity before using Render

	Vec3 getPosition();
	Vec3 getFuturePosition(double distance);
	Vec3 getViewDir();

	void Move ( Vec3 Direction );

	void Reflect ( Vec3 reflectionNormal );
	
	void RotateY ( double Angle );

	void MoveForward ( double Distance );
	
	void MoveForward();

};

#endif

