
#ifndef COSMIC_TEXTURE2D_HPP
#define COSMIC_TEXTURE2D_HPP

#include <glad/glad.h>

namespace CosmicEngine
{
    class Texture2D
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
        Texture2D(const char* imagePath);
        ~Texture2D();
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

#endif // COSMIC_TEXTURE2D_HPP