#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <iostream>
#include <stdint.h>
#include <math.h>
#include <cmath>
#include <vector>
#include "cos_tb.h"
#include <float.h>

static inline bool isdzero(double a)
{
	if (abs(a) <= DBL_EPSILON * abs(a)){
        return true;
    }
    return false;
}

static inline int sign(double x){return (x<0.0)?-1:1;}

template<class T>
struct Vec3{
	T x,y,z;	
	
	Vec3() : x(0), y(0), z(0) {}
	Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
	
    Vec3 	operator ^(const Vec3 &v) const { return Vec3(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
    Vec3 	operator +(const Vec3 &v) const { return Vec3(x+v.x, y+v.y, z+v.z); }
    Vec3 	operator -(const Vec3 &v) const { return Vec3(x-v.x, y-v.y, z-v.z); }
    Vec3 	operator *(T f)       const { return Vec3(x*f, y*f, z*f); }
    T	operator *(const Vec3 &v) const { return x*v.x + y*v.y + z*v.z; }
    T norm () const { return std::sqrt(x*x+y*y+z*z); }
	T& operator[](const int i) { return i<=0 ? x : (1==i ? y : z); }
	friend std::ostream& operator<<(std::ostream& s, Vec3& v){s<<v.x<<":"<<v.y<<":"<<v.z; return s;}
};

typedef Vec3<int32_t> Vec3i;
typedef Vec3<double> Vec3d;	

/*
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
	friend std::ostream& operator<<(std::ostream& s, Vec3d& v){s<<v.x<<":"<<v.y<<":"<<v.z; return s;}
};
*/

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
	
	double operator() (double x, bool fs){
		double d = (dt/2)*(x + xp);
		xp = x;

		if(fs) if( ((a>0)&&(d>0))||((a<0)&&(d<0)) ) d = 0.0;

		a += d;
		return a;
	}	
};

#endif //__GEOMETRY_H__
