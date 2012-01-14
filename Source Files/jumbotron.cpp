#include "jumbotron.h"
#include "planarmesh.h"

Jumbotron::Jumbotron(){

}


//http://www.gamedev.net/page/resources/_/technical/opengl/opengl-frame-buffer-object-101-r2331
void Jumbotron::setUp(){
	
	GLuint frame_buffer_object;
	GLuint depth_buffer;
	GLuint jumbotron_id;

	//Size of jumbotron
	GLsizei frame_buffer_width = 500; //500 for us
	GLsizei frame_buffer_height = 250; //250 for us

	
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

   //At this point we 'Bind' the framebuffer so we can begin rendering to is
   glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_object);
   glPushAttrib(GL_VIEWPORT_BIT);
   glViewport(0 , 0 , frame_buffer_width , frame_buffer_height);
  // */
	/*
   glGenFramebuffersEXT(1, &frame_buffer_object);
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object);
   glGenRenderbuffersEXT(1, &depth_buffer);
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_buffer);
   glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, frame_buffer_width, frame_buffer_height);
   glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_buffer);

   glGenTextures(1, &jumbotron_id);
   glBindTexture(GL_TEXTURE_2D, jumbotron_id);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, jumbotron_id, 0);
   GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
   if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
       throw std::string("Frame buffer failed to initialize properly");

   //At this point we 'Bind' the framebuffer so we can begin rendering to is
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object);
   glPushAttrib(GL_VIEWPORT_BIT);
   glViewport(0 , 0 , frame_buffer_width , frame_buffer_height);
   */


   //Start Rendering


   glTranslated( 0 , 0 , -2);
   glBegin(GL_TRIANGLES);
   glColor3d(.2 , .7 , .5);
   glVertex3d(-1 , 0 , 0);
   glVertex3d(0 , 1 , 0);
   glVertex3d(1 , 0 , 0);
   glEnd();

  glPopAttrib();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


   //When we finish up
   /*
   Cleaning Up
	Finally, when you are done with your FBO you need to clean things up and delete it. This, like textures, is done via a single function:

	glDeleteFramebuffersEXT(1, &frame_buffer_height); You also have to clean up any renderbuffers you might have allocated, 
	in this case the depthbuffer renderbuffer needs to deleted, which again works the same way as cleaning up a texture:

	glDeleteRenderbuffersEXT(1, &depthbuffer); At this point both the FBO and the renderbuffer have been freed and your clean up is completed.
	*/
}