#pragma once

#ifndef MATERIALHANDLE_H
#define MATERIALHANDLE_H

#include "HeadersRequeriments.h"
#include "Material.h"

enum RenderType
{
    FORWARD_RENDER = 0,
    FORWARD_QUALITY_RENDER = 1,
    DEFERRED_RENDER = 2
};

class MaterialHandle
{
public:
    RenderType current_render_mode = RenderType::FORWARD_QUALITY_RENDER;
    MaterialType type;
    std::vector<Material*> listMaterials;
    Shader* shader;
    Shader* deferred;
    Shader* forward;
    Shader* forwardQA;
    Shader* shader2;
    Shader* shaderShadow;
    Shader* shaderPointShadow;
    Shader* shaderVolumeShadow;
    Shader* occlussionShader;
    Shader* forwardVolumeShadowShader;
    bool isAmbientReflective;
    bool isChangeNumInstances;
    bool generalDrawMode = true;
    int numInstances;
    const int MAX_INSTANCES = 1000000;

    MaterialHandle();
    MaterialHandle(Shader* sh);
    void AddMaterialToList(Material* mat);
    void AddLights(std::vector<Light*> lights);
    void AddCamera(Camera* mainCamera);
    void EditMaterial(MaterialComponent component, Shader* sh);
    void EditMaterial(MaterialComponent component, float value); 
    void EditMaterial(MaterialComponent component, bool value);
    void EditMaterial(MaterialComponent component, glm::vec4 value);
    void EditMaterial(MaterialComponent component, MaterialType type, int numInstances=0);
    void EditMaterial(MaterialComponent component, DrawMode type, bool guimode = false);
    void EditMaterial(MaterialComponent component, RenderType renderTypeSelected);
    void EditMaterial(MaterialComponent component, std::vector<Texture> textures, unsigned int id = NULL);
	void ActivateShadowMap(unsigned int idTexShadow, unsigned int idLight, TypeLight type = TypeLight::DIRL);
	void ActivateIrradianceMap(unsigned int idTexIrradiance, unsigned int idTexPrefilter, unsigned int idTexBrdf);
    void ChangeCurrentShader(MaterialComponent component);
    //void ActivateShadowMap(unsigned int idTexShadow, bool isOmni = false);
};

#endif
#pragma once
