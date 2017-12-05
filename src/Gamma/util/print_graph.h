/**
 \file 		print_graph.h
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
 \brief		Function for printing the graph for graphviz: this is strictly for debugging purposes and needs
            to be enabled by defining DEBUG_GRAPH. Then, the output file can be inputted in graphviz and the
            circuit can be debugged by means of the graph.
 */

#ifndef PRINT_GRAPH_H_INCLUDED
#define PRINT_GRAPH_H_INCLUDED

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

#include "../../config.h"
#include "../../uc/2way/valiant.h"

using namespace std;

void print_Gamma1_simple(DAG_Gamma1* g);
void print_Gamma2_simple(DAG_Gamma2* g);
void print_gamma2(DAG_Gamma2* , string);
void print_gamma2_full(DAG_Gamma2*);

void print_Graph_full(Valiant_DAG*, bool top_order, bool merged);
void print_Graph(Valiant_DAG*, string, string, bool top_order);

void print_labels_merged(Valiant_DAG* g, string substring1, string substring2, bool outest);
void print_Graph_merged(Valiant_DAG* g, string substring1, string substring2, bool outest);

#endif // PRINT_GRAPH_H_INCLUDED
