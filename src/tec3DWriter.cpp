/*
/Fichier tec3DWriter.cpp
/Date: Janvier 2020
/Projet: Projet Intégrateur 4
*/

/*Includes*/
#include "./tec3DWriter.hpp"

using std::ofstream;

Tec3DWriter::Tec3DWriter(string &path, MeshData *meshData, Solver *solver)
{
  _path = path;
  _meshData = meshData;
  _solver = solver;
  _solution = _solver->getSolution();
  _outputFile = NULL;
}

/// Validation du fichier
Tec3DWriter::~TecWriter()
{
  if (_outputFile != NULL)
  {
    fclose(_outputFile);
  }
  return;
}

void Tec3DWriter::writeFile()
{
  int Nzone=0;
  for (Nzone=0; Nzone<_meshData->getNZONE(); Nzone=Nzone+1)
  {
  /// Suite de void qui ecrivent le fichier*
  ofstream fileStream(_path);
  beginFile(fileStream);
  writeNewZone(fileStream);
  writeCoord(fileStream);
  writeVar(fileStream);
  writeElementConnectivity(fileStream);
  }
  fileStream.close();
}

/// Début du fichier
void Tec3DWriter::beginFile(ofstream &fileStream)
{
  fileStream << "TITLE = \"Exemple\"\nVARIABLES = \"X\",\"Y\",\"Z\",\"Densité\",\"Vitesse U\",\"Vitesse V\", \"Pression\",\"Énergie\"" << endl;
}

void Tec3DWriter::writeNewZone(ofstream &fileStream)
{
  fileStream << "ZONE "
             << "ZONETYPE=FEQUADRILATERAL "
             << "NODES=" << _meshData->getNPOIN() << ", "
             << "ELEMENTS=" << _meshData->getNELEM() << ", "
             << "FACES=" << _meshData->getNFACE() << ", "
             << "NUMCONNECTEDBOUNDARYFACES=0, TOTALNUMBOUNDARYCONNECTIONS=0\n"
             << "DATAPACKING=BLOCK, VARLOCATION=([3-7]=CELLCENTERED)\n";
}

/// Ecriture des coordonnees des points
void Tec3DWriter::writeCoord(ofstream &fileStream)
{
  /// Indicateur de retour à la ligne
  uint32_t returnline = 0;

  /// Coordonnees X
  for (returnline = 0; returnline < unsigned(_meshData->getNPOIN()); returnline = returnline + 1)
  {
    fileStream << _meshData->getNodes()->at(3 * returnline) << "\n";
  }
  /// Coordonnees Y
  for (returnline = 0; returnline < unsigned(_meshData->getNPOIN()); returnline = returnline + 1)
  {
    fileStream << _meshData->getNodes()->at(3 * returnline + 1) << "\n";
  }
  /// Coordonnees Z
  for (returnline = 0; returnline < unsigned(_meshData->getNPOIN()); returnline = returnline + 1)
  {
    fileStream << _meshData->getNodes()->at(3 * returnline + 2) << "\n";
  }
}
/// Ecriture des variables
void Tec3DWriter::writeVar(ofstream &fileStream)
{
  for (int iElem = 0; iElem < _meshData->getNELEM(); iElem++)
  {
    fileStream << _solution->rho[iElem] << "\n";
  }
  for (int iElem = 0; iElem < _meshData->getNELEM(); iElem++)
  {
    fileStream << _solution->rhoU[iElem] / _solution->rho[iElem] << "\n";
  }
  for (int iElem = 0; iElem < _meshData->getNELEM(); iElem++)
  {
    fileStream << _solution->rhoV[iElem] / _solution->rho[iElem] << "\n";
  }
  for (int iElem = 0; iElem < _meshData->getNELEM(); iElem++)
  {
    fileStream << _solution->rhoE[iElem] / _solution->rho[iElem] << "\n";
  }
  for (int iElem = 0; iElem < _meshData->getNELEM(); iElem++)
  {
    fileStream << _solution->p[iElem] << "\n";
  }
}

/// Ecriture des Connectivités
/// Je ne suis pas sur ici

void Tec3DWriter::writeElementConnectivity(ofstream &fileStream)
{
  for (int iElem = 0; iElem < _meshData->getNELEM(); iElem++)
  {
    for (int jNode = _meshData->getElement2NodesStart()->at(iElem); jNode < _meshData->getElement2NodesStart()->at(iElem + 1); jNode++)
    {
      fileStream << _meshData->getElement2Nodes()->at(jNode) + 1 << "\t";
    }
    fileStream << "\n";
  }
}
