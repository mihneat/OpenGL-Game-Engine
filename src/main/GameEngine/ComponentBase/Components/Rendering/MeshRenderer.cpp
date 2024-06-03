#include "main/GameEngine/ComponentBase/Components/Rendering/MeshRenderer.h"

#include <iostream>
#include <glm/gtx/color_space.hpp>

#include "main/GameEngine/Systems/Rendering/MeshResourceManager.h"

using namespace std;
using namespace managers;
using namespace component;
using namespace transform;

std::unordered_set<std::string> MeshRenderer::meshNames;

MeshRenderer::MeshRenderer(
	Transform* transform,
    MeshEnum meshType,
	string meshName,
    const rendering::Material* material,
    LayerEnum layer,
    glm::vec3 meshScale,
    glm::vec4 meshColor,
    bool renderUI,
    bool debugOnly
) : Component(transform)
{
    // Get the scene reference
    scene = GameManager::GetInstance()->GetSceneReference();

    static long long staticMeshIndex = 0;
    // ++staticMeshIndex;
    this->meshIndex = staticMeshIndex;

    this->type = meshType;
    this->baseMeshName = std::string(meshName);
    std::string colorString = std::string(to_string(meshColor.r))
        .append(to_string(meshColor.g))
        .append(to_string(meshColor.b))
        .append(to_string(meshColor.a));
    this->meshName = std::string(meshName) + colorString;
	this->color = meshColor;
    this->scaleMatrix = Transform::GetScalingMatrix(meshScale);
    this->debugOnly = debugOnly;
    this->renderUI = renderUI;
    this->layer = layer;
    this->texture = "";
    this->material = material;
    this->materialOverrides = nullptr;

    generateMesh = true;

    if (meshNames.find(this->meshName) == meshNames.end()) {
        meshNames.insert(this->meshName);
        MeshFactory();
    }
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Start()
{
    MeshFactory();
}

/// <summary>
/// All of this hassle just to avoid a huge memory leak :)
/// </summary>
void MeshRenderer::SetColor(glm::vec4 newColor)
{
    color = newColor;
    std::string colorString = std::string(to_string(color.r))
        .append(to_string(color.g))
        .append(to_string(color.b))
        .append(to_string(color.a));

    // Create the new mesh name
    std::string newMeshName = std::string(this->baseMeshName).append(colorString);

    if (meshNames.find(newMeshName) == meshNames.end()) {
        meshNames.insert(newMeshName);

        this->meshName = newMeshName;
        MeshFactory();

        return;
    }

    this->meshName = newMeshName;
}

void MeshRenderer::SetTexture(std::string newTexture, glm::vec2 newTexScale)
{
    texture = newTexture;
    texScale = newTexScale;
}

void MeshRenderer::SetMaterialOverrides(rendering::MaterialOverrides* materialOverrides)
{
    this->materialOverrides = materialOverrides;
}

void MeshRenderer::MeshFactory()
{
    mesh_desc meshDescription;
    generateMesh = true;

    switch (this->type) {
    case Square:
        meshDescription = CreateSquare();
        break;

    case FragmentedSquare:
        meshDescription = CreateFragmentedSquare();
        break;

    case Circle:
        meshDescription = CreateCircle(100, false, false);
        break;

    case Cube:
        meshDescription = CreateCube();
        break;

    case CubeMesh:
        LoadMesh("box.obj", "primitives");
        break;

    case Sphere:
        LoadMesh("sphere.obj", "primitives");
        break;

    case ZeldaHeart:
        LoadMesh("zelda_heart.fbx", "my_models/steep/zelda_heart");
        break;

    case Cone:
        LoadMesh("cone.obj", "primitives");
        break;

    case None:
    default:
        return;
    }

    if (!generateMesh) {
        return;
    }

    rendering::MeshResourceManager::GenerateMesh(meshName, meshDescription.drawMode, meshDescription.vertices, meshDescription.indices);
}

void MeshRenderer::LoadMesh(const std::string name, const std::string path)
{
    generateMesh = false;
    rendering::MeshResourceManager::LoadMesh(meshName, name, path);
}

mesh_desc MeshRenderer::CreateSquare()
{
    vector<VertexFormat> vertices
    {
        VertexFormat(glm::vec3(-1, -1, 0), color), // 0
        VertexFormat(glm::vec3(-1,  1, 0), color), // 1
        VertexFormat(glm::vec3(1, -1, 0), color), // 2
        VertexFormat(glm::vec3(1,  1, 0), color), // 3
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
                glm::mix(-1.0f, 1.0f, 1.0f * c / (fragCount - 1)),
                glm::mix(-1.0f, 1.0f, 1.0f * l / (fragCount - 1))
            );

            vertices.push_back(VertexFormat(verticesMat[l][c], color, glm::vec3(0.0f, 0.0f, 1.0f), texMat[l][c]));
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
        VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f), makeRainbow ? glm::vec3(1.0f) : color)
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
            : color));

        // Add the new vertex to the indices vector
        indices.push_back(i + 1);
    }

    // Add the first circle index to loop back the triangle on itself
    indices.push_back(1);

    return { vertices, indices, makeHollow ? GL_LINE_LOOP : GL_TRIANGLE_FAN };
}

mesh_desc MeshRenderer::CreateCube()
{
    //vector<VertexFormat> vertices
    //{
    //    VertexFormat(glm::vec3(-1, -1, -1), color, glm::vec3(-1, -1, -1)), // 0
    //    VertexFormat(glm::vec3(-1, -1,  1), color, glm::vec3(-1, -1,  1)), // 1
    //    VertexFormat(glm::vec3(1, -1, -1), color, glm::vec3(1, -1, -1)), // 2
    //    VertexFormat(glm::vec3(1, -1,  1), color, glm::vec3(1, -1,  1)), // 3

    //    VertexFormat(glm::vec3(-1,  1, -1), color, glm::vec3(-1,  1, -1)), // 4
    //    VertexFormat(glm::vec3(-1,  1,  1), color, glm::vec3(-1,  1,  1)), // 5
    //    VertexFormat(glm::vec3(1,  1, -1), color, glm::vec3(1,  1, -1)), // 6
    //    VertexFormat(glm::vec3(1,  1,  1), color, glm::vec3(1,  1,  1)), // 7
    //};

    //vector<unsigned int> indices =
    //{
    //    2, 1, 0,
    //    2, 3, 1,

    //    5, 7, 4,
    //    7, 6, 4,

    //    1, 3, 5,
    //    3, 7, 5,

    //    2, 0, 6,
    //    0, 4, 6,

    //    3, 2, 7,
    //    2, 6, 7,

    //    0, 1, 4,
    //    1, 5, 4,
    //};

    vector<VertexFormat> vertices
    {
        // Face 2-3-0-1
        VertexFormat(glm::vec3(-1, -1, -1), color, glm::vec3( 0, -1,  0)), // 0
        VertexFormat(glm::vec3( 1, -1, -1), color, glm::vec3( 0, -1,  0)), // 1
        VertexFormat(glm::vec3(-1, -1,  1), color, glm::vec3( 0, -1,  0)), // 2
        VertexFormat(glm::vec3( 1, -1,  1), color, glm::vec3( 0, -1,  0)), // 3


        // Face 4-5-6-7
        VertexFormat(glm::vec3(-1,  1,  1), color, glm::vec3( 0,  1,  0)), // 4
        VertexFormat(glm::vec3( 1,  1,  1), color, glm::vec3( 0,  1,  0)), // 5
        VertexFormat(glm::vec3(-1,  1, -1), color, glm::vec3( 0,  1,  0)), // 6
        VertexFormat(glm::vec3( 1,  1, -1), color, glm::vec3( 0,  1,  0)), // 7


        // Face 0-1-4-5
        VertexFormat(glm::vec3(-1, -1,  1), color, glm::vec3( 0,  0,  1)), // 8
        VertexFormat(glm::vec3( 1, -1,  1), color, glm::vec3( 0,  0,  1)), // 9
        VertexFormat(glm::vec3(-1,  1,  1), color, glm::vec3( 0,  0,  1)), // 10
        VertexFormat(glm::vec3( 1,  1,  1), color, glm::vec3( 0,  0,  1)), // 11


        // Face 3-2-7-6
        VertexFormat(glm::vec3( 1, -1, -1), color, glm::vec3( 0,  0, -1)), // 12
        VertexFormat(glm::vec3(-1, -1, -1), color, glm::vec3( 0,  0, -1)), // 13
        VertexFormat(glm::vec3( 1,  1, -1), color, glm::vec3( 0,  0, -1)), // 14
        VertexFormat(glm::vec3(-1,  1, -1), color, glm::vec3( 0,  0, -1)), // 15


        // Face 1-3-5-7
        VertexFormat(glm::vec3( 1, -1,  1), color, glm::vec3( 1,  0,  0)), // 16
        VertexFormat(glm::vec3( 1, -1, -1), color, glm::vec3( 1,  0,  0)), // 17
        VertexFormat(glm::vec3( 1,  1,  1), color, glm::vec3( 1,  0,  0)), // 18
        VertexFormat(glm::vec3( 1,  1, -1), color, glm::vec3( 1,  0,  0)), // 19


        // Face 2-0-6-4
        VertexFormat(glm::vec3(-1, -1, -1), color, glm::vec3(-1,  0,  0)), // 20
        VertexFormat(glm::vec3(-1, -1,  1), color, glm::vec3(-1,  0,  0)), // 21
        VertexFormat(glm::vec3(-1,  1, -1), color, glm::vec3(-1,  0,  0)), // 22
        VertexFormat(glm::vec3(-1,  1,  1), color, glm::vec3(-1,  0,  0)), // 23


    };

    vector<unsigned int> indices =
    {
        // Face 2-3-0-1: 0 - 3
        0, 1, 2,
        3, 1, 2,

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
