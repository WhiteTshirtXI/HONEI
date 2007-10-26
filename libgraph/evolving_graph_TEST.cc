/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Danny van Dyk <danny.dyk@uni-dortmund.de>
 * Copyright (c) 2007 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
 *
 * This file is part of the LA C++ library. LibLa is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * LibLa is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <unittest/unittest.hh>


#include <libgraph/evolving_graph.hh>
#include <libgraph/position.hh>

#include <string>
#include <iostream>

using namespace honei;
using namespace tests;

template <typename DataType_>
// our PositionsTest is a BaseTest. 
class EvolvingGraphTest :
    public BaseTest
 {
    private:
        typedef Node<DataType_> NodeType;
    public:
        // overridden to draw a nice hello when test starts...
        EvolvingGraphTest(const std::string & type) :
            BaseTest("evolving_graph_test<" + type + ">")
        {
        }

    virtual void run() const
         {
            // create evolving graph and some nodes
            EvolvingGraph<DataType_> eg(2,1);
            eg.addNode(new NodeType(1, 1, 1, 1));
            eg.addNode(new NodeType(2, 2, 2, 2));
            eg.addNode(new NodeType(3, 3, 3, 3));

            // create a graph for timeslice 1, add some nodes already defined and define some edges
            Graph<DataType_> * t1 = new Graph<DataType_>(3,2);
            t1->addNode(eg.getNode(1));
            t1->addNode(eg.getNode(2));
            t1->addNode(eg.getNode(3));
            t1->addEdge(1,2, 1);
            t1->addEdge(2,3, 2);
            // add the whole stuff to the ev.graph
            eg.addTimeslice(t1);

            // same procedure for timeslice 2
            eg.addNode(new NodeType(4, 4, 4, 4));
            Graph<DataType_> * t2 = new Graph<DataType_>(3,2);
            t2->addNode(eg.getNode(1));
            t2->addNode(eg.getNode(3));
            t2->addNode(eg.getNode(4));
            t2->addEdge(3,4, 3);
            t2->addEdge(1,4, 4);
            eg.addTimeslice(t2);

            // and also for timeslice 3
            eg.addNode(new NodeType(5, 5, 5, 5));
            eg.addNode(new NodeType(6, 6, 6, 6));
            t2 = new Graph<DataType_>(5,2);
            t2->addNode(eg.getNode(1));
            t2->addNode(eg.getNode(3));
            t2->addNode(eg.getNode(4));
            t2->addNode(eg.getNode(5));
            t2->addNode(eg.getNode(6));
            t2->addEdge(3,4, 3);
            t2->addEdge(1,4, 4);
            t2->addEdge(3,5, 2);
            t2->addEdge(4,5, 4);
            eg.addTimeslice(t2);

            // see what happens if we assemble the whole graph's matrices
            std::cout << "evolving graph with " << eg.nodeCount() << " nodes and " << eg.sliceCount() << " timeslices " << std::            endl;
            std::cout << "Coordinates: " << std::endl;
            std::cout << *eg.getCoordinates() << std::endl;

            std::cout << " NodeWeights: " << std::endl;
            std::cout << *eg.getNodeWeights() << std::endl;
            std::cout << "Edges: " << std::endl;
            std::cout << *eg.getEdges() << std::endl;

            Positions<DataType_, methods::WeightedKamadaKawai> positions(*eg.getCoordinates(), *eg.getNodeWeights(), *eg.getEdges());
            positions.update(0.01, 10);
            std::cout << positions.coordinates() << std::endl;
            TEST_CHECK(true);
    }
};

// instantiate test cases
EvolvingGraphTest<float> evolving_graph_test_float("float");
EvolvingGraphTest<double> evolving_graph_test_doube("double");
