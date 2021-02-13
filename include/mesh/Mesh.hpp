#pragma once

#include <string>
#include <memory>
#include "parser/Element.hpp"
#include "parser/SU2MeshParser.hpp"
#include "connectivity/Connectivity.hpp"


namespace E3D {

    struct Mesh {
        explicit Mesh(const std::string &fileName);

        // ------------------ Mesh parsing Info ----------------------

        // CONSTANTES
        inline int GetMeshDim() const { return _parser.GetnDim(); }

        inline int GetMeshInteriorElemCount() const { return _parser.GetVolumeElemCount(); }

        inline int GetMeshBoundaryElemCount() const { return _parser.GetBoundaryElemCount(); }

        inline int GetMeshBConditionCount() const { return _parser.GetMarkersCount(); }

        inline int GetMeshNodeCount() const { return _parser.GetPointsCount(); }


        /**
         * @param NodeID ID of the node
         * @return object Node
         * @example : To get x coordinates of first node : double x = GetNodeCoord[0].getX()
         */
        inline const E3D::Parser::Node GetNodeCoord(const int NodeID) const {
            return _parser.GetPoints()[NodeID];
        }

        /**
         * @return Node vector
         */
        inline const std::vector<E3D::Parser::Node> &GetNodeVector() const {
            return _parser.GetPoints();
        }

        /**
         *
         * @param tagID BC position in mesh file (first [0], second [1] ...)
         * @param ElementID Element position
         * @return Element Object
         * @example To get 3rd element in first BC : Element e = GetBoundaryElement(0,2)
         */
        inline const E3D::Parser::Element GetBoundaryElement(int tagID, int ElementID) const {
            return _parser.GetBoundaryElems()[tagID].second[ElementID];
        }

        /**
         * @brief Get the Boundary Condition Vector object
         * 
         * @return const E3D::Parser::BC_Structure 
         */
        inline const E3D::Parser::BC_Structure& GetBoundaryConditionVector() const {
            return _parser.GetBoundaryElems();
        }

        /**
         * @param ElementID Element Position
         * @return Element object
         */
        inline const E3D::Parser::Element GetInteriorElement(int ElementID) const {
            return _parser.GetVolumeElems()[ElementID];
        }

        /**
         * @return Interior Element vector
         */
        inline const std::vector<E3D::Parser::Element> &GetInteriorElementVector() const {
            return _parser.GetVolumeElems();
        }

        /**
         * @param tagID BC position in mesh file (first [0], second [1] ...)
         * @return string holding the name of the BC
         */
        inline const std::string GetTagName(int tagID) const {
            return _parser.GetTags()[tagID].first;
        }

        /**
         * @param tagID BC position in mesh file (first [0], second [1] ...)
         * @return int holding the number of element in this BC
         */
        inline int GetNumberOfElementsInTag(int tagID) const {
            return _parser.GetTags()[tagID].second;
        }

        inline const std::vector<int> &GetInteriorVTKID() const {
            return _parser.GetInteriorElementVtkID();
        }

        // ------------------ Connectivity Info ----------------------

        void solveConnectivity();


    private:
        Parser::SU2MeshParser _parser;
        std::vector<int> _connectivity;
        // variable calculees et assignees par connectivity
        int nFace;
        int nElemTot;
        std::unique_ptr<std::vector<int>>  node2element;
        std::unique_ptr<std::vector<int>>  node2elementStart;
        std::unique_ptr<std::vector<int>>  element2element;
        std::unique_ptr<std::vector<int>>  element2elementStart;
        std::unique_ptr<std::vector<int>>  element2face;
        std::unique_ptr<std::vector<int>>  element2faceStart ;
        std::unique_ptr<std::vector<int>>  face2element;
        std::unique_ptr<std::vector<int>>  face2nodeStart;
        std::unique_ptr<std::vector<int>>  face2node;
        Connectivity connectivityObj;
    };

}
