#include "ResourceManager.hpp"

namespace WandEngine
{
    bool ResourceManager::loadBitmap(const std::string &key, const std::string &path)
    {
        if (bitmap_resources.find(key) != bitmap_resources.end())
        {
            return false;
        }

        ALLEGRO_BITMAP *bitmap = al_load_bitmap(path.c_str());
        if (!bitmap)
        {
            return false;
        }

        bitmap_resources[key] = bitmap;
        return true;
    }

    bool ResourceManager::loadSpriteSheet(const std::string &key, const std::string &path, int rows, int columns)
    {
        if (spriteSheet_resources.find(key) != spriteSheet_resources.end())
        {
            return false;
        }

        ALLEGRO_BITMAP *spriteSheet = al_load_bitmap(path.c_str());
        if (!spriteSheet)
        {
            return false;
        }

        int spritesWidth = al_get_bitmap_width(spriteSheet) / columns;
        int spritesHeight = al_get_bitmap_height(spriteSheet) / rows;

        std::vector<std::vector<ALLEGRO_BITMAP *>> sprites(rows, std::vector<ALLEGRO_BITMAP *>(columns, nullptr));

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                ALLEGRO_BITMAP *subBitmap = al_create_sub_bitmap(spriteSheet, j * spritesWidth, i * spritesHeight, spritesWidth, spritesHeight);
                if (!subBitmap)
                {
                    std::cerr << "Error: could not create sub-bitmap at [" << i << "][" << j << "]" << std::endl;
                    for (int x = 0; x <= i; ++x)
                    {
                        for (int y = 0; y < ((x == i) ? j : columns); ++y)
                        {
                            al_destroy_bitmap(sprites[x][y]);
                        }
                    }
                    al_destroy_bitmap(spriteSheet);
                    return false;
                }

                ALLEGRO_BITMAP *independentSprite = al_create_bitmap(spritesWidth, spritesHeight);
                if (!independentSprite)
                {
                    std::cerr << "Error: could not create independent bitmap at [" << i << "][" << j << "]" << std::endl;
                    al_destroy_bitmap(subBitmap);
                    continue;
                }

                al_set_target_bitmap(independentSprite);
                al_draw_bitmap(subBitmap, 0, 0, 0);
                al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));

                al_destroy_bitmap(subBitmap);

                sprites[i][j] = independentSprite;
            }
        }
        spriteSheet_resources[key] = sprites;

        return true;
    }

    bool ResourceManager::loadFont(const std::string &key, const std::string &path, int size)
    {
        if (font_resources.find(key) != font_resources.end())
        {
            return false;
        }

        ALLEGRO_FONT *font = al_load_font(path.c_str(), size, 0);
        if (!font)
        {
            return false;
        }

        font_resources[key] = font;
        return true;
    }

    ALLEGRO_BITMAP *ResourceManager::getBitmap(const std::string &key) const
    {
        auto it = bitmap_resources.find(key);
        if (it == bitmap_resources.end())
        {
            return nullptr;
        }
        return it->second;
    }

    ALLEGRO_BITMAP *ResourceManager::getBitmapRegionFromSpriteSheet(const std::string &key, int row, int column) const
    {
        auto it = spriteSheet_resources.find(key);
        if (it == spriteSheet_resources.end())
        {
            return nullptr;
        }

        if (row < 0 || row > it->second.size() - 1 || column < 0 || column > it->second[row].size() - 1)
        {
            return nullptr;
        }

        return it->second[row][column];
    }

    ALLEGRO_FONT *ResourceManager::getFont(const std::string &key) const
    {
        auto it = font_resources.find(key);
        if (it == font_resources.end())
        {
            return nullptr;
        }
        return it->second;
    }

    void ResourceManager::Clear()
    {
        for (auto &pair : bitmap_resources)
        {
            al_destroy_bitmap(pair.second);
        }
        bitmap_resources.clear();

        for (auto &pair : spriteSheet_resources)
        {
            if (!pair.second.empty())
            {
                for (auto &row : pair.second)
                {
                    if (!row.empty())
                    {
                        for (auto &column : row)
                        {
                            al_destroy_bitmap(column);
                        }
                    }
                }
            }
        }
        spriteSheet_resources.clear();

        for (auto &pair : font_resources)
        {
            al_destroy_font(pair.second);
        }
        font_resources.clear();
        std::cout << "Resource manager cleared" << std::endl;
    }

    ResourceManager::~ResourceManager()
    {
        Clear();
        std::cout << "Resource manager destroyed" << std::endl;
    }
}
