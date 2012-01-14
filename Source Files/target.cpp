/*
	Class representing one of the target spheres on the gameboard

	CS 559 Project 2 (Fall 2011)

	Brennan Payne
	Nick Svoboda

	Targets also act as nodes in a targetlist.
	Actually, that's pretty much all they are.

	Can be linked forward or backwards, but 
	TargetList only implements forwards links.
*/

#include "target.h"
#include "rangeRand.h"

#define PI 3.1415926535897932384626433832795 

bool Target::lists = false;

GLuint Target::Sphere = -1;

/*
	Create the geometry for the sphere (we only need 1 of them)
*/
void Target::makeLists(){

	GLUquadric *sphere = gluNewQuadric();

	Sphere = glGenLists(1);
	glNewList(Sphere, GL_COMPILE);
	
	//we probably don't need this (outside is the default)
	gluQuadricOrientation(sphere, GLU_OUTSIDE);

	glShadeModel (GL_SMOOTH);
	gluSphere(sphere, 50, 50, 50);

	glEndList();

	gluDeleteQuadric(sphere);
	lists = true;
}

/*
	Constructor, sets all to empty or zero values
*/
Target::Target(){
	position = Vec3();
	hit = false;
	countTime=0;
	accumulate = 0;
}

/*
	Copy constructor
*/
Target::Target(const Target &other){
	position = other.position;
	next = other.next;
	last = other.last;
	hit = false;
	accumulate = 0;
}

/*
	Constructor for a stand-alone Target
*/
Target::Target(Vec3 Pos, int time){
	position = Pos;
	hit = false;
	countTime = time;
	accumulate = 0;
}

/*
	Costructor for a Target within a list
*/
Target::Target(Vec3 pos, int time, Target *lst){
	position = pos;
	last = lst;
	hit = false;
	countTime = time;
	accumulate = 0;
}

/*
	Constructor for a Target with provided pointers to the 
	last and next targets in the list.
//TODO- probably won't need this one.
*/
Target::Target(Vec3 pos, int time, Target *nxt, Target *lst){
	position = pos;
	next = nxt;
	last = lst;
	hit = false;
	countTime = time;
	accumulate = 0;
}

/*
	Pointer to the next Target in the list.
*/
void Target::setNext(Target *nxt){
	next = nxt;
}


Target *Target::getNext(){
	return next;
}

/*
	Pointer to the last target in the list. 
	Not used.
*/
void Target::setLast(Target *lst){
	last = lst;
}

Target *Target::getLast(){
	return last;
}

bool Target::isHit(){
	return hit;
}

double Target::getCountTime(){
	return countTime;
}

void Target::changePosition(Vec3 v){
	position = v;
}

Vec3 Target::getPosition(){
	return position;
}

/*
	Function drawing this Target. Creates the display list if it hasn't
	been done yet.
*/
void Target::Draw(freetype::font_data our_font, Vec3 pos){

	if(!lists){
		makeLists();
	}

	//Begin drawing targets
	GLboolean depth = glIsEnabled(GL_DEPTH_TEST);
	if(!depth) glEnable(GL_DEPTH_TEST);

	GLboolean lights = glIsEnabled(GL_LIGHTING);
	if(!lights){
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}

	glPushMatrix();

	glTranslated(position.x , position.y , position.z);

	if(!hit){
		
		GLfloat	ambient[4] = { 0.0f, 0.8f, 0.3f, 0.20f};
		GLfloat	diffuse[4] = { 0.0f, 0.8f, 0.78f, 0.65f };
		GLfloat	specular[4] = {.1f , .5f, .5f, .1f};
		glMaterialf(GL_FRONT, GL_SHININESS, 76.0f);
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

		//cout<< position.x <<endl;
		glCallList(Sphere);

	}

	//Displaying a ball that has been hit (color change and countdown timer)
	if(hit){
		GLfloat	ambient[4] = { 0.5f, 0.12f, 0.09f, 0.17f};
		GLfloat	diffuse[4] = { 0.44f, 0.13f, 0.78f, 0.65f };
		GLfloat	specular[4] = {.1f , .1f, .1f, .1f};
		glMaterialf(GL_FRONT, GL_SHININESS, 76.0f);
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

		glCallList(Sphere);
		//Draw text for remaining time:

		glUseProgram(0);
		glPushMatrix();
		glTranslated(0, 55, 0);
		glScaled(.2, .2, .2);

		//Make sure the text is always facing the player
		glRotated((180.0/PI)*atan2(pos.x - position.x , pos.z - position.z ), 0, 1, 0);

		//glColor3d(0, 0, 0);
		int asInt = (int) timeLeft;
		/*Not sure if this works - Brennan Dec 3
		if(asInt > 10)
			glTranslated(-10 , 0 , 0);
		else
			glTranslated(-20 , 0 , 0);
			*/
		freetype::print(our_font, 0, 0, "%i", asInt);

		//TODO- make the text larger, give it a reasonable color, print the actual time not 
		//some sort of gibberish
		//center the text over the sphere.
		glPopMatrix();
	}

	glPopMatrix();

	if(!depth) glDisable(GL_DEPTH_TEST);
	if(!lights) glDisable(GL_LIGHTING);

}

void Target::setHit(bool h){
	timeLeft = (double)countTime;
	hit = h;
}

/**
	Can eventually use this to update the displayed timer on the target before it switches back.

	Countdown fixed to be more reasonable
**/
void Target::computeTime(double timeSinceLast){
	accumulate += timeSinceLast;
	if((int)accumulate >= 1){
		accumulate = 0;
		timeLeft -= 1;
	}

	if(timeLeft <= 0) hit = false;
}