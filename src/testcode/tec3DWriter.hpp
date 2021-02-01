/*
/Fichier tec3DWriter.hpp
/Date: Janvier 2020
/Projet: Projet Intégrateur 4
*/
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

// #include "../mesh/meshData/meshData.hpp"
// #include "../../tools/stringTools.hpp"
// #include "../solver/solutionStruct.hpp"

using namespace std;

class Tec3DWriter
{
private:
    string _path;
    MeshData *_meshData;
    Solution *_solution;
    FILE *_outputFile;

public:
    Tec3DWriter(string &path, MeshData *meshData);
    ~Tec3DWriter();
    void WriteFile();
    void BeginFile(std::ofstream &);
    void WriteNewZone(std::ofstream &);
    void WriteCoord(std::ofstream &);
    void WriteVar(std::ofstream &);
    void WriteFaceConnectivity(ofstream &fileStream);
};
