/*
/Fichier solChoice.hpp
/Date: Janvier 2020
/Projet: Projet Intégrateur 4
*/


#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// #include "../mesh/meshData/meshData.hpp"
#include "./TecWriter.hpp"
// #include "../../tools/stringTools.hpp"
// #include "../solver/solutionStruct.hpp"

using namespace std;

class SolWrite : public StringTools
{
private:
    string _path;
    MeshData *_meshData;
//    Solution *_solution;

public:
    SolChoice(string &path, MeshData *meshData);
    ~SolChoice();
    void writeFile();
};
