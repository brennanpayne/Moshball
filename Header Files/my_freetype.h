#ifndef	_MY_FREETYPE_
#define	_MY_FREETYPE_

#include <GL\glew.h>
#include <gl/freeglut.h>
#include <vector>
#include <string>
#include <stdexcept>

#include <ft2build.h>
#include <freetype\freetype.h>
#include <freetype\ftglyph.h>
#include <freetype\ftoutln.h>
#include <freetype\fttrigon.h>

/*
#include <freetype2/freetype/freetype.h>
#include <freetype2/freetype/ftglyph.h>
#include <freetype2/freetype/ftoutln.h>
#include <freetype2/freetype/fttrigon.h>
*/


//#pragma warning(disable: 4786)

namespace freetype {
	// Inside Of This Namespace, Give Ourselves The Ability
	// To Write Just "vector" Instead Of "std::vector"
	using std::vector;
 
	// Ditto For String.
	using std::string;
 
	// This Holds All Of The Information Related To Any
	// FreeType Font That We Want To Create. 
	struct font_data {
		float h;                                        // Holds The Height Of The Font.
		GLuint * textures;                                  // Holds The Texture Id's
		GLuint list_base;                                   // Holds The First Display List Id
	// The Init Function Will Create A Font With
	// The Height h From The File fname.
	void init(const char * fname, unsigned int h);
 
	// Free All The Resources Associated With The Font.
	void clean();
	void axis(double x, double y, double z);
	};

	// The Flagship Function Of The Library - This Thing Will Print
	// Out Text At Window Coordinates X, Y, Using The Font ft_font.
	// The Current Modelview Matrix Will Also Be Applied To The Text.
	void print(const font_data &ft_font, float x, float y, const char *fmt, ...);
}

#endif
