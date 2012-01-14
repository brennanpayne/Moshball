#include "rangeRand.h"

double rangeRand(double min, double max) {

	double t = ((double)rand())/((double)(RAND_MAX));

	return t*(max-min) + min;

}
