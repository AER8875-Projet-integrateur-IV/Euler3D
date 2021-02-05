#include "partition/Partition.hpp"
#include "partition/SU2Writer.hpp"
#include <metis.h>
#include <algorithm>    
#include <utility>
#include <iomanip>
#include <regex>

using namespace E3D::Partition;

void SU2Mesh::AddMarkerElement(const std::string& tag, int VTKid, int* elem2Node, int nNode){
    // Create element vector
    std::vector<int> nodes (elem2Node, elem2Node+nNode);
    E3D::Parser::Element elem(VTKid, nodes);
    
    if(Markers.count(tag) == 0){
        // No match was found in existing markers
        // Create new marker
        std::vector<E3D::Parser::Element> newElemVector;
        newElemVector.push_back(elem);
        Markers[tag] = newElemVector;
    } else{
        // The border condition exists
        // Add the new element to this condition        
        Markers[tag].push_back(elem);
    }
}

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
    long int npart(_m_nPart); // METIS utilise le type long
    _m_elem2Part.resize(NELEM);

    // Connectivité elem2node du mmaillage global
    _m_elem2NodeStart.reserve(NELEM + 1);
    _m_elem2Node.reserve(4 * NELEM);
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

    // Paramètres utiles pour appeler METIS
	std::vector<long> node2Part(NPOIN);// vecteur qui va contenir la partition de chaque noeud
	long int objval;
	long int ncommon;
	if (_m_meshGlobal->GetMeshDim() == 2)
    {
        ncommon = 2;
    }
    else if (_m_meshGlobal->GetMeshDim() == 3)
    {
        ncommon = 3;
    }

    // Appel de METIS
	int success = METIS_PartMeshDual(&NELEM, &NPOIN, _m_elem2NodeStart.data(),
	                                 _m_elem2Node.data(), nullptr, nullptr,
	                                 &ncommon, &npart, nullptr, nullptr, &objval,
	                                 _m_elem2Part.data(), node2Part.data());

	std::cout << std::setw(30)
	          << "METIS partition success : "
	          << std::setw(6)
	          << success
	          << "\n";
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
        iMesh.ID = iPart;
        _m_part.push_back(iMesh);
    }

    return;
}

int Partition::Global2Local(int &iPart, int &nodeGlobal)
{
    int nLocalNode = _m_localNode2GlobalStart[iPart + 1] - _m_localNode2GlobalStart[iPart];
    // Parcourir les noeuds locaux déjà affectés pour retrouver le noeud global nodeGlobal
    for (int i = 0; i < nLocalNode; i++)
    {
        if (nodeGlobal == _m_localNode2Global[i + _m_localNode2GlobalStart[iPart]])
        {
            return i;
        }
    }
    // Créer un nouveau noeuds local pour le noeud global nodeGlobal
    _m_localNode2Global.push_back(nodeGlobal);
    _m_localNode2GlobalStart[iPart + 1]++;
    return nLocalNode;
}

void Partition::SolveBorder()
{
    // Initialsation
    int NELEM = _m_meshGlobal->GetMeshInteriorElemCount();
    for (int iPart = 0; iPart < _m_nPart; iPart++)
    {
        _m_part[iPart].Ninterface.assign(_m_nPart, 0);
        _m_part[iPart].Ninterface_elem.assign(_m_nPart, 0);
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

void Partition::PartitionPhysicalBorder(){
    std::vector<int> localMarkerNodes;
    for(auto const &Marker : _m_meshGlobal->GetBoundaryConditionVector()){
        const std::string &tag = Marker.first;
        const std::vector<E3D::Parser::Element> &elemVector = Marker.second;
        
        
        // Find a match for each border element of the global mesh
        for(auto const &elem : elemVector){
            const std::vector<int> &markerNodes = elem.getElemNodes();
            int failedMatch = 0;
            // Look for a match in each partition
            for (int partI = 0; partI < _m_nPart; partI++)
            {
                this->FindMarkerInPartition(partI, markerNodes, localMarkerNodes);
                if(localMarkerNodes.empty()){
                    // No match has been found in this partition
                    failedMatch++;
                } else{
                    // A match has been found
                    int VTKid = elem.getVtkID();
                    E3D::Partition::SU2Mesh &part = _m_part[partI];
                    part.AddMarkerElement(tag, VTKid, localMarkerNodes.data(), localMarkerNodes.size());
                }
            }
            if(failedMatch == _m_nPart){
                throw std::runtime_error("Marker could not be found in a subpartition");
            }
        }
    }
}

void Partition::FindMarkerInPartition(int partID,
    const std::vector<int> &markerNodes, std::vector<int>& localMarkerNodes){
    // This function could be optimised if needed

    localMarkerNodes = std::vector<int>();
    localMarkerNodes.reserve(markerNodes.size());

    E3D::Partition::SU2Mesh &part =_m_part[partID];
    // Go over every element in the partition, if all nodes in
    // markerNodes are found in a element, elem is part of the 
    // current partition
    for (int elemI = 0; elemI < part.NELEM; elemI++)
    {
        int start = part.elem2nodeStart[elemI];
        int end = part.elem2nodeStart[elemI+1];
        int size = end-start;
        std::vector<int> globalPartNode;
        globalPartNode.reserve(size);
        std::vector<int> localPartNode;
        localPartNode.reserve(size);
        
        // Build local node vector
        for (int i = start; i < end; i++)
        {
            localPartNode.push_back(part.elem2node[i]);
        }
        // Build global node vector
        for (int localID : localPartNode)
        {     
            globalPartNode.push_back(this->Local2GlobalNode(localID, partID));
        }
        
        std::vector<int> matchIndex;
        FindContainedElements(markerNodes, globalPartNode, matchIndex);
        
        if (matchIndex.size() == markerNodes.size())
        {
            for(int index : matchIndex){
                localMarkerNodes.push_back(localPartNode[index]);
            }

            // a match has been found
            return;
        }
    }
    // No match has been found
}

void Partition::FindContainedElements(const std::vector<int>& subSet, const std::vector<int>& globalSet, std::vector<int>& indexVector){
    const int *findStart = globalSet.data();
    const int *findEnd = findStart+globalSet.size();

    // This vector contains the index (taken from globalSet) of matching values
    indexVector = std::vector<int>();
    indexVector.reserve(subSet.size());

    for(const int &value : subSet){
        const int* p = std::find(findStart, findEnd, value);
        if (p == findEnd){
            // value not found in globalSet
            break;
        } else{
            // value is part of globalSet
            // save the index
            indexVector.push_back(p-findStart);
        }        
    }
    // All elements were found    
}

int Partition::Local2GlobalNode(int localNodeID, int partID){
    int start = _m_localNode2GlobalStart[partID];
    int globalID = _m_localNode2Global[start+localNodeID];
    return globalID; 
}

void Partition::Write(const std::string &SU2OuputPath) {
	std::cout << std::string(24, '#') << "  Partitionning  " << std::string(24, '#') << "\n\n"
	          << std::endl;
	std::cout << std::setw(30)
	          << "Number of Partitions : "
	          << std::setw(6)
	          << _m_nPart
	          << "\n";

	SolveElem2Part();
    SolvePart2Elem();
    SolveElem2Node();
    PartitionPhysicalBorder();
    WriteTecplot("test.dat");

    // Define name of output
    std::cout << std::setw(30)
              << "OutputPath : "
              << std::setw(6)
              << SU2OuputPath
              << "\n";
    

    for(int i = 0;i<_m_nPart;i++){
        SU2Mesh &part = _m_part[i];
        std::string path = std::regex_replace(SU2OuputPath, std::regex("#"), std::to_string(i));
        this->WriteSU2(part, path);
    }
    std::cout << std::string(58, '#') << std::endl;
}

void Partition::WriteSU2(E3D::Partition::SU2Mesh const &partition, const std::string &path) {
	// vector Node
	std::vector<E3D::Parser::Node> const &globNodeVector = _m_meshGlobal->GetNodeVector();

	std::vector<E3D::Parser::Node> nodeVector;
    nodeVector.reserve(partition.NPOIN);
    
    for (int i = 0; i < partition.NPOIN; i++)
    {
        int nodeGlobI = Local2GlobalNode(i, partition.ID);
        nodeVector.push_back(globNodeVector[nodeGlobI]);
    }

    // Element to Node
    std::vector<E3D::Parser::Element> elemVector;
    elemVector.reserve(partition.NELEM);

    for(int i = 0; i<partition.NELEM; i++){
        std::vector<int> vectorNodeID;
        int nNode = partition.elem2nodeStart[i+1]-partition.elem2nodeStart[i];
        vectorNodeID.reserve(nNode);

        for(int nodeI = partition.elem2nodeStart[i]; nodeI<partition.elem2nodeStart[i+1]; nodeI++){
            vectorNodeID.push_back(partition.elem2node[nodeI]);
        }

        // TODO change VTK ID from 0 to good value
		elemVector.emplace_back(E3D::Parser::Element(-1, vectorNodeID));
	}

	// Physical boundary conditions
	E3D::Parser::BC_Structure bc;
    for(const auto &marker : partition.Markers){
        bc.push_back(marker);
    }

    SU2Writer writer(path);
    writer.Write(elemVector, partition.NDIM, nodeVector, bc);
}

void Partition::WriteTecplot(const std::string &fileName) {
	// Création du fichier
	FILE *fid = fopen(fileName.c_str(), "w");
	// Cas d'un maillage 2D
    if (_m_meshGlobal->GetMeshDim() == 2)
    {
        // Entête du fichier
        fprintf(fid, "VARIABLES=\"X\",\"Y\"\n");
        // Écriture de chaque partition dans une zone de Tecplot
        for (int iPart = 0; iPart < _m_nPart; iPart++)
        {
            // Entête de la zone
            int nNodes = _m_nNodePerPart[iPart];
            int nElements = _m_nElemPerPart[iPart];
            fprintf(fid, "ZONE T=\"Element\"\nNodes=%d, Elements=%d, ZONETYPE=FEPOLYGON\nDATAPACKING=POINT\n", nNodes, nElements);

            // Coordonnées des noeuds de la partition
            for (int nodeI = 0; nodeI < nNodes; nodeI++)
            {
                E3D::Parser::Node node = _m_meshGlobal->GetNodeCoord(_m_localNode2Global[nodeI + _m_localNode2GlobalStart[iPart]]);
                fprintf(fid, "%.12e %.12e\n", node.getX(), node.getY());
            }

            // Connectivité des éléments de la partition
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
    // Cas d'un maillage 3D
    // Elements ayant seulement 8 noeuds seront considérés
    else if (_m_meshGlobal->GetMeshDim() == 3)
    {
        // Entête du fichier
        fprintf(fid, "VARIABLES=\"X\",\"Y\",\"Z\"\n");
        for (int iPart = 0; iPart < _m_nPart; iPart++)
        {
            // Entête de la zone
            int nNodes = _m_nNodePerPart[iPart];
            int nElements = _m_nElemPerPart[iPart];
            fprintf(fid, "ZONE T=\"Element\"\nNodes=%d, Elements=%d, ZONETYPE=FEBRICK\nDATAPACKING=POINT\n", nNodes, nElements);

            // Coordonnées des noeuds de la partition
            for (int nodeI = 0; nodeI < nNodes; nodeI++)
            {
                E3D::Parser::Node node = _m_meshGlobal->GetNodeCoord(_m_localNode2Global[nodeI + _m_localNode2GlobalStart[iPart]]);
                fprintf(fid, "%.12e %.12e %.12e\n", node.getX(), node.getY(), node.getZ());
            }

            // Connectivité des éléments de la partition
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
    // Fermeture du fichier
    fclose(fid);
    return;
}