import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

class MapTileAttr {
    char light_lvl;
    char clng_type;
    char flor_type;
}

class Map {
    static final int cMapSize_bytes = 45696,
                     cSizeT_bytes = 8,
                     cSkyTexture_offset = 0,
                     cWallHeight_offset = 1056,
                     cMapH_offset = 1060,
                     cMapV_offset = 2116,
                     cMapT_offset = 3172,
                     cSprites_offset = 6248,
                     cSpritesAmm_offset = 34920,
                     cActors_offset = 34928,
                     cActorsAmm_offset = 45680,
                     cPlayerInitPos_offset = 45688,
                     cMapTileAttr_bytes = 3,
                     cSpriteAttr_bytes = 448,
                     cActor_bytes = 168;

    String sky_texture;
    String[] face_tex_idx;

    float wall_height;
    char[] mapH;
    char[] mapV;
    MapTileAttr[] mapT;

    float[] player_init_pos;

    ByteBuffer GetBuffer() {
        ByteBuffer b = ByteBuffer.allocate(cMapSize_bytes).order(ByteOrder.nativeOrder());

        // stuff //

        return b;
    }
}

public class Editor {

    public static void main(String[] args) {
        
    }
}
