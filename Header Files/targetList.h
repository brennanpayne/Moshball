/*
	Header file defining functions for the game board

	CS 559 Project 2 (Fall 2011)

	Brennan Payne
	Nick Svoboda

*/

#ifndef TARGETLIST_H
#define TARGETLIST_H

//my_freetype wants this to be here, but it's not explicitly used

#include <GL\glew.h>

#include "target.h"
#include "my_freetype.h"
#include "camera.h"



class TargetList{

public:
	TargetList();
	TargetList(int size, int seed, freetype::font_data font);
	TargetList(const TargetList &other);

	~TargetList();

	int GetSize();

	bool add(Target item);
	Target remove();

	void DrawAll(Vec3 playerPos);

	void DrawAsPoints();

	void checkCollisions(CCamera *player, double velocity, double timeSinceLast);

	int checkVictory();

private:

	struct closestWall;

	Target *closestTarget;
	//TODO- remove index, not used any more
	int closestIndex;
	double closestDistance;

	CCamera *player;

	Vec3 playerPos;

	freetype::font_data our_font;
	void MakeList();
	bool checkCircles(Vec3 target, double velocity, double timeSinceLast);
	int startTime;
	Target *head;
	int size;

	void reflect(Target *object, double distanceAlreadyTraveled);

	bool hitAWall;

	//bool checkWalls(double velocity, double timeSinceLast);

	closestWall checkWalls(double velocity, double timeSinceLast);

	Vec3 circleLineCollision(Vec3 lineP1, Vec3 lineP2, Vec3 CircleCenter, double distMoving);
};

#endif