if [[ -z "$1" ]] then
    exit 1
fi

echo "#ifndef ${1}_H"
echo "#define ${1}_H"

cat <<'EOF'

#include "../map.h"

EOF
echo "void InitActor_$1(Map* map);"
cat <<'EOF'

#endif
EOF
