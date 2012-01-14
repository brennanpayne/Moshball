/*
	Header file defining functions for the game board

	CS 559 Project 2 (Fall 2011)

	Brennan Payne
	Nick Svoboda

*/

#ifndef ARENA_H
#define ARENA_H
#include "planarmesh.h"
#include "GL/glew.h"
#include <GL/freeglut.h>
#include "camera.h"
#include "targetList.h"

class Arena {

public:
	Arena();
	void DrawArena(GLuint skybox_id, Vec3 playerPos, TargetList *list, GLuint shader_program);
	
		
	//Only public so we can do godmode (and some other stuff) easier
	void MakeFloor();
	void MakeWalls();
	void MakeScreens();
	void MakeScreens2(Vec3 playerPos , TargetList *list, GLuint shader_program);
	void Skybox(GLuint id);
	void setUpScreens();
	void deleteScreens();


protected:
	static bool listMade;
	static GLuint arenaList;
	static GLuint floor;
	static GLuint walls;
	static bool first;

	PlanarMesh* bottom;
	PlanarMesh* wall;
	
	void CreateList();

};

#endif