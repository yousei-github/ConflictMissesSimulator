#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H

#define OS WIN32

#define WIN32 1
#define LINUX 2

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <limits.h>
#include <stdint.h> 

#if (OS == WIN32)
#include <time.h>
#elif (OS == LINUX)
#include <sys/time.h>
#endif



#endif 
