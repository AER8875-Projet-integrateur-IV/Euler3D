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