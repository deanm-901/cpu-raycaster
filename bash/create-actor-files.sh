if [[ -z "$1" ]] then
    exit 1
fi

./bash/print-c-actor-file.sh $1 > src/actor-funcs/actor-$1.c
./bash/print-h-actor-file.sh $1 > src/actor-funcs/actor-$1.h