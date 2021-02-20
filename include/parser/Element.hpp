#pragma once

#include <vector>
#include <fstream>

namespace E3D::Parser {
    /**
     * @author Amin
     * @brief Element structure to hold VTK id and nodes surrounding the element.
     */
    struct Element {
        /**
         * @brief ctor for ELement structure.
         * @param vtkType VTk ID of the element
         * @param ElemPoints Points surrounding the element
         */
        Element(int vtkType, std::vector<int> ElemPoints) : _vtkType(vtkType), _ElemPoints(ElemPoints) {};
        /**
         * @brief Construct a new empty Element object
         * 
         */
        Element(){};
        /**
         * @brief getter for vtk id of the element
         * @return vtk ID
         */
        inline int getVtkID() const { return this->_vtkType; }

        /**
         * @brief getter for node surrounding the element
         * @return vector of nodes ID surrounding the element
         */
        inline std::vector<int> getElemNodes() const { return this->_ElemPoints; }


        friend std::ostream &operator<<(std::ostream &out, const Element &elem) {

            out << "Element ID: " << elem.getVtkID() << " | Element Nodes : ";
            for (auto &nodeID : elem.getElemNodes()) {
                out << nodeID << " ";
            }
            return out;
        }

        bool operator==(const E3D::Parser::Element& other) const
        {
            return (this->_vtkType == other.getVtkID() && 
                    this->_ElemPoints == other.getElemNodes());
        }

    private:
        int _vtkType;
        std::vector<int> _ElemPoints;
    };


    /**
    * @author Amin
    * @brief A structure (Node) to hold coordinates of the nodes
    */
    struct Node {
    private:
        double _x, _y, _z;
    public:
        /**
         * @brief default ctor for Node object
         * @param x X coordinate (1st column in su2 file)
         * @param y Y coordinate (2nd column in su2 file)
         * @param z Z coordinate (3rd column in su2 file)
         */
        Node(std::vector<double> xyz_coord) : _x(xyz_coord[0]), _y(xyz_coord[1]), _z(xyz_coord[2]) {};
        /**
         * @brief Construct a new empty Node object
         * 
         */
        Node(){};
        /**
         * @brief Getter for node x coordinate
         * @return x coordinate
         */
        inline double getX() const { return _x; }

        /**
         * @brief Getter for node y coordinate
         * @return y coordinate
         */
        inline double getY() const { return _y; }

        /**
         * @brief Getter for node z coordinate
         * @return z coordinate
         */
        inline double getZ() const { return _z; }

        friend std::ostream &operator<<(std::ostream &out, const Node &node) {

            out << "X: " << node.getX() << " | Y: " << node.getY() << " | Z: " << node.getZ();
            return out;
        }

    };
}