/*
	Header file defining functions for the target spheres.
	CS 559 Project 2 (Fall 2011)

	Brennan Payne
	Nick Svoboda

*/

#ifndef TARGET_H
#define TARGET_H

#include <GL\glew.h>
#include <GL\freeglut.h>
#include "vec3.h"

#include "my_freetype.h"

class Target {

public:

	Vec3 position;

	Target();
	Target(const Target &other);
	Target(Vec3 pos, int time);
	Target(Vec3 pos, int time, Target* lst);
	Target(Vec3 pos, int time, Target* nxt, Target* lst);

	void   setNext(Target *nxt);
	Target* getNext();
	void setLast(Target *lst);
	Target* getLast();

	bool isHit();
	void setHit(bool h);
	double getCountTime();

	void changePosition(Vec3 pos);

	void Draw(freetype::font_data font,  Vec3 playerPos);

	void computeTime(double timeSinceLast);

	Vec3 getPosition();

private:

	static bool lists;

	static GLuint Sphere;
	//GLuint notHitSphere;

	double accumulate;

	Target *next;
	Target *last;

	bool hit;

	int countTime;
	double timeLeft;

	void makeLists();
};


#endif