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
void process_material(aiMaterial *material, const aiScene *scene);

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

    if (mesh->mMaterialIndex >= 0)
        process_material(scene->mMaterials[mesh->mMaterialIndex], scene);

    json j;
    j["type"]                   = "StaticMesh";
    j["name"]                   = name;
    std::vector<uint8> msg_pack = json::to_msgpack(j);

    std::ofstream mesh_out(name + ".trb", std::ios::binary);
    mesh_out.write("TRBF", 4);

    Serializer<std::ofstream> ser(mesh_out);
    ser << V_MAJ << V_MIN << msg_pack;
    ser << vertices << indices;
}

struct texture_t {
    std::string name;
    std::string type;
    json        handle;
};

texture_t create_material(const std::string &name, const std::string &type);

void process_material(aiMaterial *material, const aiScene *scene)
{

    std::vector<texture_t> diffuse_textures;
    std::vector<texture_t> specular_textures;
    std::vector<texture_t> normal_textures;
    float                  shininess;
    aiString               materialname;

    for (uint i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE);
         i++) {
        aiString path;
        material->GetTexture(aiTextureType_DIFFUSE, i, &path);
        std::string file(path.C_Str());
        diffuse_textures.push_back(create_material(file, "diffuse"));
    }

    for (uint i = 0; i < material->GetTextureCount(aiTextureType_SPECULAR);
         i++) {
        aiString path;
        material->GetTexture(aiTextureType_SPECULAR, i, &path);
        std::string file(path.C_Str());
        specular_textures.push_back(create_material(file, "specular"));
    }

    for (uint i = 0; i < material->GetTextureCount(aiTextureType_NORMALS);
         i++) {
        aiString path;
        material->GetTexture(aiTextureType_NORMALS, i, &path);
        std::string file(path.C_Str());
        normal_textures.push_back(create_material(file, "normal"));
    }

    material->Get(AI_MATKEY_SHININESS, shininess);
    material->Get(AI_MATKEY_NAME, materialname);

    json phongmaterial;
    phongmaterial["type"]         = "PhongMaterial";
    phongmaterial["name"]         = materialname.C_Str();
    phongmaterial["dependencies"] = json::array();

    for (auto &t : diffuse_textures) {
        phongmaterial["dependencies"].push_back(t.handle);
    }

    for (auto &t : specular_textures) {
        phongmaterial["dependencies"].push_back(t.handle);
    }

    for (auto &t : normal_textures) {
        phongmaterial["dependencies"].push_back(t.handle);
    }

    if (diffuse_textures.size() > 0) {
        phongmaterial["diffuse"] = diffuse_textures[0].name;
    }
    if (specular_textures.size() > 0) {
        phongmaterial["specular"] = diffuse_textures[0].name;
    }
    if (normal_textures.size() > 0) {
        phongmaterial["normal"] = diffuse_textures[0].name;
    }

    phongmaterial["shininess"] = shininess;

    std::ofstream of(fmt::format("material_{}.json", materialname.C_Str()));
    of << phongmaterial;
}

texture_t create_material(const std::string &name, const std::string &type)
{
    if (!fs::FileExists(name)) {
        Log().error("Couldnt find {} for texture loading", name);
        return {};
    }

    Log().info("Exporting texture name: {} type: {}", name, type);

    json j;
    j["type"] = "Image";
    j["file"] = fmt::format("$ENGINE/{}", name);
    j["name"] = fmt::format("image_{}_{}", type, fs::Stem(name));

    json texture;
    texture["type"]             = "Texture";
    texture["compression"]      = false;
    texture["generate_mipmaps"] = true;
    texture["mag_filter"]       = 1;
    texture["min_filter"]       = 1;
    texture["wrap_s"]           = 1;
    texture["wrap_t"]           = 1;
    texture["dependencies"]     = json::array({ j });

    std::string handlename
        = fmt::format("texture_{1}_{0}", fs::Stem(name), type);

    texture["name"] = handlename;

    texture_t t;
    t.handle = texture;
    t.type   = type;
    t.name   = handlename;

    return t;
}
