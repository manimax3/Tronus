#include "tr/filesystem/Filesystem.h"
#include "tr/util/Log.h"
#include "tr/graphics/StaticMesh.h"
#include "tiny_obj_loader.h"

#include <vector>

int main(int argc, char *argv[])
{
    if (argc != 1) {
        tr::Log().error("Invalid amount of arguments! Aborting...");
        return 1;
    } 

    std::string file(argv[0]);

    if (!tr::fs::FileExists(file)){
        tr::Log().error("Couldnt find file {}", file);
        return 1;
    }

    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;

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

    std::vector<tr::Vertex_PNTBU> tr_vertices(attrib.vertices.size());

    for (tr::uint32 i = 0; i < attrib.vertices.size(); i++) {
        tr_vertices[i].position.x = attrib.vertices[i + 0];
        tr_vertices[i].position.y = attrib.vertices[i + 1];
        tr_vertices[i].position.z = attrib.vertices[i + 2];

        tr_vertices[i].normal.x = attrib.vertices[i + 0];
        tr_vertices[i].normal.y = attrib.normals[i + 1];
        tr_vertices[i].normal.z = attrib.normals[i + 2];

        tr_vertices[i].uv.x = attrib.texcoords[i + 0];
        tr_vertices[i].uv.y = attrib.texcoords[i + 1];
    }

    for (auto& shape : shapes) {
        
    }

    return 0;
}
