#include "sdldrawing.h"

#define FRAMES_PER_SECOND 20
#define SCREEN_XSIZE 640
#define SCREEN_YSIZE 480

#define WAITING_CHILD_WIDTH 9
#define WAITING_CHILD_HEIGHT 6
#define WAITING_CHILD_BASEX 24
#define WAITING_CHILD_BASEY 16
#define WAITING_CHILD_XSTRIDE 32
#define WAITING_CHILD_YSTRIDE 32

#define ACTIVE_CHILD_WIDTH 9
#define ACTIVE_CHILD_HEIGHT 6
#define ACTIVE_CHILD_BASEX 24
#define ACTIVE_CHILD_BASEY 256
#define ACTIVE_CHILD_XSTRIDE 32
#define ACTIVE_CHILD_YSTRIDE 32

#define WAITING_ADULT_WIDTH 9
#define WAITING_ADULT_HEIGHT 6
#define WAITING_ADULT_BASEX 336
#define WAITING_ADULT_BASEY 16
#define WAITING_ADULT_XSTRIDE 32
#define WAITING_ADULT_YSTRIDE 32

#define ACTIVE_ADULT_WIDTH 9
#define ACTIVE_ADULT_HEIGHT 6
#define ACTIVE_ADULT_BASEX 336
#define ACTIVE_ADULT_BASEY 256
#define ACTIVE_ADULT_XSTRIDE 32
#define ACTIVE_ADULT_YSTRIDE 32

#define MAX_RANDOM_TRIES  5

// Drawing matrix references with mutex
static sem_t mxWaitingChild_mutex;
static int mxWaitingChild[ WAITING_CHILD_WIDTH * WAITING_CHILD_HEIGHT ] = {0}; 
static sem_t mxActiveChild_mutex;
static int mxActiveChild[ ACTIVE_CHILD_WIDTH * ACTIVE_CHILD_HEIGHT ] = {0}; 
static sem_t mxWaitingAdult_mutex;
static int mxWaitingAdult[ WAITING_ADULT_WIDTH * WAITING_ADULT_HEIGHT ] = {0};
static sem_t mxActiveAdult_mutex;
static int mxActiveAdult[ ACTIVE_ADULT_WIDTH * ACTIVE_ADULT_HEIGHT ] = {0};

// Screen and renderer references
static SDL_Window *screen = NULL;
static SDL_Renderer *renderer = NULL;

// Generic image for children and adults (may add different frames)
static SDL_Texture * child_tx = NULL;
static SDL_Texture * adult_tx = NULL;

// Loaded font
static TTF_Font * font = NULL;

// Boolean value for the drawing thread to check if it should end
static sem_t KeepDrawing_mutex;
static int KeepDrawing = 0;

// Generate proper texture for a given text
SDL_Texture * GetTextureFromString( char * text ) {
	SDL_Surface * surface = NULL;
	SDL_Texture * tx = NULL;
	SDL_Color color = {255,255,255};

	// Currently ignoring errors
	surface = TTF_RenderText_Blended( font, text, color );
	tx = SDL_CreateTextureFromSurface( renderer, surface );
	SDL_FreeSurface( surface );
	return tx;
}

// Main draw and event loop
void DrawAndEventLoop() {
	SDL_Event event;
	SDL_Texture * font_tx;
	char font_text[50];
	int x, y, basex, basey, xstride, ystride;
	Uint32 timer = SDL_GetTicks();
	SDL_Rect rect;

	// Start Draw and event loop
	while( 1 ) {
	// Check for termination signal
	sem_wait( &KeepDrawing_mutex );
	if( !KeepDrawing ) {
		sem_post( &KeepDrawing_mutex );
		return;
	}
	sem_post( &KeepDrawing_mutex );

	// Handle events (currently only termination)
	while( SDL_PollEvent( &event ) ) {
		switch( event.type ) {
		case SDL_QUIT:
			sem_wait( &KeepDrawing_mutex );
			KeepDrawing = 0;
			sem_post( &KeepDrawing_mutex );
			break;
		default:
			break;
		}
	}

	// Clear screen
	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
	SDL_RenderClear( renderer );

	// Set fixed background TODO
//	SDL_RenderCopy( renderer, background_tx, NULL, NULL );

	// Set waiting children
	basex = WAITING_CHILD_BASEX;
	basey = WAITING_CHILD_BASEY;
	xstride = WAITING_CHILD_XSTRIDE;
	ystride = WAITING_CHILD_YSTRIDE;
	SDL_QueryTexture( child_tx, NULL, NULL, &rect.w, &rect.h );
	sem_wait( &mxWaitingChild_mutex );
	for( y = 0; y < WAITING_CHILD_HEIGHT; ++y )
		for( x = 0; x < WAITING_CHILD_WIDTH; ++x )
	if( mxWaitingChild[ x + y * WAITING_CHILD_WIDTH ] > 0 ) {
		rect.x = basex + x * xstride;
		rect.y = basey + y * ystride;
		SDL_RenderCopy( renderer, child_tx, NULL, &rect );
	}
	sem_post( &mxWaitingChild_mutex );

	// Set waiting children counter
	sprintf( font_text, "Waiting children:%d", GetWaitingChildTotal() );
	font_tx = GetTextureFromString( font_text );
	SDL_QueryTexture( font_tx, NULL, NULL, &rect.w, &rect.h );
	rect.x = SCREEN_XSIZE / 4 - rect.w / 2;
       	rect.y = SCREEN_YSIZE / 2 - rect.h - 5;
	SDL_RenderCopy( renderer, font_tx, NULL, &rect );
	SDL_DestroyTexture( font_tx );

	// Set active children
	basex = ACTIVE_CHILD_BASEX;
	basey = ACTIVE_CHILD_BASEY;
	xstride = ACTIVE_CHILD_XSTRIDE;
	ystride = ACTIVE_CHILD_YSTRIDE;
	SDL_QueryTexture( child_tx, NULL, NULL, &rect.w, &rect.h );
	sem_wait( &mxActiveChild_mutex );
	for( y = 0; y < ACTIVE_CHILD_HEIGHT; ++y )
		for( x = 0; x < ACTIVE_CHILD_WIDTH; ++x )
	if( mxActiveChild[ x + y * ACTIVE_CHILD_WIDTH ] > 0 ) {
		rect.x = basex + x * xstride;
		rect.y = basey + y * ystride;
		SDL_RenderCopy( renderer, child_tx, NULL, &rect );
	}
	sem_post( &mxActiveChild_mutex );

	// Set active children counter
	sprintf( font_text, "Active children:%d", GetActiveChildTotal() );
	font_tx = GetTextureFromString( font_text );
	SDL_QueryTexture( font_tx, NULL, NULL, &rect.w, &rect.h );
	rect.x = SCREEN_XSIZE / 4 - rect.w / 2;
       	rect.y = SCREEN_YSIZE - rect.h - 5;
	SDL_RenderCopy( renderer, font_tx, NULL, &rect );
	SDL_DestroyTexture( font_tx );

	// Set active adults
	basex = ACTIVE_ADULT_BASEX;
	basey = ACTIVE_ADULT_BASEY;
	xstride = ACTIVE_ADULT_XSTRIDE;
	ystride = ACTIVE_ADULT_YSTRIDE;
	SDL_QueryTexture( child_tx, NULL, NULL, &rect.w, &rect.h );
	sem_wait( &mxActiveAdult_mutex );
	for( y = 0; y < ACTIVE_ADULT_HEIGHT; ++y )
		for( x = 0; x < ACTIVE_ADULT_WIDTH; ++x )
	if( mxActiveAdult[ x + y * ACTIVE_ADULT_WIDTH ] > 0 ) {
		rect.x = basex + x * xstride;
		rect.y = basey + y * ystride;
		SDL_RenderCopy( renderer, adult_tx, NULL, &rect );
	}
	sem_post( &mxActiveAdult_mutex );

	// Set active adults counter
	sprintf( font_text, "Active adults:%d", GetActiveAdultTotal() );
	font_tx = GetTextureFromString( font_text );
	SDL_QueryTexture( font_tx, NULL, NULL, &rect.w, &rect.h );
	rect.x = SCREEN_XSIZE * 3 / 4 - rect.w / 2;
       	rect.y = SCREEN_YSIZE - rect.h - 5;
	SDL_RenderCopy( renderer, font_tx, NULL, &rect );
	SDL_DestroyTexture( font_tx );

	// Set waiting adults
	basex = WAITING_ADULT_BASEX;
	basey = WAITING_ADULT_BASEY;
	xstride = WAITING_ADULT_XSTRIDE;
	ystride = WAITING_ADULT_YSTRIDE;
	SDL_QueryTexture( adult_tx, NULL, NULL, &rect.w, &rect.h );
	sem_wait( &mxWaitingAdult_mutex );
	for( y = 0; y < WAITING_ADULT_HEIGHT; ++y )
		for( x = 0; x < WAITING_ADULT_WIDTH; ++x )
	if( mxWaitingAdult[ x + y * WAITING_ADULT_WIDTH ] > 0 ) {
		rect.x = basex + x * xstride;
		rect.y = basey + y * ystride;
		SDL_RenderCopy( renderer, adult_tx, NULL, &rect );
	}
	sem_post( &mxWaitingAdult_mutex );

	// Set waiting adults counter
	sprintf( font_text, "Waiting adults:%d", GetWaitingAdultTotal() );
	font_tx = GetTextureFromString( font_text );
	SDL_QueryTexture( font_tx, NULL, NULL, &rect.w, &rect.h );
	rect.x = SCREEN_XSIZE * 3 / 4 - rect.w / 2;
       	rect.y = SCREEN_YSIZE / 2 - rect.h - 5;
	SDL_RenderCopy( renderer, font_tx, NULL, &rect );
	SDL_DestroyTexture( font_tx );

	// Finnaly render everything
	SDL_RenderPresent( renderer );
	
	// Wait the time needed to match FPS if not late
	if( SDL_GetTicks() - timer < 1000 / FRAMES_PER_SECOND )
		SDL_Delay( (1000/FRAMES_PER_SECOND) - (SDL_GetTicks()-timer) );
	timer = SDL_GetTicks();

	} /*infinite loop*/
}

int InitMySDL() {
	int i, j;

	// Main SDL initializer
	if( SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS ) < 0 )
		return 0;

	// Create screen
	screen = SDL_CreateWindow("Child Care Problem",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_XSIZE, SCREEN_YSIZE,
			SDL_WINDOW_OPENGL );
	if( screen == NULL)
		return 0;

	// Get renderer
	renderer = SDL_CreateRenderer( screen, -1, 0 );
	if( renderer == NULL )
		return 0;

	// Load SDL_ttf
	if( TTF_Init() < 0 )
		return 0;

	// Load actual font
	font = TTF_OpenFont("res/font/linear-by-braydon-fuller.otf", 30);
	if( font == NULL ) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return 0;
	}

	// Load Textures
	// extra TODO Get a proper image to represent children and adults
	//child_tx = IMG_LoadTexture( renderer, "res/child.png" );
	child_tx = GetTextureFromString( "C" );
	if( child_tx == NULL )
		return 0;
	//adult_tx = IMG_LoadTexture( renderer, "res/adult.png" );
	adult_tx = GetTextureFromString( "A" );
	if( adult_tx == NULL )
		return 0;

	// Initialize Drawing position matrix
	sem_init( &mxActiveAdult_mutex, 0, 1 );
	sem_init( &mxActiveChild_mutex, 0, 1 );
	sem_init( &mxWaitingAdult_mutex, 0, 1 );
	sem_init( &mxWaitingChild_mutex, 0, 1 );

	for( i = 0; i < WAITING_ADULT_HEIGHT; ++i )
		for( j = 0; j < WAITING_ADULT_WIDTH; ++j )
			mxWaitingAdult[ j + i * WAITING_ADULT_WIDTH ] = 0;

	for( i = 0; i < ACTIVE_ADULT_HEIGHT; ++i )
		for( j = 0; j < ACTIVE_ADULT_WIDTH; ++j )
			mxActiveAdult[ j + i * ACTIVE_ADULT_WIDTH ] = 0;

	for( i = 0; i < WAITING_CHILD_HEIGHT; ++i )
		for( j = 0; j < WAITING_CHILD_WIDTH; ++j )
			mxWaitingChild[ j + i * WAITING_CHILD_WIDTH ] = 0;

	for( i = 0; i < ACTIVE_CHILD_HEIGHT; ++i )
		for( j = 0; j < ACTIVE_CHILD_WIDTH; ++j )
			mxActiveChild[ j + i * ACTIVE_CHILD_WIDTH ] = 0;

	// Set the drawing signal
	KeepDrawing = 1;
	sem_init( &KeepDrawing_mutex, 0, 1 );

	return 1;
}

void CleanMySDL() {
	// Destroy the drawing reference semaphore
	sem_destroy( &KeepDrawing_mutex );

	// Destroy matrix semaphores
	sem_destroy( &mxActiveAdult_mutex );
	sem_destroy( &mxWaitingAdult_mutex );
	sem_destroy( &mxActiveChild_mutex );
	sem_destroy( &mxWaitingChild_mutex );

	// Destroy SDL screen and renderer
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( screen );

	// Destroy textures
	SDL_DestroyTexture( child_tx );
	SDL_DestroyTexture( adult_tx );

	// Destroy font
	TTF_CloseFont( font );

	// Quit SDL_ttf
	TTF_Quit();

	// Quit SDL
	SDL_Quit();
}

// Series of synchronized changes on the drawing matrix references
// Extra TODO Add some logic to associate random positions for new elements

int  Screen_AddWaitingChild() {
	int pos, i, j;
	int check = 0;

	// Return invalid position (to be ignored in removal) if drawing halts
	sem_wait( &KeepDrawing_mutex );
	if( !KeepDrawing ) {
		sem_post( &KeepDrawing_mutex );
		return ( WAITING_CHILD_WIDTH * WAITING_CHILD_HEIGHT );
	}
	sem_post( &KeepDrawing_mutex );

	sem_wait( &mxWaitingChild_mutex );
	// Try 5 random positions
	for( j = 0; j < MAX_RANDOM_TRIES; ++j ) {
		i = rand() % ( WAITING_CHILD_WIDTH * WAITING_CHILD_HEIGHT );
		if( mxWaitingChild[i] == 0 ) {
			check = 1;
			break;
		}
	}
	// If unsuccessful, occupy first vacant position
	if( !check )
	for( i = 0; i < WAITING_CHILD_WIDTH * WAITING_CHILD_HEIGHT; ++i )
		if( mxWaitingChild[i] == 0 )
			break;

	pos = i;
	mxWaitingChild[pos] = 1;
	sem_post( &mxWaitingChild_mutex );		

	return pos;
}

void Screen_RemoveWaitingChild( int pos ) {
	// Ignore invalid positions
	if( pos >= WAITING_CHILD_WIDTH * WAITING_CHILD_HEIGHT )
		return;

	// Mark spot as vacant
	sem_wait( &mxWaitingChild_mutex );
	mxWaitingChild[pos] = 0;
	sem_post( &mxWaitingChild_mutex );
}

int  Screen_AddActiveChild() {
	int pos, i, j;
	int check = 0;

	// Return invalid position (to be ignored in removal) if drawing halts
	sem_wait( &KeepDrawing_mutex );
	if( !KeepDrawing ) {
		sem_post( &KeepDrawing_mutex );
		return ( ACTIVE_CHILD_WIDTH * ACTIVE_CHILD_HEIGHT );
	}
	sem_post( &KeepDrawing_mutex );

	sem_wait( &mxActiveChild_mutex );
	// Try 5 random positions
	for( j = 0; j < MAX_RANDOM_TRIES; ++j ) {
		i = rand() % ( ACTIVE_CHILD_WIDTH * ACTIVE_CHILD_HEIGHT );
		if( mxActiveChild[i] == 0 ) {
			check = 1;
			break;
		}
	}
	// If unsuccessful, occupy first vacant position
	if( !check )
	for( i = 0; i < ACTIVE_CHILD_WIDTH * ACTIVE_CHILD_HEIGHT; ++i )
		if( mxActiveChild[i] == 0 )
			break;

	pos = i;
	mxActiveChild[pos] = 1;
	sem_post( &mxActiveChild_mutex );		

	return pos;
}

void Screen_RemoveActiveChild( int pos ) {
	// Ignore invalid positions
	if( pos >= ACTIVE_CHILD_WIDTH * ACTIVE_CHILD_HEIGHT )
		return;

	// Mark spot as vacant
	sem_wait( &mxActiveChild_mutex );
	mxActiveChild[pos] = 0;
	sem_post( &mxActiveChild_mutex );
}

int  Screen_AddWaitingAdult() {
	int pos, i, j;
	int check = 0;

	// Return invalid position (to be ignored in removal) if drawing halts
	sem_wait( &KeepDrawing_mutex );
	if( !KeepDrawing ) {
		sem_post( &KeepDrawing_mutex );
		return ( WAITING_ADULT_WIDTH * WAITING_ADULT_HEIGHT );
	}
	sem_post( &KeepDrawing_mutex );

	sem_wait( &mxWaitingAdult_mutex );
	// Try 5 random positions
	for( j = 0; j < MAX_RANDOM_TRIES; ++j ) {
		i = rand() % ( WAITING_ADULT_WIDTH * WAITING_ADULT_HEIGHT );
		if( mxWaitingAdult[i] == 0 ) {
			check = 1;
			break;
		}
	}
	// If unsuccessful, occupy first vacant position
	if( !check )
	for( i = 0; i < WAITING_ADULT_WIDTH * WAITING_ADULT_HEIGHT; ++i )
		if( mxWaitingAdult[i] == 0 )
			break;
	pos = i;
	mxWaitingAdult[pos] = 1;
	sem_post( &mxWaitingAdult_mutex );		

	return pos;
}

void Screen_RemoveWaitingAdult( int pos ) {
	// Ignore invalid positions
	if( pos >= WAITING_ADULT_WIDTH * WAITING_ADULT_HEIGHT )
		return;

	// Mark spot as vacant
	sem_wait( &mxWaitingAdult_mutex );
	mxWaitingAdult[pos] = 0;
	sem_post( &mxWaitingAdult_mutex );
}

int  Screen_AddActiveAdult() {
	int pos, i, j;
	int check = 0;

	// Return invalid position (to be ignored in removal) if drawing halts
	sem_wait( &KeepDrawing_mutex );
	if( !KeepDrawing ) {
		sem_post( &KeepDrawing_mutex );
		return ( ACTIVE_ADULT_WIDTH * ACTIVE_ADULT_HEIGHT );
	}
	sem_post( &KeepDrawing_mutex );

	sem_wait( &mxActiveAdult_mutex );
	// Try 5 random positions
	for( j = 0; j < MAX_RANDOM_TRIES; ++j ) {
		i = rand() % ( ACTIVE_ADULT_WIDTH * ACTIVE_ADULT_HEIGHT );
		if( mxActiveAdult[i] == 0 ) {
			check = 1;
			break;
		}
	}
	// If unsuccessful, occupy first vacant position
	if( !check )
	for( i = 0; i < ACTIVE_ADULT_WIDTH * ACTIVE_ADULT_HEIGHT; ++i )
		if( mxActiveAdult[i] == 0 )
			break;
	pos = i;
	mxActiveAdult[pos] = 1;
	sem_post( &mxActiveAdult_mutex );		

	return pos;
}
void Screen_RemoveActiveAdult( int pos ) {
	// Ignore invalid positions
	if( pos >= ACTIVE_ADULT_WIDTH * ACTIVE_ADULT_HEIGHT )
		return;

	// Mark spot as vacant
	sem_wait( &mxActiveAdult_mutex );
	mxActiveAdult[pos] = 0;
	sem_post( &mxActiveAdult_mutex );
}

