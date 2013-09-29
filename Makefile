CC = gcc

#Project directories
IDIR := include
ODIR := obj
LDIR := lib
SDIR := src

#SDL lib
#SDL_LIB = -L/usr/local/lib/ -lSDL2 -lSDL2_ttf -Wl,-rpath=/usr/local/lib
SDL_LIB = -Wl,-rpath=./SDL/x64 -Wl,-rpath=./SDL/x86 -L./SDL/x64 -L./SDL/x86 -lSDL2 -lSDL2_ttf
SDL_INCLUDE = -ISDL

##Debug Compiling flags
#CFLAGS = -g -O0 -Wall -D _DEBUG $(SDL_INCLUDE) -I$(IDIR) -std=gnu99 -pedantic
#LDFLAGS = $(SDL_LIB) -lpthread -lrt
##Release Compiling flags
CFLAGS = -O3 -Wall $(SDL_INCLUDE) -I$(IDIR) -std=gnu99 -pedantic
LDFLAGS = $(SDL_LIB) -lpthread -lrt

#Output name
EXE = ChildCare

#headers
_DEPS = sdldrawing.h \
	childcare.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

#objects
_OBJ = main.o \
       sdldrawing.o \
       childcare.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ) | libsdllinks
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

.PHONY : clean libsdllinks
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	rm $(EXE)
	rmdir $(ODIR)
	rm SDL/x64/libSDL2.so SDL/x64/libSDL2-2.0.so.0 SDL/x64/libSDL2_ttf.so SDL/x64/libSDL2_ttf-2.0.so.0
	rm SDL/x86/libSDL2.so SDL/x86/libSDL2-2.0.so.0 SDL/x86/libSDL2_ttf.so SDL/x86/libSDL2_ttf-2.0.so.0

libsdllinks:
	$(MAKE) -C SDL/x64 links
	$(MAKE) -C SDL/x86 links

$(OBJ): | $(ODIR)

$(ODIR):
	mkdir $(ODIR)

