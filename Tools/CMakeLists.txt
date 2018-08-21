## Tools
# ObjToTRB Converter Tools

find_package(Assimp REQUIRED)

add_executable(ObjToTRB obj_to_trb.cpp)
target_link_libraries(ObjToTRB PUBLIC Tronus)

add_executable(TronusAssetConverter trassetconverter.cpp)
target_link_libraries(TronusAssetConverter PUBLIC Tronus)
target_include_directories(TronusAssetConverter PUBLIC ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(TronusAssetConverter PUBLIC ${ASSIMP_LIBRARIES})
