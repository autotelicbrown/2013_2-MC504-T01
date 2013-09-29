#include "childcare.h"
static sem_t mutex, adultQueue, childQueue;
static int children = 0, waiting = 0, adults = 0, leaving = 0;

void ChildcareInit ()
{
	sem_init (&mutex, 0, 1);
	sem_init (&adultQueue, 0, 0);
	sem_init (&childQueue, 0, 0);
}

void * Child (void *v) {
	int i;
	sem_wait(&mutex);
	if (children < 3 * adults) {
		children++;
		sem_post(&mutex);
	}
	else {
		waiting++;
		sem_post(&mutex);
		i = Screen_AddWaitingChild();
		sem_wait(&childQueue);
		Screen_RemoveWaitingChild( i );
	}

	i = Screen_AddActiveChild();
	SDL_Delay( ( rand() % 6000 ) + 2000 );

	sem_wait(&mutex);
	children--;
	if (leaving && children <= 3*(adults-1)) {
		leaving--;
		adults--;
		sem_post(&adultQueue);
	}
	else if (waiting) {
		waiting--;
		children++;
		sem_post(&childQueue);
	}
	sem_post(&mutex);

	Screen_RemoveActiveChild( i );

	return 0;
}

void * Adult (void *v) {
	int i;
	sem_wait(&mutex);
	adults++;

	if (waiting) {
		for (i = 0; i < 3; i++) {
			if (waiting) {
				waiting--;
				children++;
				sem_post(&childQueue);
			}
			else
				break;
		}
	}
	else if (leaving) {
		adults--;
		leaving--;
		sem_post(&adultQueue);
	}

	sem_post(&mutex);
	i = Screen_AddActiveAdult();

	SDL_Delay( ( rand() % 10000 ) + 4444 );

	sem_wait(&mutex);
	Screen_RemoveActiveAdult( i );
	if (children <= 3*(adults-1)) {
		adults--;
		sem_post(&mutex);
	}
	else {
		leaving++;
		sem_post(&mutex);
		i = Screen_AddWaitingAdult();
		sem_wait(&adultQueue);
		Screen_RemoveWaitingAdult( i );
	}

	return 0;
}

int GetWaitingChildTotal() {
       	return waiting;
}

int GetActiveChildTotal() {
       	return children;
}

int GetWaitingAdultTotal() {
       	return leaving;
}

int GetActiveAdultTotal() {
       	return adults - leaving;
}
