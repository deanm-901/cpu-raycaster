mkdir -p comp/windows

./bash/build-assets-c.sh > src/assets.c

x86_64-w64-mingw32-gcc -O0 -g src/*.c -o comp/windows/run.exe \
  -static-libgcc \
  -Wl,-Bstatic -lmingw32 -lSDL2main -lSDL2 -lpng -lz -lpthread \
  -Wl,-Bdynamic \
  -lsetupapi -lcfgmgr32 -lwinmm -limm32 -lole32 -loleaut32 \
  -lversion -luuid -luser32 -lgdi32 -ldxguid -lm 