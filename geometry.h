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

/**
 * @brief
 * 3D вектор
 */
template<class T>
struct Vec3{
	T x,y,z;	
	
	Vec3() : x(0), y(0), z(0) {}
	Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
	/** @brief Векторное произведение двух векторов */
    Vec3 	operator ^(const Vec3 &v) const { return Vec3(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
    /** @brief Сложение двух векторов */
    Vec3 	operator +(const Vec3 &v) const { return Vec3(x+v.x, y+v.y, z+v.z); }
    /** @brief Вычитание двух векторов */
    Vec3 	operator -(const Vec3 &v) const { return Vec3(x-v.x, y-v.y, z-v.z); }
    /** @brief Масштабирование вектора */
    Vec3 	operator *(T f)       const { return Vec3(x*f, y*f, z*f); }
    /** @brief Скалярное произведение двух векторов */
    T	operator *(const Vec3 &v) const { return x*v.x + y*v.y + z*v.z; }
    /** @brief Длина вектора */
    T norm () const { return std::sqrt(x*x+y*y+z*z); }
	T& operator[](const int i) { return i<=0 ? x : (1==i ? y : z); }
	friend std::ostream& operator<<(std::ostream& s, Vec3& v){s<<v.x<<":"<<v.y<<":"<<v.z; return s;}
};

typedef Vec3<int32_t> Vec3i;
typedef Vec3<double> Vec3d;	

/**
 * @brief
 * Численный вычислитель квадратур методом трапеций
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
	/**
	 * @brief интегрирование
	 */
	double operator() (double x){
		a += (dt/2)*(x + xp);
		xp = x;
		return a;
	}
	
	/**
	 * @brief интегрирование с насыщением
	 */	
	double operator() (double x, bool fs){
		double d = (dt/2)*(x + xp);
		xp = x;

		if(fs) if( ((a>0)&&(d>0))||((a<0)&&(d<0)) ) d = 0.0;

		a += d;
		return a;
	}	
};

#endif //__GEOMETRY_H__
