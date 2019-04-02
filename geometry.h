#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <vector>

struct Vec3d{
	double x,y,z;	
	
	Vec3d() : x(0), y(0), z(0) {}
	Vec3d(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
	
    Vec3d 	operator ^(const Vec3d &v) const { return Vec3d(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
    Vec3d 	operator +(const Vec3d &v) const { return Vec3d(x+v.x, y+v.y, z+v.z); }
    Vec3d 	operator -(const Vec3d &v) const { return Vec3d(x-v.x, y-v.y, z-v.z); }
    Vec3d 	operator *(double f)       const { return Vec3d(x*f, y*f, z*f); }
    double	operator *(const Vec3d &v) const { return x*v.x + y*v.y + z*v.z; }
    double norm () const { return std::sqrt(x*x+y*y+z*z); }
	double& operator[](const int i) { return i<=0 ? x : (1==i ? y : z); }
};

class quadrature_trapez{
	double a;
	double xp;
	double dt;
	
	public:
	quadrature_trapez(double dt) : dt(dt) { 
		a = 0.0; 
		xp = 0.0;
	}
	
	double operator() (double x){
		a += (dt/2)*(x + xp);
		xp = x;
		return a;
	}
};

#endif //__GEOMETRY_H__
