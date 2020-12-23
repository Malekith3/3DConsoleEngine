
#ifndef __MY3DENGINE_H
#define __MY3DENGINE_H

#include <algorithm>
#include <fstream>
#include <strstream>
#include "Vector.h"
#include "Matrix.h"
#include "Triangle.h"
#include "olcConsoleGameEngine.h"




struct Mesh
{
	std::vector<Triangle> tris;
	
	bool LoadFromObjectFile(std::string sFilename)
	{
		std::ifstream f(sFilename);
		if (!f.is_open())
			return false;

		// Local cache of verts
		std::vector<Vector3D> verts;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::strstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				Vector3D v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

			if (line[0] == 'f')
			{
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}

		return true;
	}
};



class MyEngine3d : public olcConsoleGameEngine
{
	Mesh meshCube;
	Mat4X4 matProj;

	float fTheta;
	float fYaw;
	
	Vector3D vCamera;
	Vector3D vLookDir;
public:
	MyEngine3d()
	{
		m_sAppName = L"3D Demo";
	}

	CHAR_INFO GetColour(float lum);
	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;

};

CHAR_INFO MyEngine3d::GetColour(float lum)
{
	short bg_col, fg_col;
	wchar_t sym;
	int pixel_bw = (int)(13.0f * lum);
	switch (pixel_bw)
	{
	case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

	case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
	case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
	case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
	case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

	case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
	case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
	case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
	case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

	case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
	case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
	case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
	case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
	default:
		bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
	}

	CHAR_INFO c;
	c.Attributes = bg_col | fg_col;
	c.Char.UnicodeChar = sym;
	return c;
}

bool MyEngine3d::OnUserCreate()
{
	meshCube.LoadFromObjectFile("source\\mountains.obj");
	matProj = Matrix_MakeProjection(90.0f, (float)ScreenHeight() / (float)ScreenWidth(), 0.1f, 1000.0f);
	return true;
}

bool MyEngine3d::OnUserUpdate(float fElapsedTime)
{

	// Clear Screen
	Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

	Vector3D vForward = Vector_Mul(vLookDir, 8.0f * fElapsedTime);

	if (GetKey(VK_UP).bHeld)
		vCamera.y += 8.0f * fElapsedTime;	// Travel Upwards

	if (GetKey(VK_DOWN).bHeld)
		vCamera.y -= 8.0f * fElapsedTime;	// Travel Downwards


	// Dont use these two in FPS mode, it is confusing :P
	if (GetKey(VK_LEFT).bHeld)
		vCamera.x -= 8.0f * fElapsedTime;	// Travel Along X-Axis

	if (GetKey(VK_RIGHT).bHeld)
		vCamera.x += 8.0f * fElapsedTime;	// Travel Along X-Axis
	///////
	// Standard FPS Control scheme, but turn instead of strafe
	if (GetKey(L'W').bHeld)
		vCamera = Vector_Add(vCamera, vForward);

	if (GetKey(L'S').bHeld)
		vCamera = Vector_Sub(vCamera, vForward);

	if (GetKey(L'A').bHeld)
		fYaw -= 2.0f * fElapsedTime;

	if (GetKey(L'D').bHeld)
		fYaw += 2.0f * fElapsedTime;

	// Set up rotation matrices
	Mat4X4 matRotZ, matRotX;
	//fTheta += 1.0f * fElapsedTime;

	Mat4X4 matTrans = Matrix_MakeTranslation(0.0f, 0.0f, 5.0f);
	Mat4X4 matWorld = Matrix_MakeIdentity();
	// Rotation Z
	matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);
	// Rotation X
	matRotX = Matrix_MakeRotationX(fTheta);
	matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX);
	matWorld = Matrix_MultiplyMatrix(matWorld, matTrans);

	//Create "Point At" Matrix for camera
	Vector3D vUp = { 0,1,0 };
	Vector3D vTarget = { 0,0,1 };
	Mat4X4 matCameraRot = Matrix_MakeRotationY(fYaw);
	vLookDir = Matrix_MultiplyVector(matCameraRot, vTarget);
	vTarget = Vector_Add(vCamera, vLookDir);
	Mat4X4 matCamera = Matrix_PointAt(vCamera, vTarget, vUp);

	// Make view matrix from camera
	Mat4X4 matView = Matrix_QuickInverse(matCamera);

	// Store triagles for rastering later
	std::vector<Triangle> vecTrianglesToRaster;

	for (auto tri : meshCube.tris)
	{
		Triangle triProjected, triTransformed, triViewed;

		// World Matrix Transform
		triTransformed.p[0] = Matrix_MultiplyVector(matWorld, tri.p[0]);
		triTransformed.p[1] = Matrix_MultiplyVector(matWorld, tri.p[1]);
		triTransformed.p[2] = Matrix_MultiplyVector(matWorld, tri.p[2]);

		Vector3D normal, line1, line2;

		// Get lines either side of triangle
		line1 = Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
		line2 = Vector_Sub(triTransformed.p[2], triTransformed.p[0]);

		// Take cross product of lines to get normal to triangle surface
		normal = Vector_CrossProduct(line1, line2);

		// You normally need to normalize a normal!
		normal = Vector_Normalise(normal);

		// Get Ray from triangle to camera
		Vector3D vCameraRay = Vector_Sub(triTransformed.p[0], vCamera);



		if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
		{
			// Illumination
			Vector3D light_direction = { 0.0f, 1.0f, -1.0f };
			light_direction = Vector_Normalise(light_direction);

			// How similar is normal to light direction
			float dp = max(0.1f, Vector_DotProduct(light_direction, normal));

			// Choose console colours as required (much easier with RGB)
			CHAR_INFO c = GetColour(dp);
			triTransformed.col = c.Attributes;
			triTransformed.sym = c.Char.UnicodeChar;

			// Convert World Space --> View Space
			triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
			triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
			triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);
			triViewed.sym = triTransformed.sym;
			triViewed.col = triTransformed.col;

			// Clip Viewed Triangle against near plane, this could form two additional
				// additional triangles. 
			int nClippedTriangles = 0;
			Triangle clipped[2];
			nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);
			for (int n = 0; n < nClippedTriangles; n++)
			{
				// Project triangles from 3D --> 2D
				triProjected.p[0] = Matrix_MultiplyVector(matProj, clipped[n].p[0]);
				triProjected.p[1] = Matrix_MultiplyVector(matProj, clipped[n].p[1]);
				triProjected.p[2] = Matrix_MultiplyVector(matProj, clipped[n].p[2]);
				triProjected.col = triTransformed.col;
				triProjected.sym = triTransformed.sym;

				// Scale into view, we moved the normalising into cartesian space
				triProjected.p[0] = Vector_Div(triProjected.p[0], triProjected.p[0].w);
				triProjected.p[1] = Vector_Div(triProjected.p[1], triProjected.p[1].w);
				triProjected.p[2] = Vector_Div(triProjected.p[2], triProjected.p[2].w);

				// X/Y are inverted so put them back
				triProjected.p[0].x *= -1.0f;
				triProjected.p[1].x *= -1.0f;
				triProjected.p[2].x *= -1.0f;
				triProjected.p[0].y *= -1.0f;
				triProjected.p[1].y *= -1.0f;
				triProjected.p[2].y *= -1.0f;
				
				// Scale into view
				Vector3D vOffsetView = { 1,1,0 };
				triProjected.p[0] = Vector_Add(triProjected.p[0], vOffsetView);
				triProjected.p[1] = Vector_Add(triProjected.p[1], vOffsetView);
				triProjected.p[2] = Vector_Add(triProjected.p[2], vOffsetView);
				triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
				triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
				triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

				// Store triangle for sorting
				vecTrianglesToRaster.push_back(triProjected);
			}
		}
	}

	// Sort triangles from back to front
	std::sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](Triangle& t1, Triangle& t2)
		{
			float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
			float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
			return z1 > z2;
		});
	// Clear Screen
	Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

	// Loop through all transformed, viewed, projected, and sorted triangles
	for (auto& triToRaster : vecTrianglesToRaster)
	{
		// Clip triangles against all four screen edges, this could yield
		// a bunch of triangles, so create a queue that we traverse to 
		//  ensure we only test new triangles generated against planes
		Triangle clipped[2];
		std::list<Triangle> listTriangles;

		// Add initial triangle
		listTriangles.push_back(triToRaster);
		int nNewTriangles = 1;

		for (int p = 0; p < 4; p++)
		{
			int nTrisToAdd = 0;
			while (nNewTriangles > 0)
			{
				// Take triangle from front of queue
				Triangle test = listTriangles.front();
				listTriangles.pop_front();
				nNewTriangles--;

				// Clip it against a plane. We only need to test each 
				// subsequent plane, against subsequent new triangles
				// as all triangles after a plane clip are guaranteed
				// to lie on the inside of the plane. I like how this
				// comment is almost completely and utterly justified
				switch (p)
				{
				case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)ScreenHeight() - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)ScreenWidth() - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				}

				// Clipping may yield a variable number of triangles, so
				// add these new ones to the back of the queue for subsequent
				// clipping against next planes
				for (int w = 0; w < nTrisToAdd; w++)
					listTriangles.push_back(clipped[w]);
			}
			nNewTriangles = listTriangles.size();
		}
		for (auto& t : listTriangles)
		{
			// Rasterize triangle
			FillTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, t.sym, t.col);
			//DrawTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, PIXEL_SOLID, FG_BLACK);

		}
	}

		return true;
	}


#endif
