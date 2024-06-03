#include "MeshResourceManager.h"

#include "core/managers/resource_path.h"
#include "main/GameEngine/Systems/FileSystem.h"
#include "utils/text_utils.h"

using namespace rendering;

std::unordered_map<std::string, Mesh *> MeshResourceManager::meshes;

void MeshResourceManager::LoadMesh(std::string meshId, std::string meshName, std::string meshPath)
{
    Mesh* mesh = new Mesh(meshId);
    mesh->LoadMesh(PATH_JOIN(FileSystem::rootDirectory, RESOURCE_PATH::MODELS, meshPath), meshName);
    meshes[mesh->GetMeshID()] = mesh;
}

void MeshResourceManager::GenerateMesh(
    const std::string& meshName,
    const int drawMode,
    const std::vector<VertexFormat>& vertices,
    const std::vector<unsigned int>& indices
 )
{
    Mesh* mesh = new Mesh(meshName);
    mesh->SetDrawMode(drawMode);
    mesh->InitFromData(vertices, indices);
    meshes[mesh->GetMeshID()] = mesh;
}