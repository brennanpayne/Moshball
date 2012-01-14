/*  This code was inspired by the following sources:
 *		- http://plib.sourceforge.net/sg/index.html
 *		- http://chortle.ccsu.edu/vectorlessons/vectorIndex.html
 *		- http://www.videotutorialsrock.com/opengl_tutorial/vec3f/video.php
 *			* most of what I have comes from this video tutorial
 *		- course notes (http://oberon.cs.wisc.edu/cs559/Documents/intro_math.pdf)
 *		- http://www.cplusplus.com
 *	Implementation was not lifted, but is my own



 *  Nick: Changed implementation from using an array of doubles v[3] to 3 doubles
 *	(x, y, z), to fit more easily with planarmesh
 */

#include <math.h>
#include <iostream>

#ifndef VEC3_H
#define VEC3_H

using namespace std;

class Vec3 {
	private:
		
	public:
		
		double x, y, z;

		Vec3();
		Vec3(double x, double y, double z);

		double operator[](int index) const;

		void operator=(const Vec3 newvec);

		const Vec3 operator+(const Vec3 addvec);
		void operator+=(const Vec3 addvec);
		const Vec3 operator-(const Vec3 subvec);
		void operator-=(const Vec3 subvec);
		const Vec3 operator*(double scale);
		const Vec3 operator/(double scale);

		void set(double x, double y, double z);

		double magnitude() const;
		Vec3 normalize() const;
		double dotProd(const Vec3 other) const;
		Vec3 crossProd(const Vec3 other) const;

		void vecPrint() const;


		Vec3 relfectOverVector(const Vec3& norm) const;
};

#endif