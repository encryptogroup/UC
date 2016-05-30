/**
 \file 		gamma1.h
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
 \brief		Gamma1 class for graphs with at most one incoming and at most one outgoing edge for
            each node.
 */

#ifndef GAMMA1_H_INCLUDED
#define GAMMA1_H_INCLUDED

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

class DAG_Gamma1 {
  public:
      struct Node {
        // The number of the node (from 1 to n)
        uint32_t number;
        // Used for the embedding, specifies if the edge coming from the node is already embedded or not
        bool is_embedded;
        // Pointer to the parent node, i.e., to the node which has an incoming edge to this
        Node* parent;
        // Pointer to the child node, i.e., to the node which has an outgoing edge from this
        Node* child;
        // Bool true if the node is an output
        bool is_output;

        Node(uint32_t id);
      };
  public:
      // Array of nodes that belong to the Gamma1 graph
      Node** node_array;
      // Number of nodes in node_array
      uint32_t node_number;

      DAG_Gamma1(uint32_t num);

      void add_edge(Node* a, Node* b);
      void delete_last_node();
      bool check_exist(uint32_t new_index1);
      bool check_right(uint32_t new_index1, uint32_t new_index2);

      ~DAG_Gamma1();
};

#endif // GAMMA1_H_INCLUDED
