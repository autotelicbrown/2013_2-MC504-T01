CC = gcc

#Project directories
IDIR := include
ODIR := obj
LDIR := lib
SDIR := src

#SDL lib
SDL_LIB = -L/usr/local/lib/ -lSDL2 -lSDL2_image -lSDL2_ttf -Wl,-rpath=/usr/local/lib
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

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

.PHONY : clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	rm $(EXE)
	rmdir $(ODIR)

$(OBJ): | $(ODIR)

$(ODIR):
	mkdir $(ODIR)
