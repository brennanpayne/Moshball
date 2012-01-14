
//***************************************************************************
//Code and Ideas taken from: 
//
//
// Advanced CodeColony Camera
// Philipp Crocoll, 2003
// http://www.codecolony.de/opengl.htm
// Specific Description:
// http://www.codecolony.de/docs/camera2.htm\
//
//
// Edited by Nick to use our Vec3 class. Their F3dVector left in for reference
// for a while, but I finally got rid of them entirely.
//
//***************************************************************************


#include "camera.h"
#include "math.h"
#include <iostream>
#include "windows.h"

#include "vec3.h"



/***************************************************************************************/

CCamera::CCamera()
{
	//Changed init with vec3s:
	Position = Vec3(0, 0, 0);
	//to 1.0 z
	ViewDir = Vec3(0, 0, 1.0);
	RightVector = Vec3(1.0, 0.0, 0.0);
	UpVector = Vec3(0.0, 1.0, 0.0);
	Velocity = Vec3(0, 0, 0);

	//Only to be sure:
	RotatedX = RotatedY = RotatedZ = 0.0;
}

void CCamera::Move (Vec3 Direction)
{
	Position = Position + Direction;
}

Vec3 CCamera::getPosition(){
	return Position;
}

Vec3 CCamera::getViewDir(){
	return ViewDir;
}


void CCamera::RotateY (double Angle)		//Compilation doesn't like the double->float conversion
{											//Doesn't affect how it works, but Perry won't like it -NICK
											//Never mind, fixed it at some point

	RotatedY += Angle;
	
	//Rotate viewdir around the up vector:

	//ViewDir = (ViewDir*cos(Angle*PIdiv180)) - (RightVector*sin(Angle*PIdiv180));
	ViewDir = ViewDir * cos(Angle * PIdiv180);
	Vec3 tmp = RightVector * sin(Angle * PIdiv180);

	ViewDir = ViewDir - tmp;

	//just making sure it's still normalized
	ViewDir = ViewDir.normalize();

	//now compute the new RightVector (by cross product)
	//RightVector = CrossProduct(&ViewDir, &UpVector);

	RightVector = ViewDir.crossProd(UpVector);
}

void CCamera::Reflect( Vec3 reflectionNormal){

	double dot = reflectionNormal.dotProd(ViewDir);

	Vec3 twice = reflectionNormal * 2;

	Vec3 R = ViewDir - twice * dot; 
	//r = i - (2 * n * dot(n, i)) 

	ViewDir = R.normalize();
}


void CCamera::Render( void )
{

	//The point at which the camera looks:
	Vec3 ViewPoint = Position+ViewDir;

	//as we know the up vector, we can easily use gluLookAt:
	gluLookAt(	Position.x,Position.y,Position.z,
				ViewPoint.x,ViewPoint.y,ViewPoint.z,
				UpVector.x,UpVector.y,UpVector.z);

}


void CCamera::MoveForward(double Distance )
{		//removing a negative on distance
	Position = Position + (ViewDir*-Distance);
}

//Should work to find the point on the sphere that is being hit
//...assuming we add the radius of this sphere to it
Vec3 CCamera::getFuturePosition(double distance){
	
	return Position + (ViewDir*-distance);
}
