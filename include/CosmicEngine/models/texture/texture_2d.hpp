
#ifndef COSMIC_TEXTURE2D_HPP
#define COSMIC_TEXTURE2D_HPP

/**
 * @file texture_2d.hpp
 * @brief Declares the 2D texture wrapper used by the engine renderer.
 */

#include <glad/glad.h>

namespace CosmicEngine
{
    /**
     * @brief Wraps an OpenGL 2D texture resource.
     *
     * Texture2D handles loading an image file via stb_image and uploading it to
     * the GPU.  The engine creates textures through the ResourceManager when you
     * call RS_MN.LoadTexture(); you typically reference them by key rather than
     * using this class directly.
     *
     * @par Example â€” loading a texture through ResourceManager
     * @code
     * RS_MN.LoadTexture("test_texture", "assets/textures/test.png");
     * RS_MN.Render2DSprite("test_texture", position, size);
     * @endcode
     */
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
        /**
         * @brief Loads a texture from disk.
         * @param imagePath Source image file path.
         */
        Texture2D(const char* imagePath);
        /** @brief Releases the OpenGL texture resource. */
        ~Texture2D();
        /** @brief Returns the OpenGL texture identifier. */
        unsigned int GetID();
        /** @brief Returns the texture width in pixels. */
        unsigned int GetWidth();
        /** @brief Returns the texture height in pixels. */
        unsigned int GetHeight();

        /**
         * @brief Replaces the current texture image with a new file.
         * @param imagePath Source image file path.
         */
        void SetNewImage(const char* imagePath);

        /**
         * @brief Sets the OpenGL internal texture format.
         * @param internalFormat Value provided by the caller.
         */
        void SetInternalFormat(unsigned int internalFormat);
        /** @brief Returns the OpenGL internal texture format. */
        unsigned int GetInternalFormat();

        /**
         * @brief Sets the source image format used for uploads.
         * @param imageFormat Value provided by the caller.
         */
        void SetImageFormat(unsigned int imageFormat);
        /** @brief Returns the source image format used for uploads. */
        unsigned int GetImageFormat();

        /** @brief Binds the texture to the requested texture unit. */
        void Bind(unsigned int index = 0) const;
        /** @brief Unbinds the texture from the active texture target. */
        void Unbind() const;
    };
}

#endif // COSMIC_TEXTURE2D_HPP
