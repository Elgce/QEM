// Copyright (c) 2017 Amir Baserinia

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <fstream>
#include <chrono>
#include "importstl.h"
#include "vectornd.h"
#include "kdtree.h"

template<typename T>
T read(std::ifstream& stream)
{
    char buffer[sizeof(T)];
    stream.read(buffer, sizeof(T));
    return *(T*)buffer;
}

// specialization
template<>
VectorND<> read<VectorND<>>(std::ifstream& stream)
{
    return VectorND<>(
        read<float>(stream),
        read<float>(stream),
        read<float>(stream)
    );
}

void ImportSTL::load(Geometry& model)
{
//  let's time the STL import
    auto t0 = std::chrono::high_resolution_clock::now();

    std::ifstream fileSTL (filename_.c_str(), std::ios::in | std::ios::binary);

    char head[6] = {};
    char solid[6] = "solid";
    char end[] = "endsolid";

    fileSTL.read(head, 5);
    if (strcmp(head, solid) == 0) {
        char buf[255];
        fileSTL>>buf;
        KDTree<3> tree;
        int numOfTris = 0;
        float v1, v2, v3;

        while (fileSTL >> buf) {
            if (strcmp(buf, end) == 0) {
                break;
            }
            fileSTL >> buf;

            fileSTL >> v1;
            fileSTL >> v2;
            fileSTL >> v3;
            VectorND<> normal = VectorND<>(v1,v2 ,v3);

            fileSTL >> buf;
            fileSTL >> buf;

            for (unsigned j = 0; j < 3; j++) {
                fileSTL >> buf;
                unsigned index;
                fileSTL >> v1;
                fileSTL >> v2;
                fileSTL >> v3;
                VectorND<> vec = VectorND<>(v1, v2, v3);
                int ind = tree.findNearest(vec);
                if ((ind < 0) || (VectorND<>::get_dist(vec, tree.getPoint(ind)) > 1.0e-8)) {
                    index = tree.size();
                    tree.insert(vec);
                    model.verts_.push_back(vec);
                }
                else {
                    index = ind;
                }
                model.faces_.push_back(index);
            }
            fileSTL >> buf;    
            fileSTL >> buf;     
            numOfTris++;
        }
        std::cout << numOfTris <<std::endl;
        std::cout << "Points reduced from " << 3 * numOfTris << " to " <<
            tree.size() << " after merging!" << std::endl;

        std::chrono::duration<double> duration =
            std::chrono::high_resolution_clock::now() - t0;
        std::cout << "Finished reading STL in " << (double)duration.count() <<
            " seconds!" << std::endl;

    }
    else {

        std::ifstream fileBinarySTL(filename_.c_str(), std::ios::in | std::ios::binary);

        char header[80];
        fileBinarySTL.read(header, 80);

        char numStr[4];
        fileBinarySTL.read(numStr, 4);
        unsigned numOfTris = *(uint32_t*)numStr;
        std::cout << "Reading " << numOfTris << " triangles ..." << std::endl;

        //  build search tree
        KDTree<3> tree;
        for (unsigned i = 0; i < numOfTris; i++) {
            //      read the normal vector but ignore it.
            auto norm = read<VectorND<>>(fileBinarySTL);

            for (unsigned j = 0; j < 3; j++) {
                unsigned index;
                auto vec = read<VectorND<>>(fileBinarySTL);
                int ind = tree.findNearest(vec);
                if ((ind < 0) || (VectorND<>::get_dist(vec, tree.getPoint(ind)) > 1.0e-8)) {
                    index = tree.size();
                    tree.insert(vec);
                    model.verts_.push_back(vec);
                }
                else {
                    index = ind;
                }
                model.faces_.push_back(index);
            }

            //      skip 2 bytes of dummy data
            char dummy[2];
            fileBinarySTL.read(dummy, 2);
        }

        std::cout << "Points reduced from " << 3 * numOfTris << " to " <<
            tree.size() << " after merging!" << std::endl;

        std::chrono::duration<double> duration =
            std::chrono::high_resolution_clock::now() - t0;
        std::cout << "Finished reading STL in " << (double)duration.count() <<
            " seconds!" << std::endl;
    }

    
        

}

