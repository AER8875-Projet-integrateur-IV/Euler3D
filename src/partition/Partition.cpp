#include "partition/Partition.hpp"
#include <metis.h>

using namespace E3D::Partition;

Partition::Partition(Mesh *meshGlobal, int &nPart)
{
    _m_meshGlobal = meshGlobal;
    _m_nPart = nPart;

    return;
}

Partition::~Partition()
{
    return;
}

void Partition::SolveElem2Part()
{
    // Initialisation
    long int NELEM = _m_meshGlobal->GetMeshInteriorElemCount();
    long int NPOIN = _m_meshGlobal->GetMeshNodeCount();

    // Connectivité elem2node du mmaillage global
    _m_elem2NodeStart.reserve(NELEM + 1);
    _m_elem2NodeStart.push_back(0);
    std::vector<E3D::Parser::Element> elems = _m_meshGlobal->GetInteriorElementVector();
    for (int iElem = 0; iElem < NELEM; iElem++)
    {
        for (auto node : elems[iElem].getElemNodes())
        {
            _m_elem2Node.push_back(node);
        }
        _m_elem2NodeStart.push_back(_m_elem2Node.size());
    }
    _m_elem2Part.resize(NELEM);
    long node2Part[NPOIN]; // vecteur qui va contenir la partition de chaque noeud
    long int objval;
    long int ncommon(4); // A revoir
    long int npart(_m_nPart);
    int success = METIS_PartMeshDual(&NELEM, &NPOIN, _m_elem2NodeStart.data(),
                                     _m_elem2Node.data(), NULL, NULL,
                                     &ncommon, &npart, NULL, NULL, &objval,
                                     _m_elem2Part.data(), &node2Part[0]);

    /*     for (int i = 0; i < NELEM; i++)
    {
        std::cout << _m_elem2Part[i] << " ";
    }
    std::cout << "\n"; */

    return;
}

void Partition::SolvePart2Elem()
{
    int NELEM = _m_meshGlobal->GetMeshInteriorElemCount();
    // Calcul du nombre d'éléments par partition
    _m_nElemPerPart.assign(_m_nPart, 0);
    for (int iElem = 0; iElem < NELEM; iElem++)
    {
        _m_nElemPerPart[_m_elem2Part[iElem]]++;
    }
    // Construction de la connectivité partition vers éléments
    // // Initialisation
    _m_Part2ElemStart.reserve(_m_nPart + 1);
    _m_Part2Elem.resize(NELEM);
    _m_Part2ElemStart.push_back(0);
    // // Calcul du vecteur d'indexation
    for (int iPart = 0; iPart < _m_nPart; iPart++)
    {
        _m_Part2ElemStart.push_back(_m_nElemPerPart[iPart] + _m_Part2ElemStart.back());
    }
    // // Calcul du vecteur de connectivité
    for (int iElem = 0; iElem < NELEM; iElem++)
    {
        int part = _m_elem2Part[iElem];
        _m_Part2Elem[_m_Part2ElemStart[part]] = iElem;
        _m_Part2ElemStart[part]++;
    }
    // // Réorganisation du vecteur d'indexation
    for (int iPart = _m_nPart + 1; iPart > 0; iPart--)
    {
        _m_Part2ElemStart[iPart] = _m_Part2ElemStart[iPart - 1];
    }
    _m_Part2ElemStart[0] = 0;
    /*     for (int i = 0; i < NELEM; i++)
    {
        std::cout << _m_Part2Elem[i] << " ";
    }
    std::cout << "\n"; */
    return;
}

void Partition::SolveElem2Node()
{
    // Initialisation
    int NELEM = _m_meshGlobal->GetMeshInteriorElemCount();
    _m_localNode2GlobalStart.reserve(_m_nPart + 1);
    _m_localNode2Global.reserve(_m_meshGlobal->GetMeshNodeCount());
    _m_localNode2GlobalStart.push_back(0);
    _m_nNodePerPart.reserve(_m_nPart);
    _m_globalElem2Local.assign(NELEM, 0);
    // Constructuction de la connectivité element vers noeuds de chaque partition
    for (int iPart = 0; iPart < _m_nPart; iPart++)
    {
        // Initialisation du maillage de la partition
        SU2Mesh iMesh;
        iMesh.NDIM = _m_meshGlobal->GetMeshDim();
        iMesh.NELEM = _m_nElemPerPart[iPart];
        iMesh.elem2nodeStart.reserve(iMesh.NELEM + 1);
        iMesh.elem2node.reserve(4 * _m_nElemPerPart[iPart]); //nbre de noeuds per elem>=4 (tetraedres)
        iMesh.elem2nodeStart.push_back(0);
        // Parcours de chaque elements de la partition et renumérotation des noeuds
        _m_localNode2GlobalStart.push_back(_m_localNode2GlobalStart.back());
        int debutE = _m_Part2ElemStart[iPart];
        int finE = _m_Part2ElemStart[iPart + 1];
        for (int iElemLoc = 0; iElemLoc < finE - debutE; iElemLoc++)
        {
            int iElemGlob = _m_Part2Elem[debutE + iElemLoc];
            _m_globalElem2Local[iElemGlob] = iElemLoc;
            // Parcours des noeuds de l'élément
            int debutN = _m_elem2NodeStart[iElemGlob];
            int finN = _m_elem2NodeStart[iElemGlob + 1];
            for (int iNode = debutN; iNode < finN; iNode++)
            {
                // Récupération du noeud global de l'élément
                int nodeGlob = _m_elem2Node[iNode];
                // Identification du noeud local
                int nodeLoc = Global2Local(iPart, nodeGlob);
                // Ajout du noeud local à la connectivité du maillage de la partition
                iMesh.elem2node.push_back(nodeLoc);
            }
            // Mis à jour de l'indexation
            iMesh.elem2nodeStart.push_back(finN - debutN + iMesh.elem2nodeStart.back());
        }
        // Calcul du nombre de noeuds de la partition
        _m_nNodePerPart.push_back(_m_localNode2GlobalStart[iPart + 1] - _m_localNode2GlobalStart[iPart]);
        // Enregistrement du maillage de la partition
        iMesh.NPOIN = _m_nNodePerPart[iPart];
        _m_part.push_back(iMesh);
    }

    return;
}

int Partition::Global2Local(int &iPart, int &nodeGlobal)
{
    int nLocalNode = _m_localNode2GlobalStart[iPart + 1] - _m_localNode2GlobalStart[iPart];

    for (int i = 0; i < nLocalNode; i++)
    {
        if (nodeGlobal == _m_localNode2Global[i + _m_localNode2GlobalStart[iPart]])
        {
            return i;
        }
    }

    _m_localNode2Global.push_back(nodeGlobal);
    _m_localNode2GlobalStart[iPart + 1]++;
    return nLocalNode;
}

/* void Partition::SolveBorder()
{
    int NELEM = _m_meshGlobal->getNELEM();
    for (int iPart = 0; iPart < _m_nPart; iPart++)
    {
        // Initialisation
        _m_part[iPart].interfaceStart.push_back(0);
        // Parcours de chaque elements de la partition
        int debutE = _m_Part2ElemStart[iPart];
        int finE = _m_Part2ElemStart[iPart + 1];
        for (int iElemLoc = 0; iElemLoc < finE - debutE; iElemLoc++)
        {
            int iElemGlob = _m_Part2Elem[debutE + iElemLoc];
            // Parcours des voisins de iElemGlob
            int debutV = _m_meshGlobal->getElem2ElemStart()->at(iElemGlob);
            int finV = _m_meshGlobal->getElem2ElemStart()->at(iElemGlob + 1);
            for (int elemGlobj = debutV; elemGlobj < finV; elemGlobj++)
            {
                // Récupération de l'élément voisin
                int jElemGlob = _m_meshGlobal->getElem2Elem()->at(elemGlobj);
                // Vérifier si l'élément est dans la partition
                int jPart = _m_elem2Part[jElemGlob];
                if (jElemGlob < NELEM)
                {
                    if (jPart != iPart) // Interface entre deux partitions
                    {
                        _m_part[iPart].interface.push_back(iElemGlob);
                        _m_part[iPart].interface.push_back(_m_globalElem2Local[jElemGlob]);
                        _m_part[iPart].interface.push_back(jPart);
                        for (int nodeLoci = _m_part[iPart].elem2nodeStart[iElemLoc]; nodeLoci < _m_part[iPart].elem2nodeStart[iElemLoc + 1]; nodeLoci++)
                        {
                            int iNodeLoc = _m_part[iPart].elem2node[nodeLoci];
                            int iNodeGlob = _m_localNode2Global[iNodeLoc + _m_localNode2GlobalStart[iPart]];
                            for (int nodeGlobj = _m_meshGlobal->getElem2NodeStart()->at(jElemGlob); nodeGlobj < _m_meshGlobal->getElem2NodeStart()->at(jElemGlob + 1); nodeGlobj++)
                            {
                                // Récupération du noeud global de l'élément voisin
                                int jNodeGlob = _m_meshGlobal->getElem2Node()->at(nodeGlobj);
                                if (iNodeGlob == jNodeGlob)
                                {
                                    _m_part[iPart].interface.push_back(iNodeLoc);

                                    break;
                                }
                            }
                        }
                        _m_part[iPart].interfaceStart.push_back(_m_part[iPart].interface.size());
                    }
                }
                else // Condition Limite du maillage global
                {
                    int iFace = _m_meshGlobal->getFace(iElemGlob, jElemGlob); // Méthode à adapter
                    int iMark = _m_meshGlobal->getFace2BC()[2 * iFace + 1];   // à adapter
                }
            }
        }
    }
    return;
} */

void Partition::SolveBorder()
{
    // Initialsation
    int NELEM = _m_meshGlobal->GetMeshInteriorElemCount();
    for (int iPart = 0; iPart < _m_nPart; iPart++)
    {
        _m_part[iPart].Ninterface.assign(_m_nPart, 0);
        _m_part[iPart].Ninterface.assign(_m_nPart, 0);
        _m_part[iPart].interface_elem.resize(_m_nPart);
        //_m_part[iPart].interface_elemStart.resize(_m_nPart, std::vector<int>(1, 0));
    }
    // Calcul des interfaces entre les partitions
    for (int iPart = 0; iPart < _m_nPart; iPart++)
    {
        // Parcours de chaque elements de la partition
        int debutE = _m_Part2ElemStart[iPart];
        int finE = _m_Part2ElemStart[iPart + 1];
        for (int iElemLoc = 0; iElemLoc < finE - debutE; iElemLoc++)
        {
            int iElemGlob = _m_Part2Elem[debutE + iElemLoc];
            // Parcours des voisins de iElemGlob
            int debutV = _m_meshGlobal->GetElement2ElementStart()[iElemGlob];
            int finV = _m_meshGlobal->GetElement2ElementStart()[iElemGlob + 1];
            for (int elemGlobj = debutV; elemGlobj < finV; elemGlobj++)
            {
                // Récupération de l'élément voisin
                int jElemGlob = _m_meshGlobal->GetElement2Element()[elemGlobj];
                if (jElemGlob < NELEM) // Element interne du maillage global
                {
                    // Vérifier si l'élément est dans la partition
                    int jPart = _m_elem2Part[jElemGlob];
                    if (jPart > iPart) // Interface entre deux partitions
                    {
                        _m_part[iPart].Ninterface[jPart] = 1;
                        _m_part[jPart].Ninterface[iPart] = 1;

                        _m_part[iPart].Ninterface_elem[jPart]++;
                        _m_part[jPart].Ninterface_elem[iPart]++;

                        _m_part[iPart].interface_elem[jPart].push_back(iElemLoc);
                        _m_part[iPart].interface_elem[jPart].push_back(_m_globalElem2Local[jElemGlob]);

                        _m_part[jPart].interface_elem[iPart].push_back(_m_globalElem2Local[jElemGlob]);
                        _m_part[jPart].interface_elem[iPart].push_back(iElemLoc);
                    }
                }
                else // Condition Limite du maillage global
                {
                    continue;
                }
            }
        }
    }
    return;
}

void Partition::Write()
{
    std::cout << "Début Partionnement:\n";
    SolveElem2Part();
    SolvePart2Elem();
    SolveElem2Node();
    WriteTecplot("test.dat");
    std::cout << "Fin Partionnement:\n";
}

void Partition::WriteTecplot(std::string fileName)
{
    FILE *fid = fopen(fileName.c_str(), "w");
    if (_m_meshGlobal->GetMeshDim() == 2)
    {
        fprintf(fid, "VARIABLES=\"X\",\"Y\"\n");
        for (int iPart = 0; iPart < _m_nPart; iPart++)
        {
            int nNodes = _m_nNodePerPart[iPart];
            int nElements = _m_nElemPerPart[iPart];

            fprintf(fid, "ZONE T=\"Element\"\nNodes=%d, Elements=%d, ZONETYPE=FEPOLYGON\nDATAPACKING=POINT\n", nNodes, nElements);

            for (int nodeI = 0; nodeI < nNodes; nodeI++)
            {
                E3D::Parser::Node node = _m_meshGlobal->GetNodeCoord(_m_localNode2Global[nodeI + _m_localNode2GlobalStart[iPart]]);
                fprintf(fid, "%.12e %.12e\n", node.getX(), node.getY());
            }

            for (int elementI = 0; elementI < nElements; elementI++)
            {
                for (int iNode = _m_part[iPart].elem2nodeStart[elementI]; iNode < _m_part[iPart].elem2nodeStart[elementI + 1]; iNode++)
                {
                    fprintf(fid, "%d ", _m_part[iPart].elem2node[iNode] + 1);
                }
                fprintf(fid, "\n");
            }
        }
    }
    else if (_m_meshGlobal->GetMeshDim() == 3)
    {
        fprintf(fid, "VARIABLES=\"X\",\"Y\",\"Z\"\n");
        for (int iPart = 0; iPart < _m_nPart; iPart++)
        {
            int nNodes = _m_nNodePerPart[iPart];
            int nElements = _m_nElemPerPart[iPart];

            fprintf(fid, "ZONE T=\"Element\"\nNodes=%d, Elements=%d, ZONETYPE=FEBRICK\nDATAPACKING=POINT\n", nNodes, nElements);

            for (int nodeI = 0; nodeI < nNodes; nodeI++)
            {
                E3D::Parser::Node node = _m_meshGlobal->GetNodeCoord(_m_localNode2Global[nodeI + _m_localNode2GlobalStart[iPart]]);
                fprintf(fid, "%.12e %.12e %.12e\n", node.getX(), node.getY(), node.getZ());
            }

            for (int elementI = 0; elementI < nElements; elementI++)
            {
                for (int iNode = _m_part[iPart].elem2nodeStart[elementI]; iNode < _m_part[iPart].elem2nodeStart[elementI + 1]; iNode++)
                {
                    fprintf(fid, "%d ", _m_part[iPart].elem2node[iNode] + 1);
                }
                fprintf(fid, "\n");
            }
        }
    }

    fclose(fid);
    return;
}