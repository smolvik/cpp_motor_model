#include "tgaplotter.h"

TGAPlotter::TGAPlotter(const std::string fn, const double xsc, const Vec3d ysc)
{
	fname = fn;
	
	y0 = Vec3d(0,0,0);
	x0 = 0;
	
	yscale = Vec3d(width/ysc.x, width/ysc.y, width/ysc.z);
	xscale = height/xsc;
	
	image = new TGAImage(width, height, TGAImage::RGB);
	
	for(int i =0; i <= height; i += height/10){
		//line(0, i, width, i, TGAColor(255, 255, 255, 0));
		for(int j = 0; j < width; j+=10){
			image->set(j, i, TGAColor(255, 255, 255, 0));
			image->set(i, j, TGAColor(255, 255, 255, 0));
		}
	}
	
}

TGAPlotter::~TGAPlotter()
{
	image->flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image->write_tga_file(fname.c_str());
	
	delete image;
}

void TGAPlotter::line(int x0, int y0, int x1, int y1, TGAColor color) 
{
	if(abs(y1-y0)<=abs(x1-x0)){
	
		int y = y0;
		int err = 0;	
		int de1 = abs(2*(y1-y0));
		int de2 = abs(2*(x1-x0));
		int dx = (x1>x0)?1:-1;
		int dy = (y1>y0)?1:-1;

		for(int x=x0; x!=x1; x+=dx){
			err += de1;
			if(abs(err) >= abs(x1-x0)) {
				y += dy;
				err -= de2;
			}
			image->set(x, y, color);			
		}
	}
	else{

		int x = x0;
		int err = 0;	
		int de1 = abs(2*(x1-x0));
		int de2 = abs(2*(y1-y0));
		int dy = (y1>y0)?1:-1;
		int dx = (x1>x0)?1:-1;

		for(int y=y0; y!=y1; y+=dy){
			err += de1;
			if(abs(err) >= abs(y1-y0)) {
				x += dx;
				err -= de2;
			}
			image->set(x, y, color);			
		}		

	}		
}

void TGAPlotter::operator() (const Vec3d &y, const double x){
	//std::cout << x << '\t' << y.x << '\t' << y.y <<'\t' << y.z << std::endl;

	//double sc[3] = {50000,1,0.2};
	//double st = 1000;

	line(x0*xscale, width/2 + y0.x*yscale.x, x*xscale, width/2 + y.x*yscale.x, red);
	line(x0*xscale, width/2 + y0.y*yscale.y, x*xscale, width/2 + y.y*yscale.y, green);
	line(x0*xscale, width/2 + y0.z*yscale.z, x*xscale, width/2 + y.z*yscale.z, blue);
	
	y0 = y;
	x0 = x;
}
