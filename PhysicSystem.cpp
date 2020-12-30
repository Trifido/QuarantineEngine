#include "PhysicSystem.h"

PhysicSystem::PhysicSystem()
{
    // Create the world settings 
    reactphysics3d::PhysicsWorld::WorldSettings settings;
    settings.defaultVelocitySolverNbIterations = 20;
    settings.defaultPositionSolverNbIterations = 20;
    settings.persistentContactDistanceThreshold = 0.01;
    settings.isSleepingEnabled = true;
    settings.gravity = reactphysics3d::Vector3(0, -9.81, 0);

    // Create the physics world with your settings 
    world = physicsCommon.createPhysicsWorld(settings);

    // Change the number of iterations of the velocity solver 
    world->setNbIterationsVelocitySolver(15);
    // Change the number of iterations of the position solver 
    world->setNbIterationsPositionSolver(8);
}

void PhysicSystem::Update()
{
    accumulator += *deltaTime;

    while (accumulator >= timeStep) {
        // Update the Dynamics world with a constant time step 
        world->update(timeStep);

        // Decrease the accumulated time 
        accumulator -= timeStep;
    }

    float factor = accumulator / timeStep;

    for (unsigned int i = 0; i < rBodies.size(); i++)
    {
        const reactphysics3d::Transform currentTransform = rBodies.at(i).second->getTransform();
        rBodies.at(i).first = reactphysics3d::Transform::interpolateTransforms(previusTransform.at(i), currentTransform, factor);

        previusTransform.at(i) = currentTransform;
    }

    //for (unsigned int i = 0; i < cBodies.size(); i++)
    //{
    //    const reactphysics3d::Transform& transform = cBodies.at(i)->getTransform();
    //    const reactphysics3d::Vector3& position = transform.getPosition();
    //    // Display the position of the body 
    //    std::cout << "Body Position: (" << position.x << ", " <<
    //        position.y << ", " << position.z << ")" << std::endl;
    //}
}

void PhysicSystem::SetDeltaTime(float* delta)
{
    deltaTime = delta;
}

void PhysicSystem::AddCollisionBody(reactphysics3d::Transform* trMatrix)
{
    //trMatrix = new reactphysics3d::Transform(Vector3(0.0, 10.0, 0.0), Quaternion::identity());
    //reactphysics3d::Transform tr = reactphysics3d::Transform(Vector3(0.0, 10.0, 0.0), Quaternion::identity());
    //std::cout << trMatrix->getPosition().x << " " << trMatrix->getPosition().y << "" << trMatrix->getPosition().z << std::endl;
    //reactphysics3d::CollisionBody* cbody;
    //cbody = world->createCollisionBody(reactphysics3d::Transform::identity());
    //cBodies.push_back(world->createCollisionBody(*trMatrix));
}

void PhysicSystem::AddRigidBody(reactphysics3d::Transform* trMatrix, PhysicBodyType type)
{
    previusTransform.push_back(*trMatrix);
    rBodies.push_back(std::pair<reactphysics3d::Transform, reactphysics3d::RigidBody*>(*trMatrix, world->createRigidBody(*trMatrix)));

    unsigned int id = rBodies.size() - 1;
    if (type == PhysicBodyType::RIGIDBODY_STATIC)
        rBodies.at(id).second->setType(reactphysics3d::BodyType::STATIC);
    if (type == PhysicBodyType::RIGIDBODY_KINEMATIC)
        rBodies.at(id).second->setType(reactphysics3d::BodyType::KINEMATIC);
}

void PhysicSystem::AddCollider(ColliderShapeType shapeType, float radius)
{
    if (shapeType == ColliderShapeType::SPHERE_SHAPE)
    {
        unsigned int id = rBodies.size() - 1;
        rigidColliders.push_back(rBodies.at(id).second->addCollider(physicsCommon.createSphereShape(radius), reactphysics3d::Transform::identity()));
    }
}

void PhysicSystem::AddCollider(ColliderShapeType shapeType, reactphysics3d::Vector3 size)
{
    if (shapeType == ColliderShapeType::BOX_SHAPE)
    {
        unsigned int id = rBodies.size() - 1;
        
        //rigidColliders.push_back(rBodies.at(id).second->addCollider(physicsCommon.createBoxShape(size), reactphysics3d::Transform::identity()));
        if (rBodies.at(id).second->getType() != reactphysics3d::BodyType::STATIC)
        {
            /*
            rigidColliders.push_back(rBodies.at(id).second->addCollider(physicsCommon.createBoxShape(size),
                reactphysics3d::Transform(reactphysics3d::Vector3(-0.25f, -0.25f, -0.25f), reactphysics3d::Quaternion::identity())));*/
            rigidColliders.push_back(rBodies.at(id).second->addCollider(physicsCommon.createBoxShape(size),
                reactphysics3d::Transform(reactphysics3d::Vector3(0.0, -0.65, 0.0), reactphysics3d::Quaternion::identity())));

            rBodies.at(id).second->updateMassPropertiesFromColliders();
            rp3d::Material& material = rigidColliders.at(id)->getMaterial();
            material.setBounciness(rp3d::decimal(0.0));
            material.setFrictionCoefficient(rp3d::decimal(0.9));
        }
        else
        {
            rigidColliders.push_back(rBodies.at(id).second->addCollider(physicsCommon.createBoxShape(size),
                reactphysics3d::Transform(reactphysics3d::Vector3(0.0f, -1.080, -2.150), reactphysics3d::Quaternion::identity())));

            rp3d::Material& material = rigidColliders.at(id)->getMaterial();
            material.setBounciness(rp3d::decimal(0.1));
            material.setFrictionCoefficient(rp3d::decimal(0.9));
        }
    }
}

void PhysicSystem::AddCollider(ColliderShapeType shapeType, reactphysics3d::Vector2 size)
{
    if (shapeType == ColliderShapeType::CAPSULE_SHAPE)
    {
        unsigned int id = rBodies.size() - 1;
        rigidColliders.push_back(rBodies.at(id).second->addCollider(physicsCommon.createCapsuleShape(size.x, size.y), reactphysics3d::Transform::identity()));
    }
}

void PhysicSystem::AddCollider(ColliderShapeType shapeType, std::vector<rp3d::Vector3> rawVertices, std::vector<unsigned int> rawIndices, unsigned int numFaces, unsigned int numVertices)
{
    if (shapeType == ColliderShapeType::MESH_SHAPE)
    {
        polygonFaces = new rp3d::PolygonVertexArray::PolygonFace[numFaces];
        rp3d::PolygonVertexArray::PolygonFace* face = polygonFaces;
        for (unsigned int f = 0; f < numFaces; f++)
        {
            for (unsigned int v = 0; v < 3; v++)
            {
                reactphysics3d::Vector3 vertex(rawVertices.at(rawIndices.at(f * 3 + v)));
                int vIndex = findVertexIndex(rawVertices, vertex);
                if (vIndex == -1) {
                    vIndex = rawVertices.size();
                    rawVertices.push_back(vertex);
                }
                mConvexMeshIndices.push_back(vIndex);
            }

            face->indexBase = f * 3;
            face->nbVertices = 3;
            face++;
        }

        // Create the polygon vertex array 
        polygonVertexArray = new rp3d::PolygonVertexArray(rawVertices.size(), &(rawVertices[0]), sizeof(reactphysics3d::Vector3),
            &(mConvexMeshIndices[0]), sizeof(int), numFaces, polygonFaces,
            rp3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
            rp3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

        // Create the polyhedron mesh 
        polyhedronMesh = physicsCommon.createPolyhedronMesh(polygonVertexArray);
        //// Create the convex mesh collision shape
        unsigned int id = rBodies.size() - 1;
        rigidColliders.push_back(rBodies.at(id).second->addCollider(physicsCommon.createConvexMeshShape(polyhedronMesh), reactphysics3d::Transform::identity()));
    }
}

reactphysics3d::Transform* PhysicSystem::GetTransform(unsigned int id)
{
    return &rBodies.at(id).first;
}

int PhysicSystem::findVertexIndex(const std::vector<reactphysics3d::Vector3>& vertices, const reactphysics3d::Vector3& vertex) {

    for (int i = 0; i < vertices.size(); i++) {
        if (vertices[i] == vertex) {
            return i;
        }
    }

    return -1;
}
