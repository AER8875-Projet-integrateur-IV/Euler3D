//TODO : include licence

#include <string>
#include <fstream>

#pragma once

namespace E3D {
    /**
     * @author Amin
     * @brief SU2MeshParser class Parses info from SU2 Mesh files.
     */

    class SU2MeshParser {
        //TODO Add private member variable "Elements = vector<Element>" to stock the mesh

        std::string _filename;                 /** @brief SU2 mesh file path and name */
        std::ifstream _ifilestream;            /** @brief Input file stream */
        int _nDim=0;                             /** @brief Geometry's number of dimensions */
        int _nElem=0;                            /** @brief Total number of Elements */
        int _nSurfaceElem=0;                     /** @brief Number of surface elements */
        int _nVolumeElem=0;                      /** @brief Number of volume elements */
        int _nPoints=0;                          /** @brief Number of points */
        int _nMarkers=0;                         /** @brief Number of boundary conditions */

        // Volume VTK codes
        const std::pair<int,int> _VtkTetra   = std::make_pair(10,4);   /** @brief VTK code for a tetrahedron (4 Nodes) */
        const std::pair<int,int> _VtkVoxel   = std::make_pair(11,8);   /** @brief VTK code for a voxel (perfect cube) */
        const std::pair<int,int> _VtkHexa    = std::make_pair(12,8);   /** @brief VTK code for a hexahedron (8 nodes) */
        const std::pair<int,int> _VtkWedge   = std::make_pair(13,6);   /** @brief VTK code for a wedge (6 nodes) */
        const std::pair<int,int> _VtkPyramid = std::make_pair(14,5);   /** @brief VTK code for a pyramid (5 nodes) */

        // Surface VTK codes
        const std::pair<int,int> _VtkTria    = std::make_pair(5,3);    /** @brief VTK code for a triangle (3 nodes) */
        const std::pair<int,int> _VtkPixel   = std::make_pair(8,4);    /** @brief VTK code for a pixel (perfect rectangle) */
        const std::pair<int,int> _VtkQuad    = std::make_pair(10,4);   /** @brief VTK code for a quad (4 nodes) */


        /**
         * @brief Parse and write number of dimensions to a private member variable (_nDim).
         * @param filestream input file stream handle.
         */
        void parseDim(std::ifstream &filestream);

        /**
         * @brief Parse and write number of volume elements to private member variables (_nVolumeElem).
         * @param filestream input file stream handle.
         */
        void parseVolumeElem(std::ifstream &);

        /**
         * @brief Parse and write number of points to private member variables (_nPoints).
         * @param filestream input file stream handle.
         */
        void parsePoints(std::ifstream &);

        /**
         * @brief Parse boundary condition info . Write to _nSurfaceElem and _nMarkers.
         * @param filestream input file stream handle.
         */
        void parseBC(std::ifstream&);



    public:
        /**
         * @brief Default ctor, Executes private member functions to parse data from mesh file.
         */
        SU2MeshParser(const std::string &fileName);


        /**
         *
         * @return Mesh's number of dimensions.
         */
        inline int GetMeshDim() {return this->_nDim;}
    };
} // namespace E3D
