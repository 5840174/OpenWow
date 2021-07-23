#include "stdafx.h"

struct Result
{
	float distance, sqrDistance;
	float parameter[3];  // barycentric coordinates for triangle.v[3]
	glm::vec3 closest;
};

inline void GetMinEdge02(const float& a11, const float& b1, glm::vec2 p)
{
	p[0] = 0.0f;
	if (b1 >= 0.0f)
	{
		p[1] = 0.0f;
	}
	else if (a11 + b1 <= 0.0f)
	{
		p[1] = 1.0f;
	}
	else
	{
		p[1] = -b1 / a11;
	}
}

inline void GetMinEdge12(const float& a01, const float& a11, const float& b1, const float& f10, const float& f01, glm::vec2& p)
{
	float h0 = a01 + b1 - f10;
	if (h0 >= 0.0f)
	{
		p[1] = 0.0f;
	}
	else
	{
		float h1 = a11 + b1 - f01;
		if (h1 <= 0.0f)
		{
			p[1] = 1.0f;
		}
		else
		{
			p[1] = h0 / (h0 - h1);
		}
	}
	p[0] = 1.0f - p[1];
}

inline void GetMinInterior(glm::vec2 const& p0, const float& h0, glm::vec2 const& p1, const float& h1, glm::vec2& p)
{
	float z = h0 / (h0 - h1);
	p = (1.0f - z) * p0 + z * p1;
}

struct Triangle
{
	glm::vec3 v[3];
};

Result Calculate(glm::vec3 const& point, Triangle const& triangle)
{
	glm::vec3 diff = point - triangle.v[0];
	glm::vec3 edge0 = triangle.v[1] - triangle.v[0];
	glm::vec3 edge1 = triangle.v[2] - triangle.v[0];
	float a00 = glm::dot(edge0, edge0);
	float a01 = glm::dot(edge0, edge1);
	float a11 = glm::dot(edge1, edge1);
	float b0 = (-1.0f) * glm::dot(diff, edge0);
	float b1 = (-1.0f) * glm::dot(diff, edge1);

	float f00 = b0;
	float f10 = b0 + a00;
	float f01 = b0 + a01;

	glm::vec2 p0, p1, p;
	float dt1, h0, h1;

	// Compute the endpoints p0 and p1 of the segment.  The segment is
	// parameterized by L(z) = (1-z)*p0 + z*p1 for z in [0,1] and the
	// directional derivative of half the quadratic on the segment is
	// H(z) = glm::dot(p1-p0,gradient[Q](L(z))/2), where gradient[Q]/2 =
	// (F,G).  By design, F(L(z)) = 0 for cases (2), (4), (5), and
	// (6).  Cases (1) and (3) can correspond to no-intersection or
	// intersection of F = 0 with the triangle.
	if (f00 >= 0.0f)
	{
		if (f01 >= 0.0f)
		{
			// (1) p0 = (0,0), p1 = (0,1), H(z) = G(L(z))
			GetMinEdge02(a11, b1, p);
		}
		else
		{
			// (2) p0 = (0,t10), p1 = (t01,1-t01),
			// H(z) = (t11 - t10)*G(L(z))
			p0[0] = 0.0f;
			p0[1] = f00 / (f00 - f01);
			p1[0] = f01 / (f01 - f10);
			p1[1] = 1.0f - p1[0];
			dt1 = p1[1] - p0[1];
			h0 = dt1 * (a11 * p0[1] + b1);
			if (h0 >= 0.0f)
			{
				GetMinEdge02(a11, b1, p);
			}
			else
			{
				h1 = dt1 * (a01 * p1[0] + a11 * p1[1] + b1);
				if (h1 <= 0.0f)
				{
					GetMinEdge12(a01, a11, b1, f10, f01, p);
				}
				else
				{
					GetMinInterior(p0, h0, p1, h1, p);
				}
			}
		}
	}
	else if (f01 <= 0.0f)
	{
		if (f10 <= 0.0f)
		{
			// (3) p0 = (1,0), p1 = (0,1), H(z) = G(L(z)) - F(L(z))
			GetMinEdge12(a01, a11, b1, f10, f01, p);
		}
		else
		{
			// (4) p0 = (t00,0), p1 = (t01,1-t01), H(z) = t11*G(L(z))
			p0[0] = f00 / (f00 - f10);
			p0[1] = 0.0f;
			p1[0] = f01 / (f01 - f10);
			p1[1] = 1.0f - p1[0];
			h0 = p1[1] * (a01 * p0[0] + b1);
			if (h0 >= 0.0f)
			{
				p = p0;  // GetMinEdge01
			}
			else
			{
				h1 = p1[1] * (a01 * p1[0] + a11 * p1[1] + b1);
				if (h1 <= 0.0f)
				{
					GetMinEdge12(a01, a11, b1, f10, f01, p);
				}
				else
				{
					GetMinInterior(p0, h0, p1, h1, p);
				}
			}
		}
	}
	else if (f10 <= 0.0f)
	{
		// (5) p0 = (0,t10), p1 = (t01,1-t01),
		// H(z) = (t11 - t10)*G(L(z))
		p0[0] = 0.0f;
		p0[1] = f00 / (f00 - f01);
		p1[0] = f01 / (f01 - f10);
		p1[1] = 1.0f - p1[0];
		dt1 = p1[1] - p0[1];
		h0 = dt1 * (a11 * p0[1] + b1);
		if (h0 >= 0.0f)
		{
			GetMinEdge02(a11, b1, p);
		}
		else
		{
			h1 = dt1 * (a01 * p1[0] + a11 * p1[1] + b1);
			if (h1 <= 0.0f)
			{
				GetMinEdge12(a01, a11, b1, f10, f01, p);
			}
			else
			{
				GetMinInterior(p0, h0, p1, h1, p);
			}
		}
	}
	else
	{
		// (6) p0 = (t00,0), p1 = (0,t11), H(z) = t11*G(L(z))
		p0[0] = f00 / (f00 - f10);
		p0[1] = 0.0f;
		p1[0] = 0.0f;
		p1[1] = f00 / (f00 - f01);
		h0 = p1[1] * (a01 * p0[0] + b1);
		if (h0 >= 0.0f)
		{
			p = p0;  // GetMinEdge01
		}
		else
		{
			h1 = p1[1] * (a11 * p1[1] + b1);
			if (h1 <= 0.0f)
			{
				GetMinEdge02(a11, b1, p);
			}
			else
			{
				GetMinInterior(p0, h0, p1, h1, p);
			}
		}
	}

	Result result;
	result.parameter[0] = 1.0f - p[0] - p[1];
	result.parameter[1] = p[0];
	result.parameter[2] = p[1];
	result.closest = triangle.v[0] + p[0] * edge0 + p[1] * edge1;
	diff = point - result.closest;
	result.sqrDistance = glm::dot(diff, diff);
	result.distance = std::sqrt(result.sqrDistance);
	return result;
}