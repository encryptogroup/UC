/**
 \file 		valiant.h
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
 \brief		Valiant_DAG class for universal graphs with at most two incoming and at most
            two outgoing edge for each node.
 */

#ifndef VALIANT_H_INCLUDED
#define VALIANT_H_INCLUDED

#include <iostream>
#include <limits.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <queue>
#include <list>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdint.h>

#include "../../config.h"
#include "../../Gamma/gamma/gamma2.h"
#include "../../Gamma/gamma/gamma1.h"

using namespace std;

/*-------------------------- Class Valiant_DAG ---------------------------------*/

class Valiant_DAG {
  public:
  struct Node {
    // Number identifying the node
	uint32_t number;
	// Topological order of the node
	uint32_t top_order;
	// Bool true if node is topologically ordered already
	bool top_ordered;
	// Bool true if the node has two incoming and two outgoing edges, otherwise false
	bool is_x;
	// For correctly defining the programming of the output Y nodes */
	bool is_node2;

	// Bool true if the node has a parent that is x and its coming from its first wire
	bool is_first;
    // Bool true if the node has two parents that are x and its coming from its first wire
	bool is_first2;
	// Bool true if the node has a parent that is x and its coming from its second wire
	bool is_second;
    // Bool true if the node has two parents that are x and its coming from its second wire
	bool is_second2;
    // Node the parent that is x and its coming from its first wire
	Node* firstof;
	// Second Node the parent that is x and its coming from its first wire
	Node* firstof2;
	// Node the parent that is x and its coming from its second wire
	Node* secondof;
	// Second Node the parent that is x and its coming from its second wire
	Node* secondof2;

	// Pointer to the next node on the left
	Node* left;
	// Pointer to the next node on the right
	Node* right;
	// Pointer to the previous node on the left, with a special property:
    //    If b->left_parent = a, then a->left = b and vice versa
	Node* left_parent;
	// Pointer to the previous node on the left, with a special property:
    //    If b->right_parent = a, then a->right = b and vice versa
	Node* right_parent;
    // Pointer to the node with the next topological order, i.e.,
    //    If this->top_order == n, then next_top_order->top_order = n + 1
	Node* next_top_order;

#ifdef DEBUG_GRAPH
    /* Bool true if left node is in outer graph compared to this node, false otherwise */
	bool is_left_outer;
    /* Bool true if right node is in outer graph compared to this node, false otherwise */
	bool is_right_outer;
    /* Bool true if left node is in sub_left graph compared to this node, false otherwise */
    bool is_left_in_subleft;
    /* Bool true if left node is in sub_right graph compared to this node, false otherwise */
    bool is_left_in_subright;
    /* Bool true if right node is in sub_left graph compared to this node, false otherwise */
    bool is_right_in_subleft;
    /* Bool true if right node is in sub_right graph compared to this node, false otherwise */
    bool is_right_in_subright;
#endif

    /* Bool true if node is a pole and false otherwise */
	bool is_pole;
	/* Bool true if node is a pole of the outest graph and false otherwise */
	bool is_outest_pole;

    /* Control_bit for X and Y switches, function table for the poles (universal gates) */
    uint16_t control_num;
    /* Bool true if the control_bit is set by embedding and false otherwise */
    bool is_set;

    Node(uint32_t num);

	void set_control_bit(Node*, Node*);
	void delete_edge_to_node();

	// For printing the circuit into file
	void set_first_or_secondof(Node* next, bool first);
	uint32_t check_x(Node* child);
  };
 public:
  // Number of poles
  uint32_t pole_number;
  // Array of poles that are inputs, outputs or gates
  Node** pole_array;
  // Array of nodes that are X or Y gates
  Node** node_array;
  // Array of nodes that are only present in the outest universal graph
  Node** node_array_outest;
  // Left subgraph
  struct Valiant_DAG* sub_left;
  // Right subgraph
  struct Valiant_DAG* sub_right;
  // Second left subgraph only in the outest case
  struct Valiant_DAG* sub_left2;
  // Second right subgraph only in the outest case
  struct Valiant_DAG* sub_right2;
  // Number of nodes
  uint32_t node_number;

  Valiant_DAG(uint32_t a, uint32_t b, uint32_t c, bool, bool); //the merged version

  void create_poles(uint32_t pole_num, bool outest);
  void generic_node_path(uint32_t i, uint32_t j, Node** node_arr, Node** pole_arr, bool side);
  void node4(Node** node_arr, Node** pole_arr, bool side);
  // With special property
  void add_edge(Node*, Node*, bool);
  void set_random_control_bits(bool);
  void init_even_odd(uint32_t n, bool side, bool even);
  void gates(bool side, uint32_t l, uint32_t k);

  //Embedding
  Valiant_DAG::Node* node2(uint32_t index1, uint32_t index2, Valiant_DAG::Node* parent);
  Valiant_DAG::Node* node3(uint32_t index1, uint32_t index2, Valiant_DAG::Node* parent);
  Valiant_DAG::Node* node4(uint32_t index1, uint32_t index2, Valiant_DAG::Node* parent);
  void set_indeces(uint32_t u, uint32_t v, uint32_t index1, uint32_t index2, int64_t& n_index1, int64_t& n_index2);
  bool check_side(Valiant_DAG::Node* node1, Valiant_DAG::Node* node2, bool& returned);
  Valiant_DAG::Node* pathfinder(uint32_t a, uint32_t b, Valiant_DAG::Node*, vector<bool>&);
  bool pathfinder(uint32_t a, uint32_t b, Valiant_DAG::Node*, vector<bool>&, uint32_t c, uint32_t d, bool);
  void embed_side(DAG_Gamma1* gamma1, DAG_Gamma2* gamma2, uint32_t u, uint32_t v, bool side);

  // Printing circuit
  void topologically_sort(unsigned int);
  void print_circuit(uint32_t a, uint32_t b, string);

  ~Valiant_DAG();
};

Valiant_DAG* init_merged(uint32_t gate, uint32_t u, uint32_t v, bool side, bool outest);
Valiant_DAG* init(uint32_t a, bool);
void clearup(Valiant_DAG*);
void embedding_with_supergraph(DAG_Gamma2* g, uint32_t index1, uint32_t index2, vector<bool>&, bool);
Valiant_DAG* embedding_merged(DAG_Gamma2* g, uint32_t u, uint32_t v);

uint32_t neighbouring_index(uint32_t index);

#endif // VALIANT_H_INCLUDED
