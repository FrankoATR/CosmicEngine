
#ifndef GAMETEXTURE_H
#define GAMETEXTURE_H

#include <glad/glad.h>

namespace WandEngine
{
    class GameTexture2D
    {
    private:
        unsigned int ID;
        unsigned int Width, Height;
        unsigned int InternalFormat;
        unsigned int ImageFormat;
        unsigned int Wrap_S;
        unsigned int Wrap_T;
        unsigned int Filter_Min;
        unsigned int Filter_Max;

    public:
        GameTexture2D();
        ~GameTexture2D();
        unsigned int GetID();
        unsigned int GetWidth();
        unsigned int GetHeight();

        void SetInternalFormat(unsigned int internalFormat);
        unsigned int GetInternalFormat();

        void SetImageFormat(unsigned int imageFormat);
        unsigned int GetImageFormat();

        void Generate(unsigned int width, unsigned int height, unsigned char *data);
        void Bind(unsigned int index = 0) const;
        void Unbind() const;
    };
}

#endif //GAMETEXTURE_H