/**
 * @file texture_2d.cpp
 * @brief Implements the OpenGL 2D texture wrapper used by the engine renderer.
 */

#include <iostream>

#include "texture_2d.hpp"
#include "stb_image.h"

namespace CosmicEngine
{
    Texture2D::Texture2D(const char* imagePath)
        : Width(0), Height(0), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_NEAREST), Filter_Max(GL_NEAREST)
    {
        glGenTextures(1, &this->ID);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(false);
        unsigned char *data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
        
        if (data == nullptr)
        {
            std::cerr << "[Texture2D] Failed to load image: " << imagePath << std::endl;
            std::cerr << "[Texture2D] stbi_load error: " << stbi_failure_reason() << std::endl;
            return;
        }

        this->Width = width;
        this->Height = height;

        // Infer the texture upload format directly from the decoded image channel count.
        if (nrChannels == 4)
        {
            this->InternalFormat = GL_RGBA;
            this->ImageFormat = GL_RGBA;
        }
        else if (nrChannels == 3)
        {
            this->InternalFormat = GL_RGB;
            this->ImageFormat = GL_RGB;
        }
        else if (nrChannels == 1)
        {
            this->InternalFormat = GL_RED;
            this->ImageFormat = GL_RED;
        }

        Generate(data);

        stbi_image_free(data);
    }

    Texture2D::~Texture2D()
    {
        if (ID != 0)
        {
            glDeleteTextures(1, &ID);
            ID = 0;
        }
    }

    unsigned int Texture2D::GetID()
    {
        return ID;
    }

    unsigned int Texture2D::GetWidth()
    {
        return Width;
    }

    unsigned int Texture2D::GetHeight()
    {
        return Height;
    }

    void Texture2D::SetNewImage(const char* imagePath)
    {
        int width, height, nrChannels;
        unsigned char *data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
        
        if (data == nullptr)
        {
            std::cerr << "[Texture2D::SetNewImage] Failed to load image: " << imagePath << std::endl;
            std::cerr << "[Texture2D::SetNewImage] stbi_load error: " << stbi_failure_reason() << std::endl;
            return;
        }

        this->Width = width;
        this->Height = height;

        if (nrChannels == 4)
        {
            this->InternalFormat = GL_RGBA;
            this->ImageFormat = GL_RGBA;
        }
        else if (nrChannels == 3)
        {
            this->InternalFormat = GL_RGB;
            this->ImageFormat = GL_RGB;
        }
        else if (nrChannels == 1)
        {
            this->InternalFormat = GL_RED;
            this->ImageFormat = GL_RED;
        }

        Generate(data);

        stbi_image_free(data);
    }

    void Texture2D::SetInternalFormat(unsigned int internalFormat)
    {
        this->InternalFormat = internalFormat;
    }

    unsigned int Texture2D::GetInternalFormat()
    {
        return InternalFormat;
    }

    void Texture2D::SetImageFormat(unsigned int imageFormat)
    {
        this->ImageFormat = imageFormat;
    }

    unsigned int Texture2D::GetImageFormat()
    {
        return ImageFormat;
    }


    void Texture2D::Generate(unsigned char *data)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, this->ID);
        glTexImage2D(GL_TEXTURE_2D, 0, this->InternalFormat, Width, Height, 0, this->ImageFormat, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
        glBindTexture(GL_TEXTURE_2D, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    void Texture2D::Bind(unsigned int index) const
    {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, this->ID);
    }

    void Texture2D::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}