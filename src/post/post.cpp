/**
 * @file Post.cpp
 * @author Vincent Bar, Aziz Ndiaye, Abraham Bherer-Constant
 * @date 2021/02/01
 * @brief Write a solution file to read on Tecplot
 *
 * Wrtie a solution file. Write a mesh on which connectivity has already been solved with
 * the METIS library into multiple smaller mesh and write as well solution for other parameters
 * such as Pressure, Density, Velocity and Energy already solved in Solver.
 */
#include "post/post.hpp"
#include <algorithm>
#include <utility>


using namespace E3D::Post;


Post::Post(std::string Meshpath, std::string Varpath, int nPart)
{
    _fileName = Meshpath;
    _fileVar = Varpath;
    _nPart = nPart;
    return;
}

Post::~Post()
{
    return;
}

void Post::Write()
{
    std::cout << "Début Post:\n";
    WriteTecplot("test.dat");
    std::cout << "Fin Post:\n";
}

void Post::WriteTecplot(std::string fileName)
{
    E3D::Parser::SU2MeshParser maillage(_fileName);
    // Création du fichier
    FILE *fid = fopen(fileName.c_str(), "w");
    // Cas d'un maillage 2D
    if (maillage.GetNDim() == 3)
    {
        // Entête du fichier
        fprintf(fid, "VARIABLES=\"X\",\"Y\",\"Z\"\n");
        // Écriture de chaque partition dans une zone de Tecplot
        // for (int iPart = 0; iPart < _m_nPart; iPart++)
            // {
            // Entête de la zone
            int nNodes = maillage.GetPointsCount();
            int nElements = maillage.GetVolumeElemCount();
            fprintf(fid, "ZONE T=\"Element\"\nNodes=%d, Elements=%d, ZONETYPE=FEBRICK\nDATAPACKING=BLOCK\n", nNodes, nElements);

            // Coordonnées des noeuds de la partition
            for (int nodeI = 0; nodeI < nNodes; nodeI++)
            {
                E3D::Parser::Node node = maillage.GetPoints()[nodeI];
                fprintf(fid, "%.12e\n", node.getX());
            }
            for (int nodeI = 0; nodeI < nNodes; nodeI++)
            {
                E3D::Parser::Node node = maillage.GetPoints()[nodeI];
                fprintf(fid, "%.12e\n", node.getY());
            }
            for (int nodeI = 0; nodeI < nNodes; nodeI++)
            {
                E3D::Parser::Node node = maillage.GetPoints()[nodeI];
                fprintf(fid, "%.12e\n", node.getZ());
            }
            // Variables Pression, Vitesse, Densité, Energie
            // for (int elementI = 0; elementI < nElements; elementI++)
            // {
            //
            // }

            // Connectivité des éléments de la partition
            for (const E3D::Parser::Element &elem : maillage.GetVolumeElems())
            {
                for (const int &iNode: elem.getElemNodes())
                {
                    fprintf(fid, "%d ", iNode+1);
                }
                // for (const int &iNode: elem.getElemNodes())
                // {
                //     fprintf(fid, "%d ", iNode+1);
                // }
                fprintf(fid, "\n");
            }
        //}
    }
    // Cas d'un maillage 3D
    // Elements ayant seulement 8 noeuds seront considérés
    // else if (_m_meshGlobal->GetMeshDim() == 3)
    // {
    //     // Entête du fichier
    //     fprintf(fid, "VARIABLES=\"X\",\"Y\",\"Z\"\n");
    //     for (int iPart = 0; iPart < _m_nPart; iPart++)
    //     {
    //         // Entête de la zone
    //         int nNodes = _m_nNodePerPart[iPart];
    //         int nElements = _m_nElemPerPart[iPart];
    //         fprintf(fid, "ZONE T=\"Element\"\nNodes=%d, Elements=%d, ZONETYPE=FEBRICK\nDATAPACKING=POINT\n", nNodes, nElements);
    //
    //         // Coordonnées des noeuds de la partition
    //         for (int nodeI = 0; nodeI < nNodes; nodeI++)
    //         {
    //             E3D::Parser::Node node = _m_meshGlobal->GetNodeCoord(_m_localNode2Global[nodeI + _m_localNode2GlobalStart[iPart]]);
    //             fprintf(fid, "%.12e %.12e %.12e\n", node.getX(), node.getY(), node.getZ());
    //         }
    //
    //         // Variables Pression, Vitesse, Densité, Energie
    //         // for (int elementI = 0; elementI < nElements; elementI++)
    //         // {
    //         //
    //         // }
    //
    //         // Connectivité des éléments de la partition
    //         for (int elementI = 0; elementI < nElements; elementI++)
    //         {
    //             for (int iNode = _m_part[iPart].elem2nodeStart[elementI]; iNode < _m_part[iPart].elem2nodeStart[elementI + 1]; iNode++)
    //             {
    //                 fprintf(fid, "%d ", _m_part[iPart].elem2node[iNode] + 1);
    //             }
    //             fprintf(fid, "\n");
    //         }
    //     }

    // // Fermeture du fichier
    fclose(fid);
    return;
}
