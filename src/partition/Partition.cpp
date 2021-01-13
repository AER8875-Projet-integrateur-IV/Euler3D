#include "../../include/Partition.hpp"
#include <metis.h>

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
    long int NELEM = _m_meshGlobal->getNELEM();
    long int NPOIN = _m_meshGlobal->getNPOIN();

    _m_elem2Part.assign(NELEM, 0);
    std::vector<int> node2Part(NPOIN);
    long int objval;
    long int ncommon(3); // A revoir
    int success = METIS_PartMeshDual(&NELEM, &NPOIN, (long int *)_m_meshGlobal->getElem2NodeStart()->data(),
                                     (long int *)_m_meshGlobal->getElem2NodeStart()->data(), NULL, NULL,
                                     &ncommon, (long int *)&_m_nPart, NULL, NULL, &objval,
                                     (long int *)_m_elem2Part.data(), (long int *)node2Part.data());
    return;
}

void Partition::SolvePart2Elem()
{
    int NELEM = _m_meshGlobal->getNELEM();
    // Calcul du nombre d'éléments par partition
    _m_nElemPerPart.assign(_m_nPart, 0);
    for (int iElem = 0; iElem < NELEM; iElem++)
    {
        _m_nElemPerPart[_m_elem2Part[iElem]]++;
    }
    // Construction de la connectivité partition vers éléments
    // // Initialisation
    _m_Part2ElemStart.reserve(_m_nPart + 1);
    _m_Part2Elem.assign(NELEM, 0);
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
    return;
}

void Partition::SolveElem2Node()
{
    // Initialisation
    _m_localNode2GlobalStart.reserve(_m_nPart);
    _m_localNode2Global.reserve(_m_meshGlobal->getNPOIN());
    _m_localNode2GlobalStart.push_back(0);
    _m_nNodePerPart.reserve(_m_nPart);
    // Constructuction de la connectivité element vers noeuds de chaque partition
    for (int iPart = 0; iPart < _m_nPart; iPart++)
    {
        // Initialisation du maillage de la partition
        SU2Mesh iMesh;
        iMesh.NDIM = _m_meshGlobal->getNDIM();
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
            // Parcours des noeuds de l'élément
            int debutN = _m_meshGlobal->getElem2NodeStart()->at(iElemGlob);
            int finN = _m_meshGlobal->getElem2NodeStart()->at(iElemGlob + 1);
            for (int iNode = debutN; iNode < finN; iNode++)
            {
                // Récupération du noeud global de l'élément
                int nodeGlob = _m_meshGlobal->getElem2Node()->at(iNode);
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