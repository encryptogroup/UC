/**
 \file 		SHDL_to_SHDL.h
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
 \brief		Collection of function corresponding to the processing of the input SHDL circuit
            to have at most two outgoing wires per gate.
 */
#ifndef SHDL_TO_SHDL_H_INCLUDED
#define SHDL_TO_SHDL_H_INCLUDED

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

using namespace std;

struct Node{
    uint32_t number;
    uint32_t remaining_fanout;
    Node* left;
    Node* parent;

    Node(uint32_t, uint32_t, uint32_t&, uint32_t);
    Node* get_node_with_fanout();
    Node* check_node(uint32_t);
    ~Node();
};
//list<Node*> collection;
Node* getnode(list<Node*>, uint32_t, uint32_t);

void tokenize2(const std::string& str, std::vector<string>& tokens);
void count_gate_num(string filename2, uint32_t& gate_num);
void set_fanout(string filename2, vector<uint32_t>& fanout, uint32_t& output_num, uint32_t& inputs);
void set_shift(vector<uint32_t>& shift, vector<uint32_t>& fanout, uint32_t inputs);
void identify_problem_inputs(string filename2, vector<uint32_t>& fanout, uint32_t& inputs, uint32_t gate_num,
                             list<Node*> &collection);
void all_nodes(string filename2, uint32_t gate_num, vector<uint32_t> fanout, vector<uint32_t> shift, uint32_t inputs,
               uint32_t output_num, list<Node*> &collection);

void SHDL_to_SHDL(string filename2);

#endif // SHDL_TO_SHDL_H_INCLUDED
