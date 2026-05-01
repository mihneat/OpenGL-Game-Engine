#include "main/GameEngine/ComponentBase/Components/Rendering/MeshRenderer.h"

#include <iostream>
#include <glm/gtx/color_space.hpp>

#include "main/GameEngine/Systems/Rendering/MeshResourceManager.h"

using namespace std;
using namespace component;
using namespace component;
using namespace transform;

std::unordered_map<MeshRenderer::MeshEnum, ExtraMeshData> MeshRenderer::loadedMeshes;

MeshRenderer::MeshRenderer(
	Transform* transform,
    MeshEnum meshType,
	string meshName,
    rendering::Material* material,
    LayerEnum layer,
    glm::vec3 meshScale,
    glm::vec4 meshColor,
    bool renderInWorldSpace,
    bool debugOnly
) : Component(transform)
{
    this->meshType = meshType;
    this->meshScale = meshScale;
	this->color = meshColor;
    this->debugOnly = debugOnly;
    this->renderInWorldSpace = renderInWorldSpace;
    this->layer = layer;
    this->texture1 = nullptr;
    this->texture2 = nullptr;
    this->texture3 = nullptr;
    this->texture4 = nullptr;
    this->material = material;
    this->materialOverrides = nullptr;
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::SetMesh(MeshEnum newMeshType)
{
    meshType = newMeshType;
}

void MeshRenderer::SetColor(glm::vec4 newColor)
{
    color = newColor;
}

void MeshRenderer::SetTexture(rendering::Texture* newTexture)
{
    this->texture1 = newTexture;
}

void MeshRenderer::SetTexture2(rendering::Texture* newTexture)
{
    this->texture2 = newTexture;
}

void MeshRenderer::SetTexture3(rendering::Texture* newTexture)
{
    this->texture3 = newTexture;
}

void MeshRenderer::SetTexture4(rendering::Texture* newTexture)
{
    this->texture4 = newTexture;
}

void MeshRenderer::SetTextureScale(glm::vec2 newTexScale)
{
    this->texScale = newTexScale;
}

void MeshRenderer::SetMaterialOverrides(rendering::MaterialOverrides* materialOverrides)
{
    this->materialOverrides = materialOverrides;
}

void MeshRenderer::SetFaceCullingMode(FaceCullingMode newFaceCullingMode)
{
    this->faceCullingMode = newFaceCullingMode;
}

// Reference: https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
bool MeshRenderer::IsInFrustum(const utils::Frustum& frustum) const
{
    // Retrieve the bounding box
    const utils::AABB& boundingBox = loadedMeshes[meshType].boundingBox;
    
    // Get global scale
    const auto model = transform->GetModelMatrix();
    const glm::vec3 globalCenter{ model * glm::vec4(boundingBox.center, 1.f) };

    // Extract the scale
    glm::vec3 scale = transform->GetWorldScale();

    // Scaled orientation
    const glm::vec3 right = transform->right * boundingBox.extents.x * scale.x;
    const glm::vec3 up = transform->up * boundingBox.extents.y * scale.y;
    const glm::vec3 forward = transform->forward * boundingBox.extents.z * scale.z;

    const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

    const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

    const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

    // We don't need to divide scale because it's based on the half extension of the AABB
    const utils::AABB globalAABB(globalCenter, newIi, newIj, newIk);

    return globalAABB.IsOnOrInFrontOfPlane(frustum.leftFace) &&
           globalAABB.IsOnOrInFrontOfPlane(frustum.rightFace) &&
           globalAABB.IsOnOrInFrontOfPlane(frustum.topFace) &&
           globalAABB.IsOnOrInFrontOfPlane(frustum.bottomFace) &&
           globalAABB.IsOnOrInFrontOfPlane(frustum.nearFace) &&
           globalAABB.IsOnOrInFrontOfPlane(frustum.farFace);
}

void MeshRenderer::MeshFactory()
{
    // Check if mesh has already been created
    if (loadedMeshes.find(this->meshType) != loadedMeshes.end()) {
        return;
    }
    
    loadedMeshes[this->meshType] = ExtraMeshData();
    
    mesh_desc meshDescription;
    generateMesh = true;

    switch (this->meshType) {
    case Square:
        meshDescription = CreateSquare();
        break;

    case FragmentedSquare:
        meshDescription = CreateFragmentedSquare();
        break;

    case Circle:
        meshDescription = CreateCircle(100, false, false);
        break;

    case Cylinder:
        meshDescription = CreateCylinder(8);
        break;

    case Cube:
        meshDescription = CreateCube();
        break;

    case Line:
        meshDescription = CreateLine();
        break;

    case CubeMesh:
        LoadMesh("box.obj", "primitives");
        break;

    case Sphere:
        LoadMesh("sphere.obj", "primitives");
        break;

    case Heart:
        LoadMesh("zelda_heart.fbx", "my_models/steep/zelda_heart");
        break;

    case Cone:
        LoadMesh("cone.obj", "primitives");
        break;

    case None:
    default:
        return;
    }

    if (generateMesh) {
        rendering::MeshResourceManager::GenerateMesh(std::to_string(meshType), meshDescription.drawMode, meshDescription.vertices, meshDescription.indices);
    }

    GenerateAABB();
}

void MeshRenderer::LoadMesh(const std::string name, const std::string path)
{
    generateMesh = false;
    rendering::MeshResourceManager::LoadMesh(std::to_string(meshType), name, path);
}

void MeshRenderer::GenerateAABB()
{
    const Mesh* mesh = rendering::MeshResourceManager::meshes[std::to_string(meshType)];
    ExtraMeshData& meshData = loadedMeshes[this->meshType];
    
    // Compute the min and max points
    glm::vec3 minPoint;
    glm::vec3 maxPoint;
    
    if (generateMesh)
    {
        // Use the VertexFormat, for manually described objects
        minPoint = mesh->vertices[0].position;
        maxPoint = mesh->vertices[0].position;

        for (const VertexFormat& vertex : mesh->vertices)
        {
            minPoint = glm::min(minPoint, vertex.position);
            maxPoint = glm::max(maxPoint, vertex.position);
        }
    } else
    {
        // Use the positions vector, for imported models
        minPoint = mesh->positions[0];
        maxPoint = mesh->positions[0];

        for (const glm::vec3& position : mesh->positions)
        {
            minPoint = glm::min(minPoint, position);
            maxPoint = glm::max(maxPoint, position);
        }
    }

    meshData.boundingBox = utils::AABB(minPoint, maxPoint);
}

mesh_desc MeshRenderer::CreateSquare()
{
    vector<VertexFormat> vertices
    {
        VertexFormat(glm::vec3(-1, -1, 0), glm::vec3(1.0f), glm::vec3_forward, glm::vec2(1, 1)), // 0
        VertexFormat(glm::vec3(-1,  1, 0), glm::vec3(1.0f), glm::vec3_forward, glm::vec2(1, 0)), // 1
        VertexFormat(glm::vec3(1, -1, 0), glm::vec3(1.0f), glm::vec3_forward, glm::vec2(0, 1)), // 2
        VertexFormat(glm::vec3(1,  1, 0), glm::vec3(1.0f), glm::vec3_forward, glm::vec2(0, 0)), // 3
    };

    vector<unsigned int> indices =
    {
        0, 2, 1,
        2, 3, 1
    };

    return { vertices, indices, GL_TRIANGLES };
}

mesh_desc MeshRenderer::CreateFragmentedSquare()
{
    // Define fragment count
    constexpr int fragCount = 1000;

    // Create the vertices array
    vector<VertexFormat> vertices;

    // Create the vertices matrix
    vector<vector<glm::vec3>> verticesMat(fragCount);
    vector<vector<glm::vec2>> texMat(fragCount);
    for (int i = 0; i < fragCount; ++i) {
        verticesMat[i] = vector<glm::vec3>(fragCount);
        texMat[i] = vector<glm::vec2>(fragCount);
    }

    for (int l = 0; l < fragCount; ++l) {
        for (int c = 0; c < fragCount; ++c) {
            verticesMat[l][c] = glm::vec3(
                glm::mix(-1.0f, 1.0f, 1.0f * c / (fragCount - 1)),
                glm::mix(-1.0f, 1.0f, 1.0f * l / (fragCount - 1)),
                0.0f
            );

            texMat[l][c] = glm::vec2(
                glm::mix(1.0f, 0.0f, 1.0f * c / (fragCount - 1)),
                glm::mix(1.0f, 0.0f, 1.0f * l / (fragCount - 1))
            );

            vertices.push_back(VertexFormat(verticesMat[l][c], glm::vec3(1.0f), -glm::vec3_forward, texMat[l][c]));
        }
    }

    // Create the indices
    vector<unsigned int> indices;
    for (int l = 0; l < fragCount - 1; ++l) {
        for (int c = 0; c < fragCount - 1; ++c) {
            int bl, br, tl, tr;
            tl = l * fragCount + c;
            tr = l * fragCount + c + 1;
            bl = (l + 1) * fragCount + c;
            br = (l + 1) * fragCount + c + 1;

            indices.push_back(bl);
            indices.push_back(br);
            indices.push_back(tl);
            indices.push_back(br);
            indices.push_back(tr);
            indices.push_back(tl);
        }
    }

    return { vertices, indices, GL_TRIANGLES };
}

mesh_desc MeshRenderer::CreateCircle(const int circleVertexCount, const bool makeRainbow, const bool makeHollow)
{
    // Define the vertices array, initially only with the center
    vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f), makeRainbow ? glm::vec3(1.0f) : glm::vec3(1.0f))
    };

    // Create the indices vector, using the GL_TRIANGLE_FAN draw mode
    // Set the circle's center as the first index
    vector<unsigned int> indices;
    if (!makeHollow) {
        indices.push_back(0);
    }

    // Define the angular step (the angle of each triangle, from the center)
    float radius = 1.0f;
    float increment = 2 * glm::pi<float>() / circleVertexCount;
    for (int i = 0; i < circleVertexCount; ++i) {
        // Create a new vertex using sine and cosine, and also the angular step
        // Use the HSV color space and convert it to RGB using glm::rgbColor
        // More info on the internet :))
        vertices.push_back(VertexFormat(
            // The position on the circle
            glm::vec3(radius * glm::cos(increment * i), radius * glm::sin(increment * i), 0.0f),
            // The color of the vertex
            makeRainbow
            ? glm::rgbColor(glm::vec3(360.0f / circleVertexCount * i, 1.0f, 1.0f))
            : glm::vec3(1.0f)));

        // Add the new vertex to the indices vector
        indices.push_back(i + 1);
    }

    // Add the first circle index to loop back the triangle on itself
    indices.push_back(1);

    return { vertices, indices, makeHollow ? GL_LINE_LOOP : GL_TRIANGLE_FAN };
}

mesh_desc MeshRenderer::CreateCylinder(const int segmentCount)
{
    // Define the vertices array, initially only with the center
    vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(1.0f), glm::vec3_up),
        VertexFormat(glm::vec3(0.0f, -1.0f, 0.0f),  glm::vec3(1.0f), glm::vec3_down)
    };

    // Create the indices vector
    vector<unsigned int> indices;

    // Define the angular step (the angle of each triangle, from the center)
    constexpr float radius = 1.0f;
    const float increment = 2 * glm::pi<float>() / segmentCount;

    // Create the upper circle
    int startVertex = 2;
    for (int i = 0; i < segmentCount; ++i) {
        // Create a new vertex using sine and cosine
        vertices.push_back(VertexFormat(
            // The position on the circle
            glm::vec3(radius * glm::cos(increment * i), 1.0f, radius * glm::sin(increment * i)),
            // The color of the vertex
            glm::vec3(1.0f),
            glm::vec3_up));

        // Create the new triangle
        if (i > 0)
        {
            indices.push_back(startVertex + i - 1);
            indices.push_back(startVertex + i);
            indices.push_back(0);
        }
    }
    
    indices.push_back(startVertex + segmentCount - 1);
    indices.push_back(startVertex);
    indices.push_back(0);

    // Create the lower circle
    int startVertex2 = startVertex + segmentCount;
    for (int i = 0; i < segmentCount; ++i) {
        // Create a new vertex using sine and cosine
        vertices.push_back(VertexFormat(
            // The position on the circle
            glm::vec3(radius * glm::cos(increment * i), -1.0f, radius * glm::sin(increment * i)),
            // The color of the vertex
            glm::vec3(1.0f),
            glm::vec3_down));

        // Create the new triangle for the circle
        if (i > 0)
        {
            indices.push_back(startVertex2 + i);
            indices.push_back(startVertex2 + i - 1);
            indices.push_back(1);
        }
    }
    
    indices.push_back(startVertex2);
    indices.push_back(startVertex2 + segmentCount - 1);
    indices.push_back(1);
    
    // Create the tube
    int startVertex3 = startVertex + 2 * segmentCount;
    for (int i = 0; i < segmentCount; ++i) {
        const glm::vec3 horizontalPos = glm::vec3(glm::cos(increment * i), 0, glm::sin(increment * i)) * radius;
        vertices.push_back(VertexFormat(
            // The position on the circle
            glm::vec3(horizontalPos.x, 1.0f, horizontalPos.z),
            // The color of the vertex
            glm::vec3(1.0f),
            horizontalPos));
        
        vertices.push_back(VertexFormat(
            // The position on the circle
            glm::vec3(horizontalPos.x, -1.0f, horizontalPos.z),
            // The color of the vertex
            glm::vec3(1.0f),
            horizontalPos));

        if (i > 0)
        {
            // Link the vertices to form the body of the cylinder
            indices.push_back(startVertex3 + 2 * i);
            indices.push_back(startVertex3 + 2 * (i - 1));
            indices.push_back(startVertex3 + 2 * (i - 1) + 1);
            
            indices.push_back(startVertex3 + 2 * (i - 1) + 1);
            indices.push_back(startVertex3 + 2 * i + 1);
            indices.push_back(startVertex3 + 2 * i);
        }
        
        indices.push_back(startVertex3);
        indices.push_back(startVertex3 + 2 * (segmentCount - 1));
        indices.push_back(startVertex3 + 2 * (segmentCount - 1) + 1);
            
        indices.push_back(startVertex3 + 2 * (segmentCount - 1) + 1);
        indices.push_back(startVertex3 + 1);
        indices.push_back(startVertex3);
    }

    return { vertices, indices, GL_TRIANGLES };
}

mesh_desc MeshRenderer::CreateCube()
{
    vector<VertexFormat> vertices
    {
        // Face 2-3-0-1
        VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(1.0f), glm::vec3( 0, -1,  0), glm::vec2(0, 0)), // 0
        VertexFormat(glm::vec3( 1, -1, -1), glm::vec3(1.0f), glm::vec3( 0, -1,  0), glm::vec2(0, 1)), // 1
        VertexFormat(glm::vec3(-1, -1,  1), glm::vec3(1.0f), glm::vec3( 0, -1,  0), glm::vec2(1, 0)), // 2
        VertexFormat(glm::vec3( 1, -1,  1), glm::vec3(1.0f), glm::vec3( 0, -1,  0), glm::vec2(1, 1)), // 3


        // Face 4-5-6-7
        VertexFormat(glm::vec3(-1,  1,  1), glm::vec3(1.0f), glm::vec3( 0,  1,  0), glm::vec2(0, 0)), // 4
        VertexFormat(glm::vec3( 1,  1,  1), glm::vec3(1.0f), glm::vec3( 0,  1,  0), glm::vec2(0, 1)), // 5
        VertexFormat(glm::vec3(-1,  1, -1), glm::vec3(1.0f), glm::vec3( 0,  1,  0), glm::vec2(1, 0)), // 6
        VertexFormat(glm::vec3( 1,  1, -1), glm::vec3(1.0f), glm::vec3( 0,  1,  0), glm::vec2(1, 1)), // 7


        // Face 0-1-4-5
        VertexFormat(glm::vec3(-1, -1,  1), glm::vec3(1.0f), glm::vec3( 0,  0,  1), glm::vec2(0, 0)), // 8
        VertexFormat(glm::vec3( 1, -1,  1), glm::vec3(1.0f), glm::vec3( 0,  0,  1), glm::vec2(0, 1)), // 9
        VertexFormat(glm::vec3(-1,  1,  1), glm::vec3(1.0f), glm::vec3( 0,  0,  1), glm::vec2(1, 0)), // 10
        VertexFormat(glm::vec3( 1,  1,  1), glm::vec3(1.0f), glm::vec3( 0,  0,  1), glm::vec2(1, 1)), // 11


        // Face 3-2-7-6
        VertexFormat(glm::vec3( 1, -1, -1), glm::vec3(1.0f), glm::vec3( 0,  0, -1), glm::vec2(0, 0)), // 12
        VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(1.0f), glm::vec3( 0,  0, -1), glm::vec2(0, 1)), // 13
        VertexFormat(glm::vec3( 1,  1, -1), glm::vec3(1.0f), glm::vec3( 0,  0, -1), glm::vec2(1, 0)), // 14
        VertexFormat(glm::vec3(-1,  1, -1), glm::vec3(1.0f), glm::vec3( 0,  0, -1), glm::vec2(1, 1)), // 15


        // Face 1-3-5-7
        VertexFormat(glm::vec3( 1, -1,  1), glm::vec3(1.0f), glm::vec3( 1,  0,  0), glm::vec2(0, 0)), // 16
        VertexFormat(glm::vec3( 1, -1, -1), glm::vec3(1.0f), glm::vec3( 1,  0,  0), glm::vec2(0, 1)), // 17
        VertexFormat(glm::vec3( 1,  1,  1), glm::vec3(1.0f), glm::vec3( 1,  0,  0), glm::vec2(1, 0)), // 18
        VertexFormat(glm::vec3( 1,  1, -1), glm::vec3(1.0f), glm::vec3( 1,  0,  0), glm::vec2(1, 1)), // 19


        // Face 2-0-6-4
        VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(1.0f), glm::vec3(-1,  0,  0), glm::vec2(0, 0)), // 20
        VertexFormat(glm::vec3(-1, -1,  1), glm::vec3(1.0f), glm::vec3(-1,  0,  0), glm::vec2(0, 1)), // 21
        VertexFormat(glm::vec3(-1,  1, -1), glm::vec3(1.0f), glm::vec3(-1,  0,  0), glm::vec2(1, 0)), // 22
        VertexFormat(glm::vec3(-1,  1,  1), glm::vec3(1.0f), glm::vec3(-1,  0,  0), glm::vec2(1, 1)), // 23


    };

    vector<unsigned int> indices =
    {
        // Face 2-3-0-1: 0 - 3
        0, 1, 2,
        1, 3, 2,

        // Face 4-5-6-7: 4 - 7
        4, 5, 6,
        5, 7, 6,

        // Face 0-1-4-5: 8 - 11
        8, 9, 10,
        9, 11, 10,

        // Face 3-2-7-6: 12 - 15
        12, 13, 14,
        13, 15, 14,

        // Face 1-3-5-7: 16 - 19
        16, 17, 18,
        17, 19, 18,

        // Face 2-0-6-4: 20 - 23
        20, 21, 22,
        21, 23, 22

    };

    return { vertices, indices, GL_TRIANGLES };
}

mesh_desc MeshRenderer::CreateLine()
{
    vector<VertexFormat> vertices
    {
        VertexFormat(glm::vec3(0, 0, 0), glm::vec3(1.0f), glm::vec3_down, glm::vec2(0, 0)),
        VertexFormat(glm::vec3(0,  0, -1), glm::vec3(1.0f), glm::vec3_up, glm::vec2(0, 1)),
    };

    vector<unsigned int> indices =
    {
        0, 1
    };

    return { vertices, indices, GL_LINES };
}