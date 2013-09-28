#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "SDL2/SDL.h"
#include "sdldrawing.h"

struct MakerParams {
	int minMakeDelay;
	int maxMakeDelay;
};

// Parameter holds delay between created children
void * MakeChild( void * v ) {
	pthread_t thrChild;
	struct MakerParams * param = (struct MakerParams *) v;
	int waitTime = 0;

	while(1) {
		waitTime = rand()%(param->maxMakeDelay - param->minMakeDelay);
		waitTime += param->minMakeDelay;
		SDL_Delay( waitTime );
		// call child
		pthread_create( &thrChild, NULL, Child, NULL );
	}

	return 0;
}

// Parameter holds delay between created adults
void * MakeAdult( void * v ) {
	pthread_t thrAdult;
	struct MakerParams * param = (struct MakerParams *) v;
	int waitTime = 0;

	while(1) {
		waitTime = rand()%(param->maxMakeDelay - param->minMakeDelay);
		waitTime += param->minMakeDelay;
		SDL_Delay( waitTime );
		// call adult
		pthread_create( &thrAdult, NULL, Adult, NULL );
	}
	return 0;
}

int main( int argc, char* argv[] ) {
	pthread_t thrMakeChild, thrMakeAdult;
	struct MakerParams pmMakeChild, pmMakeAdult;

	if( argc != 5 ) {
		fprintf( stderr, 
		  "Usage: ChildCare minMakeDelay(child) maxMakeDelay(child) minMakeDelay(adult) maxMakeDelay(adult)\n");
		return 1;
	}
	pmMakeChild.minMakeDelay = atol( argv[1] );
	pmMakeChild.maxMakeDelay = atol( argv[2] );
	pmMakeAdult.minMakeDelay = atol( argv[3] );
	pmMakeAdult.maxMakeDelay = atol( argv[4] );

	srand( time( NULL ) );

	// Initialize SDL related stuff
	if( !InitMySDL() )
		return 1;

	// Create the child care problem threads
	pthread_create( &thrMakeChild, NULL, MakeChild, &pmMakeChild );
	pthread_create( &thrMakeAdult, NULL, MakeAdult, &pmMakeAdult );

	// Enter the draw and event loop (must be main thread)
	DrawAndEventLoop();

	// Kill everybody
//	pthread_join( thrMakeChild, NULL );
//	pthread_join( thrMakeAdult, NULL );

	// Clean SDL related stuff
	CleanMySDL();

	return 0;
}
