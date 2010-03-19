
all: hex

hex: hex.exe

hex.exe: hex.cpp
    cl -nologo -Ox -GF -EHsc -D_CRT_SECURE_NO_DEPRECATE hex.cpp

clean:
    -del 2>nul /q hex.obj

clobber: clean
    -del 2>nul /q hex.exe

fresh: clobber all

$(BINDIR)\hex.exe: hex.exe
    if defined BINDIR copy /y $? %%BINDIR%%

install: $(BINDIR)\hex.exe
