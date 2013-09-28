#ifndef _SDL_DATA_H_
#define _SDL_DATA_H_

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "childcare.h"

// Functions for childs and adults get themselves drawn
// The Add returns a position that should be used on Remove
// Always balance an Add with a Remove
int  Screen_AddWaitingChild();
void Screen_RemoveWaitingChild( int pos );

int  Screen_AddActiveChild();
void Screen_RemoveActiveChild( int pos );

int  Screen_AddWaitingAdult();
void Screen_RemoveWaitingAdult( int pos );

int  Screen_AddActiveAdult();
void Screen_RemoveActiveAdult( int pos );

// Helper functions
int InitMySDL();
void CleanMySDL();

// Must be called by the main thread, loops until Quit event
void DrawAndEventLoop();
#endif /*_SDL_DATA_H_*/
