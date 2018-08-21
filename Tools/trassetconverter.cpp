#include "nlohmann/json.hpp"
#include "tr/graphics/StaticMesh.h"
#include "tr/util/Log.h"
#include "tr/util/Serializer.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fstream>
#include <string>

using namespace tr;

constexpr uint16 V_MAJ = 0;
constexpr uint16 V_MIN = 0;

void process_node(aiNode *node, const aiScene *scene);
void process_mesh(aiMesh *mesh, const aiScene *scene);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        tr::Log().error("Invalide amount of argmuments: {}", argc);
        return 1;
    }

    std::string file(argv[1]);

    if (!tr::fs::FileExists(file)) {
        tr::Log().error("File {} does not exist.", file);
        return 1;
    }

    Assimp::Importer importer;
    const auto *     scene = importer.ReadFile(
        file.c_str(),
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
            | aiProcess_GenNormals | aiProcess_GenUVCoords
            | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
        || !scene->mRootNode) {
        tr::Log().error("Assimp: {}", importer.GetErrorString());
        return 1;
    }

    process_node(scene->mRootNode, scene);

    return 0;
}

void process_node(aiNode *node, const aiScene *scene)
{
    for (uint i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        process_mesh(mesh, scene);
    }

    for (uint i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene);
    }
}

void process_mesh(aiMesh *mesh, const aiScene *scene)
{
    const std::string         name(mesh->mName.C_Str());
    std::vector<Vertex_PNTBU> vertices;
    vertices.reserve(mesh->mNumVertices);

    Log().info("Exporting Mesh {}. Vertice Count: {} Index Count: {}", name,
               mesh->mNumVertices, mesh->mNumFaces * 3);

    for (uint i = 0; i < mesh->mNumVertices; i++) {
        Vertex_PNTBU vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0]) {
            vertex.uv.x = mesh->mTextureCoords[0][i].x;
            vertex.uv.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.uv = { 0.f, 0.f };
        }

        if (mesh->HasTangentsAndBitangents()) {
            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;

            vertex.bitangent.x = mesh->mBitangents[i].x;
            vertex.bitangent.y = mesh->mBitangents[i].y;
            vertex.bitangent.z = mesh->mBitangents[i].z;
        }

        vertices.push_back(vertex);
    }

    std::vector<uint> indices;
    indices.reserve(mesh->mNumFaces * 3);

    for (uint i = 0; i < mesh->mNumFaces; i++) {
        aiFace &face = mesh->mFaces[i];
        for (uint j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    json j;
    j["type"]                   = "StaticMesh";
    j["name"]                   = name;
    std::vector<uint8> msg_pack = json::to_msgpack(j);

    std::ofstream mesh_out(name + ".trb");
    mesh_out.write("TRBF", 4);

    Serializer<std::ofstream> ser(mesh_out);
    ser << V_MAJ << V_MIN << msg_pack;
    ser << vertices << indices;
}
