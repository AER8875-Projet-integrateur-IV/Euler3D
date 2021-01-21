/*
/Fichier solChoice.cpp
/Date: Janvier 2020
/Projet: Projet Intégrateur 4
*/

/*Includes*/
#include "./solChoice.hpp"

SolChoice::SolChoice(string &path, MeshData *meshData)
{
    _path = path;
    _meshData = meshData;
    // _solution = solution;
    cout << _path << endl;
    return;
};

SolChoice::~SolChoice()
{
    return;
};

void solChoice::writeFile()
{
    if (ends_With(_path, ".dat"))
    {
        cout << "Direction Tecplot Writer" << endl;
        Tec3DWriter _tec3DWriter = Tec3DWriter(_path, _meshData);
        _tec3DWriter.writeFile();
    }
    else
    {
        printf("Le format de solution n'est pas reconnu (%s).\n", _path.c_str());
        exit(1);
    };
    return;
};
