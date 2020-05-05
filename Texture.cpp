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

Texture::Texture(std::vector<std::string> imagesPath, TypeTexture type, unsigned int wrap, unsigned int filter, bool flip)
{
    //LOAD CUBEMAP
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    for (unsigned int i = 0; i < imagesPath.size(); i++)
    {
        unsigned char* data = stbi_load(imagesPath[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << imagesPath[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap);
}

void Texture::LoadImage(const char* imagePath, bool flip)
{
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);

    GLenum internalFormat, format;
    
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    bool aux = false;
    if (type == TypeTexture::DIFFUSE) //
    {
        if (nrChannels == 1)
            internalFormat = GL_RED;
        else if (nrChannels == 3)
            internalFormat = GL_SRGB;
        else if (nrChannels == 4)
            internalFormat = GL_SRGB_ALPHA;
    }
    else
    {
        internalFormat = format;
    }

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Error: Data texture\n");
    }

    stbi_image_free(data);
}

