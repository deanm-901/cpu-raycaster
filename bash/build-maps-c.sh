files=(maps/*.map)
symbols=()
names=()

cat <<'EOF'
#include "map-imports.h"
#include "hashmap.h"
#include "map.h"
#include <string.h>
EOF

i=0
for file in "${files[@]}"; do
    name=${file##*/}
    symbol="map_$i"

    names+=("$name")
    symbols+=("$symbol")

    printf 'static unsigned char %s[] = {\n#embed "../%s"\n};\n' "$symbol" "$file"

    i=$((i + 1))
done

cat <<'EOF'

static HashMap* map_map;

static void setIndex(unsigned char data[], size_t data_size, const char* key) {
    Map* map = malloc(sizeof(Map));
    memcpy(map, data, data_size);

    HashMap_set(map_map, key, map, sizeof(Map));
}

void assignMapData() {
    map_map = HashMap_init();
EOF

for i in "${!symbols[@]}"; do
    printf '    setIndex(%s, sizeof(%s), "%s");\n' "${symbols[$i]}" "${symbols[$i]}" "${names[$i]}"
done

cat <<'EOF'
}

Map* getMapData(const char* key) {
    HashMapItem* item = HashMap_get(map_map, key);
    return (Map*)item->value.value;
}
EOF