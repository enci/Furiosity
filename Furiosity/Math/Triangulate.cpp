#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Triangulate.h"

using namespace Furiosity;

static const float EPSILON = 0.0000000001f;

float Triangulate::Area(const std::vector<Vector2> &contour)
{

	size_t n = contour.size();

	float area = 0.0f;

	for (size_t p = n - 1, q = 0; q < n; p = q++)
    {
        // Cross product:
		area += contour[p].x * contour[q].y - contour[q].x * contour[p].y;
	}
    
	return area * 0.5f;
}

/// InsideTriangle decides if a point P is Inside of the triangle
/// defined by A, B, C.
bool Triangulate::InsideTriangle(float Ax, float Ay, float Bx, float By,
		float Cx, float Cy, float Px, float Py) {

	float ax = Cx - Bx;
	float ay = Cy - By;
	float bx = Ax - Cx;
	float by = Ay - Cy;
	float cx = Bx - Ax;
	float cy = By - Ay;
	float apx = Px - Ax;
	float apy = Py - Ay;
	float bpx = Px - Bx;
	float bpy = Py - By;
	float cpx = Px - Cx;
	float cpy = Py - Cy;

	const float aCROSSbp = ax * bpy - ay * bpx;
	const float cCROSSap = cx * apy - cy * apx;
	const float bCROSScp = bx * cpy - by * cpx;

	return (aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f);
}

bool Triangulate::Snip(const std::vector<Vector2> &contour,
                       int u, int v, int w, int n, int *V)
{
	float Ax = contour[V[u]].x;
	float Ay = contour[V[u]].y;

	float Bx = contour[V[v]].x;
	float By = contour[V[v]].y;

	float Cx = contour[V[w]].x;
	float Cy = contour[V[w]].y;

	if (EPSILON > (((Bx - Ax) * (Cy - Ay)) - ((By - Ay) * (Cx - Ax)))) {
		return false;
	}

	for (int p = 0; p < n; p++)
    {
		if ((p == u) || (p == v) || (p == w))
        {
			continue;
        }
        
		float Px = contour[V[p]].x;
		float Py = contour[V[p]].y;
        
		if (InsideTriangle(Ax, Ay, Bx, By, Cx, Cy, Px, Py))
        {
			return false;
        }
	}

	return true;
}

std::vector<unsigned short> Triangulate::Process(const std::vector<Vector2> &contour)
{
	// allocate and initialize list of Vertices in polygon
    std::vector<unsigned short> result;
    
	const int n = (int)contour.size();
	
    if (n < 3)
    {
        LOG("Polygon needs at least 3 vertices.");
		return result; // Empty collection.
	}
    
    // Early out:
    if(n == 3)
    {
        result.push_back(0);
        result.push_back(1);
        result.push_back(2);
        return result;
    }
    
    
	std::vector<int> V;
	V.resize(n);

	// we want a counter-clockwise polygon in V
    // TODO: perhaps always assume a certain direction? Calculating the
    // area eats useless cpu cycles.
	if (0.0f < Area(contour)) {
		for (int v = 0; v < n; v++) {
			V[v] = v;
		}
	} else {
		for (int v = 0; v < n; v++) {
			V[v] = (n - 1) - v;
		}
    }
    
	int nv = n;

	//  remove nv-2 Vertices, creating 1 triangle every time
	int count = 2 * nv; // error detection

	for (int m = 0, v = nv - 1; nv > 2; )
    {
		// three consecutive vertices in current polygon, <u,v,w>
		int u = v;

		if (nv <= u) {
			u = 0; // previous
        }

		v = u + 1;
		if (nv <= v) {
			v = 0; // new v
        }
        
		int w = v + 1;
		if (nv <= w) {
			w = 0; // next
        }

        // "Snip" returned false during the prevous iteration. Something
        // must've gone wrong.
		if (0 >= (count--))
        {
            LOG("Triangulate: ERROR - probable bad polygon!");
            
            const Vector2& vU = contour[V[u]];
            const Vector2& vV = contour[V[v]];
            const Vector2& vW = contour[V[w]];
            
            if(vU == vV || vU == vW || vV == vW) {
                LOG("The polygon contains duplicate coordinates."
                    " A triangle will be skipped.");
            } else {
                LOG("The polygon is too complex to draw. E.g., lines "
                    "cross. A triangle will be skipped.");
            }
            
            LOG("Coordinates: U x:%f y:%f", vU.x, vU.y);
            LOG("Coordinates: V x:%f y:%f", vV.x, vV.y);
            LOG("Coordinates: W x:%f y:%f", vW.x, vW.y);
            
            // No idea what I'm doing here. D'oh.
            m++;
            nv--;
            
            // It makes more sense to render a half-baked polygon than
            // to completely hide it.
			//return result; // empty collection
		}

		if (Snip(contour, u, v, w, nv, &V[0]))
        {
			// true names of the vertices 
			const int a = V[u];
			const int b = V[v];
			const int c = V[w];

			// Output with vertices:
			//result.push_back(contour[a]);
			//result.push_back(contour[b]);
			//result.push_back(contour[c]);
            
            // Return indices. This is OpenGL friendly.
            result.push_back(a);
			result.push_back(b);
			result.push_back(c);
            
            //LOG("Pushing indices: %i %i %i", a ,b ,c);

			m++;

			// remove v from remaining polygon 
			for (int s = v, t = v + 1; t < nv; s++, t++)
            {
				V[s] = V[t];
			}
            
			nv--;

			// reset error detection counter
			count = 2 * nv;
		}
	}

	return result;
}
