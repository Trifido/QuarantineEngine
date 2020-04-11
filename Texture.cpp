#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "includes\stb_image.h"

Texture::Texture(std::string imagePath, unsigned int wrap, unsigned int filter, bool flip)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    LoadImage(imagePath.c_str(), flip);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

Texture::Texture(std::string imagePath, TypeTexture type, unsigned int wrap, unsigned int filter, bool flip)
{
    this->type = type;

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    LoadImage(imagePath.c_str(), flip);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

void Texture::LoadImage(const char* imagePath, bool flip)
{
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);

    GLenum format;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Error: Data texture\n");
    }

    stbi_image_free(data);
}

