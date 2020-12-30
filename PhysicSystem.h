#pragma once

#ifndef PHYSICSYSTEM_H
#define PHYSICSYSTEM_H

#include <reactphysics3d/reactphysics3d.h>
#include <vector>

//using namespace reactphysics3d;

enum class PhysicBodyType
{
    COLLISION_BODY,
    RIGIDBODY_STATIC,
    RIGIDBODY_KINEMATIC,
    RIGIDBODY_DINAMIC
};

enum class ColliderShapeType
{
    NULL_SHAPE,
    BOX_SHAPE,
    SPHERE_SHAPE,
    CAPSULE_SHAPE,
    MESH_SHAPE
};

class PhysicSystem
{
private:
    reactphysics3d::PhysicsCommon physicsCommon;
    reactphysics3d::PhysicsWorld* world;
    rp3d::PolygonVertexArray::PolygonFace* polygonFaces;
    rp3d::PolygonVertexArray* polygonVertexArray;
    rp3d::PolyhedronMesh* polyhedronMesh;
    rp3d::ConvexMeshShape* convexMeshShape;
    //Vector3 position;
    //Quaternion orientation = reactphysics3d::Quaternion::identity();
    reactphysics3d::RigidBody* body;
    const reactphysics3d::decimal timeStep = 1.0f / 120.0f;
    float accumulator = 0.0f;
    float* deltaTime;

    std::vector<reactphysics3d::CollisionBody*> cBodies;
    std::vector<reactphysics3d::Transform> previusTransform;
    int findVertexIndex(const std::vector<reactphysics3d::Vector3>& vertices, const reactphysics3d::Vector3& vertex);
    //std::vector<Box*> mBoxes;

    std::vector<int> mConvexMeshIndices;
public:
    std::vector<reactphysics3d::Collider*> rigidColliders;
    std::vector<std::pair<reactphysics3d::Transform, reactphysics3d::RigidBody*>> rBodies;
    PhysicSystem();
    void Update();
    void SetDeltaTime(float* delta);
    void AddCollisionBody(reactphysics3d::Transform* trMatrix);
    void AddRigidBody(reactphysics3d::Transform* trMatrix, PhysicBodyType type = PhysicBodyType::RIGIDBODY_DINAMIC);
    void AddCollider(ColliderShapeType shapeType, float radius);
    void AddCollider(ColliderShapeType shapeType, reactphysics3d::Vector3 size);
    void AddCollider(ColliderShapeType shapeType, reactphysics3d::Vector2 size);
    void AddCollider(ColliderShapeType shapeType, std::vector<rp3d::Vector3> rawVertices, std::vector<unsigned int> rawIndices, unsigned int numFaces, unsigned int numVertices);
    inline reactphysics3d::PhysicsWorld* GetWorld() {return world; }
    reactphysics3d::Transform* GetTransform(unsigned int id);
};

#endif // !PHYSICSYSTEM_H
