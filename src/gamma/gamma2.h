/**
 \file 		gamma2.h
 \author 	agnes.kiss@crisp-da.de
 \copyright	Valiant's Universal Circuit Construction Optimized for Private Function Evaluation
			Copyright (C) 2016 Engineering Cryptographic Protocols Group, TU Darmstadt
			This program is free software: you can redistribute it and/or modify
			it under the terms of the GNU Affero General Public License as published
			by the Free Software Foundation, either version 3 of the License, or
			(at your option) any later version.
			This program is distributed in the hope that it will be useful,
			but WITHOUT ANY WARRANTY; without even the implied warranty of
			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
			GNU Affero General Public License for more details.
			You should have received a copy of the GNU Affero General Public License
			along with this program. If not, see <http://www.gnu.org/licenses/>.
 \brief		Gamma2 class for graphs with at most two incoming and at most two outgoing edge for
            each node.
 */

#ifndef GAMMA2_H_INCLUDED
#define GAMMA2_H_INCLUDED

#include <iostream>
#include <limits.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <stdint.h>
using namespace std;

class DAG_Gamma2 {
  public:
      struct Node {
        // The number of the node (from 1 to n)
        uint32_t number;
        // Pointer to the first child node
        Node* left;
        // Pointer to the second child node
        Node* right;
        // Pointer to the first parent node
        Node* left_parent;
        // Pointer to the second parent node
        Node* right_parent;
        // True if the edge coming into the node are already colored
        bool colored;
        // True if the node corresponds to an output of the circuit
        bool output;
        // Four function bits describing the Boolean function
        uint16_t function_bits[4];
        // True if the bits are already changed in the first embedding step (and do not need to be changed in the second one)
        bool is_bitchanged;

        Node(uint32_t);

        bool node_not_colored();
        Node* other_child_or_parent(Node* other_node, Node* left_node, Node* right_node);
        void set_function_bits(uint16_t c0, uint16_t c1, uint16_t c2, uint16_t c3);
        void prepare_bipartite(DAG_Gamma2* tmp1, DAG_Gamma2* tmp2, uint32_t i, uint32_t& edge_num);
      };
   // Array of nodes that belong to the Gamma1 graph
   Node** node_array;
   // Number of nodes in node_array
   uint32_t node_number;

   // First Gamma2 subgraph for predefining the embedding (redundant, TODO)
   struct DAG_Gamma2* sub_left;
   // Second Gamma2 subgraph for predefining the embedding (redundant, TODO)
   struct DAG_Gamma2* sub_right;
   // First Gamma1 subgraph for predefining the embedding
   struct DAG_Gamma1* gamma1_left;
   // Second Gamma1 subgraph for predefining the embedding
   struct DAG_Gamma1* gamma1_right;

   DAG_Gamma2(uint32_t);

   void add_edge(Node*, Node*);
   Node* smallest_uncolored_with_child();
   void add_edges(DAG_Gamma1*);
   void create_subgraphs(uint32_t);
   void check_one_subgraph(DAG_Gamma1* gamma1);
   void check_correct_subgraphs();

   ~DAG_Gamma2();
};

DAG_Gamma2* random_init(uint32_t);

bool coloring(DAG_Gamma2::Node*, DAG_Gamma2::Node*, bool, DAG_Gamma1*, DAG_Gamma1*);
void color_parent_child(DAG_Gamma2::Node*& parent, DAG_Gamma2::Node*& child, DAG_Gamma2::Node*& old_parent, DAG_Gamma2::Node*& old_child, bool& color, bool& b1, bool& b2,
                       DAG_Gamma1* g1, DAG_Gamma1* g2, uint32_t& edge_num);
pair<DAG_Gamma1*, DAG_Gamma1*> create_from_Gamma2(DAG_Gamma2*, uint32_t);

#endif // GAMMA2_H_INCLUDED
