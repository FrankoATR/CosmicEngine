#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>

namespace WandEngine
{

    class ResourceManager {
    private:
        std::map<std::string, ALLEGRO_BITMAP*> bitmap_resources;
        std::map<std::string, std::vector<std::vector<ALLEGRO_BITMAP*>>> spriteSheet_resources;
        std::map<std::string, ALLEGRO_FONT*> font_resources;

        ResourceManager() = default;
        ~ResourceManager();
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

    public:
        static ResourceManager& GetInstance() {
            static ResourceManager instance;
            return instance;
        }

        bool loadBitmap(const std::string& key, const std::string& path);
        bool loadSpriteSheet(const std::string& key, const std::string& path, int rows, int columns);
        bool loadFont(const std::string& key, const std::string& path, int size);

        ALLEGRO_BITMAP* getBitmap(const std::string& key) const;
        ALLEGRO_BITMAP* getBitmapRegionFromSpriteSheet(const std::string& key, int row, int column) const;
        ALLEGRO_FONT* getFont(const std::string& key) const;

        void Clear();

    };

}

#endif // RESOURCEMANAGER_HPP