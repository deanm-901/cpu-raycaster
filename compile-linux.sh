mkdir -p comp/linux

gcc -g src/*.c -o comp/linux/run -lSDL2 -lm -lpng