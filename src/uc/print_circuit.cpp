/**
 \file 		print_circuit.cpp
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
 \brief		Implementation of the function in Valiant_DAG class which are for printing the circuit into a file.
 */


/** ABY X and Y gates note
 *
 * Y gates: Y 0 1 2
 * 0: first input
 * 1: second input
 * 2: output
 * if selection bit is 1 then the 0 is outputted, if it is 0, 1 is outputted
 *
 * X gates: Y 0 1 2 3
 * 0: first input
 * 1: second input
 * 2: first output
 * 3: second output
 * if selection bit is 0 then the 0 stays 0 and 1 stays 1, if it is 1, they are swapped
 *
 * 0 is false
 * 1 is true
 */

//#define CIRCUIT_DEBUG
#define CIRCUIT_INPUT_OPT

#include "../util/read_SHDL.h"
#include "../uc/valiant.h"

/**
 * Topological sorting of universal graph.
 * @param u number of inputs
 */
void Valiant_DAG::topologically_sort(uint32_t u){
    // Inputs have no incoming edges, they have to be put in the queue first
    queue <Node*> next;
    if(u == 0){
        next.push(this->pole_array[0]);
    }
    if(u > 0){
        if(u % 2 == 1){
            // Delete edges to input nodes
            this->pole_array[u - 1]->delete_edge_to_node();
        }
        for(uint32_t i = 0; i < u; ++i){
            next.push(this->pole_array[i]);
        }
    }

    // Topological sorting
    Node* actual;
    Node* right;
    Node* left;
    uint32_t index = 0;
    while(next.size() != 0){
        if(actual && next.front()){
            actual->next_top_order = next.front();
        }
        actual = next.front();
        actual->top_ordered = true;
        actual->top_order = index;
        left = actual->left;
        right = actual->right;

        if(left && ((left->right_parent && left->right_parent->top_ordered) || !(left->right_parent))){
            next.push(left);
        }
        if(right && ((right->left_parent && right->left_parent->top_ordered) || !(right->left_parent))){
            next.push(right);
        }

        index++;
        next.pop();
    }
}

/**
 * Depending on the specified input, we set the this node to be the first or second of the next node
 * @param next node
 * @param first bool is true if first and false if second
 */
void Valiant_DAG::Node::set_first_or_secondof(Node* next, bool first){
    if(first == true){
        if(!(next->firstof) && this->firstof){
            next->is_first = true;
            next->firstof = this->firstof;
        }
        else if(next->firstof && !(next->firstof2) && this->firstof){
            next->is_first2 = true;
            next->firstof2 = this->firstof;
        }
        else if(next->firstof && !(next->firstof2) && this->firstof2){
            next->is_first2 = true;
            next->firstof2 = this->firstof2;
        }
        else{
            cout << "PROBLEM in print_circuit::set_first_or_secondof" << endl;
        }
    }
    else{ //if(first == false){
        if(!(next->secondof) && this->secondof){
            next->is_second = true;
            next->secondof = this->secondof;
        }
        else if(next->secondof && !(next->secondof2) && this->secondof){
            next->is_second2 = true;
            next->secondof2 = this->secondof;
        }
        else if(next->secondof && !(next->secondof2) && this->secondof2){
            next->is_second2 = true;
            next->secondof2 = this->secondof2;
        }
        else{
            cout << "PROBLEM in print_circuit::set_first_or_secondof" << endl;
        }
    }
}


/**
 * Check the X nodes for number of the parent wire
 * @param child Node that has a parent which is an X node
 * @return number of parent wire
 */
uint32_t Valiant_DAG::Node::check_x(Node* child){
    if(this->is_x && child->is_first && child->firstof == this){
        child->is_first = false;
        return this->top_order;
    }
    else if(this->is_x && child->is_first2 && child->firstof2 == this){
        child->is_first2 = false;
        return this->top_order;
    }
    else if(this->is_x && child->is_second && child->secondof == this){
        child->is_second = false;
        return this->top_order + 1;
    }
    else if(this->is_x && child->is_second2 && child->secondof2 == this){
        child->is_second2 = false;
        return this->top_order + 1;
    }
    else if(this->is_x && (child->is_first || child->is_second || child->is_first2 || child->is_second2)){
        cerr << "PROBLEM in node::check_x: something is wrong with" << this->top_order << "  " << child->top_order << endl;
    }
    return this->top_order;
}

/**
 * Print universal circuit and its programming into file
 * @param u input number
 * @param v output number
 * filename name of file with original circuit
 */
void Valiant_DAG::print_circuit(uint32_t u, uint32_t v, string filename){
    // Output files
    string circuit_file = filename + "_circ.txt";
    string program_file = filename + "_prog.txt";
    const char* circuit = circuit_file.c_str();
    const char* program = program_file.c_str();
    ofstream file;
	file.open (circuit);
    ofstream file_xy;
	file_xy.open (program);

	#ifndef CIRCUIT_INPUT_OPT
    bool right_to_left = false;
    bool left_to_right = false;
    #endif

    // Client inputs (the party NOT defining the function)
    Valiant_DAG::Node* next = this->pole_array[0];
    file << "C ";
    uint32_t num_one_parent_nodes = 0;
    uint32_t num_x_nodes = 0;
    uint32_t left_parent_top_order = 0;
    uint32_t right_parent_top_order = 0;

    while(next){
        next->top_order -= num_one_parent_nodes;
        next->top_order += num_x_nodes;
        // Inputs, outputs or gates
        if(next->is_outest_pole){
            // Inputs
            if(next->number <= u){
                file << next->top_order << " " ;
            }
            // Gates
            else if(next->number <= this->pole_number){
                // Universal gates for real gates
                if(next->number <= this->pole_number - v){
                    file << endl << "U " ;
                }
                // For outputs, we only need Y gates instead of universal gates
                else{
                    file << endl << "Y " ;
                }

                left_parent_top_order = next->left_parent->check_x(next);
                file << left_parent_top_order << " " ;
                right_parent_top_order = next->right_parent->check_x(next);
                file << right_parent_top_order << " " ;

                file << next->top_order;

                // A function number for universal gates, control bit for output gates
                file_xy << next->control_num << endl;
            }

        }
        else{
            // Not optimized for one-input nodes
            #ifndef CIRCUIT_INPUT_OPT
            if(next->left_parent == 0 || next->right_parent == 0){ //one parent is null
                if(next->left_parent){
                    next->right_parent = next->left_parent;
                    right_to_left = true;
                }
                else{
                    next->left_parent = next->right_parent;
                    left_to_right = true;
                }
            }
            #endif

            // X gates with two inputs and two outputs
            if(next->left_parent && next->right_parent && next->left && next->right){
                // next->top order corresponds to its left output and left->top_order + 1 to its right output
                #ifdef CIRCUIT_INPUT_OPT
                left_parent_top_order = next->left_parent->check_x(next);
                right_parent_top_order = next->right_parent->check_x(next);
                #endif

                #ifndef CIRCUIT_INPUT_OPT
                if(right_to_left){
                    left_parent_top_order = next->left_parent->check_x(next);
                    right_parent_top_order = left_parent_top_order;
                    right_to_left = false;
                }
                else if(left_to_right){
                    right_parent_top_order = next->right_parent->check_x(next);
                    left_parent_top_order = right_parent_top_order;
                    left_to_right = false;
                }
                else{
                    left_parent_top_order = next->left_parent->check_x(next);
                    right_parent_top_order = next->right_parent->check_x(next);
                }
                #endif

                file << endl << "X " << left_parent_top_order << " " << right_parent_top_order << " " << next->top_order << " " << next->top_order + 1 ;
                num_x_nodes++;
                next->is_x = true;
                if(!next->left->firstof){
                    next->left->firstof = next;
                    next->left->is_first = true;
                }
                else if(!next->left->firstof2){
                    next->left->firstof2 = next;
                    next->left->is_first2 = true;
                }
                if(!next->right->secondof){
                    next->right->secondof = next;
                    next->right->is_second = true;
                }
                else if(!next->right->secondof2){
                    next->right->secondof2 = next;
                    next->right->is_second2 = true;
                }
                file_xy << next->control_num << endl;
            }
            // Y gates with two inputs and one output
            else if((next->left_parent && next->right_parent) && ((next->left && !(next->right)) || (!(next->left) && next->right))){
                #ifndef CIRCUIT_INPUT_OPT
                if(right_to_left){
                    left_parent_top_order = next->left_parent->check_x(next);
                    right_parent_top_order = left_parent_top_order;
                    right_to_left = false;
                }
                else if(left_to_right){
                    right_parent_top_order = next->right_parent->check_x(next);
                    left_parent_top_order = right_parent_top_order;
                    left_to_right = false;
                }
                else{
                    left_parent_top_order = next->left_parent->check_x(next);
                    right_parent_top_order = next->right_parent->check_x(next);
                }
                #endif
                #ifdef CIRCUIT_INPUT_OPT
                left_parent_top_order = next->left_parent->check_x(next);
                right_parent_top_order = next->right_parent->check_x(next);
                #endif
                file << endl << "Y " << left_parent_top_order << " " << right_parent_top_order << " " << next->top_order << " " ;
                // If the first output wire exists, then we know that by default the control bit is set to 0 if we want to output the first input,
                // And it is set to 1, if we want to output the second input
                if(next->left){
                    if(next->control_num == 0){
                        file_xy << 1 << endl;
                    }
                    else if(next->control_num == 1){
                        file_xy << 0 << endl;
                    }
                }
                // If the second output wire exists, then we know that by default the control bit is set to 0 if we want to output the second input,
                // And it is set to 1, if we want to output the first input
                if(next->right){
                    file_xy << next->control_num << endl;
                }
            }
            // One parent gates, i.e., identity gates which we optimize for, replace them by wires in output file
            #ifdef CIRCUIT_INPUT_OPT
            else if(next->left_parent || next->right_parent){ //one parent is null
                num_one_parent_nodes++;
                if(next->left_parent){ //right parent is null
                    if(next->left){
                        if(next->is_second){
                            next->set_first_or_secondof(next->left, false);
                        }
                        if(next->is_first){
                            next->set_first_or_secondof(next->left, true);
                        }
                        if(next->left->left_parent == next){
                            next->left->left_parent = next->left_parent;
                        }
                    }
                    if(next->right){
                        if(next->is_second){
                            next->set_first_or_secondof(next->right, false);
                        }
                        if(next->is_first){
                            next->set_first_or_secondof(next->right, true);
                        }
                        if(next->right->right_parent == next){
                            next->right->right_parent = next->left_parent;
                        }
                    }
                }
                else if(next->right_parent){ //left parent is null
                    if(next->left){
                        if(next->is_second){
                            next->set_first_or_secondof(next->left, false);
                        }
                        if(next->is_first){
                            next->set_first_or_secondof(next->left, true);
                        }
                        if(next->left->left_parent == next){
                            next->left->left_parent = next->right_parent;
                        }
                    }
                    if(next->right){
                        if(next->is_second){
                            next->set_first_or_secondof(next->right, false);
                        }
                        if(next->is_first){
                            next->set_first_or_secondof(next->right, true);
                        }
                        if(next->right->right_parent == next){
                            next->right->right_parent = next->right_parent;
                        }
                    }
                }
            }
            #endif
        }
        next = next->next_top_order;
    }

    // Output the order of the output nodes: it does not depend on the programming, but the topological order sometimes
    // is not following the order of the poles, so we specify which are the outputs in which order
    file << endl << "O " ;
    for(uint32_t i = pole_number - v; i < pole_number; ++i){
        file << this->pole_array[i]->top_order << " ";
    }

    file.close();
    file_xy.close();
}

/**
 * Plain evaluation of universal circuit to check correctness
 * @param filename name of file with UC
 * @param input_list specified input bits
 * @param output_list resulting output bits
 */
void eval_UC(string filename, vector<bool>& input_list, vector<bool>& output_list){
    string circuit_file = filename + "_circ.txt";
    string program_file = filename + "_prog.txt";
    const char* circuit = circuit_file.c_str();
    const char* program = program_file.c_str();

    ifstream file;
	file.open (circuit);
    ifstream file_xy;
	file_xy.open (program);

    vector <bool> wires_carry;

    string line;
    string line_xy;
    vector<string> tokens;
    vector<string> tokens_xy;

    if(file.is_open()){
        while (getline(file, line)) {
            if (line != "") {
                tokenize(line, tokens);
                /* Count the number of inputs and initialize a random bool list of inputs */
                if(tokens[0] == "C"){
                    for(uint32_t j = 0; j < tokens.size()-1; ++j){
                        wires_carry.push_back(input_list[j]);
                    }
                }
                if(tokens[0] == "X"){
                    getline(file_xy, line_xy);
                    tokenize(line_xy, tokens_xy);
                    if(tokens_xy[0] == "0"){
                        wires_carry.push_back(wires_carry[atoi(tokens[1].c_str())]);
                        wires_carry.push_back(wires_carry[atoi(tokens[2].c_str())]);
                    }
                    else if(tokens_xy[0] == "1"){
                        wires_carry.push_back(wires_carry[atoi(tokens[2].c_str())]);
                        wires_carry.push_back(wires_carry[atoi(tokens[1].c_str())]);
                    }
                    #ifdef CIRCUIT_DEBUG
                    cout << "X" << wires_carry[atoi(tokens[1].c_str())] << " " << wires_carry[atoi(tokens[2].c_str())] << endl;
                    #endif // CIRCUIT_DEBUG
                }
                if(tokens[0] == "Y"){
                    getline(file_xy, line_xy);
                    tokenize(line_xy, tokens_xy);
                    if(tokens_xy[0] == "0"){
                        wires_carry.push_back(wires_carry[atoi(tokens[2].c_str())]);
                    }
                    else if(tokens_xy[0] == "1"){
                        wires_carry.push_back(wires_carry[atoi(tokens[1].c_str())]);
                    }
                    #ifdef CIRCUIT_DEBUG
                    cout << "Y" << wires_carry[atoi(tokens[1].c_str())] << " " << wires_carry[atoi(tokens[2].c_str())] << endl;
                    #endif // CIRCUIT_DEBUG
                }
                if(tokens[0] == "U"){
                    getline(file_xy, line_xy);
                    tokenize(line_xy, tokens_xy);
                    wires_carry.push_back(calculate(2, atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), atoi(tokens_xy[0].c_str()), wires_carry));
                    #ifdef CIRCUIT_DEBUG
                    cout << "U" << wires_carry[atoi(tokens[1].c_str())] << " " << wires_carry[atoi(tokens[2].c_str())] << endl;
                    #endif // CIRCUIT_DEBUG
                }
                if(tokens[0] == "O"){
                    for(uint32_t j = 1; j < tokens.size(); ++j){
                        output_list.push_back(wires_carry[atoi(tokens[j].c_str())]);
                    }
                }
            }
        }
    }

	file.close();
	file_xy.close();
	return;
}
