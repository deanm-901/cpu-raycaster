#include "assets.h"

static unsigned char asset_0[] = {
#embed "../images/atlas.png"
};

static unsigned char asset_1[] = {
#embed "../images/font1.png"
};

static unsigned char asset_2[] = {
#embed "../images/sky.png"
};

static unsigned char asset_3[] = {
#embed "../images/test-sprite.png"
};

void InitAssetData() {
    setImageHolderIndex(asset_0, sizeof asset_0, "atlas.png");
    setImageHolderIndex(asset_1, sizeof asset_1, "font1.png");
    setImageHolderIndex(asset_2, sizeof asset_2, "sky.png");
    setImageHolderIndex(asset_3, sizeof asset_3, "test-sprite.png");
}
