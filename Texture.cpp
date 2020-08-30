#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "includes\stb_image.h"

Texture::~Texture()
{

}

Texture::Texture(std::string imagePath, bool isGamma, unsigned int wrap, unsigned int filter, bool flip)
{
    this->isGammaTexture = isGamma;
    this->imgPath = imagePath;
    this->wrap = wrap;
    this->filter = filter;
    this->flip = flip;

    glGenTextures(1, &ID);

    if (type != TypeTexture::HDR_SKYBOX)
        LoadTextureObj();
    else
        LoadTextureHDR();
}

Texture::Texture(std::string imagePath, TypeTexture type, bool isGamma, unsigned int wrap, unsigned int filter, bool flip)
{
    this->type = type; 
    this->isGammaTexture = isGamma;
    this->imgPath = imagePath;
    this->wrap = wrap;
    this->filter = filter;
    this->flip = flip;

    glGenTextures(1, &ID);

    if(type != TypeTexture::HDR_SKYBOX)
        LoadTextureObj();
    else
        LoadTextureHDR();
}

Texture::Texture(std::vector<std::string> imagesPath, TypeTexture type, bool isGamma, unsigned int wrap, unsigned int filter, bool flip)
{
    this->isGammaTexture = isGamma;

    //LOAD CUBEMAP
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    for (unsigned int i = 0; i < imagesPath.size(); i++)
    {
        unsigned char* data = stbi_load(imagesPath[i].c_str(), &width, &height, &nrChannels, 0);

        GLenum internalFormat, format;
        format = GL_RGBA;

        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        bool aux = false;

        internalFormat = format;

        if (isGammaTexture)
        {
            if (nrChannels == 1)
                internalFormat = GL_RED;
            else if (nrChannels == 3)
                internalFormat = GL_SRGB;
            else if (nrChannels == 4)
                internalFormat = GL_SRGB_ALPHA;
        }

        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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

void Texture::EditComponet(TextureComponent component, bool value)
{
    switch (component)
    {
    case TextureComponent::TEX_GAMMA:
        if (isGammaTexture != value)
        {
            isGammaTexture = value;
            LoadTextureObj();
        }
        break;
    }
}

void Texture::EditComponet(TextureComponent component, int value)
{
    switch (component)
    {
    case TEX_WIDTH:
        width = value;
        break;
    case TEX_HEIGHT:
        height = value;
        break;
    case TEX_CHANNELS:
        nrChannels = value;
        break;
    }
    LoadTextureObj();
}

void Texture::EditComponet(TextureComponent component, std::string value)
{
    imgPath = path;
    LoadTextureObj();
}

void Texture::EditComponet(TextureComponent component, TypeTexture value)
{
    type = value;
    LoadTextureObj();
}

unsigned int Texture::RawTypeTexture()
{
    switch (this->type)
    {
    case TypeTexture::DIFFUSE:
        return 0;
    case TypeTexture::SPECULAR:
        return 1;
    case TypeTexture::NORMAL:
        return 2;
    case TypeTexture::HEIGHT:
        return 3;
    case TypeTexture::EMISSIVE:
        return 4;
    case TypeTexture::CUBEMAP:
        return 5;
    case TypeTexture::AO:
        return 6;
    case TypeTexture::ROUGHNESS:
        return 7;
    case TypeTexture::METALLIC:
        return 8;
    case TypeTexture::BUMP:
        return 9;
    case TypeTexture::HDR_SKYBOX:
        return 10;
    case TypeTexture::NOISE:
        return 11;
    }
}

void Texture::SetRawTypeTexture(unsigned int id)
{
    switch (id)
    {
    case 0:
        type = TypeTexture::DIFFUSE;
        break;
    case 1:
        type = TypeTexture::SPECULAR;
        break;
    case 2:
        type = TypeTexture::NORMAL;
        break;
    case 3:
        type = TypeTexture::HEIGHT;
        break;
    case 4:
        type = TypeTexture::EMISSIVE;
        break;
    case 5:
        type = TypeTexture::CUBEMAP;
        break;
    case 6:
        type = TypeTexture::AO;
        break;
    case 7:
        type = TypeTexture::ROUGHNESS;
        break;
    case 8:
        type = TypeTexture::METALLIC;
        break;
    case 9:
        type = TypeTexture::BUMP;
        break;
    case 10:
        type = TypeTexture::HDR_SKYBOX;
        break;
    case 11:
        type = TypeTexture::NOISE;
        break;
    }
}

void Texture::LoadTextureObj()
{
    glBindTexture(GL_TEXTURE_2D, ID);

    LoadImage(imgPath.c_str(), flip);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

void Texture::LoadTextureHDR()
{
    glBindTexture(GL_TEXTURE_2D, ID);
    LoadHDRImage(imgPath.c_str(), true);
}

void Texture::LoadHDRImage(const char* imagePath, bool flip)
{
    stbi_set_flip_vertically_on_load(flip);
    int width, height, nrComponents;
    float* data = stbi_loadf(imagePath, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }
}

void Texture::LoadImage(const char* imagePath, bool flip)
{
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);

    GLenum internalFormat, format;
    format = GL_RGBA;

    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    bool aux = false;

    internalFormat = format;

    if (type == TypeTexture::DIFFUSE && isGammaTexture) 
    {
        if (nrChannels == 1)
            internalFormat = GL_RED;
        else if (nrChannels == 3)
            internalFormat = GL_SRGB;
        else if (nrChannels == 4)
            internalFormat = GL_SRGB_ALPHA;
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

