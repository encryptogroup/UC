/**
 \file 		gamma1.cpp
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
 \brief		Implementation of Gamma1 class for graphs with at most one incoming and at most one
            outgoing edge for each node.
 */

#include "gamma1.h"

/**
 * Node constructor that takes its number num
 * @param num the number that identifies the node
 */
DAG_Gamma1::Node::Node(uint32_t num)
  : number(num)
  , is_embedded(false)
  , parent(0)
  , child(0)
  , is_output(false) {
}

DAG_Gamma1::Node::Node(const Node& other){
    number = other.number;
    is_embedded = other.is_embedded;
    parent = new Node(*(other.parent));
    child = new Node(*(other.child));
    is_output = other.is_output;
}

/**
 * Gamma1 constructor that creates a gamma1 graph with specified number of Nodes
 * @param num the number specifying the number of Nodes
 */
DAG_Gamma1::DAG_Gamma1(uint32_t num){
	node_number = num;
    // every Node gets its index + 1 in node_array as number
	node_array = new Node*[node_number];
	for(uint32_t i = 0; i < node_number; ++i){
		node_array[i] = new Node(i + 1);
	}
}

DAG_Gamma1::DAG_Gamma1(const DAG_Gamma1& other){
    node_number = other.node_number;
    node_array = new Node*[node_number];
	for(uint32_t i = 0; i < node_number; ++i){
		node_array[i] = new Node(*(other.node_array[i]));
	}
}

/**
 * Creating edge between two nodes a and b if possible, otherwise returns error.
 * @param n1 a Node from where the edge starts
 * @param n2 a Node into which the edge ends
 */
void DAG_Gamma1::add_edge(Node* n1, Node* n2){
	if(n1->child == 0 && n2->parent == 0){
		n1->child = n2;
		n2->parent = n1;
	}
	else{
		cerr << "failed edge insert " << n1->number << ": " << n2->number << endl;
	}
}

/**
 * Deleting the last node in the graph.
 * Used in the coloring when two Gamma1 instances are created and it is an odd case,
 * i.e., to delete the last node of the one that becomes smaller.
 */
void DAG_Gamma1::delete_last_node(){
    delete this->node_array[this->node_number - 1];
    node_number--;
}

/**
 * Check if node with index exists and has a child (for embedding)
 * @param new_index1 index
 * @return bool true if exists, false if not
 */
bool DAG_Gamma1::check_exist(uint32_t new_index1){
    if(new_index1 > this->node_number-1){
        return false;
    }
    if(this && this->node_array[new_index1] && this->node_array[new_index1]->child){
        return true;
    }
    return false;
}

/**
 * Check if node with child (for embedding) is the same as node with new_index2 and if the node is not yet embedded
 * @param new_index1 index1
 * @param new_index2 index2
 * @return bool true if condition holds, false if not
 */
bool DAG_Gamma1::check_right(uint32_t new_index1, uint32_t new_index2){
    if(this->node_array[new_index1]->child == this->node_array[new_index2] && !this->node_array[new_index1]->is_embedded){
        return true;
    }
    return false;
}

/**
 * Gamma1 Destructor
 */
DAG_Gamma1::~DAG_Gamma1(){
    if(this && this->node_number > 0){
        for(uint32_t i = 0; i < this->node_number; i++){
            delete node_array[i];
            node_number = 0;
        }
    }
}
