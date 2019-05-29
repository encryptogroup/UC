/**
 \file 		bristol_to_SHDL.cpp
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
			Function to convert bristol format to SHDL format.
 */

// TODO TO-BE-COMMENTED

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string.h>
#include <vector>
#include <list>
#include <math.h>
#include <stdint.h>
#include "config.h"

using namespace std;

class DAG_fanin2 {
 public:
  struct Node {
    uint32_t number;
    uint32_t number2; //for outputting in different order
    vector<Node *> neighbours;
    Node *right_parent;
    Node *left_parent;
    uint8_t type;
    bool output;
    int arity;

    Node(uint32_t, uint32_t);
  };
  Node **gateA;
  uint32_t gate_number;
  uint32_t input_number;
  uint32_t output_number;

  DAG_fanin2(uint32_t, uint32_t, uint32_t);
  DAG_fanin2(uint32_t, uint32_t, uint32_t, bool);

  void add_edge(Node *, Node *);
  Node *find_node(uint32_t number);

  ~DAG_fanin2();
};

uint64_t read_Bristol_circuit(string string_file, uint32_t &input_num_total, uint32_t &output_num);

/**
 * node construction with specified number, by default it has no children and parents, is not colored
 * @param number of the node
 * @param typ type of the node
 */
DAG_fanin2::Node::Node(uint32_t num, uint32_t typ)
    : number(num),
      number2(num),
      right_parent(0),
      left_parent(0),
      type(typ) //0 for XOR, 1 for AND, 2 for INV, 3 for input, 4 for uninitialized
    ,
      output(false) {}

/**
 * gamma2 instance with n nodes
 * @param k number of gates
 * @param u number of inputs
 * @param v number of outputs
 */
DAG_fanin2::DAG_fanin2(uint32_t k, uint32_t u, uint32_t v) {
  this->gate_number = k;
  this->input_number = u;
  this->output_number = v;

  gateA = new Node *[gate_number + input_number + output_number];
  for (uint32_t i = 0; i < gate_number + input_number; ++i) {
    gateA[i] = new Node(i, 5);
  }
  for (uint32_t i = 0; i < input_number; ++i) {
    gateA[i]->type = 3;
  }
}

/**
 * find the node with the given number
 * @param number number of the node
 * @return node with the given number
 */
DAG_fanin2::Node *DAG_fanin2::find_node(uint32_t number) {
  Node *current;
  for (uint32_t i = 0; i < gate_number + input_number; ++i) {
    current = gateA[i];
    if (current->number == number) {
      return current;
    }
  }
  cerr << "PROBLEM in find_node: Not found" << endl;
  return 0;
}

/**
 * Deconstructor
 */
DAG_fanin2::~DAG_fanin2() {
  Node *current;
  for (uint32_t i = 0; i < gate_number + input_number; i++) {
    current = gateA[i];
    current->neighbours.clear();
    delete current;
  }
}

/**
 * add an edge between two nodes if valid
 * @param n1 node 1
 * @param n2 node 2
 */
void DAG_fanin2::add_edge(DAG_fanin2::Node *n1, DAG_fanin2::Node *n2) {
  n1->neighbours.push_back(n2);
  if (n2->left_parent == 0) {
    n2->left_parent = n1;
  } else if (n2->right_parent == 0) {
    n2->right_parent = n1;
  } else {
    cout << "PROBLEM in add_edge: failed edge insert " << n1->number << " , " << n2->number << endl;
  }
}

/**
 * read a bristol circuit and converts it into SHDL
 * @param filename bristol circuit
 * @param input_num_total number of inputs
 * @param output_num number of outputs
 * @return copy gates
 */
uint64_t read_Bristol_circuit(string filename, uint32_t &input_num_total, uint32_t &output_num) {
  string suffix("_SHDL.circuit");
  string out_file = filename + suffix;
  uint32_t input_num1;
  uint32_t input_num2;
  uint32_t wires_num;
  uint32_t gate_num;
  ifstream file;
  file.open(filename);
  cout << filename << endl;
  file >> gate_num >> wires_num >> input_num1 >> input_num2 >> output_num;
  input_num_total = input_num1 + input_num2;
  vector<uint32_t> output_order(output_num);
  DAG_fanin2 *g = new DAG_fanin2(gate_num, input_num_total, output_num);

  string type;
  uint16_t gate_input;
  uint16_t gate_output;
  uint32_t input1;
  uint32_t input2;
  uint32_t output;

  ofstream file2;
  file2.open(out_file);

  //XOR[0] = 0;     XOR[1] = 1;     XOR[2] = 1;     XOR[3] = 0;
  //AND[0] = 0;     AND[1] = 0;     AND[2] = 0;     AND[3] = 1;
  uint32_t num = input_num_total;
  uint32_t inverter_num = 0;
  DAG_fanin2::Node *current;
  DAG_fanin2::Node *left_p;
  DAG_fanin2::Node *right_p;

  /* Reading through the file, checking if input, output or gate, adding one to the fanout of each input when a gate is considered */
  for (uint32_t i = 0; i < input_num_total; i++) {
    file2 << i << " input" << endl;
  }
  while (!file.eof()) {
    if (file >> gate_input) {
      file >> gate_output >> input1;
      if (gate_input == 2) {
        file >> input2;
      }
      file >> output >> type;
      current = g->gateA[output];
      if (type == "INV") {
        inverter_num++;
        current->number = num;
        current->number2 = num;
        num++;
        current->type = 2;
        current->arity = 1;
        g->add_edge(g->gateA[input1], current);
        type == "";
        if (output >= input_num_total + gate_num - output_num) {
          current->output = true;
          output_order[output - (input_num_total + gate_num - output_num)] = current->number;
        }
      } else if (type == "AND") {
        current->number = num;
        current->number2 = num;
        num++;
        current->type = 1;
        current->arity = 2;
        g->add_edge(g->gateA[input1], current);
        g->add_edge(g->gateA[input2], current);
        type == "";
        if (output >= input_num_total + gate_num - output_num) {
          current->output = true;
          output_order[output - (input_num_total + gate_num - output_num)] = current->number;
        }
      } else if (type == "XOR") {
        current->number = num;
        current->number2 = num;
        num++;
        current->type = 0;
        current->arity = 2;
        g->add_edge(g->gateA[input1], current);
        g->add_edge(g->gateA[input2], current);
        type == "";
        if (output >= input_num_total + gate_num - output_num) {
          current->output = true;
          output_order[output - (input_num_total + gate_num - output_num)] = current->number;
        }
      } else {
        cerr << "PROBLEM in read_Bristol_circuit: type not known " << type << endl;
      }
    }
  }

  uint32_t current_inv_gatenum = 0;
  for (uint32_t i = input_num_total; i < gate_num + input_num_total; ++i) {
    current = g->find_node(i);
    left_p = current->left_parent;
    right_p = current->right_parent;

    if ((current->type == 0) || (current->type == 1) || (current->type == 2 && current->output)) {
      current->number2 = current->number2 - current_inv_gatenum;
      file2 << current->number2;
    }
    if (current->output) {
      file2 << " output";
    }
    if ((current->type == 0) || (current->type == 1) || (current->type == 2 && current->output)) {
      file2 << " gate arity " << current->arity << " table [ ";
      if (current->type == 2 && current->output) {
        file2 << "1 0";
        file2 << " ] inputs [ " << left_p->number2;
      }
    }
    if (current->type == 0) {
      if ((left_p->type == 2 && right_p->type == 2)
          || left_p->type != 2 && right_p->type != 2) {  //either a XOR b    or    NOT a XOR NOT b
        file2 << "0 1 1 0";
      } else { //NOT a XOR b    or    a XOR NOT b
        file2 << "1 0 0 1";
      }
      file2 << " ] inputs [ " << left_p->number2 << " " << right_p->number2;
    }
    if (current->type == 1) {
      if ((left_p->type != 2 && right_p->type != 2)) {  // a AND b
        file2 << "0 0 0 1";
      } else if ((left_p->type == 2 && right_p->type != 2)) {  // NOT a AND b
        file2 << "0 1 0 0";
      } else if ((left_p->type != 2 && right_p->type == 2)) {  // a AND NOT b
        file2 << "0 0 1 0";
      } else if ((left_p->type == 2 && right_p->type == 2)) {  //NOT a AND NOT b
        file2 << "1 0 0 0";
      }
      file2 << " ] inputs [ " << left_p->number2 << " " << right_p->number2;
    }
    if (current->type == 2 && !(current->output)) { //non-output inverter nodes
      current->number2 = left_p->number2;
      current_inv_gatenum++;
    }
    if ((current->type == 0) || (current->type == 1) || (current->type == 2 && current->output)) {
      file2 << " ]" << endl;
    }
  }

  file2 << "outputs ";
  for (uint32_t i = 0; i < output_num; ++i) {
    file2 << g->find_node(output_order[i])->number2 << " ";
  }

  uint64_t copy_gates = 0;

  file.close();
  g->~DAG_fanin2();
  return copy_gates;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "Enter a circuit file (e.g. adder_32bit.txt)" << endl;
  } else {
    string bristol(argv[1]);
    string directory(CIRCUIT_DIRECTORY);
    string filename = directory + bristol;
    uint32_t input_num_total;
    uint32_t output_num;
    cout << "run bristol with file " << filename << endl;
    read_Bristol_circuit(filename, input_num_total, output_num);
    cout << "finished bristol" << endl;
  }
}
