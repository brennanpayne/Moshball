/*  This code was inspired by the following sources:
 *		- http://plib.sourceforge.net/sg/index.html
 *		- http://chortle.ccsu.edu/vectorlessons/vectorIndex.html
 *		- http://www.videotutorialsrock.com/opengl_tutorial/vec3f/video.php
 *			* most of what I have comes from this video tutorial
 *		- course notes (http://oberon.cs.wisc.edu/cs559/Documents/intro_math.pdf)
 *		- http://www.cplusplus.com
 *	
 *	Many of my functions and much of my structure comes from the video tutorial
 *	Implementation was not lifted, but is my own
 */

#include "vec3.h"

/*
 * Empty constructor
 * Working
 */
Vec3::Vec3 () {
	x = 0;
	y = 0;
	z = 0;
};

/*
Vec3 Vec3::relfectOverVector(const Vec3& norm) const{
	Vec3 reflectedVec;
	reflectedVec.set(x , y, z);
	reflectedVec = (-2 * norm);
	reflectedVec = ((norm * (-2) * reflectedVec.dotProd(norm)) + reflectedVec;
	return reflectedVec.normalize();
	//Normal of a sphere = sphere - yourPosition
}
*/

/*
 * Overloaded constructor
 * Working
 */
Vec3::Vec3 (double x2, double y2, double z2) {
	x = x2;
	y = y2;
	z = z2;
};

/*
 * Return an element of the vector at a certain index
 * Working

 Removed for change to x/y/z doubles -NICK

double Vec3::operator[] (int index) const {
	return v[index];
};
 */
/*
 * Overload addition operator (vector addition)
 * Working
 */
const Vec3 Vec3::operator+ (const Vec3 addvec) {
	Vec3 result;
	
	result.x = x + addvec.x;
	result.y = y + addvec.y;
	result.z = z + addvec.z;

	return result;
};

void Vec3::operator+= (const Vec3 addvec) {
	x = x + addvec.x;
	y = y + addvec.y;
	z = z + addvec.z;
};

void Vec3::operator= (const Vec3 newvec) {
	x = newvec.x;
	y = newvec.y;
	z = newvec.z;
};

/*
 * Overload subtraction operator (vector subtraction)
 * Working
 */
const Vec3 Vec3::operator- (const Vec3 subvec) {
	Vec3 result;
	
	result.x = x - subvec.x;
	result.y = y - subvec.y;
	result.z = z - subvec.z;

	return result;
};

void Vec3::operator-= (const Vec3 subvec) {
	x = x - subvec.x;
	y = y - subvec.y;
	z = z - subvec.z;
};

/*
 * Overload multiplication operator (scalar multiplication)
 * Working
 *    -0 case is strange, but should be ok...
 */
const Vec3 Vec3::operator* (const double scale) {
	Vec3 result;

	result.x = x * scale;
	result.y = y * scale;
	result.z = z * scale;

	return result;
};

/*
 * Overload division operator (scalar division)
 * Working
 *	  -0 case is strange but should be ok...
 *    should add error-handling for division by 0
 */
const Vec3 Vec3::operator/ (const double scale) {
	Vec3 result;

	result.x = x / scale;
	result.y = y / scale;
	result.z = z / scale;

	return result;
};

void Vec3::set(double x, double y, double z) {
	x = x;
	y = y;
	z = z;
};

/*
 * Calculate vector magnitude (length)
 * Working
 */
double Vec3::magnitude () const {
	double magnitude;
	double squares;

	squares = pow(x, 2) + pow(y, 2) + pow(z, 2);
	magnitude = sqrt(squares);

	return magnitude;
};

/*
 * Normalize the given vector
 * Working
 *    should add error-handling for division by 0
 */
Vec3 Vec3::normalize () const {
	Vec3 normal;
	double magnitude;

	magnitude = Vec3::magnitude();

	normal.x = x / magnitude;
	normal.y = y / magnitude;
	normal.z = z / magnitude;

	return normal;
};

/*
 * Dot product of given vector with another (passed) vector
 * Working
 */
double Vec3::dotProd (const Vec3 other) const {
	double result;

	result = x*other.x + y*other.y + z*other.z;

	return result;
};

/*
 * Cross product of given vector with another (passed) vector
 * Working
 */
Vec3 Vec3::crossProd (const Vec3 other) const {
	Vec3 result;

	result.x = y*other.z - z*other.y;
	result.y = z*other.x - x*other.z;
	result.z = x*other.y - y*other.x;

	return result;
};

/*
 * Print the vector's elements
 * Working
 */
void Vec3::vecPrint () const {
	cout << "\nelement 1: " << x << endl;
	cout << "element 2: " << y << endl;
	cout << "element 3: " << z << endl;
};