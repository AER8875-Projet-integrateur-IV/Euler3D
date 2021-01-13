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