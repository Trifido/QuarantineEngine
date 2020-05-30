#pragma once

#ifndef MATERIALHANDLE_H
#define MATERIALHANDLE_H

#include "HeadersRequeriments.h"
#include "Material.h"

enum RenderType
{
    FORWARD_RENDER,
    DEFERRED_RENDER
};

class MaterialHandle
{
public:
    RenderType current_render_mode = RenderType::FORWARD_RENDER;
    MaterialType type;
    std::vector<Material*> listMaterials;
    Shader* shader;
    Shader* deferred;
    Shader* forward;
    Shader* shader2;
    Shader* shaderShadow;
    Shader* shaderPointShadow;
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
    void EditMaterial(MaterialComponent component, RenderType type);
    void EditMaterial(MaterialComponent component, std::vector<Texture> textures, unsigned int id = NULL);
	void ActivateShadowMap(unsigned int idTexShadow, unsigned int idLight, TypeLight type = TypeLight::DIRL);
    //void ActivateShadowMap(unsigned int idTexShadow, bool isOmni = false);
};

#endif
#pragma once
