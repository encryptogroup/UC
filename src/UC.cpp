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

#include <cstdint>
#include <string>
#include <iostream>
#include <cstring>

#include "config.h"
#include "Gamma/util/SHDL_to_SHDL.h"
#include "Gamma/util/read_SHDL.h"
#include "Gamma/gamma/gamma2.h"
#include "uc/2way/valiant.h"
#include "uc/4way/ValiantUC.h"
#include "debug.h"
#include "Gamma/util/print_graph.h"
#include "Gamma/util/hybrid_size.h"

using namespace std;

/**
 * Get miliseconds
 * @return milliseconds
 */
inline clock_t getMilliSecs() {
  return clock() / (CLOCKS_PER_SEC / 1000);
}

int main(int argc, char *argv[]) {
  if(argc == 1) {
    cout << "Enter a Circuit File (e.g. adder_32bit.txt)" << endl;
    return 1;
  }

  uint32_t i;
  string filename;
  auto version = 0;
  bool zhao = true;
  bool random = false;
  uint32_t random_size = 0;

  for (i = 1; i < argc; i++) {
    string temp(argv[i]);
    if (temp == "-version") {
      if (i + 1 < argc) {
        version = atoi(argv[i+1]);
        i++;
      }
    } else if (temp == "-valiant") {
      zhao = false;
    } else if (temp == "-random") {
      random = true;
      if (i + 1 < argc) {
        random_size = atoi(argv[i + 1]);
        i++;
      }
    } else {
      filename = argv[i];
    }
  }

  if (abs(version) != 2 && version != 4 && version != 0) {
    cout << "Only allow version 2, -2, 0 and version 4" << endl;
    return 1;
  }

  bool hybrid = version == 0;

  cout << "Generating ";
  if(hybrid)
     cout << "hybrid " ;
  cout << "Universal Circuit with ";
  if(zhao)
     cout << "Zhao et al\'s " ;
  else
     cout << "Valiant\'s " ;
  if(version == 0)
     cout << "4" ;
  else
     cout << version ;
  cout << "-way Split Construction" << endl;
  cout << "Circuit:\t" << filename << endl << endl;

    char shdl2[strlen(filename.c_str()) + strlen(SHDL_CIRCUIT_FILE_FORMAT) + strlen(CIRCUIT_DIRECTORY)];
    strcpy(shdl2, CIRCUIT_DIRECTORY);
    strcat(shdl2, filename.c_str());
    strcat(shdl2, SHDL_CIRCUIT_FILE_FORMAT);

    char shdl[strlen(shdl2) + strlen(SHDL_MOD_CIRCUIT_FILE_FORMAT)];
    strcpy(shdl, shdl2);
    strcat(shdl, SHDL_MOD_CIRCUIT_FILE_FORMAT);

  if(!random){
    cout << endl << "START SHDL" << endl;
    SHDL_to_SHDL(shdl2);
    cout << "END SHDL" << endl;
  }

  cout << endl << "START UC" << endl;

  uint32_t input = 0;
  uint32_t output = 0;
  DAG_Gamma2 *gg;
  if(!random) {
    gg = read_SHDL(shdl, input, output);
  }
  else {
    input = 1;
    output = 1;
    gg = random_init(random_size);
  }
#ifdef STATE
  cout << "1. Creating the Gamma2 graph with " << input << " inputs and " << output << " outputs is done" << endl;
#endif // STATE

#ifdef TIME
  clock_t start = getMilliSecs();
#endif // TIME

  if (version == -2) {
    Valiant_DAG* G = embedding_merged(gg, input, output);

#ifdef TIME
    clock_t time2 = getMilliSecs();
    cout << "Embedding time: " << (time2 - start) << "ms" << endl;
#endif // TIME

    G->print_circuit(input, output, shdl);

#ifdef TIME
    clock_t time3 = getMilliSecs();
    cout << "I/O time: " << (time3 - time2) + (time2 - start) << "ms" << endl;
#endif // TIME
  } else {
    auto n = gg->node_number;
    std::vector<uint64_t> hybrid_choice;
    
    if (hybrid) {
      Dynamic_Hybrid(hybrid_choice, n, zhao);
    }
    auto *uc = new ValiantUC(n, version, zhao, hybrid_choice);
    bool fourWay = version == 4;
    if (hybrid) {
      fourWay = nextK(n, hybrid_choice) == 4;
    }
    gg->create_subgraphs(2 * n, true, fourWay, hybrid, hybrid_choice);
    gg->check_correct_subgraphs();
#ifdef DEBUG_GRAPH
    print_gamma2_full(gg);
    print_gamma_tree(gg, true, true);
#endif // DEBUG_GRAPH
    uc->start(gg, input, output, shdl, zhao);

#ifdef TIME
    clock_t time2 = getMilliSecs();
    cout << "UC Construction Time: " << (time2 - start) << "ms" << endl;
#endif // TIME

#ifdef DEBUG_CORRECTNESS
    if (validate_block_edge_embedding(uc)) {
      cout << "Block Edge-Embedding passed!" << endl;
    } else {
      cout << "Block Edge-Embedding failed!" << endl;
    }
    if (validate_recursion_point_edge_embedding(uc, gg, abs(version), hybrid_choice)) {
      cout << "Edge-Embedding passed!" << endl;
    } else {
      cout << "Edge-Embedding failed!" << endl;
    }
#endif // DEBUG_CORRECTNESS
  }

  cout << "END UC" << endl;

#ifdef DEBUG_CORRECTNESS
  if(!random){
	  cout << endl << "Evaluation result of original SHDL file " << endl;
	  vector<bool> input_list;
	  vector<bool> output_list;
	  eval_SHDL(shdl, input_list, output_list);

	  for (auto &&j : output_list) {
		cout << j << " ";
	  }
	  cout << endl << endl;

	  cout << endl << "Evaluation result of UC files" << endl;
	  vector<bool> output_list2;
	  // Pre-defined input bits, output bits to-be-compared
	  eval_UC(shdl, input_list, output_list2);

	  for (auto &&j : output_list2) {
		cout << j << " ";
	  }
	  cout << endl;

	  bool both_equal = true;
	  if(output_list2.size() == output_list.size()) {
		for(i = 0; i < output_list.size(); i++) {
		  if(output_list2[i] != output_list[i]) {
			both_equal = false;
		  }
		}
	  } else {
		both_equal = false;
	  }

	  if (both_equal) {
		cout << "Correctness Check passed!" << endl;
	  } else {
		cout << "Correction Check failed!" << endl;
	  }
  }

#endif // DEBUG_CORRECTNESS

  return 0;

}
