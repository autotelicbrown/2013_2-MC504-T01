#include "childcare.h"

void * Child( void * v ) {
	// Child problem thread
	// Call Add/Remove Waiting/Active Child to update drawing
	// see include/sdldrawing.h
	
	// TEST ONLY CODE - example of interaction with drawing stuff
	int i;
	i = Screen_AddWaitingChild();
	SDL_Delay( 2000 );
	Screen_RemoveWaitingChild( i );
	i = Screen_AddActiveChild();
	SDL_Delay( 6000 );
	Screen_RemoveActiveChild( i );

       	return 0;
}

void * Adult( void * v ) {
	// Adult problem thread
	// Call Add/Remove Waiting/Active Adult to update drawing
	// see include/sdldrawing.h

	// TEST ONLY CODE - example of interaction with drawing stuff
	int i;
	i = Screen_AddActiveAdult();
	SDL_Delay( 6000 );
	Screen_RemoveActiveAdult( i );
	i = Screen_AddWaitingAdult();
	SDL_Delay( 2000 );
	Screen_RemoveWaitingAdult( i );
       	return 0;
}

int GetWaitingChildTotal() {
       	return 1;
}

int GetActiveChildTotal() {
       	return 2;
}

int GetWaitingAdultTotal() {
       	return 3;
}

int GetActiveAdultTotal() {
       	return 7;
}

