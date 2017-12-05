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

#include "../../Gamma/util/read_SHDL.h"


/**
 * Plain evaluation of universal circuit to check correctness
 * @param filename name of file with UC
 * @param input_list specified input bits
 * @param output_list resulting output bits
 */
void eval_UC(string filename, vector<bool>& input_list, vector<bool>& output_list){
    string circuit = OUTPUT_DIRECTORY + filename + CIRCUIT_FILE_FORMAT;
    string program = OUTPUT_DIRECTORY + filename + PROGRAMMING_FILE_FORMAT;

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
                    auto temp = calculate(2, atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), atoi(tokens_xy[0].c_str()), wires_carry);
                    wires_carry.push_back(temp);
                    #ifdef CIRCUIT_DEBUG
                    cout << "U" << wires_carry[atoi(tokens[1].c_str())] << " " << wires_carry[atoi(tokens[2].c_str())] << "\t" << temp << endl;
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
}
