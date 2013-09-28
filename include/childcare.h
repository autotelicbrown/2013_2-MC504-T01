#ifndef _CHILD_CARE_H_
#define _CHILD_CARE_H_

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "SDL2/SDL.h"
#include "sdldrawing.h"

void * Child( void * v );
void * Adult( void * v );

int GetWaitingChildTotal();
int GetActiveChildTotal();
int GetWaitingAdultTotal();
int GetActiveAdultTotal();

#endif