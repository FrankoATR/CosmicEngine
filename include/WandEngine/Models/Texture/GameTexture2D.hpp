
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

        void Generate(unsigned char *data);

    public:
        GameTexture2D(const char* imagePath, unsigned int internalFormat = GL_RGB, unsigned int imageFormat = GL_RGB);
        ~GameTexture2D();
        unsigned int GetID();
        unsigned int GetWidth();
        unsigned int GetHeight();

        void SetNewImage(const char* imagePath);

        void SetInternalFormat(unsigned int internalFormat);
        unsigned int GetInternalFormat();

        void SetImageFormat(unsigned int imageFormat);
        unsigned int GetImageFormat();

        void Bind(unsigned int index = 0) const;
        void Unbind() const;
    };
}

#endif //GAMETEXTURE_H