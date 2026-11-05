if [[ -z "$1" ]] then
    exit 1
fi
echo "#include \"$1.h\""
cat <<'EOF'
#include "../player.h"
#include "../defs.h"

static void Init() {
    // Fires when actor is initialized
}

static void Frame() {
    // Fires every frame
}

static void Destroy() {
    // Fires when actor is destroyed
}
EOF
echo "void InitActor_$1(Map* map) {"
cat <<'EOF'
    // sprite data
    SpriteImageList sprite_rotation = {
        .img_keys = {
            // example: "starman%n.png"
        },
        .img_amm = 12
    };
    SpriteAttr sprite_attr = {
        .rotation_imgs = sprite_rotation,
        .size = 0.8,
        .a = 0,
    };
    size_t sprite_idx = Map_addSprite(map, sprite_attr);

    // actor data
    ActorAttr actor_attr = {
        .map_sprite = &map->sprites[sprite_idx],
    }

    Actor actor = {
        .Func_Frame = &Frame,
        .Func_Init = &Init,
        .Func_Destroy = &Destroy,
        .attr = actor_attr,
    }

    // create actor
    Map_addActor(map, actor);
}
EOF