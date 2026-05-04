#include "ResourceManager.h"

ResourceManager::ResourceManager()
{

}

bool ResourceManager::loadBitmap(const std::string& key, const std::string& path) {
    if (bitmap_resources.find(key) != bitmap_resources.end()) {
        return false;
    }

    ALLEGRO_BITMAP* bitmap = al_load_bitmap(path.c_str());
    if (!bitmap) {
        return false;
    }

    bitmap_resources[key] = bitmap;
    return true;
}



bool ResourceManager::loadSpriteSheet(const std::string& key, const std::string& path, int files, int columns) {
    if (spriteSheet_resources.find(key) != spriteSheet_resources.end()) {
        return false;
    }

    ALLEGRO_BITMAP* spriteSheet = al_load_bitmap(path.c_str());
    if (!spriteSheet) {
        return false;
    }

    spriteSheet_resources[key] = std::make_tuple(spriteSheet, al_get_bitmap_width(spriteSheet)/columns, al_get_bitmap_height(spriteSheet)/files);
    return true;
}



bool ResourceManager::loadFont(const std::string& key, const std::string& path, int size) {
    if (font_resources.find(key) != font_resources.end()) {
        return false;
    }

    ALLEGRO_FONT* font = al_load_font(path.c_str(), size, NULL);
    if (!font) {
        return false;
    }

    font_resources[key] = font;
    return true;
}


ALLEGRO_BITMAP* ResourceManager::getBitmap(const std::string& key) const {
    auto it = bitmap_resources.find(key);
    if (it == bitmap_resources.end()) {
        return nullptr;
    }
    return it->second;
}



ALLEGRO_BITMAP* ResourceManager::getBitmapRegionFromSpriteSheet(const std::string& key, int file, int column) const {
    auto it = spriteSheet_resources.find(key);
    if (it == spriteSheet_resources.end()) {
        return nullptr;
    }
    ALLEGRO_BITMAP* sprite = std::get<0>(it->second);
    int spritesWidth = std::get<1>(it->second);
    int spritesHeight = std::get<2>(it->second);
    return al_create_sub_bitmap(sprite, column*spritesWidth, file*spritesHeight, spritesWidth, spritesHeight);
}



ALLEGRO_FONT* ResourceManager::getFont(const std::string& key) const {
    auto it = font_resources.find(key);
    if (it == font_resources.end()) {
        return nullptr;
    }
    return it->second;
}


void ResourceManager::clear() {
    for (auto& pair : bitmap_resources) {
        al_destroy_bitmap(pair.second);
    }
    bitmap_resources.clear();

    for (auto& pair : spriteSheet_resources) {
        if (std::get<0>(pair.second)) {
            al_destroy_bitmap(std::get<0>(pair.second));
        }
    }
    spriteSheet_resources.clear();

    for (auto& pair : font_resources) {
        al_destroy_font(pair.second);
    }
    font_resources.clear();
}


ResourceManager::~ResourceManager()
{
    clear();
}