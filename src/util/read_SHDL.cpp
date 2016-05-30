/**
 \file 		read_SHDL.cpp
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
 \brief		Reading in an SHDL circuit which is assumed to have fanin and fanout 2 (if it does not satisfy this assumption,
            use SHDL_to_SHDL.cpp) and returning the Gamma2 graph corresponding to this circuit.
 */

//For debugging read_SHDL by itself with its own main
//#define DEBUG_READ_SHDL

#ifdef DEBUG_READ_SHDL
#include "valiant.h"
#include "Node.cpp"
#include "valiant.cpp"
#include "functions.cpp"
#include "gamma1.h"
#include "gamma1.cpp"
#include "gamma2.h"
#include "gamma2.cpp"
#include "print_graph.cpp"
#include "embedding_new.cpp"
#include "top_sort.cpp"
#include "merged.cpp"
#include "print_circuit.cpp"
#include <stdint.h>
#include <ctime>

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string.h>
#include <vector>

#endif // DEBUG_READ_SHDL

#include "read_SHDL.h"

using namespace std;

/**
 * Takes a string in the Format "i i i ..."
 * (integers separated by '|') and returns a vector of all integers
 * @param str the string to tokenize
 * @param tokens the result vector of wire id
 */
void tokenize(const std::string& str, std::vector<string>& tokens) {
	tokens.clear();
	// Skip delimiters at beginning.
	std::string::size_type last_pos = str.find_first_not_of(' ', 0);
	// Find first "non-delimiter".
	std::string::size_type pos = str.find_first_of(' ', last_pos);
	while (std::string::npos != pos || std::string::npos != last_pos) {
		// Add it to the vector.
		tokens.push_back(str.substr(last_pos, pos - last_pos).c_str());
		last_pos = str.find_first_not_of(' ', pos);
		pos = str.find_first_of(' ', last_pos);
	}
}

/**
 * Takes a string, the filename of a file that contains a circuit in SHDL format
 * and returns a Gamma2 graph with fanin and fanout 2, the graph of the circuit
 * @param str_filename the name of the file containing the SHDL circuit
 * @param in_num the number of inputs to be read from the file
 * @param out_num the number of outputs to be read from the file
 * @return Gamma2 graph of the circuit
 */
DAG_Gamma2* read_SHDL(string str_filename, uint32_t& in_num, uint32_t& out_num){
    const char* filename = str_filename.c_str();
    ifstream file;
    file.open(filename);

    uint32_t gate_num = 0;
    out_num = 0;

    // Count the number of lines in the SHDL file --> inputs + gates
    gate_num = count(istreambuf_iterator<char>(file), istreambuf_iterator<char>(), '\n');
    #ifdef DEBUG_READ_SHDL
    cout << gate_num << " number of gates " << endl;
    #endif
    file.clear();
    file.seekg(0, ios::beg);

    // Count the number of output gates in the file, since we will need to create nodes for the outputs as well
    out_num = count(istreambuf_iterator<char>(file), istreambuf_iterator<char>(), 'o') - 1;
    file.clear();
    file.seekg(0, ios::beg);
    #ifdef DEBUG_READ_SHDL
    cout << out_num << " outputs " << endl;
    #endif

    // Create g graph with gate_num + outputs nodes
    DAG_Gamma2* g = new DAG_Gamma2(gate_num + out_num);

    uint32_t inputs = 0;
    uint32_t num;
    uint8_t arity;
    uint16_t c[4];
    uint32_t k;

    DAG_Gamma2::Node* current;
    DAG_Gamma2::Node* input1;
    DAG_Gamma2::Node* input2;

    string line;
    vector<string> tokens;
    uint16_t index;

    if(file.is_open()){
        while(getline(file, line)){
            index = 0;
            if (line != "") {
                tokenize(line, tokens);
                num = atoi(tokens[0].c_str());
                // Count the number of inputs
                if(tokens[1] == "input"){
                    inputs++;
                }
                // Count the number of outputs, set the node to output
                else if(tokens[1] == "output"){
                    index++;
                }
                // Deal with the gates depending on the arity (1 or 2)
                if(tokens[index + 1] == "gate"){
                    current = g->node_array[num];
                    arity = atoi(tokens[index + 3].c_str());
                    for(int i = 0; i < arity * 2; ++i){ //works for arity <= 2
                        c[i] = atoi(tokens[index + i + 6].c_str());
                    }
                    // Second input when arity is 2
                    if(arity == 2){
                        index += 2;
                        input2 = g->node_array[atoi(tokens[index + 12].c_str())];
                        g->add_edge(input2, current);
                    }
                    // First input for both 1 and 2 arity
                    input1 = g->node_array[atoi(tokens[index + 11].c_str())];
                    g->add_edge(input1, g->node_array[num]);
                    // Set function bits for arity 1 gates
                    if(arity == 1){
                        if(current->left_parent == input1){
                            current->set_function_bits(c[0], c[0], c[1], c[1]);
                            #ifdef DEBUG_READ_SHDL
                            cout << "Setting to " << c[0] << c[0] << c[1] << c[1] << " " << num << endl;
                            #endif // DEBUG_READ_SHDL
                        }
                        else if(current->right_parent == input1){
                            current->set_function_bits(c[0], c[1], c[0], c[1]);
                            #ifdef DEBUG_READ_SHDL
                            cout << "Setting to " << c[0] << c[1] << c[0] << c[1] << " " << num << endl;
                            #endif // DEBUG_READ_SHDL
                        }
                    }
                    // Set function bits for arity 2 gates
                    else if(arity == 2){
                        if(current->left_parent == input1 && current->right_parent == input2){
                            current->set_function_bits(c[0], c[1], c[2], c[3]);
                            #ifdef DEBUG_READ_SHDL
                            cout << "Setting to " << c[0] << c[1] << c[2] << c[3] << " " << num << endl;
                            #endif // DEBUG_READ_SHDL
                        }
                        else if(current->right_parent == input1 && current->left_parent == input2){
                            current->set_function_bits(c[0], c[2], c[1], c[3]);
                            #ifdef DEBUG_READ_SHDL
                            cout << "Setting to " << c[0] << c[1] << c[2] << c[3] << " " << num << endl;
                            #endif // DEBUG_READ_SHDL
                        }
                    }
                    else{
                        cerr << "PROBLEM in read_SHDL::read_SHDL: printing control bits " << input1->number - 1 << " " << input2->number - 1 << " " << num << endl;
                    }
                }
                // Add additional edges in the graph for output gates in the specified order
                else if(tokens[0] == "outputs"){
                    k = gate_num;
                    for(uint32_t i = 0; i < out_num; i++){
                        num = atoi(tokens[1 + i].c_str());
                        #ifdef DEBUG_READ_SHDL
                        cout << "output will be " << num << " + " << k << endl;
                        #endif // DEBUG_READ_SHDL
                        g->add_edge(g->node_array[num], g->node_array[k]);
                        g->node_array[k]->output = true;
                        // Function_bits are specified later during embedding for these nodes
                        k++;
                    }
                }
            }
        }
        file.close();
    }
   in_num = inputs;

   return g;
}

/**
 * Takes two booleans as inputs and returns the result of their logical AND operation (0001)
 * @param in1 first input
 * @param in2 second input
 * @return result of logical AND with inputs in1 and in2
 */
bool AND(bool in1, bool in2){
    if(in1 && in2){
        return true;
    }
    return false;
}

/**
 * Takes two booleans as inputs and returns the result of their logical OR operation (0111)
 * @param in1 first input
 * @param in2 second input
 * @return result of logical OR with inputs in1 and in2
 */
bool OR(bool in1, bool in2){
    if(in1 || in2){
        return true;
    }
    return false;
}

/**
 * Takes two booleans as inputs and returns the result of their logical XOR operation (0110)
 * @param in1 first input
 * @param in2 second input
 * @return result of logical XOR with inputs in1 and in2
 */
bool XOR(bool in1, bool in2){
    if((in1 || in2) && !(in1 && in2)){
        return true;
    }
    return false;
}

/**
 * Takes as inputs the arity of the logical operation (1 or 2), 2 inputs (possibly one dummy), the number defining the function
 * and all the wires previously defined, and returns the result
 * @param arity of the logical operation
 * @param input1 first input
 * @param input2 second input
 * @param function_number the number defining the function (from 0 to 15, the integer representation of the function table)
 * @param wires_carry the vector of booleans that carry the already defined wires' values
 * @return result of logical operation
 */
bool calculate(uint8_t arity, uint32_t input1, uint32_t input2, uint32_t function_number, vector<bool> wires_carry){
    bool in1;
    bool in2;
    if(arity == 1){
        in1 = wires_carry[input1];
        switch(function_number){
        case 0:
            return false;
        case 1:
            return !in1;
        case 2:
            return in1;
        case 3:
            return true;
        default:
            cerr << "PROBLEM in read_SHDL::calculate: Not correct function number" << endl;
            return false;
        }
    }
    else if(arity == 2){
        in1 = wires_carry[input1];
        in2 = wires_carry[input2];
        switch(function_number){
        case 0:
            return false;
        case 1:
            return !(OR(in1, in2));
        case 2:
            return XOR(in1, OR(in1, in2));
        case 3:
            return !(in1);
        case 4:
            return XOR(in2, OR(in1, in2));
        case 5:
            return !(in2);
        case 6:
            return XOR(in1, in2);
        case 7:
            return !(AND(in1, in2));
        case 8:
            return AND(in1, in2);
        case 9:
            return !(XOR(in1, in2));
        case 10:
            return in2;
        case 11:
            return !(XOR(in2, OR(in1, in2)));
        case 12:
            return in1;
        case 13:
            return !(XOR(in1, OR(in1, in2)));
        case 14:
            return OR(in1, in2);
        case 15:
            return true;
        default:
            cerr << "PROBLEM in read_SHDL::calculate: Not correct function number" << endl;
            return false;
        }
    }
    cerr << "PROBLEM in read_SHDL::calculate: Arity is greater than 2 or less than 1: cannot be calculated" << endl;
    return false;
}

/**
 * Takes the filename of a file that contains a circuit in SHDL format, the (undefined) inputs and outputs,
 * and evaluates the circuit, storing the random inputs and the outputs
 * @param filename the name of the file containing the SHDL circuit
 * @param input_list the inputs that are to be defined in this function
 * @param output_list the outputs that are to be calculated in this function given the input_list
 */
void eval_SHDL(string filename, vector<bool>& input_list, vector<bool>& output_list){ //the inputs get initialized here and returned
    const char* filen = filename.c_str();
    ifstream file;
    file.open(filen);

    uint32_t inputs = 0;
    uint8_t arity;
    uint32_t input1;
    uint32_t input2;
    uint32_t c[4];

    bool temp;
    vector <bool> wires_carry;
    bool current;
    uint32_t function_number;

    string line;
    vector<string> tokens;
    srand(time(0));
    uint16_t index;
    if(file.is_open()){
        while (getline(file, line)) {
            index = 0;
            c[2] = 0; c[3] = 0;
            if (line != "") {
                tokenize(line, tokens);
                // Count the number of inputs and initialize a random bool list of inputs
                if(tokens[1] == "input"){
                    inputs++;
                    temp = (0 == (rand() % 2));
                    input_list.push_back(temp);
                    wires_carry.push_back(temp);
                }
                // Count the number of outputs
                else if(tokens[1] == "output"){
                    index++;
                }
                if(tokens[index + 1] == "gate"){
                    arity = atoi(tokens[index + 3].c_str());
                    for(int i = 0; i < arity * 2; ++i){ //until arity <= 2
                        c[i] = atoi(tokens[index + i + 6].c_str());
                    }
                    function_number = c[0] * 1 + c[1] * 2 + c[2] * 4 + c[3] * 8;

                    if(arity == 2){
                        index += 2;
                    }
                    input1 = atoi(tokens[index + 11].c_str());
                    if(arity == 2){
                        input2 = atoi(tokens[index + 12].c_str());
                    }
                    current = calculate(arity, input1, input2, function_number, wires_carry);
                    wires_carry.push_back(current);
                }
                if(tokens[0] == "outputs"){
                    for(uint32_t j = 1; j < tokens.size(); ++j){
                        output_list.push_back(wires_carry[atoi(tokens[j].c_str())]);
                    }
                }
            }
        }
        file.close();
    }
}

#ifdef DEBUG_READ_SHDL
int main(int argc, char * []){

  cout << "--------START -------" << endl;
  uint32_t input = 0;
  uint32_t output = 0;

string SHDL[17] = {
        "example_circuits/Bristol/block_ciphers/AES-non-expanded.txt_SHDL.circuit_Mod_SHDL.circuit", //0 done without error
        "example_circuits/Bristol/block_ciphers/AES-expanded.txt_SHDL.circuit_Mod_SHDL.circuit", //1 done, some bits unset
        "example_circuits/Bristol/block_ciphers/DES-non-expanded.txt_SHDL.circuit_Mod_SHDL.circuit", //2 done without problem
        "example_circuits/Bristol/block_ciphers/DES-expanded.txt_SHDL.circuit_Mod_SHDL.circuit", //3 PROBLEM

        "example_circuits/Bristol/hash_functions/md5.txt_SHDL.circuit_Mod_SHDL.circuit", //4 worked with two runs
        "example_circuits/Bristol/hash_functions/sha-1.txt_SHDL.circuit_Mod_SHDL.circuit", //5 PROBLEM
        "example_circuits/Bristol/hash_functions/sha-256.txt_SHDL.circuit_Mod_SHDL.circuit", //6 PROBLEM

        "example_circuits/Bristol/arithmetic_functions/adder_32bit.txt_SHDL.circuit_Mod_SHDL.circuit", //7 works fine
        "example_circuits/Bristol/arithmetic_functions/adder_64bit.txt_SHDL.circuit_Mod_SHDL.circuit", //8 PROBLEM
        "example_circuits/Bristol/arithmetic_functions/comparator_32bit_signed_lt.txt_SHDL.circuit_Mod_SHDL.circuit", //9 works fine
        "example_circuits/Bristol/arithmetic_functions/comparator_32bit_signed_lteq.txt_SHDL.circuit_Mod_SHDL.circuit", //10 work fine
        "example_circuits/Bristol/arithmetic_functions/comparator_32bit_unsigned_lt.txt_SHDL.circuit_Mod_SHDL.circuit", //11 work fine
        "example_circuits/Bristol/arithmetic_functions/comparator_32bit_unsigned_lteq.txt_SHDL.circuit_Mod_SHDL.circuit", //12 works fine
        "example_circuits/Bristol/arithmetic_functions/mult_32x32.txt_SHDL.circuit_Mod_SHDL.circuit", //13 done works fine

        "example_circuits/SHDL/FairplayPF/MobileCode.circuit_Mod_SHDL.circuit", //14
        "example_circuits/SHDL/FairplayPF/CreditChecking.circuit_Mod_SHDL.circuit", //14
        "example_circuits/SHDL/FairplayPF/Branching_18.circuit_Mod_SHDL.circuit"
   };

  uint16_t i = 16;

  DAG_Gamma2* gg = read_SHDL(SHDL[i].c_str(), input, output);

  gg->~DAG_Gamma2();
  return 0;
}
#endif // DEBUG_READ_SHDL
