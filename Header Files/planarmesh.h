#ifndef	PLANARMESH_H
#define	PLANARMESH_H
#include "vec3.h"

/*
	Builds a planar mesh suitable for use with OpenGL vector arrays.

	Perry Kivolowitz - University of Wisconsin CS 559
*/

class PlanarMesh
{
public:
	typedef struct
	{
		int V1;
		int V2;
		int V3;
	} TRIANGLE;

	enum WhichArray
	{
		InArray,
		OutArray
	};

	enum MeshStyle
	{
		Regular,
		Flexible
	};

	PlanarMesh(int xDensity, int yDensity, MeshStyle meshStyle = Regular);
	PlanarMesh();
	~PlanarMesh();
	void Draw(WhichArray whichArray = InArray, bool drawNormals = false);
	void ResizeMesh(int xDensity, int yDensity, MeshStyle meshStyle = Regular);
	void GetDensity(int & x, int & y);
	const char * BoundsString();

	Vec3 * GetInArray();
	Vec3 * GetOutArray();
	Vec3 * GetInTextureCoordinates();
	Vec3 * GetOutTextureCoordinates();
	Vec3 * GetInNormals();
	Vec3 * GetOutNormals();


private:
	Vec3		* VInArray;			// A pristine grid.
	Vec3		* VTInArray;		// The unchanging texture coordinate grid.
	Vec3		* VTOutArray;		// A changable texture coordinate array.
	Vec3		* VOutArray;		// A potentially modified grid.
	TRIANGLE	* TArray;			// An array defining the triangles in the grid.
	Vec3		* NInArray;			// A printing grid of NORMALS
	Vec3		* NOutArray;		// A potentially modified grid of NORMALS

	int xDensity;
	int yDensity;

	void MakeMesh(MeshStyle meshStyle);
	void ReleaseMemory();
	void AllocateMemory();

	char boundsBuffer[1024]; // Used with sprintf_s.
};

#endif
