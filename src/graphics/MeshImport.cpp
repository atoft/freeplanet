//
// Created by alastair on 02/12/19.
//

#include "MeshImport.h"

#include <algorithm>
#include <fstream>
#include <unordered_map>

#include <src/tools/globals.h>
#include <src/tools/StringHelpers.h>

// A rough parser for .obj files, based on https://en.wikipedia.org/wiki/Wavefront_.obj_file#File_format.
// Only supports the features we actually use.
std::optional<MeshImport::ImportedMeshData> MeshImport::ImportOBJ(const std::string& _path)
{
    std::fstream infile;
    infile.open(_path);

    if (!infile)
    {
        LogError("File \"" + _path + "\" cannot be opened.");
        return std::nullopt;
    }

    LogMessage("Importing \"" + _path + "\".");

    std::vector<glm::vec3> vertexPositions;
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec3> vertexNormals;

    struct FaceMapping
    {
        glm::uvec3 m_Face;
        u32 m_VertexIndex;
    };

    std::vector<FaceMapping> faceToVertexIndex;
    u32 verticesCount = 0;

    ImportedMeshData result;

    u32 lineNumberForLogging = 0;

    while (!infile.eof())
    {
        ++lineNumberForLogging;

        std::string currentLine;
        std::getline(infile, currentLine);

        if (currentLine.empty())
        {
            continue;
        }

        const std::vector<std::string> tokens = StringHelpers::Split(currentLine, " ");

        if (tokens.empty())
        {
            continue;
        }
        else if (StringHelpers::StartsWith(tokens[0], "#"))
        {
            continue;
        }
        else if (tokens[0] == "v")
        {
            if (tokens.size() < 4 || tokens.size() > 5)
            {
                LogError("Line " + std::to_string(lineNumberForLogging) + ": Incorrect number of vertex coordinates. Need 3 or 4, found " + std::to_string(tokens.size() - 1) + ".");
                return std::nullopt;
            }

            if (tokens.size() == 5)
            {
                LogWarning("Line " + std::to_string(lineNumberForLogging) + ": W vertex skipped.");
            }

            glm::vec3 vertex;

            vertex.x = atof(tokens[1].data());
            vertex.y = atof(tokens[2].data());
            vertex.z = atof(tokens[3].data());

            vertexPositions.push_back(vertex);
        }
        else if (tokens[0] == "vt")
        {
            if (tokens.size() < 2 || tokens.size() > 3)
            {
                LogError("Line " + std::to_string(lineNumberForLogging) + ": Incorrect number of texture coordinates. Need 1 or 2 " + std::to_string(tokens.size() - 1) + ".");
                return std::nullopt;
            }

            glm::vec2 texCoord = glm::vec2();

            texCoord.x = atof(tokens[1].data());

            // Optional, 1 dimensional texcoords are allowed.
            if (tokens.size() == 3)
            {
                texCoord.y = atof(tokens[2].data());
            }

            textureCoords.push_back(texCoord);
        }
        else if (tokens[0] == "vn")
        {
            if (tokens.size() != 4)
            {
                LogError("Line " + std::to_string(lineNumberForLogging) + ": Incorrect number of normal indexes. Need 3, found " + std::to_string(tokens.size() - 1) + ".");
                return std::nullopt;
            }

            glm::vec3 normal;

            normal.x = atof(tokens[1].data());
            normal.y = atof(tokens[2].data());
            normal.z = atof(tokens[3].data());

            vertexNormals.push_back(normal);
        }
        else if (tokens[0] == "f")
        {
            if (tokens.size() < 4)
            {
                LogError("Line " + std::to_string(lineNumberForLogging) + ": Not enough face elements. Need 3, found " + std::to_string(tokens.size() - 1) + ".");
                return std::nullopt;
            }

            if (tokens.size() > 4)
            {
                LogWarning("Line " + std::to_string(lineNumberForLogging) + ": Non-triangular faces are not supported. Skipping " + std::to_string(tokens.size() - 4) + " face elements.");
            }

            // Each time we see a unique v/vt/vn combination, we have to add it to the result as a new vertex.
            // Then for list of elements, we need indexes into the unique list of vertices.

            std::vector<glm::uvec3> indexesForFace;

            for (u32 i = 1; i <= 3; ++i)
            {
                const std::vector<std::string> faceIndexes = StringHelpers::Split(tokens[i], "/");
                assert(!faceIndexes.empty());

                glm::uvec3 indexes = glm::ivec3();

                // Vertex
                indexes.x = atoi(faceIndexes[0].data()) - 1;

                // Tex coord
                if (faceIndexes.size() >= 2 && !faceIndexes[1].empty())
                {
                    indexes.y = atoi(faceIndexes[1].data()) - 1;
                }

                // Normal
                if (faceIndexes.size() >= 3 && !faceIndexes[2].empty())
                {
                    indexes.z = atoi(faceIndexes[2].data()) -1;
                }

                // @Performance This runs really badly for large numbers of faces. Need to rewrite.
                auto existingElement = std::find_if(faceToVertexIndex.begin(), faceToVertexIndex.end(), [indexes](const auto& _elem)
                {
                    return _elem.m_Face == indexes;
                });

                if (existingElement != faceToVertexIndex.end())
                {
                    result.m_Elements.push_back(existingElement->m_VertexIndex);
                }
                else
                {
                    faceToVertexIndex.push_back({ indexes, verticesCount });
                    result.m_Elements.push_back(verticesCount);

                    if (indexes.x < vertexPositions.size() && indexes.x >= 0)
                    {
                        result.m_Vertices.push_back(vertexPositions[indexes.x].x);
                        result.m_Vertices.push_back(vertexPositions[indexes.x].y);
                        result.m_Vertices.push_back(vertexPositions[indexes.x].z);
                    }
                    else
                    {
                        LogError(std::to_string(indexes.x) + " is not a valid vertex index.");
                        return std::nullopt;
                    }

                    if (indexes.z < vertexNormals.size() && indexes.z >= 0)
                    {
                        result.m_Vertices.push_back(vertexNormals[indexes.z].x);
                        result.m_Vertices.push_back(vertexNormals[indexes.z].y);
                        result.m_Vertices.push_back(vertexNormals[indexes.z].z);
                    }
                    else if (!vertexNormals.empty())
                    {
                        LogError(std::to_string(indexes.x) + " is not a valid normal index.");
                        return std::nullopt;
                    }
                    else
                    {
                        result.m_Vertices.push_back(1.f);
                        result.m_Vertices.push_back(0.f);
                        result.m_Vertices.push_back(0.f);
                    }

                    if (indexes.y < textureCoords.size() && indexes.y >= 0)
                    {
                        result.m_Vertices.push_back(textureCoords[indexes.y].x);
                        result.m_Vertices.push_back(textureCoords[indexes.y].y);
                    }
                    else if (!textureCoords.empty())
                    {
                        LogError(std::to_string(indexes.x) + " is not a valid texture coordinate index.");
                        return std::nullopt;
                    }
                    else
                    {
                        result.m_Vertices.push_back(0.f);
                        result.m_Vertices.push_back(0.f);
                    }

                    ++verticesCount;
                }

                indexesForFace.push_back(indexes);
            }
        }
        else if (tokens[0] == "vp")
        {
            // Don't support parameter space vertex.
            continue;
        }
        else if (tokens[0] == "l")
        {
            // Don't support line element.
            continue;
        }
        else if (tokens[0] == "o")
        {
            // Don't separate by object.
            continue;
        }
        else if (tokens[0] == "g")
        {
            // Don't separate by group.
            continue;
        }
        else if (tokens[0] == "mtllib")
        {
            // Don't use materials.
            continue;
        }
        else if (tokens[0] == "usemtl")
        {
            // Don't use materials.
            continue;
        }
        else if (tokens[0] == "s")
        {
            // Ignore smoothing groups.
            continue;
        }
        else
        {
            LogError("Failed to parse line " + std::to_string(lineNumberForLogging) + ": (" + currentLine + ").");
            return std::nullopt;
        }
    }
    infile.close();

    return result;
}

// TODO ideally this should be the other way round, the mesh import should
// produce a RawMesh that can get shuffled about to produce the vtex array for
// OpenGL.
RawMesh MeshImport::ConvertToRawMesh(const MeshImport::ImportedMeshData& _importedMesh)
{
    RawMesh result;

    for (u32 elementIdx = 0; elementIdx < _importedMesh.m_Elements.size(); elementIdx += 3)
    {
        result.m_Faces.push_back({
            _importedMesh.m_Elements[elementIdx],
            _importedMesh.m_Elements[elementIdx + 1],
            _importedMesh.m_Elements[elementIdx + 2]});
    }

    for (u32 vertIdx = 0; vertIdx < _importedMesh.m_Vertices.size(); vertIdx += 8)
    {
        result.m_Vertices.emplace_back(
            _importedMesh.m_Vertices[vertIdx],
            _importedMesh.m_Vertices[vertIdx + 1],
            _importedMesh.m_Vertices[vertIdx + 2]);

        result.m_Normals.emplace_back(
            _importedMesh.m_Vertices[vertIdx + 3],
            _importedMesh.m_Vertices[vertIdx + 4],
            _importedMesh.m_Vertices[vertIdx + 5]);

        result.m_TextureCoordinates.emplace_back(
            _importedMesh.m_Vertices[vertIdx + 6],
            _importedMesh.m_Vertices[vertIdx + 7]);
    }

    result.m_Colors.resize(result.m_Vertices.size());
    
    return result;
}

