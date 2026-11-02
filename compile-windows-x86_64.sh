##!!!!##
#TO COMPILE ON WINDOWS, YOU NEED minGW / MSYS2
#https://www.msys2.org/
#After installing, run the terminal and install the requirements by pacman:
: '
$ sudo pacman -S mingw-w64-x86_64-gcc \
            mingw-w64-x86_64-SDL2 \
            mingw-w64-x86_64-SDL2_mixer \
            mingw-w64-x86_64-SDL2_sound \
            mingw-w64-x86_64-libpng  # <= this "should" be installed by SDL2 (but just in case)
'
#Now you can ./ the compile script all you want!
#No, I dont want a makefile
##!!!!##

mkdir -p comp/windows

x86_64-w64-mingw32-gcc -g src/*.c -o comp/windows/run.exe \
  -static-libgcc \
  -Wl,-Bstatic -lmingw32 -lSDL2main -lSDL2 -lpng -lz -lpthread \
  -Wl,-Bdynamic \
  -lsetupapi -lcfgmgr32 -lwinmm -limm32 -lole32 -loleaut32 \
  -lversion -luuid -luser32 -lgdi32 -ldxguid -lm