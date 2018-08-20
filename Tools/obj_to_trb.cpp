#include "nlohmann/json.hpp"
#include "tr/filesystem/Filesystem.h"
#include "tr/graphics/StaticMesh.h"
#include "tr/util/Log.h"
#include "tr/util/Serializer.h"

#include <fstream>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using json = nlohmann::json;

constexpr tr::uint16 VERSION_MAJOR = 0;
constexpr tr::uint16 VERSION_MINOR = 0;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        tr::Log().error("Invalid amount of arguments! Aborting...");
        return 1;
    }

    std::string file(argv[1]);

    if (!tr::fs::FileExists(file)) {
        tr::Log().error("Couldnt find file {}", file);
        return 1;
    }

    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes(1);
    std::vector<tinyobj::material_t> materials(1);

    std::string err;
    bool        ret
        = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file.c_str());

    if (!err.empty()) {
        tr::Log().warn("TinyObjLoader reported on loading: {}", err);
    }

    if (!ret) {
        tr::Log().error("Error during loading");
        return 1;
    }

    std::vector<tr::Vertex_PNTBU> tr_vertices;
    tr_vertices.reserve(attrib.vertices.size() / 3);

    for (tr::uint32 i = 0; i < attrib.vertices.size(); i += 3) {
        tr_vertices.push_back(tr::Vertex_PNTBU{});
        auto &v      = tr_vertices.back();
        v.position.x = attrib.vertices[i + 0];
        v.position.y = attrib.vertices[i + 1];
        v.position.z = attrib.vertices[i + 2];
        tr::Log().info("Added vertex {:.5f} {} {}", v.position.x, v.position.y,
                       v.position.y);
    }

    for (tr::uint32 i = 0; i < attrib.normals.size(); i += 3) {
        auto &v    = tr_vertices[i];
        v.normal.x = attrib.vertices[i + 0];
        v.normal.y = attrib.normals[i + 1];
        v.normal.z = attrib.normals[i + 2];
    }

    for (tr::uint32 i = 0; i < attrib.texcoords.size(); i += 2) {
        auto &v = tr_vertices[i];
        v.uv.x  = attrib.texcoords[i + 0];
        v.uv.y  = attrib.texcoords[i + 1];
    }

    for (auto &shape : shapes) {
        std::vector<tr::uint> indices;
        indices.reserve(shape.mesh.indices.size());
        for (auto index : shape.mesh.indices) {
            indices.push_back(static_cast<tr::uint>(index.vertex_index));
            tr::Log().info("Added index: {}", indices.back());
        }

        json j;
        j["type"]                       = "StaticMesh";
        j["name"]                       = shape.name;
        std::vector<tr::uint8> msg_pack = json::to_msgpack(j);

        std::ofstream mesh_out(shape.name + ".trb");
        mesh_out.write("TRBF", 4);

        tr::Serializer<std::ofstream> ser(mesh_out);
        ser << VERSION_MAJOR << VERSION_MINOR;
        ser << msg_pack;
        ser << tr_vertices << indices;
    }

    return 0;
}
