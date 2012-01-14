#include <stdio.h>
#include <gl/freeglut.h>
#include "planarmesh.h"

/*	Perry Kivolowitz - University of Wisconsin CS 559
	
	A class to draw a planar mesh grid triangulated in one
	of two ways. The mesh is drawn over +/- 0.5 so scale to
	taste. Vector3s can be drawn to assist in the debugging
	of displacement maps.

	Vector3 arrays are used. The caller is given access to
	both the original data (the planar mesh which should be
	kept pristine) and the output array (which the user must
	fill in). When the mesh is asked to draw itself, the user
	specifies which mesh (in or out) to draw from.

	EDIT- changed all 'Vector3' to 'Vec3' to fit with our vec3 class
*/

// The default constructure which allows all settings
// to be set later.
PlanarMesh::PlanarMesh()
{
	VInArray = VTInArray = VTOutArray = VOutArray = NULL;
	TArray = NULL;
}

// A constructur which sets the settings and builds the
// mesh right away.
PlanarMesh::PlanarMesh(int xDensity, int yDensity, MeshStyle meshStyle)
{
	this->xDensity = xDensity;
	this->yDensity = yDensity;

	VInArray = VTInArray = VTOutArray = VOutArray = NULL;
	TArray = NULL;

	AllocateMemory();
	MakeMesh(meshStyle);
}

PlanarMesh::~PlanarMesh()
{
	ReleaseMemory();
}

// ReleaseMemory is abstracted from the destructor because it
// can then be shared with ResizeMesh().
void PlanarMesh::ReleaseMemory()
{
	if (VInArray != NULL)
		delete [] VInArray;

	if (VOutArray != NULL)
		delete [] VOutArray;

	if (VTInArray != NULL)
		delete [] VTInArray;

	if (VTOutArray != NULL)
		delete [] VTOutArray;

	if (TArray != NULL)
		delete [] TArray;

	if (NInArray != NULL)
		delete [] NInArray;

	if (NOutArray != NULL)
		delete [] NOutArray;

	VInArray = VTInArray = VTOutArray = VOutArray = NULL;
	NInArray = NOutArray = NULL;
	TArray = NULL;
}

// AllocateMemory is similarly abstracted from the constructor allowing
// it to be shared with ResizeMesh.
void PlanarMesh::AllocateMemory()
{
	VInArray = new Vec3[xDensity * yDensity];
	VTInArray = new Vec3[xDensity * yDensity];
	VTOutArray = new Vec3[xDensity * yDensity];
	VOutArray = new Vec3[xDensity * yDensity];
	TArray = new TRIANGLE[(xDensity - 1) * (yDensity - 1) * 2];
	NInArray = new Vec3[xDensity * yDensity];
	NOutArray = new Vec3[xDensity * yDensity];
}

void PlanarMesh::ResizeMesh(int xDensity, int yDensity, MeshStyle meshStyle)
{
	ReleaseMemory();
	this->xDensity = xDensity;
	this->yDensity = yDensity;
	AllocateMemory();
	MakeMesh(meshStyle);
}

// These two inline functions help in defining the "flexible" mesh.
inline bool EVEN(int n)
{
	return ((n % 2) == 0);
}

inline bool ODD(int n)
{
	return ((n & 1) == 1);
}

void PlanarMesh::MakeMesh(MeshStyle meshStyle)
{
	int		y , x , i;
	float		HalfW;
	float		HalfH;
	Vec3 *	v;
	Vec3 *	n;
	TRIANGLE *	t;

	// assigns values to vertices

	HalfW = (xDensity - 1) / 2.0f;
	HalfH = (yDensity - 1) / 2.0f;

	// Define a grid of Vector3es with Vector3s that point down the Z axis.
	// The Vector3 grid ranges over +/- 0.5.

	// We also define a grid of texture coordinates. These simply range
	// from zero to one.
	for (y = 0; y < yDensity; y++)
	{
		for (x = 0; x < xDensity; x++)
		{
			i	= y * xDensity + x;
			v	= &VInArray[i];
			n   = &NInArray[i];

			v->x	= (((float) x) - HalfW) / ((float) xDensity - 1);
			v->y	= (((float) y) - HalfH) / ((float) yDensity - 1);
			v->z	= 0.0;

			n->x	= 0.0;
			n->y	= 0.0;
			n->z	= 1.0;

			VTInArray[i].x = ((float) x) / ((float) xDensity - 1);
			VTInArray[i].y = ((float) y) / ((float) yDensity - 1);
		}
	}

	// To use vector arrays, we must define triangles by indicating which
	// Vector3es live in the same triangle. With more complicated code
	// TRIANGLE_STRIPS could have been drawn force large changes to the
	// following code.

	// This code provides two means of assigning Vector3es to triangles.
	// The two methods produce grids conforming to two schools of thought.
	// For a planar mesh, once a texture is placed on them it won't really
	// make a difference. However, if you start manipulating the components
	// of the mesh (in the VOutArray) one triangulation method might 
	// produce better results than another.

	for (y = 0; y < yDensity - 1; y++)
	{
		for (x = 0; x < xDensity - 1; x++)
		{
			t     = &TArray[(y * (xDensity - 1) + x) * 2];

			if (meshStyle == Flexible)
			{
				if ((EVEN(x) && EVEN(y)) || (ODD(x) && ODD(y)))
				{
					t->V1 = (y + 0) * xDensity + x + 0;
					t->V2 = (y + 0) * xDensity + x + 1;
					t->V3 = (y + 1) * xDensity + x + 0;
					t++;
					t->V1 = (y + 0) * xDensity + x + 1;
					t->V2 = (y + 1) * xDensity + x + 1;
					t->V3 = (y + 1) * xDensity + x + 0;
				}
				else 
				{
					t->V1 = (y + 0) * xDensity + x + 0;
					t->V2 = (y + 0) * xDensity + x + 1;
					t->V3 = (y + 1) * xDensity + x + 1;
					t++;
					t->V1 = (y + 0) * xDensity + x + 0;
					t->V2 = (y + 1) * xDensity + x + 1;
					t->V3 = (y + 1) * xDensity + x + 0;
				}
			}
			else // Regular
			{
				// "top" triangle
				t->V1 = (y + 0) * xDensity + x + 0;
				t->V2 = (y + 0) * xDensity + x + 1;
				t->V3 = (y + 1) * xDensity + x + 0;

				t++;
				// "bottom" triangle
				t->V1 = (y + 0) * xDensity + x + 1;
				t->V2 = (y + 1) * xDensity + x + 1;
				t->V3 = (y + 1) * xDensity + x + 0;
			}
		}
	}
}

// When you ask the mesh to draw itself, you must specify which array
// to draw. There are two arrays. The InArray should always be kept
// pristine. The OutArray might contain new vertices after some type
// of displacement map operation is performed by the calling code.
void PlanarMesh::Draw(WhichArray whichArray, bool drawNormals)
{
	// We will specify array pointers for Vector3es, Vector3s and texture
	// coordinates. Therefore, we must enable OpenGL to process these
	// arrays.
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// These functions specify the array pointers of the various types.
	// Notice we can point to either the IN arrays or the OUT arrays.
	// This permits the use of the OUT array after processing using
	// a displacement map.
	glVertexPointer(3 , GL_DOUBLE , sizeof(Vec3) , (whichArray == InArray) ? &VInArray[0] : &VOutArray[0]);
	glNormalPointer(GL_DOUBLE, sizeof(Vec3), (whichArray == InArray) ? &NInArray[0] : &NOutArray[0]);
	glTexCoordPointer(3 , GL_DOUBLE , sizeof(Vec3) , (whichArray == InArray) ? &VTInArray[0].x : &VTOutArray[0].x);

	// Here is the actual drawing is made.
	glDrawElements(GL_TRIANGLES , (xDensity - 1) * (yDensity - 1) * 2 * 3 , GL_UNSIGNED_INT , TArray);

	// For debugging purposes you might wish to draw the Vector3s at each Vector3.
	if (drawNormals)
	{
		GLboolean lightingState = glIsEnabled(GL_LIGHTING);
		glDisable(GL_LIGHTING);

		GLboolean textureState = glIsEnabled(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_2D);

		glColor3d(1,1,1);

		Vec3 * v = (whichArray == InArray) ? &VInArray[0] : &VOutArray[0];
		Vec3 * n = (whichArray == InArray) ? &NInArray[0] : &NOutArray[0];

		int maxDensity = (xDensity > yDensity) ? xDensity : yDensity;

		glBegin(GL_LINES);
		for (int i = 0; i < xDensity * yDensity; i++)
		{
			glVertex3d(v->x, v->y, v->z);
			glVertex3d(v->x + n->x / maxDensity, v->y + n->y / maxDensity, v->z + n->z / maxDensity);
			// By scaling the length of the Vector3s by the maximum axis density, the Vector3s
			// will get short when the mesh gets dense. This way, if you are doing displacement
			// mapping, the Vector3 vectors will tend to get in each other's way less.
			v++;
			n++;
		}
		glEnd();

		if (lightingState)
			glEnable(GL_LIGHTING);

		if (textureState)
			glEnable(GL_TEXTURE_2D);
	}
}

// All these Get function assist in the creation of displacement
// map code. I acknowledge that returning pointers to the actual
// data breaks the object oriented model and permits destructive
// operations. However, they are liste as "protected".

void PlanarMesh::GetDensity(int & x, int & y)
{
	x = xDensity;
	y = yDensity;
}

Vec3 * PlanarMesh::GetInArray()
{
	return VInArray;
}

Vec3 * PlanarMesh::GetOutArray()
{
	return VOutArray;
}

Vec3 * PlanarMesh::GetInNormals()
{
	return NInArray;
}

Vec3 * PlanarMesh::GetOutNormals()
{
	return NOutArray;
}

const char * PlanarMesh::BoundsString()
{
	sprintf_s<1024>(this->boundsBuffer, "(0,0) == (%f,%f), (%d,%d) == (%f,%f)", VInArray[0].x, VInArray[0].y, this->xDensity, this->yDensity, VInArray[xDensity - 1].x, VInArray[yDensity - 1].y);
	return boundsBuffer;
}

Vec3 * PlanarMesh::GetInTextureCoordinates()
{
	return this->VTInArray;
}

Vec3 * PlanarMesh::GetOutTextureCoordinates()
{
	return this->VTOutArray;
}
