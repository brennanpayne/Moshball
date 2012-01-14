/**
	Implementation of the arena (field and background the game is played on)
	CS 559 Project 2 (Fall 2011)

	Brennan Payne
	Nick Svoboda

	There used to be a display list here, but I got rid of it.
	-Display lists are back in business!

**/

#include "arena.h"
#include "planarmesh.h"
#include <GL/glew.h>
#include <GL\freeglut.h>


bool Arena::listMade = false;
bool Arena::first = true;
GLuint skybox_ID;

GLuint Arena::arenaList = -1;
GLuint Arena::floor = -1;
GLuint Arena::walls = -1;

GLuint frame_buffer_object;
GLuint depth_buffer;
GLuint jumbotron_id;
	//Size of jumbotron
GLuint frame_buffer_width = 500; //500 for us
GLuint frame_buffer_height = 250; //250 for us


Arena::Arena(){

	bottom = new PlanarMesh(264, 264);
	wall = new PlanarMesh(264, 20); //was 528,33
}

/*
Function from Brennan- edited to use planarmesh
12/3- planarmesh removed, glQuads used instead
	because it was easier to make the shader work for them
*/
void Arena::MakeWalls(){

	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	
	for(int i=1; i < 5; i++){
		
		glRotated(90, 0, 1, 0);
		
		glPushMatrix();

		//Gl color is only for god mode
		glTranslated(0, -100, -2640);
		glNormal3d(0 , 0 , 1);
		glBegin(GL_QUADS);
		glVertex3d(-2640 , 150 , 0);
		glVertex3d(2640 , 150 , 0);
		glVertex3d(2640 , -50 , 0);
		glVertex3d(-2640 , -50 , 0 );
		glEnd();

		
		glScaled(5280, 100, 0);

	
		//glNormal3d(wall->GetOutArray()[0].x , wall->GetOutArray()[0].y , wall->GetOutArray()[0].z);
		//wall->Draw();
	
		glPopMatrix();
	}
	glPopMatrix();
}

/*
Floor of the arena (don't call it 'floor', that's a math function)
*/
void Arena::MakeFloor(){

	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glTranslated(0, -50, 0);


	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4d(0.4, 0.4, 0.4, 0.80);  

	/*
	glBegin(GL_QUADS);
	glVertex3d(-2640, 0 , -2640);
	glVertex3d(2640 , 0 , -2640);
	glVertex3d(2640, 0 , 2640);
	glVertex3d(-2640, 0 , 2640);
	glEnd();
	*/

	glRotated(90, 0, 1, 0);
	glRotated(90, 1, 0, 0);

	glScaled(5280, 5280, 0);

	
	bottom->Draw(bottom->InArray, true);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glPopMatrix();
}

/*
	For creation of framebuffers for the Jumbotron
*/
void Arena::setUpScreens(){
	glGenFramebuffers(1, &frame_buffer_object);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_object);
	glGenRenderbuffers(1, &depth_buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, frame_buffer_width, frame_buffer_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);

	glGenTextures(1, &jumbotron_id);
	glBindTexture(GL_TEXTURE_2D, jumbotron_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, jumbotron_id, 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		throw std::string("Frame buffer failed to initialize properly");
	glBindTexture(GL_TEXTURE_2D, 0);
}


void Arena::deleteScreens(){
	glDeleteFramebuffers(1, &frame_buffer_height); 
	glDeleteRenderbuffers(1, &depth_buffer);
}


void Arena::MakeScreens2(Vec3 playerPos, TargetList *list, GLuint shader_program){


	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_object);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0 , 0 , frame_buffer_width , frame_buffer_height);
	//At this point we 'Bind' the framebuffer so we can begin rendering to is


	glPushMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
		

	glPushMatrix();
	glTranslated(playerPos.x, playerPos.y, playerPos.z);
	glScaled(60, 35, 60);
	Skybox(skybox_ID);
	glPopMatrix();
	
	glPushMatrix();
	glEnable(GL_LIGHTING);
	GLfloat	ambient2[4] = { 0.5f, 0.12f, 0.69f, 0.17f};
	GLfloat	diffuse2[4] = { 0.2f, 0.13f, 0.78f, 0.65f };
	GLfloat	specular2[4] = {.1f , .1f, .3f, .1f};	
	glMaterialf(GL_FRONT, GL_SHININESS, 76.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular2);
	glCallList(walls);
	glPopMatrix();


	//Draw reflection
	glPushMatrix();
	glTranslated(0 , -100 , 0);
	glScaled( 1 , -1 , 1);  
	glUseProgram(shader_program);
	list->DrawAll(playerPos);
	glUseProgram(0);
	glPopMatrix();

	glCallList(floor);

	glUseProgram(shader_program);
	list->DrawAll(playerPos);
	glUseProgram(0);
		
	//Currently doing absolutely nothing
	/*
	if(first){
		first = false;
		glPushMatrix();
		MakeScreens2(playerPos,list);
		glPopMatrix();
	}*/

	glDisable(GL_LIGHTING);

	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//End rendering into frame buffer

	glBindTexture(GL_TEXTURE_2D, jumbotron_id);

	GLUquadricObj *qobj = gluNewQuadric();
	gluQuadricNormals(qobj, GLU_SMOOTH);

	

	for(int i = 0; i < 4 ; i++){
		glRotated(90 , 0 , 1 , 0); 

		//Draw posts
		glPushMatrix();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glUseProgram(shader_program);

		GLfloat	ambient[4] = { 0.5f, 0.12f, 0.09f, 0.17f};
		GLfloat	diffuse[4] = { 0.44f, 0.13f, 0.78f, 0.65f };
		GLfloat	specular[4] = {.1f , .1f, .1f, .1f};
		glMaterialf(GL_FRONT, GL_SHININESS, 76.0f);
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

		glTranslated(0 , 0 , -2650);


		glRotated(-90, 1, 0, 0);

		//does the larger cylinder demand more slices/stacks to look good??
		//should we just scale up a smaller one instead of making this huge thing?
		//12/4- nope, this is an old problem. After decent lighting got set up, it was fine

		gluCylinder(qobj, 10 , 7 , 300 , 50 , 50);
		glDisable(GL_LIGHTING);
		glPopMatrix();
		glUseProgram(0);
		//End Posts

		//Begin Drawing Screens
		glPushMatrix();
		glEnable(GL_DEPTH_TEST);
		glTranslated(0, 250 , -2640);
		
		glEnable(GL_TEXTURE_2D);
		//Texture code initially from Perry's DevIL demo
		
		//To scale becuase we put the texture from the frame buffer directly on it
		//mapping the jumbotron texture to the screen
		//TODO- should we explicity load the texture in this method? I know it seems to work,
		//but it could cause problems later??
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);	glVertex2d(-250 , 0);
		glTexCoord2d(0, 1);	glVertex2d(-250 , 250);
		glTexCoord2d(1, 1); glVertex2d(250 , 250);
		glTexCoord2d(1, 0);	glVertex2d(250 , 0 );
		glEnd();

		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	
		//End Screens
	}
	gluDeleteQuadric(qobj);
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
	first = true;
}


void Arena::MakeScreens(){

	GLUquadricObj *qobj = gluNewQuadric();
	gluQuadricNormals(qobj, GLU_SMOOTH);

		for(int i = 0; i < 4 ; i++){
		glRotated(90 , 0 , 1 , 0); 

		//Draw posts
		glPushMatrix();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		GLfloat	ambient[4] = { 0.5f, 0.12f, 0.09f, 0.17f};
		GLfloat	diffuse[4] = { 0.44f, 0.13f, 0.78f, 0.65f };
		GLfloat	specular[4] = {.1f , .1f, .1f, .1f};

		//glShadeModel (GL_SMOOTH);

		glMaterialf(GL_FRONT, GL_SHININESS, 76.0f);
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

		glTranslated(0 , 0 , -2650);


		glRotated(-90, 1, 0, 0);

		gluCylinder(qobj, 10 , 7 , 300 , 50 , 50);
		glDisable(GL_LIGHTING);
		glPopMatrix();
		//End Posts

	}

	gluDeleteQuadric(qobj);

}

/*
	Draws the skybox on a box 1x1x1.
	We were going to have the roof as well for the corners at least, but 
	it was giving us problems, so we got rid of it and just scaled up the 
	walls a bit more.

	Also draws the inverse of the wall below itself to look like reflection
*/
void Arena::Skybox(GLuint skybox_id){
	
	glPushMatrix();
	double aspect = 3.0 / 2.0;	

	glMatrixMode(GL_MODELVIEW);
	GLboolean lights = glIsEnabled(GL_LIGHTING);
	GLboolean depth = glIsEnabled(GL_DEPTH_TEST);
	if(depth) glDisable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
	//Texture code initially from Perry's DevIL demo
	glBindTexture(GL_TEXTURE_2D, skybox_id);

	glColor4d(1,1,1, 0);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//Draw center rectangle of texture
	glPushMatrix();
	glTranslated(0, 0, -aspect);

	glBegin(GL_QUADS);
	glTexCoord2d(.25, .5);		glVertex2d(-aspect, 0);
	glTexCoord2d(.25, 2.0/3.0);	glVertex2d(-aspect, 1);
	glTexCoord2d(.5, 2.0/3.0);	glVertex2d(aspect, 1);
	glTexCoord2d(.5, .5);		glVertex2d(aspect, 0);
	glEnd();

	//Draw Reflection
	glScaled(1 , -1 , 1);
	glBegin(GL_QUADS);
	glTexCoord2d(.25, .5);		glVertex2d(-aspect, 0);
	glTexCoord2d(.25, 2.0/3.0);	glVertex2d(-aspect, 1);
	glTexCoord2d(.5, 2.0/3.0);	glVertex2d(aspect, 1);
	glTexCoord2d(.5, .5);		glVertex2d(aspect, 0);
	glEnd();

	glPopMatrix();


	//Draw mid-right rectangle of texture.
	glPushMatrix();
	glRotated(-90, 0, 1, 0);
	glTranslated(0, 0, -aspect);

	glBegin(GL_QUADS);

	glTexCoord2d(.5, .5);		glVertex2d(-aspect, 0);
	glTexCoord2d(.5, 2.0/3.0);	glVertex2d(-aspect, 1);
	glTexCoord2d(.75, 2.0/3.0);	glVertex2d(aspect, 1);
	glTexCoord2d(.75, .5);		glVertex2d(aspect, 0);
	glEnd();

	
	//Draw Reflection
	glScaled(1 , -1 , 1);

	glBegin(GL_QUADS);
	glTexCoord2d(.5, .5);		glVertex2d(-aspect, 0);
	glTexCoord2d(.5, 2.0/3.0);	glVertex2d(-aspect, 1);
	glTexCoord2d(.75, 2.0/3.0);	glVertex2d(aspect, 1);
	glTexCoord2d(.75, .5);		glVertex2d(aspect, 0);
	glEnd();

	glPopMatrix();

	//Draw far right rectangle of texture
	glPushMatrix();
	glRotated(-180, 0, 1, 0);
	glTranslated(0, 0, -aspect);

	glBegin(GL_QUADS);
	glTexCoord2d(.75, .5);		glVertex2d(-aspect, 0);
	glTexCoord2d(.75, 2.0/3.0);	glVertex2d(-aspect, 1);
	glTexCoord2d(1, 2.0/3.0);	glVertex2d(aspect, 1);
	glTexCoord2d(1, .5);		glVertex2d(aspect, 0);
	glEnd();

	//Draw Reflection
	glScaled(1 , -1 , 1);
	glBegin(GL_QUADS);
	glTexCoord2d(.75, .5);		glVertex2d(-aspect, 0);
	glTexCoord2d(.75, 2.0/3.0);	glVertex2d(-aspect, 1);
	glTexCoord2d(1, 2.0/3.0);	glVertex2d(aspect, 1);
	glTexCoord2d(1, .5);		glVertex2d(aspect, 0);
	glEnd();

	glPopMatrix();

	//Draw far left rectangle of texture
	glPushMatrix();
	glRotated(-270, 0, 1, 0);
	glTranslated(0, 0, -aspect);

	glBegin(GL_QUADS);
	glTexCoord2d(0, .5);		glVertex2d(-aspect, 0);
	glTexCoord2d(0, 2.0/3.0);	glVertex2d(-aspect, 1);
	glTexCoord2d(.25, 2.0/3.0);	glVertex2d(aspect, 1);
	glTexCoord2d(.25, .5);		glVertex2d(aspect, 0);
	glEnd();

	//Draw Reflection
	glScaled(1 , -1 , 1);
	glBegin(GL_QUADS);
	glTexCoord2d(0, .5);		glVertex2d(-aspect, 0);
	glTexCoord2d(0, 2.0/3.0);	glVertex2d(-aspect, 1);
	glTexCoord2d(.25, 2.0/3.0);	glVertex2d(aspect, 1);
	glTexCoord2d(.25, .5);		glVertex2d(aspect, 0);
	glEnd();

	glPopMatrix();

	glPopMatrix();


	glDisable(GL_TEXTURE_2D);
	if(depth) glEnable(GL_DEPTH_TEST);
	if(lights) glEnable(GL_LIGHTING);
}

/*
	Make some display lists for various pieces of the arena. 
	We were using one list for a while, but split it up so we can 
	decide to draw each piece in some of the other places we need it.
*/
void Arena::CreateList(){

	floor = glGenLists(1);
	glNewList(floor , GL_COMPILE);
	MakeFloor();
	glEndList();

	walls = glGenLists(1);
	glNewList(walls, GL_COMPILE);
	MakeWalls();
	glEndList();

	arenaList = glGenLists(1);
	glNewList(arenaList, GL_COMPILE);

	glCallList(floor);
	glCallList(walls);

	glEndList();

	listMade = true;
}


/*
	Central function for drawing our arena. Creates display lists if they haven't been done yet,
	and draws the arena and its reflections on the floor.
*/
void Arena::DrawArena(GLuint skybox_id, Vec3 playerPos, TargetList *list, GLuint shader_program){

	skybox_ID = skybox_id;
	if(!listMade){
		CreateList();
	}


	GLboolean lights = glIsEnabled(GL_LIGHTING);
	GLboolean depth = glIsEnabled(GL_DEPTH_TEST);

	glPushMatrix();
	if(!lights){
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	
	glPushMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
		

	glPushMatrix();
	glTranslated(playerPos.x, playerPos.y, playerPos.z);
	glScaled(60, 35, 60);
	Skybox(skybox_ID);
	glPopMatrix();

	
	glPushMatrix();
	glEnable(GL_LIGHTING);
	GLfloat	ambient2[4] = { 0.5f, 0.12f, 0.69f, 0.17f};
	GLfloat	diffuse2[4] = { 0.2f, 0.13f, 0.78f, 0.65f };
	GLfloat	specular2[4] = {.1f , .1f, .3f, .1f};	
	glMaterialf(GL_FRONT, GL_SHININESS, 76.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular2);
	glCallList(walls);
	glPopMatrix();


	//Draw reflection
	glPushMatrix();
	glTranslated(0 , -100 , 0);
	glScaled( 1 , -1 , 1);  
	glUseProgram(shader_program);
	list->DrawAll(playerPos);
	glUseProgram(0);
	glPopMatrix();

	glCallList(floor);

	glUseProgram(shader_program);
	list->DrawAll(playerPos);
	glUseProgram(0);
		

	MakeScreens2(playerPos , list, shader_program);

	if(!lights){
		glDisable(GL_LIGHTING);
	}

	if(!depth){
		glDisable(GL_DEPTH_TEST);
	}
	glPopMatrix();

}

//TODO- get rid of this (once we're sure we don't need it)
/*Old fucntion that was drawing the reflection incorrectly
void Arena::DrawArena(GLuint skybox_id, Vec3 pos, TargetList *list, GLuint shader_program){
	skybox_ID = skybox_id;
	if(!listMade){
		CreateList();
	}


	GLboolean lights = glIsEnabled(GL_LIGHTING);
	GLboolean depth = glIsEnabled(GL_DEPTH_TEST);

	glPushMatrix();
	if(!lights){
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	
	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);
	glScaled(60, 35, 60);
	Skybox(skybox_id);
	glPopMatrix();

	if(!depth) glEnable(GL_DEPTH_TEST);
	glPushMatrix();

	glUseProgram(shader_program);
	//Currently Sets the color of the walls
	GLfloat	ambient[4] = { 0.5f, 0.12f, 0.69f, 0.17f};
	GLfloat	diffuse[4] = { 0.2f, 0.13f, 0.78f, 0.65f };
	GLfloat	specular[4] = {.1f , .1f, .3f, .1f};	
	glMaterialf(GL_FRONT, GL_SHININESS, 76.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glCallList(walls);
	glUseProgram(0);
	glPopMatrix();

	glCallList(floor);

	MakeScreens2(pos , list);

	if(!lights){
		glDisable(GL_LIGHTING);
	}

	if(!depth){
		glDisable(GL_DEPTH_TEST);
	}
	glPopMatrix();

}*/