files=(images/*.png images/*/*.png)
symbols=()
names=()

cat <<'EOF'
#include "assets.h"
EOF

i=0
for file in "${files[@]}"; do
    name=${file##*/}
    symbol="asset_$i"

    names+=("$name")
    symbols+=("$symbol")

    printf 'static unsigned char %s[] = {\n#embed "../%s"\n};\n' "$symbol" "$file"

    i=$((i + 1))
done

cat <<'EOF'
void InitAssetData() {
EOF

for i in "${!symbols[@]}"; do
    printf '    setImageHolderIndex(%s, sizeof %s, "%s");\n' "${symbols[$i]}" "${symbols[$i]}" "${names[$i]}"
done

cat <<'EOF'
}
EOF