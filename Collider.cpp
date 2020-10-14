#include "Collider.h"

Collider::Collider()
{
    type = ColliderType::BOX;
    aabb_min = glm::vec3(-1.0f);
    aabb_max = glm::vec3(1.0f);
    transform = new Transform();
    matHandle.type = MaterialType::LIT;
    Shader *colliderShader = new Shader("shaders/colliderShader.vert","shaders/colliderShader.frag");
    matHandle.EditMaterial(MaterialComponent::SHADER1, colliderShader);
    matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::INTERNAL);
}

Collider::Collider(ColliderType typeCollider)
{
    type = typeCollider;
    transform = new Transform();
    matHandle.type = MaterialType::LIT;
    Shader* colliderShader = new Shader("shaders/colliderShader.vert", "shaders/colliderShader.frag");
    matHandle.EditMaterial(MaterialComponent::SHADER1, colliderShader);
    matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::INTERNAL);

    if (type == ColliderType::MESH)
    {
        LoadMesh("");
    }
    else if (type == ColliderType::SPHERE)
    {
        LoadMesh("./resources/3DModels/sphere/sphere.obj");
        radius = 1.0f;
    }
    else
    {
        aabb_min = glm::vec3(-1.0f);
        aabb_max = glm::vec3(1.0f);
        std::vector<Vertex> verticesVertex;
        std::vector<unsigned int> indicesV;

        float verticesV[] = {
            // positions         
            -1.0f, -1.0f, -1.0f,// 0
             1.0f, -1.0f, -1.0f,// 1
             1.0f,  1.0f, -1.0f,// 2
            -1.0f,  1.0f, -1.0f,// 3
             
            -1.0f, -1.0f, 1.0f, // 4
             1.0f, -1.0f, 1.0f, // 5
             1.0f,  1.0f, 1.0f, // 6
            -1.0f,  1.0f, 1.0f  // 7
        };

        unsigned int COUNT = 8;
        for (int i = 0; i < COUNT; i++)
        {
            unsigned int index = i * 3;

            Vertex vertex;
            glm::vec3 vector;

            vector.x = verticesV[index];
            vector.y = verticesV[index + 1];
            vector.z = verticesV[index + 2];
            vertex.Position = vector;

            verticesVertex.push_back(vertex);
        }

        indicesV.push_back(0);
        indicesV.push_back(1);
        indicesV.push_back(2);
        indicesV.push_back(3);
        indicesV.push_back(0);
        indicesV.push_back(4);
        indicesV.push_back(5);
        indicesV.push_back(1);
        indicesV.push_back(2);
        indicesV.push_back(6);
        indicesV.push_back(5);
        indicesV.push_back(4);
        indicesV.push_back(7);
        indicesV.push_back(6);
        indicesV.push_back(2);
        indicesV.push_back(3);
        indicesV.push_back(7);
        indicesV.push_back(4);
        indicesV.push_back(0);
        indicesV.push_back(3);

        meshCollider.MeshCollider(verticesVertex, indicesV, matHandle);
    }
}

void Collider::DrawCollider()
{
    //SetHierarchy();
    matHandle.shader->use();
    meshCollider.material->ptrShader->setMat4("model", transform->finalModelMatrix);

    glBindVertexArray(meshCollider.GetVAO());

    switch (type)
    {
    case ColliderType::BOX:
        glDrawElements(GL_LINE_STRIP, meshCollider.indices.size(), GL_UNSIGNED_INT, 0);
        break;
    case ColliderType::SPHERE:
    case ColliderType::MESH:
        glDrawElements(GL_LINES, meshCollider.indices.size(), GL_UNSIGNED_INT, 0);
        break;
    }
    
}

bool Collider::IsRayCollision(UIRay* ray)
{
    float distRay = FLT_MAX;
    glm::vec4 ray_eye = inverse(ray->PV * transform->model) * glm::vec4(ray->ray_dir, 1.0);
    glm::vec3 ray_wor = glm::normalize(ray_eye);
    glm::vec3 ray_orig = inverse(transform->model) * glm::vec4(ray->ray_orig, 1.0f);

    //glm::vec3 ray_wor = ray->ray_dir;
    //glm::vec3 ray_orig = inverse(transform->model) * glm::vec4(ray->ray_orig, 1.0f);

    float dist = CheckCollider(ray_orig, ray_wor);

    std::cout << "MAX DIST: " << FLT_MAX << std::endl;
    std::cout << "Contact: " << (dist > 0.0f && dist < distRay) << " - " << distRay << std::endl;
    return (dist > 0.0f && dist < distRay);
}

float Collider::CheckCollider(glm::vec3 origin, glm::vec3 dir)
{
    float intersectionDist;
    float distancePoint = FLT_MAX;

    switch (type)
    {
        case ColliderType::MESH:
            for (unsigned idTri = 0; idTri < meshCollider.indices.size(); idTri += 3)
            {
                if (meshCollider.RayIntersectsTriangle(origin, dir, idTri, intersectionDist, transform->model))
                {
                    if (distancePoint > intersectionDist)
                        distancePoint = intersectionDist;
                }
            }
            break;
        case ColliderType::SPHERE:
            distancePoint = (RaySphereIntersection(origin, dir)) ? 1.0f : FLT_MAX;
            break;
        case ColliderType::BOX:
            distancePoint = RayBoxOBBIntersection(origin, dir);
            break;
    }

    return distancePoint;
}

void Collider::LoadMesh(std::string path)
{
    Assimp::Importer importer;
    scene = importer.ReadFile(path, aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        fprintf(stderr, "ERROR::ASSIMP::%s", importer.GetErrorString());
        return;
    }

    recursiveNodeProcess(scene->mRootNode);

    processNode(scene->mRootNode, scene);
}

void Collider::recursiveNodeProcess(aiNode* node)
{
    ai_nodes.push_back(node);

    for (int i = 0; i < node->mNumChildren; i++)
        recursiveNodeProcess(node->mChildren[i]);
}

void Collider::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

void Collider::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        vertices.push_back(vertex);
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    meshCollider.MeshCollider(vertices, indices, matHandle);
}

void Collider::SetHierarchy()
{
    transform->finalModelMatrix = transform->model;

    if (transform->parent != nullptr)
    {
        Transform* parent = transform->parent;

        while (parent != nullptr)
        {
            transform->finalModelMatrix *= parent->model;
            parent = parent->parent;
        }
    }
}

bool Collider::RaySphereIntersection(glm::vec3 ray_orig, glm::vec3 ray_dir)
{
    glm::vec3 m = ray_orig - transform->position;
    float b = glm::dot(m, ray_dir);
    float c = glm::dot(m, m) - radius * radius;

    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0) 
    if (c > 0.0f && b > 0.0f) return false;
    float discr = b * b - c;

    // A negative discriminant corresponds to ray missing sphere 
    if (discr < 0.0f) return false;

    // Ray now found to intersect sphere, compute smallest t value of intersection
    float t = -b - sqrt(discr);

    // If t is negative, ray started inside sphere so clamp t to zero 
    if (t < 0.0f) return false;

    return true;
}

float Collider::RayBoxOBBIntersection(glm::vec3 ray_orig, glm::vec3 ray_dir)
{
    float tMin = 0.0f;
    float tMax = FLT_MAX;

    glm::vec3 OBBposition_worldspace(transform->finalModelMatrix[3]);

    glm::vec3 delta = OBBposition_worldspace - ray_orig;

    // Test intersection with the 2 planes perpendicular to the OBB's X axis
    {
        glm::vec3 xaxis(transform->finalModelMatrix[0]);
        float e = glm::dot(xaxis, delta);
        float f = glm::dot(ray_dir, xaxis);

        if (fabs(f) > 0.001f) { // Standard case

            float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
            float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
            // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

            // We want t1 to represent the nearest intersection, 
            // so if it's not the case, invert t1 and t2
            if (t1 > t2) {
                float w = t1; t1 = t2; t2 = w; // swap t1 and t2
            }

            // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
            if (t2 < tMax)
                tMax = t2;
            // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
            if (t1 > tMin)
                tMin = t1;

            // And here's the trick :
            // If "far" is closer than "near", then there is NO intersection.
            // See the images in the tutorials for the visual explanation.
            if (tMax < tMin)
                return false;

        }
        else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
            if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
                return false;
        }
    }


    // Test intersection with the 2 planes perpendicular to the OBB's Y axis
    // Exactly the same thing than above.
    {
        glm::vec3 yaxis(transform->finalModelMatrix[1]);
        float e = glm::dot(yaxis, delta);
        float f = glm::dot(ray_dir, yaxis);

        if (fabs(f) > 0.001f) {

            float t1 = (e + aabb_min.y) / f;
            float t2 = (e + aabb_max.y) / f;

            if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
                return false;

        }
        else {
            if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
                return false;
        }
    }


    // Test intersection with the 2 planes perpendicular to the OBB's Z axis
    // Exactly the same thing than above.
    {
        glm::vec3 zaxis(transform->finalModelMatrix[2]);
        float e = glm::dot(zaxis, delta);
        float f = glm::dot(ray_dir, zaxis);

        if (fabs(f) > 0.001f) {

            float t1 = (e + aabb_min.z) / f;
            float t2 = (e + aabb_max.z) / f;

            if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
                return false;

        }
        else {
            if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
                return false;
        }
    }

    return tMin;
}
