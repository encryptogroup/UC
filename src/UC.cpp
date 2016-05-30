/**
 \file 		main.cpp
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
 */

//#define DEBUG_EMBEDDING
#define TIME
#define STATE
//#define DEBUG_GRAPH
#define DEBUG_CORRECTNESS

#include <stdint.h>
#include <ctime>

#include "gamma/gamma1.cpp"
#include "gamma/gamma2.cpp"

#include "util/print_graph.cpp"
#include "util/read_SHDL.cpp"
#include "util/SHDL_to_SHDL.cpp"

#include "uc/valiant.cpp"
#include "uc/node.cpp"
#include "uc/embedding.cpp"
#include "uc/print_circuit.cpp"

inline clock_t getMilliSecs() {
    return clock() / (CLOCKS_PER_SEC / 1000);
}

int main(int argc, char * argv[]){

#ifdef DEBUG_EMBEDDING
  DAG_Gamma2* g2 = random_init(17);
  g2->create_subgraphs(0);
  print_gamma2_full(g2);

  Valiant_DAG* g = embedding_merged("", g2, 0, 0);
  print_Graph_full(g);

  uint32_t node_num = 14532;
  DAG_Gamma2* g5 = random_init(node_num);
  Valiant_DAG* g = embedding_merged(g5, 0, 0);
#endif

  uint32_t input = 0;
  uint32_t output = 0;

string SHDL[5] = {
        "../circuits/adder_32bit.txt_SHDL.circuit_Mod_SHDL.circuit",
        "../circuits/comparator_32bit_signed_lt.txt_SHDL.circuit_Mod_SHDL.circuit",
        "../circuits/mult_32x32.txt_SHDL.circuit_Mod_SHDL.circuit",

        "../circuits/MobileCode.circuit_Mod_SHDL.circuit",
        "../circuits/CreditChecking.circuit_Mod_SHDL.circuit",
   };

  uint16_t i = 0;

  cout << endl << "START SHDL" << endl;
  SHDL_to_SHDL(SHDL[i]);
  cout << "END SHDL" << endl;

  cout << endl << "START UC" << endl;
  clock_t start = getMilliSecs();
  const char* tmp = SHDL[i].c_str();
  DAG_Gamma2* gg = read_SHDL(tmp, input, output);
  #ifdef STATE
  cout << "1. Creating the Gamma2 graph with " << input << " inputs and " << output << " outputs is done" << endl;
  #endif // STATE
  clock_t time1 = getMilliSecs();

  Valiant_DAG* G = embedding_merged(gg, input, output);
  clock_t time2 = getMilliSecs();
  cout << "Embedding time: " << (time2 - time1) << "ms" << endl;
  #ifdef DEBUG_GRAPH
  print_Graph_full_merged_top_order(G);
  #endif // DEBUG_GRAPH

  G->print_circuit(input, output, tmp);
  clock_t time3 = getMilliSecs();
  cout << "I/O time: " << (time3 - time2) + (time1 - start) << "ms" << endl;
  #ifdef DEBUG_GRAPH
  print_Gamma2_simple(gg);
  #endif
  cout << "END UC" << endl;

  #ifdef DEBUG_CORRECTNESS
  cout << endl << "Evaluation result of original SHDL file" << endl;
  vector<bool> input_list;
  vector<bool> output_list;
  eval_SHDL(SHDL[i], input_list, output_list);

  for(uint32_t j = 0; j < output_list.size(); ++j){
     cout << output_list[j] << " ";
  }
  cout << endl << endl;

  cout << endl << "Evaluation result of UC files" << endl;
  vector<bool> output_list2;
  // Pre-defined input bits, output bits to-be-compared
  eval_UC(SHDL[i], input_list, output_list2);

  for(uint32_t j = 0; j < output_list2.size(); ++j){
     cout << output_list2[j] << " ";
  }
  cout << endl;
  #endif // DEBUG_CORRECTNESS

  //clearup(G);
  //gg->~DAG_Gamma2();
  return 0;
}

