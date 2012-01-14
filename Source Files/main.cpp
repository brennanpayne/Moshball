/*
Main file for running Moshball

CS 559 Project 2 (Fall 2011)

Brennan Payne
Nick Svoboda


Some of our sources (more refrenced at use):

OpenGL Super Bible (sounds useful)
http://opengl.czweb.org/ewtoc.html\

Collision Detection: 
Some source code and explaination- ok, lots of source code
http://nehe.gamedev.net/tutorial/collision_detection/17005/

Skybox texture from:
http://www.ataricommunity.com/forums/showthread.php?t=506748

Some code for tiling textures (not currently used anywhere):
http://www.cse.msu.edu/~cse872/tutorial4.html

*/

#include <GL\glew.h>
#include <gl\freeglut.h>

//includes from various demos Perry provided (may not all be necessary)
#include <assert.h>
#include <iostream>
#undef _UNICODE
#include <IL\il.h>
#include <IL\ilu.h>
#include <IL\ilut.h>

#include "my_freetype.h"

#include <stdio.h>
#include <string>
#include <math.h>

//Various files in this project
#include "highprecisiontime.h"
#include "planarmesh.h"
#include "arena.h"
#include "rangeRand.h"
#include "target.h"
#include "targetList.h"
#include "camera.h"


using namespace std;

bool wireFrame = false;		//Toggles wireframe mode (starts off)
bool paused = false;		//Toggles paused mode
bool first = true;			//so we can set the player to start in the center
bool win = false;			//did we win the game?

int numTargets = 10;		//10 is the default number of targets (because I said so)
int targetsHit = 0;

unsigned int period = 1000 / 60;
double localTime = 0.0;

// values used for camera movement with the mouse (lifted code)
double xpos = 0, ypos = 0, zpos = 0, xrot = 0, yrot = 0, angle=0.0;
double lastx, lasty;

//Keeping Track of the center
int midX, midY;

// A global place to store w and h (for display func and multiple viewports)
int width, height;

//Timer
HighPrecisionTime *timer = new HighPrecisionTime();

//Storage for font information (for Freetype)
freetype::font_data our_font;

//Temp vars for testing
double velocity;
double spin;

//Create a new camera (replacing player)
CCamera *player = new CCamera();

//an identifier for the skybox textures
GLuint skybox_id[3];
int skyboxIndex = 0;

//Shader variables
GLuint toon_vShader_id;
GLuint toon_fShader_id;
GLuint shader_program;

Arena *ourArena;
TargetList *targets; //changed this to a pointer.



void createTargets(int seed){
	targets = new TargetList(numTargets, seed, our_font);

	//no real reason to put this in a separate function, but maybe it'll be useful later
}

/**
THINGS WE NEED- COLLISION CHECKING OVER A PERIOD OF TIME,
CHECK OUT THE LESSON30.CPP THAT I ADDED (from internet)

woo caps lock!

TODO- make sure global time pauses
**/
void TimerFunc(int v){
	double storeTime = localTime;

	if(paused){
		//double 
		spin = 0;
		velocity = 0;

		double storeTime = localTime;

		//it has to always call this or it'll never have a proper global time. (I think)
		timer->TimeSinceLastCall();
	}
	else{
		double instanceTime = timer->TimeSinceLastCall();
		localTime += instanceTime;
		//How far we shold be moving in this period
		//double distance = velocity * instanceTime;

		//Vec3 dist = player->getPosition()*distance;

		targets->checkCollisions(player, velocity, instanceTime);

		player->RotateY(spin / 180);
		player->Render();
		//player->MoveForward(distance);

	}

	//end the game if we've won.
	//TODO- double check that it actually ends the game (maybe checkVictory doesn't return high enough??)
	targetsHit = targets->checkVictory();
	//win = targets->checkVictory();

	if(targetsHit == numTargets){
		glutLeaveMainLoop();
	}

	glutPostRedisplay();
	glutTimerFunc(period, TimerFunc, v);
}


/*
Useful check function borrowed from Perry's shader demo.
Not called by anything yet (11/21)
*/
void CheckGLErrors(std::string & caller)
{
	GLenum e = glGetError();

	if (e != GL_NO_ERROR)
	{
		switch (e)
		{
		case GL_INVALID_ENUM:
			throw caller + std::string(" Invalid ENUM");

		case GL_INVALID_VALUE:
			throw caller + std::string(" Invalid VALUE");

		case GL_INVALID_OPERATION:
			throw caller + std::string(" Invalid OPERATION");

		case GL_STACK_OVERFLOW:
			throw caller + std::string(" Stack OVERFLOW");

		case GL_STACK_UNDERFLOW:
			throw caller + std::string(" Stack UNDERFLOW");

		case GL_OUT_OF_MEMORY:
			throw caller + std::string(" Out of memory");
		}
	}
}

void KeyboardFunc(unsigned char key, int x, int y){

	switch(key){

	case 'p':
		paused = !paused;
		//implement pause mode, program and related timers
		//working as far as I can tell.
		break;

	case 'w':
		wireFrame = !wireFrame;
		break;

	case 't':
		skyboxIndex++;
		if(skyboxIndex ==3) skyboxIndex = 0;
		//swicthes textures used in the skybox
		break;

	case 27:
	case 'x':
		glutLeaveMainLoop();
		break;
	}

	glutPostRedisplay();
}

/* This function is the same as the mouseMovement functions, so it also taken from:
*		http://www.swiftless.com/tutorials/opengl/camera2.html
*		http://www.cosc.brocku.ca/Offerings/3P98/course/lectures/OpenGL/glut_ref.html
* I added the bounds for rotation
* These two functions together allow the user to click and move the cursor, and when
*		the user releases the click, the orientation will be changed.  While the user
*		still has the mouse clicked, the camera will adjust to reflect where the
*		orientation would end up if the user released the click.
*/
void mouseMotion(int x, int y) {

	//TODO- decide on a decent multiplier for velocity
	velocity = (y - midY) * 1.5;//* 2.5;
	//velocity = y - midY;

	spin = (midX - x)/2;

	glutPostRedisplay();
}

/**
Code for godMode (multiple viewports) taken from example on class website
I left most of Perry's comments in here too.

11/19- seems to be setting up fine, but I'm not seeing anything yet.

Dec 1st - Working much better, we can decide to move back further if we want to display them as points rather than balls, but I think it looks pretty good
-Brennan
**/
void godMode(){

	// Figure out size and location of PIP. (Should have these vary with width, height later)
	int pipLeft = width/12;
	int pipBottom = height/10;
	int pipWidth = width/10;
	int pipHeight = width/10;

	//compiler yelled at me when I made these regular bools
	GLboolean lights = glIsEnabled(GL_LIGHTING);


	// All frame buffer updates will be confined to inside the scissor 
	// region. (I hope)
	glEnable(GL_SCISSOR_TEST);
	glScissor(pipLeft, pipBottom, pipWidth, pipHeight);

	// I start completely over - the new viewport is a NEW render!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0 , ((double) pipWidth) / ((double) pipHeight) , 1, 20000);
	glViewport(pipLeft , pipBottom , pipWidth , pipHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(5000, 0, 0, -1, 0, 0, 0, 1, 0);


	//Draw the new things we want!
	glRotated(90 ,0 , 0, 1);

	ourArena->MakeWalls();
	targets->DrawAll(player->getPosition());

	glPushMatrix();

	if(!lights) glEnable(GL_LIGHTING);

	//Lighting for our player ball on the godmode
	GLfloat	ambient[4] = { 1.0f, 1.0f, 1.0f, 0.17f};
	GLfloat	diffuse[4] = { 1.0f, 1.0f, 1.0f, 0.65f };
	GLfloat	specular[4] = {.1f , .1f, .1f, .1f};
	glMaterialf(GL_FRONT, GL_SHININESS, 76.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glTranslated(player->getPosition().x , player->getPosition().y , player->getPosition().z);

	//this probably can be smaller in quality (it's a tiny sphere) (was 40/40 slices/stacks)
	glutSolidSphere(50 , 15 , 15);


	glPopMatrix();

	// Ensure the next time this function is called, the first viewport 
	// is drawn with the environment IT is expecting.
	glDisable(GL_SCISSOR_TEST);
	//turn lights back on if they started on, or make sure they're off
	if(lights) glEnable(GL_LIGHTING);
	else glDisable(GL_LIGHTING);
}


void DisplayFunc(){

	//TODO- DOES THIS WORK BETTER FOR STARTING IN THE CENTER??
	if(first){
		timer->TimeSinceLastCall();
		Vec3 v = Vec3(0, 0, 0);
		player->Move(v);
		first = false;
	}

	glClearColor(0, 0, 0, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL); //My way of doing this was ugly, so I stole Perry's

	//This is the primary viewport, godmode is in its own function.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0 , ((double) width) / ((double) height) , 45 , 14000);

	glViewport(0, 0, width, height);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLboolean depth = glIsEnabled(GL_DEPTH_TEST);
	if(!depth) glEnable(GL_DEPTH_TEST);

		//Begin Drawing Cross Hairs
	//They're behind everything for now. Why?
	//They DO draw, but they also appear behind the skybox texture, so you can't see them at all.
	//or you wouldnt be able to see them if I had drawn the whole skybox.

	glPushMatrix();
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glScaled( 0.05, 0.05 , 1);
	glColor3d( 1 , 1 , 1);
	glBegin(GL_LINES);
	glVertex3d(-25 , 0 , -46);
	glVertex3d(25 , 0 , -46);
	glEnd();
	glBegin(GL_LINES);
	glVertex3d(0 , 25 , -46);
	glVertex3d(0 , -25 , -46);
	glEnd();
	glPopMatrix();

	player->RotateY(spin / 180);


	//TODO- this worked in Nick's project, why doesn't it work here??
//	targetsHit = targets->checkVictory();
	int targetsLeft = numTargets-targetsHit;

	glPushMatrix();

	glUseProgram(0);
	//glTranslated(width/2, height / 2, -50);
	glTranslated(12, -11, -50);
	glColor4d(0, 0, 0, 1);
	glScaled(.01, .01, .01);
	//freetype::print(our_font, 0, 0, temp.c_str());
	freetype::print(our_font, 0, 0, "Time: %f", localTime);
	glTranslated(0, -our_font.h, 0);
	freetype::print(our_font, 0, 0, "Targets Left: %i", targetsLeft);

	glPopMatrix();
	


	player->Render();

	ourArena->DrawArena(skybox_id[skyboxIndex] , player->getPosition() , targets , shader_program);

	//Draw all targets
	glUseProgram(shader_program);
	targets->DrawAll(player->getPosition());
	glUseProgram(0);


	//Draw reflected targets TODO- do we still want these to use the shader??
	glPushMatrix();
	glTranslated(0 , -100 , 0);
	glScaled(1 , -1 , 1);
	targets->DrawAll(player->getPosition());	
	glPopMatrix();


	//engage secondary viewport
	glPushMatrix();
	godMode();
	glPopMatrix();

	if(!depth) glDisable(GL_DEPTH_TEST);
	glutSwapBuffers();
	glutPostRedisplay();
}


/*
Reshape function no longer sets up the projection, becuase there are multiple
viewports (which are eventually based on the w, h here).
*/
void ReshapeFunc(int w, int h){

	if(h <= 0)
		return;
	width = w;
	height = h;

	//Keeping track of the center (may be less necessary while keeping track of width and height globally)
	midX = w/2;
	midY = h/2;
}


//TODO- SHOULD CHECK TO MAKE SURE ANY OTHER SHADERS GET DELETED TOO...
	//Nick- I'm going to look at this soon.
void DeleteShaders()
{
	// I no longer care about previous GL errors.  I am concerned
	// only with  this  function. Each time glGetError is called,
	// one more error is cleared.   Loop until all errors are set
	// to zero.
	GLenum error = glGetError();
	while (error != GL_NO_ERROR)
		error = glGetError();

	if (shader_program != 0)
	{
		// Defensive programming only. There should be no errors
		// as the loop above should have cleared all of them.
		CheckGLErrors(std::string("DeleteShaders Beginning: "));
		// Ensure the program we are taking down is not in use.
		glUseProgram(0);
		if (toon_vShader_id != 0)
			glDetachShader(shader_program, toon_vShader_id);
		if (toon_fShader_id != 0)
			glDetachShader(shader_program, toon_fShader_id);
		glDeleteProgram(shader_program);
		// Finally, check one more time - defensive programming.
		CheckGLErrors(std::string("DeleteShaders Finish: "));
	}
}

/* This function is used for clean-up during the take-down
of a glut window. Initial function taken from Perry's DevIL demo
*/
//* copy over Brennan's close func as that deletes shaders as well
void CloseFunc()
{
	ourArena->deleteScreens();
	DeleteShaders();
	iluDeleteImage(skybox_id[0]);
	iluDeleteImage(skybox_id[1]);
	iluDeleteImage(skybox_id[2]);
	ILenum ilError;
	while ((ilError  = ilGetError()) != IL_NO_ERROR)
		cout << "iluDeleteImage returns: " << ilError << endl;
}


// Graphics Shaders: Theory and Practice
// Modified by PK to throw exceptions.
void LoadShader(const char * file_name, GLuint shader_id)
{
	FILE * file_handle = NULL;
	fopen_s(&file_handle, file_name, "rb");
	if (file_handle == NULL)
		throw std::string("Cannot open shader: ") + std::string(file_name);
	// Determine the length of the file (to find out how much memory to allocate).
	fseek(file_handle, 0, SEEK_END);
	int length = ftell(file_handle);
	fseek(file_handle, 0, SEEK_SET);
	GLubyte * buffer = new GLubyte[length + 1];
	fread(buffer, sizeof(GLubyte), length, file_handle);
	// Close the file.
	fclose(file_handle);
	// Null terminate the buffer (note buffer size is length + 1).
	buffer[length] = 0;
	// Ingest the shader's contents.
	glShaderSource(shader_id, 1, (const char **) &buffer, NULL);
	// Deallocate the buffer.
	delete [] buffer;
	// Check for common OpenGL errors.
	CheckGLErrors(std::string("LoadShader ") + std::string(file_name));
}

void InitShaders()
{
	if (glewInit() != GLEW_OK)
		throw std::string("GLEW failed to initialize.");

	int check_value = 0;

	// First ID's must be defined. These are used to define
	// the "names" of the distinct  shaders. glCreateShader
	// is like glGenLists in this way.
	toon_vShader_id = glCreateShader(GL_VERTEX_SHADER);
	toon_fShader_id = glCreateShader(GL_FRAGMENT_SHADER);

	// Then, the shaders loaded from disk. They are simply ASCII files.
	// Typically, the hardest part in this is placing the files in the
	// right directory!
	LoadShader("vertex_shader", toon_vShader_id);
	LoadShader("fragment_shader", toon_fShader_id);

	// Compile the vertex shader and determine if this is successful.
	glCompileShader(toon_vShader_id);
	glGetShaderiv(toon_vShader_id, GL_COMPILE_STATUS, &check_value);
	if (check_value != GL_TRUE)
		throw std::string("GLSL compilation failed (vertex shader).");

	// Compile the fragment shader and determine if this is successful.
	glCompileShader(toon_fShader_id);
	glGetShaderiv(toon_fShader_id, GL_COMPILE_STATUS, &check_value);
	if (check_value != GL_TRUE)
		throw std::string("GLSL compilation failed (fragment shader).");

	// A shader program is a container for shaders. This function
	// simply defines a "name" for the shader program.
	shader_program = glCreateProgram();
	// Attach the two shaders we have defined to the container.
	glAttachShader(shader_program, toon_vShader_id);
	glAttachShader(shader_program, toon_fShader_id);
	// Perform the link step.
	glLinkProgram(shader_program);
	// Check for errors during the link step.
	glGetProgramiv(shader_program, GL_LINK_STATUS, &check_value);
	if (check_value != GL_TRUE)
		throw std::string("GLSL linking failed.");
	CheckGLErrors(std::string("InitShaders First"));

	// The shader_program must be "used" before its uniforms can be
	// passed to it.   In  essence,  the glGetUniformLocation calls 
	// function  exactly  like  the  function  stubs that fetch the 
	// addresses  of  dynamically  linked  library functions.   The
	// function uses the supplied string  to return a handle (name)
	// which we can use to refer to a specific uniform.
	glUseProgram(shader_program);
	CheckGLErrors(std::string("InitShaders Last"));


	//TODO- restore this line after I add a global bool for shaders on/off
	//glUseProgram(use_shader ? shader_program : 0);
}


//note to self- C++ argv[] starts with the name of the file (moshball.exe 10 is 2 arguments)
int main(int argc, char* argv[]){

	//Starting up DevIL
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL); 

	int seed = 0;
	//Dealing with command line arguments

	//2 actual arguments- number of balls , random number seed
	//more than 2 arguments is treated the same as 2 arguments
	if(argc >= 3){
		seed = atoi(argv[2]);
		numTargets = atoi(argv[1]);
	}

	//1 actual argument- number of balls
	else if(argc == 2){
		numTargets = atoi(argv[1]);
		seed = 0;
	}

	//nothing provided- default seed of 0, 10 balls(set in global)
	else{
		seed = 0;
	}

	//TODO- check input for doubles, other strange errors (if I have time)


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//TODO- any other display modes?? Probably.

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(320, 200);
	glutCreateWindow("Moshball");

	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutPassiveMotionFunc(mouseMotion);
	glutCloseFunc(CloseFunc);

	InitShaders();

	if (glewInit() != GLEW_OK)
			throw std::string("GLEW failed to initialize.");

	//Should allow closeFunc to run after exit, cleaning memory
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	glutTimerFunc(period, TimerFunc, 0);

	glutFullScreen();

	//TODO- can we even use multisampling without glext.h included??
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_NORMALIZE);


	our_font.init("C:\\windows\\fonts\\micross.ttf", 72);

	skybox_id[0] = ilutGLLoadImage("interstellar_large.jpg");
	if(skybox_id[0] == 0)
		throw std::string("Skybox1 texture not found.");

	skybox_id[1] = ilutGLLoadImage("skybox_texture.jpg");

	if(skybox_id[1] == 0)
		throw std::string("Skybox2 texture not found.");

	skybox_id[2] = ilutGLLoadImage("skybox3.png");

	if(skybox_id[1] == 0)
		throw std::string("Skybox3 texture not found.");

	assert(skybox_id != 0);
	//TODO- implement error checking for loading the texture
	//(the above throw statement doesn't work yet, and the assert won't work outside debug mode)

	//Set up the targets and arena (****after GL is initialized****)
	createTargets(seed);
	ourArena = new Arena();
	ourArena->setUpScreens();


	glutMainLoop();

	//TODO
	//After main loop: Clean memory
	//any display lists (not after GLUT is done, it won't understand)
	//all heap allocated things
	//all of the shader stuff in video memory **IMPORTANT**
	string user;

	cout << "Game has ended at time " << localTime << endl;
	if(targetsHit == numTargets){
		cout << "You've Won!" << endl;
	}
	else cout << "Game terminated early" << endl;
	cout << "Hit enter to exit:" << endl;
	//doesn't really care if you've also typed something, just consumes the line.
	getline(cin, user);


	return 0;
}