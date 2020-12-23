#ifndef __VECTOR_H
#define  __VECTOR_H

struct Vector3D
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;
};

inline Vector3D Vector_Add(Vector3D& v1, Vector3D& v2)
{
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

inline Vector3D Vector_Sub(Vector3D& v1, Vector3D& v2)
{
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

inline Vector3D Vector_Mul(Vector3D& v1, float k)
{
	return { v1.x * k, v1.y * k, v1.z * k };
}

inline Vector3D Vector_Div(Vector3D& v1, float k)
{
	return { v1.x / k, v1.y / k, v1.z / k };
}

inline float Vector_DotProduct(Vector3D& v1, Vector3D& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline float Vector_Length(Vector3D& v)
{
	return sqrtf(Vector_DotProduct(v, v));
}

inline Vector3D Vector_Normalise(Vector3D& v)
{
	float l = Vector_Length(v);
	return { v.x / l, v.y / l, v.z / l };
}

inline Vector3D Vector_CrossProduct(Vector3D& v1, Vector3D& v2)
{
	Vector3D v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}
Vector3D Vector_IntersectPlane(Vector3D& plane_p, Vector3D& plane_n, Vector3D& lineStart, Vector3D& lineEnd)
{
	plane_n = Vector_Normalise(plane_n);
	float plane_d = -Vector_DotProduct(plane_n, plane_p);
	float ad = Vector_DotProduct(lineStart, plane_n);
	float bd = Vector_DotProduct(lineEnd, plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	Vector3D lineStartToEnd = Vector_Sub(lineEnd, lineStart);
	Vector3D lineToIntersect = Vector_Mul(lineStartToEnd, t);
	return Vector_Add(lineStart, lineToIntersect);
}

#endif

