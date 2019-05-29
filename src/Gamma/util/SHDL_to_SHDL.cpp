/**
 \file 		SHDL_to_SHDL.cpp
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
			Function to convert SHDL format to SHDL format with fanout 2.
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>
#include <list>
#include <algorithm>
#include <stdint.h>

#include "SHDL_to_SHDL.h"
#include "../../config.h"

/**
 * Create a node and its binary fanout tree
 * @param num number of the node to create
 * @param orig_fanout original fanout of the node
 * @param num2 is needed, it has to change
 * @param k number of nodes + shift[num]
 */
Node::Node(uint32_t num, uint32_t orig_fanout, uint32_t& num2, uint32_t k){
    number = num;
    parent = 0;
    num2 = num + k;
    if(orig_fanout <= 2){
        remaining_fanout = orig_fanout;
        left = 0;
    }
    else{
        num2++;
        left = new Node(num2, orig_fanout - 1, num2, 0);
        left->parent = this;
        remaining_fanout = 1;
    }
}

/**
 * Delete a Node
 */
Node::~Node(){
    delete left;
}

/**
 * Returns node with number = num if add = 0, otherwise searches for num + add numbered node
 * @param coll list of nodes
 * @param num number of the node to return
 * @param add number of added nodes
 * @return nodes under the given conditions
 */
Node* getnode(list<Node*> coll, uint32_t num, uint32_t add){
    Node* tmp;
    for (list<Node*>::iterator it = coll.begin(); it != coll.end(); ++it){
        if((*it)->number == num){
            if(add == 0){
                return (*it);
            }
            else{
                tmp = (*it)->check_node(num + add)->parent;
                return tmp;
            }
        }
    }
    cout << "Returning null" << num << endl;
    return 0;
}

/**
 * Check if left node has the given number of seach int the children nodes
 * @param num number
 * @return node with the given number
 */
Node* Node::check_node(uint32_t num){
    Node* left_node = this->left;
    if(left_node && left_node->number == num){
        return left_node;
    }
    else if(left_node){
        return left_node->check_node(num);
    }
    cout << "Returning null" << endl;
    return 0;
}

/**
 * Returns node with fanout
 * @return node with fanout
 */
Node* Node::get_node_with_fanout(){
    Node* left_node = this->left;
    if(this && this->remaining_fanout > 0){
        return this;
    }
    else if(this && left_node && left_node->remaining_fanout > 0){
        return left_node;
    }
    else if(left_node){
        return left_node->get_node_with_fanout();
    }
    return 0;
}

/**
 * takes a string in the Format "i i i ..."
 * (integers separated by '|') and returns a vector of all integers
 * @param str the string to tokenize
 * @param tokens the result vector of wire id
 */
void tokenize2(const std::string& str, std::vector<string>& tokens){
	tokens.clear();
	// Skip delimiters at beginning
	std::string::size_type lastPos = str.find_first_not_of(' ', 0);
	// Find first "non-delimiter".
	std::string::size_type pos = str.find_first_of(' ', lastPos);
	while (std::string::npos != pos || std::string::npos != lastPos) {
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos).c_str());
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(' ', pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(' ', lastPos);
	}
}

/**
 * Couns the number of gates in a given file
 * @param filename2 file
 * @param gate_num number of gates
 */
void count_gate_num(string filename2, uint32_t& gate_num){
    ifstream file2;
    file2.open(filename2.c_str());
    string linee;
    while(getline(file2 , linee)){
        ++gate_num;
    }
    file2.close();
}

/**
 * sets the fanout for each node
 * @param filename2 SHDL file
 * @param fanout fanout for each node
 * @param output_num number of outputs
 * @param inputs number of inputs
 */
void set_fanout(string filename2, vector<uint32_t>& fanout, uint32_t& output_num, uint32_t& inputs){
    string line;
    vector<string> tokens;
    int32_t index; //can be negative as well
    ifstream file2;
    file2.open(filename2.c_str());
    uint8_t arity;
    uint32_t input1;
    uint32_t input2;
    // Reading through the file, checking if input, output or gate, adding one to the fanout of each input when a gate is considered */
    if(file2.is_open()){
        while (getline(file2, line)) {
            index = 0;
            if (line != "") {
                tokenize2(line, tokens);
                if(tokens[1] == "input"){
                    inputs++;
                }
                else if(tokens[1] == "output"){
                    output_num++;
                    index++;
                    fanout[atoi(tokens[0].c_str())]++;
                    arity = atoi(tokens[index + 3].c_str());
                    if(arity == 2){
                        index += 2;
                    }
                    input1 = atoi(tokens[index + 11].c_str());
                    fanout[input1]++;
                    if(arity == 2){
                        input2 = atoi(tokens[index + 12].c_str());
                        fanout[input2]++;
                    }
                }
                else if(tokens[index + 1] == "gate"){
                    arity = atoi(tokens[index + 3].c_str());
                    if(arity == 2){
                        index += 2;
                    }
                    input1 = atoi(tokens[index + 11].c_str());
                    fanout[input1]++;
                    if(arity == 2){
                        input2 = atoi(tokens[index + 12].c_str());
                        fanout[input2]++;
                    }
                }
            }
        }
        file2.close();
    }
}

/**
 * setts the shifts for each node
 * @param shift list of shifts
 * @param fanout list of fanouts
 * @param inputs number of inputs
 */
void set_shift(vector<uint32_t>& shift, vector<uint32_t>& fanout, uint32_t inputs){
   uint32_t shift_global = 0;
   uint32_t tmp;
   for(uint32_t i = 0; i < fanout.size(); ++i){
        tmp = fanout[i];
        if(tmp > 2){
            shift_global += tmp - 2;
        }
        if( i + 1  < fanout.size() ){
            shift[i + 1] += shift_global;
        }
   }
   for(uint32_t i = 0; i < inputs; ++i){
        shift[i] = 0;
   }
}

/**
 * Identify all inputs with fanout > 2
 * @param filename2 SHDL file
 * @param fanout fanout of the nodes
 * @param inputs number of inputs
 * @param gate_num number of gates
 * @param collection list of all nodes
 */
void identify_problem_inputs(string filename2, vector<uint32_t>& fanout, uint32_t& inputs, uint32_t gate_num,
                             list<Node*> &collection){
    string line;
    vector<string> tokens;

    uint32_t num;

    vector<uint32_t> problem_inputs;
    problem_inputs.reserve(gate_num);

    ofstream tofile;
    string out_file_name = filename2 + SHDL_MOD_CIRCUIT_FILE_FORMAT;
    tofile.open(out_file_name.c_str());

    ifstream file;
    file.open(filename2.c_str());
    while (getline(file, line)) {
        if (line != "") {
            tokenize2(line, tokens);
            if(tokens[1] == "input"){
                num = atoi(tokens[0].c_str());
                tofile << num << " input" << endl;
                if(fanout[num] > 2){
                    problem_inputs.push_back(num);
                }
            }
        }
    }
   file.close();

   uint32_t tmp;
   uint32_t counting = 0;
   uint32_t temp_node;
   for(uint32_t j = 0; j < problem_inputs.size(); ++j){
       tmp = problem_inputs[j];
       temp_node = problem_inputs[j];
       collection.push_back(new Node(temp_node, fanout[temp_node], tmp, inputs + counting - temp_node - 1));
       for(uint32_t i = 0; i < fanout[temp_node] - 2; ++i){
           counting++;
           tofile << inputs + counting  - 1 << " gate arity 1 table [ 0 1 ] inputs [ " << getnode(collection, temp_node, inputs + counting - temp_node - 1)->number << " ]" << endl;
       }
   }
   problem_inputs.clear();
}

/**
 * write out all nodes in the file
 * @param filename2 file of the original file
 * @param gate_num number of gates
 * @param fanout fanout of the nodes
 * @param shift shifted numbers of each node
 * @param inputs number of inputs
 * @param output_num number of outputs
 * @param collection list of nodes
 */
void all_nodes(string filename2, uint32_t gate_num, vector<uint32_t> fanout, vector<uint32_t> shift, uint32_t inputs,
               uint32_t output_num, list<Node*> &collection){
   ifstream file3;
   file3.open(filename2.c_str());
   bool outputs_written = false;
   vector<uint32_t> outputs_coll;
   outputs_coll.reserve(gate_num);

   string line;
   vector<string> tokens;
   int32_t index; //can be negative as well
   uint32_t num;
   uint32_t new_node_num = 0;

   uint8_t arity;
   uint32_t control_bit_num;
   uint32_t input1;
   uint32_t input2;

   Node* n;

    string out_file_name = filename2 + SHDL_MOD_CIRCUIT_FILE_FORMAT;
    ofstream tofile;
    tofile.open(out_file_name.c_str(), fstream::app);

   if(file3.is_open()){
        while (getline(file3, line)) {
            index = 0;
            if (line != "") {
                tokenize2(line, tokens);
                num = atoi(tokens[0].c_str());
                if(fanout[num] > 2){
                    new_node_num = num + shift[num]; // to be changed in the node method, do not use it again!
                    collection.push_back(new Node(num + shift[num], fanout[num], new_node_num, 0));
                }
                if(tokens[1] == "output"){
                    index++;
                    tofile << num + shift[num] << " output gate ";
                    if(fanout[num] > 2){
                        n = getnode(collection, num + shift[num], 0);
                        n->remaining_fanout--;
                    }
                    outputs_coll.push_back(num + shift[num]);
                }
                else if(tokens[1] == "gate"){
                    tofile << num + shift[num] << " gate ";
                }
                if(tokens[index + 1] == "gate"){
                    num = atoi(tokens[0].c_str());
                    tofile << "arity " << tokens[index + 3] << " table [ ";
                    arity = atoi(tokens[index + 3].c_str());
                    control_bit_num = arity * 2; //until arity <= 2
                    for(uint32_t i = 0; i < control_bit_num; ++i){
                        tofile << tokens[index + 6 + i] << " ";
                    }
                    tofile << "] inputs [ ";
                    input1 = atoi(tokens[index + 9 + control_bit_num].c_str());
                    if(fanout[input1] > 2){
                        if(input1 < inputs){
                            n = getnode(collection, input1, 0);
                        }
                        else{
                            n = getnode(collection, input1 + shift[input1], 0);
                        }
                        tofile << n->get_node_with_fanout()->number << " ";
                        n->get_node_with_fanout()->remaining_fanout--;
                    }
                    else{
                        tofile << input1 + shift[input1] << " ";
                    }

                    if(arity == 2){
                        input2 = atoi(tokens[index + 10 + control_bit_num].c_str());
                        if(fanout[input2] > 2){
                            if(input2 < inputs){
                                n = getnode(collection, input2, 0);
                            }
                            else{
                                n = getnode(collection, input2 + shift[input2], 0);
                            }
                            tofile << n->get_node_with_fanout()->number << " ";
                            n->get_node_with_fanout()->remaining_fanout--;
                        }
                        else{
                            tofile << input2 + shift[input2] << " ";
                        }
                    }
                    tofile << "]" << endl;
                    if(fanout[num] > 2){
                        for(uint32_t i = 0; i < fanout[num] - 2; ++i){
                            tofile << num + shift[num] + i + 1 << " gate arity 1 table [ 0 1 ] inputs [ " << getnode(collection, num + shift[num], i + 1)->number << "]" << endl;
                        }
                    }
                }
                else if(tokens[0] == "outputs"){
                    tofile << "outputs ";
                    for(uint32_t i = 0; i < output_num; ++i){
                        num = atoi(tokens[1 + i].c_str());
                        tofile << num + shift[num] << " " ;
                    }
                    outputs_written = true;
                }
            }
        }
        file3.close();
    }

    if(!outputs_written){
        tofile << "outputs ";
        for(uint16_t i = 0; i < outputs_coll.size(); ++i){
            tofile << outputs_coll[i] << " ";
        }
    }
    outputs_coll.clear();
    for(list<Node*>::iterator it = collection.begin(); it != collection.end(); ++it){
        delete (*it);
    }
    collection.clear();

}

/**
 * Converts SHDL to a modified SHDL file where each node has at most 2 outputs
 * @param filename2 filename of the original SHDL file
 */
void SHDL_to_SHDL(string filename2){
    list<Node*> collection;
    string out_file_name = filename2 + SHDL_MOD_CIRCUIT_FILE_FORMAT;
    const char* out_file = out_file_name.c_str();

    uint32_t gate_num = 0;
    uint32_t inputs = 0;

    // Count the lines of the file, that will be the number of gates excluding the output gate
    count_gate_num(filename2, gate_num);

    // Fanout of input or gate of each line, first set to 0
    vector<uint32_t> fanout(gate_num, 0);
    uint32_t output_num = 0;
    set_fanout(filename2, fanout, output_num, inputs);
    cout << "1. Fanout setting is done" << endl;

   /* the shift of each input or gate, first set to 0 */
   vector<uint32_t> shift(gate_num, 0);
   /* Setting the shift of each node --> separate it for input and other? */
   set_shift(shift, fanout, inputs);
   cout << "2. Shift setting is done" << endl;

   /* Ouput SHDL file */
   cout << "3. Output SHDL file created: " << out_file << endl;

   /* Part dealing with input nodes */
   identify_problem_inputs(filename2, fanout, inputs, gate_num, collection);
   cout << "4. Problematic inputs identified" << endl;

   /* Part dealing with the rest of the nodes */
   cout << "5. All other nodes done" << endl;
   all_nodes(filename2, gate_num, fanout, shift, inputs, output_num, collection);

   fanout.clear();
   shift.clear();
}
