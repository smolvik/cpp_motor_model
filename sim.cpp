/** @file 
 * Пример использования класса Driver.
 * Симуляция работы привода в реальном времени
 * 
 * */

#include <iostream>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include "driver.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>
#include "oscilloscope.h"

#define MY_PORT 12345

pthread_mutex_t vex_mutex = PTHREAD_MUTEX_INITIALIZER;

int start_server(char *hname, int port)
{
	struct sockaddr_in local;
	int s,rc;
	int yes = 1;	
	
	bzero(&local, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_port = htons(port);	
	
	if(hname != 0){
		inet_aton(hname, &local.sin_addr);
	}else{
		local.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	
	
	s = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(s <0)
	{
		perror("socket error");
		exit(1);		
	}
	
	//fcntl(s, F_SETFL, O_NONBLOCK); 
	

	if ( setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 )
	{
		perror("setsockopt");
		exit(1);				
	}	
	
	rc = bind(s, (struct sockaddr*) &local, sizeof(struct sockaddr_in));
	if(rc < 0)
	{
		perror("bind error");
		exit(1);				
	}
	
	return s;
	
}

double get_ticks()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec*1.0 + tv.tv_usec*1e-6;
}

int wndsz = 0;
double *wndbuf;
double *wndbuf2;

void* getref_proc(void *v)
{
	int sfd = start_server(0, MY_PORT);

	char mes[256];
	int msz;
	struct sockaddr_in ca;	
	
	for(;;){
		socklen_t n = sizeof(struct sockaddr_in);		
		if((msz = recvfrom(sfd, mes, 256, 0, (sockaddr*)&ca, &n))>0){
			mes[msz] = 0;
			double x = atof(mes);
			
			pthread_mutex_lock(&vex_mutex);
			*(double*)v = x;
			pthread_mutex_unlock(&vex_mutex);
		}
	}
}

int main ()
{
	double dtsim = 4e-6;
	double tmax = 1;
	int nskip = 100;
	double vex = 0.0;
	double vex_thr = 0.0;
	
	Oscilloscope oscill(tmax, dtsim*nskip, -10, 10);
	
	pthread_t  sthr_id;
	pthread_create(&sthr_id, 0, &getref_proc, &vex_thr);
	
	Driver driver(dtsim);
	
	double tsim = get_ticks();

	int loop = 0;
	int fscr = 0;

	while(1)
	{	
		while(get_ticks() > tsim){
			//vex = 3*sin(2*M_PI*freq*tsim);
			//double vex = sign(cos(2*M_PI*freq*tsim));
			if(0 == pthread_mutex_trylock(&vex_mutex)){
				vex = vex_thr;
				pthread_mutex_unlock(&vex_mutex);
			}

			Vec3d vs = driver(vex);

			tsim += dtsim;

			if(++loop == nskip){			
				oscill.update_buffer(1000*vs.x);
				loop = 0;
				fscr = 1;
			}			
		}

		if(fscr){
			fscr = 0;
			oscill.update_screen();
		 }	 
	}	

   return 0;
}     
