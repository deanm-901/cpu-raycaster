if [[ -z "$1" ]] then
    exit 1
fi

touch src/actor-funcs/actor-$1.c
touch src/actor-funcs/actor-$1.h

# NOT WORKING #
./print-c-actor-file.sh $1 > src/actor-funcs/actor-$1.c
./print-h-actor-file.sh $1 > src/actor-funcs/actor-$1.h