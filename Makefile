#
# Makefile for caswcx
#
TARGETDIR = ./install
IDIR = .

CC32 = i686-w64-mingw32-gcc
CPP32 = i686-w64-mingw32-g++
CC64 = x86_64-w64-mingw32-gcc
CPP64 = x86_64-w64-mingw32-g++
WRES32 = i686-w64-mingw32-windres
WRES64 = x86_64-w64-mingw32-windres

TARGET32 = $(TARGETDIR)/casMSX.wcx
TARGET64 = $(TARGETDIR)/casMSX.wcx64

RESDIR = res
ODIR = obj
ODIR32 = $(ODIR)/w32
ODIR64 = $(ODIR)/w64
DIR_GUARD = @mkdir -p $(@D)

#DEBUG = -DDEBUG
WINFLAGS = -s -DWINDOWS
CFLAGS = -shared -I./includes -I$(IDIR) -Wall -O3 $(WINFLAGS) $(DEBUG)
CFLAGS32 = $(CFLAGS) -m32 -Wl,--out-implib="$(ODIR32)/libcasMSX.wcx"
CFLAGS64 = $(CFLAGS) -m64 -Wl,--out-implib="$(ODIR64)/libcasMSX.wcx64"

LIBS = 

_DEPS = wcxapi.h \
        wcxhead.h \
        cunicode.h \
        types.h \
        resource.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ =	caswcx.o \
        wcxapi.o \
        cunicode.o
OBJ32 = $(patsubst %,$(ODIR32)/%,$(_OBJ))
OBJ64 = $(patsubst %,$(ODIR64)/%,$(_OBJ))

RESOURCES32 = $(ODIR32)/resources.res
RESOURCES64 = $(ODIR64)/resources.res

.PHONY: clean all

all: $(TARGET32) $(TARGET64)

$(ODIR32)/%.res: $(RESDIR)/%.rc
	$(DIR_GUARD)
	$(WRES32) -O coff -i $^ -o $@ -I.

$(ODIR64)/%.res: $(RESDIR)/%.rc
	$(DIR_GUARD)
	$(WRES64) -O coff -i $^ -o $@ -I.

$(ODIR32)/%.o: %.c $(DEPS)
	$(DIR_GUARD)
	$(CC32) -c -o $@ $< $(CFLAGS32)

$(ODIR64)/%.o: %.c $(DEPS)
	$(DIR_GUARD)
	$(CC64) -c -o $@ $< $(CFLAGS64)

$(ODIR32)/%.o: %.cpp $(DEPS)
	$(DIR_GUARD)
	$(CPP32) -c -o $@ $< $(CFLAGS32)

$(ODIR64)/%.o: %.cpp $(DEPS)
	$(DIR_GUARD)
	$(CPP64) -c -o $@ $< $(CFLAGS64)

$(TARGET32): $(OBJ32) $(RESOURCES32)
	$(CC32) -o $@ $^ $(CFLAGS32) $(LIBS)

$(TARGET64): $(OBJ64) $(RESOURCES64)
	$(CC64) -o $@ $^ $(CFLAGS64) $(LIBS)

clean:
	rm -Rf $(ODIR) *~ core $(IDIR)/*~ $(TARGETDIR)/*.wcx*
