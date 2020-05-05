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
    Shader* shaderShadow;
    bool isAmbientReflective;
    bool isChangeNumInstances;
    int numInstances;
    const int MAX_INSTANCES = 1000000;

    MaterialHandle();
    MaterialHandle(Shader* sh);
    void AddMaterialToList(Material* mat);
    void EditMaterial(MaterialComponent component, Shader* sh);
    void EditMaterial(MaterialComponent component, float value); 
    void EditMaterial(MaterialComponent component, bool value);
    void EditMaterial(MaterialComponent component, glm::vec4 value);
    void EditMaterial(MaterialComponent component, MaterialType type, int numInstances=0);
    void EditMaterial(MaterialComponent component, DrawMode type);
    void EditMaterial(MaterialComponent component, std::vector<Texture> textures, unsigned int id = NULL);
    void ActivateShadowMap(unsigned int idTexShadow);
};

#endif
#pragma once
