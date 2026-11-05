## It comes as no suprise that you need linux 
## and the dependencies of this program to run 
## this shell command

mkdir -p comp/linux

compile_norm() {
    gcc -g src/*.c src/*/*.c -o comp/linux/run -lSDL2 -lm -lpng -lpthread
}

compile_debug() {
    gcc -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer \
        src/*.c src/*/*.c -o comp/linux/run -lSDL2 -lm -lpng -lpthread

    #env ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer\
    #    ASAN_OPTIONS=fast_unwind_on_malloc=0:malloc_context_size=40:detect_leaks=1
}

./bash/build-assets-c.sh > src/assets.c

#mv src/assets.c.tmp src/assets.c

if [ "$#" -ne 1 ]; then
    compile_norm
    exit 0
fi

compile_debug