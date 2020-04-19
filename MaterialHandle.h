#pragma once

#ifndef MATERIALHANDLE_H
#define MATERIALHANDLE_H

#include "HeadersRequeriments.h"
#include "Material.h"

class MaterialHandle
{
public:
    MaterialType type;
    std::vector<Material*> listMaterials;
    Shader* shader;
    Shader* shader2;
    bool isAmbientReflective = false;

    MaterialHandle();
    MaterialHandle(Shader* sh);
    void AddMaterialToList(Material* mat);
    void EditMaterial(MaterialComponent component, Shader& sh);
    void EditMaterial(MaterialComponent component, float value);
    void EditMaterial(MaterialComponent component, bool value);
    void EditMaterial(MaterialComponent component, glm::vec4 value);
    void EditMaterial(MaterialComponent component, MaterialType type);
    void EditMaterial(MaterialComponent component, std::vector<Texture> textures, unsigned int id = NULL);  
};

#endif
#pragma once
