#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "includes\stb_image.h"

Texture::Texture(char* imagePath, unsigned int wrap, unsigned int filter, bool flip)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

    LoadImage(imagePath, flip);
}

void Texture::LoadImage(char* imagePath, bool flip)
{
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);

    unsigned int colorChannels = GL_RGB;

    if(nrChannels == 4)
        colorChannels = GL_RGBA; 

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, colorChannels, width, height, 0, colorChannels, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Error: Data texture\n");
    }

    stbi_image_free(data);
}

