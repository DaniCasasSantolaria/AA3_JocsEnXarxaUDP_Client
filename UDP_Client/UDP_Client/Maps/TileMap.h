#pragma once
#include <string>
#include "../Vector2.h"

class TileMap {
private:
    const float SOURCE_TILE_SIZE = 16.0f;
    const float TILE_SCALE = 3.0f;

    const std::string worldTilesetPath = "resources/Tilesets/world_tileset.png";
    const std::string platformsTilesetPath = "resources/Tilesets/platforms.png";

    void SpawnTile(char tileType, unsigned short row, unsigned short col);

public:
    void UpdateLocalMap(const std::string& localMapPath, const std::string& serverMapContent);
    bool LoadFromFile(const std::string& mapPath);
};
