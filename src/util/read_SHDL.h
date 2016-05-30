/**
 \file 		read_SHDL.h
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
 \brief		Collection of function corresponding to the processing of the input SHDL circuit.
 */
#ifndef READ_SHDL_H_INCLUDED
#define READ_SHDL_H_INCLUDED

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

#include "../gamma/gamma2.h"

void tokenize(const std::string& str, std::vector<string>& tokens);
DAG_Gamma2* read_SHDL(string str_filename, uint32_t& in_num, uint32_t& out_num);

bool AND(bool in1, bool in2);
bool OR(bool in1, bool in2);
bool XOR(bool in1, bool in2);
bool calculate(uint8_t arity, uint32_t input1, uint32_t input2, uint32_t function_number, vector<bool> wires_carry);
void eval_SHDL(string filename, vector<bool>& input_list, vector<bool>& output_list);

void eval_UC(string filename, vector<bool>& input_list, vector<bool>& output_list);

#endif // READ_SHDL_H_INCLUDED
