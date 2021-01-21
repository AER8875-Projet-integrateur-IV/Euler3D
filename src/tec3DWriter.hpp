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
    void writeFile();
    void beginFile(std::ofstream &);
    void writeNewZone(std::ofstream &);
    void writeCoord(std::ofstream &);
    void writeVar(std::ofstream &);
    void writeFaceConnectivity(ofstream &fileStream);
};
