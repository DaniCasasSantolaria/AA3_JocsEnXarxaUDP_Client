#include "TileMap.h"
#include "../ImageObject.h"
#include "../Spawner.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool TileMap::LoadFromFile(const std::string& mapPath) {
    std::ifstream file(mapPath);

    if (!file.is_open()) {
        std::cout << "No se pudo abrir el mapa: " << mapPath << std::endl;
        return false;
    }

    std::string line;
    unsigned short row = 0;

    while (std::getline(file, line)) {
        for (unsigned short col = 0; col < line.size(); col++) {
            SpawnTile(line[col], row, col);
        }

        row++;
    }

    return true;
}

void TileMap::UpdateLocalMap(const std::string& localMapPath, const std::string& serverMapContent){
    std::ifstream inputFile(localMapPath);
    std::stringstream localMapBuffer;

    if (inputFile.is_open()) {
        localMapBuffer << inputFile.rdbuf();
    }

    std::string localMapContent = localMapBuffer.str();

    if (localMapContent == serverMapContent) {
        std::cout << "El mapa local ya esta actualizado." << std::endl;
        return;
    }

    std::ofstream outputFile(localMapPath, std::ios::trunc);

    if (!outputFile.is_open()) {
        std::cerr << "No se pudo actualizar el mapa local: " << localMapPath << std::endl;
        return;
    }

    outputFile << serverMapContent;

    std::cout << "Mapa local actualizado: " << localMapPath << std::endl;
}

void TileMap::SpawnTile(char tileType, unsigned short row, unsigned short col) {
    std::string texturePath;

    switch (tileType) {
    case '#':
        texturePath = worldTilesetPath;
        break;
    case 'P':
        texturePath = platformsTilesetPath;
        break;
    case '.':
        return;
    default:
        return;
    }

    Object* tile = new ImageObject(texturePath, Vector2{ 0.0f, 0.0f }, Vector2{ SOURCE_TILE_SIZE, SOURCE_TILE_SIZE });

    const float scaledTileSize = SOURCE_TILE_SIZE * TILE_SCALE;
    tile->GetTransform()->position = Vector2{
        col * scaledTileSize + scaledTileSize / 2.0f,
        row * scaledTileSize + scaledTileSize / 2.0f
    };

    tile->GetTransform()->scale = Vector2{ TILE_SCALE, TILE_SCALE };

    SPAWN.SpawnObject(tile);
}
