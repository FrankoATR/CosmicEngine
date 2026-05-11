
#include <iostream>

#include "GameTexture2D.hpp"

namespace WandEngine
{
    GameTexture2D::GameTexture2D()
        : Width(0), Height(0), InternalFormat(GL_RGB), ImageFormat(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_NEAREST), Filter_Max(GL_NEAREST)
    {
        glGenTextures(1, &this->ID);
    }

    GameTexture2D::~GameTexture2D()
    {
        if (ID != 0)
        {
            glDeleteTextures(1, &ID);
            ID = 0;
        }
    }

    unsigned int GameTexture2D::GetID()
    {
        return ID;
    }

    unsigned int GameTexture2D::GetWidth()
    {
        return Width;
    }

    unsigned int GameTexture2D::GetHeight()
    {
        return Height;
    }

    void GameTexture2D::SetInternalFormat(unsigned int internalFormat)
    {
        this->InternalFormat = internalFormat;
    }

    unsigned int GameTexture2D::GetInternalFormat()
    {
        return InternalFormat;
    }

    void GameTexture2D::SetImageFormat(unsigned int imageFormat)
    {
        this->ImageFormat = imageFormat;
    }

    unsigned int GameTexture2D::GetImageFormat()
    {
        return ImageFormat;
    }


    void GameTexture2D::Generate(unsigned int width, unsigned int height, unsigned char *data)
    {
        this->Width = width;
        this->Height = height;
        glBindTexture(GL_TEXTURE_2D, this->ID);
        glTexImage2D(GL_TEXTURE_2D, 0, this->InternalFormat, width, height, 0, this->ImageFormat, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GameTexture2D::Bind(unsigned int index) const
    {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, this->ID);
    }

    void GameTexture2D::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}