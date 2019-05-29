/**
 \file 		ValiantUC.cpp
 \author 	guenther@rangar.de
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
 \brief		Implementation of the ValiantUC class part of Valiant's 4-way Split Construction.
 */

#include <stack>
#include <iostream>
#include <fstream>
#include "ValiantUC.h"
#include "../../Gamma/util/hybrid_size.h"

/**
 * computes the programming bits for Zhao
 * @return programming bits for Zhao
 */
std::map<uint32_t, uint32_t> getMapping(){
	//This is only run for Zhao
	std::ifstream file;
	file.open("src/uc/4way/prog2.txt");

	std::string line;
	std::vector<std::string> tokens;
	uint32_t line_num = 0;
	uint32_t id, prog;

	std::map<uint32_t, uint32_t> mapping;
	while(getline(file, line)){
		tokenize(line, tokens);
		if(line_num % 2 == 0){ //every odd row
			id = 0;
			id = atoi(tokens[0].c_str());
		}
		else{
			prog = 0;
			prog = atoi(tokens[0].c_str());
			mapping[id] = prog;
		}

		line_num++;
	}
	file.close();
	return mapping;
}

/**
 * create Valiant's UC
 * @param nodeNum number of poles of the UC
 * @param k k-way split, 0 for hybrid
 * @param zhao true if Zhao's 4-way split is used
 * @param hybrid_choice mapping between number of poles and next k (for hybrid)
 */
ValiantUC::ValiantUC(uint32_t nodeNum, int k, bool zhao, std::vector<uint64_t>& hybrid_choice) {
  for(uint32_t i = 0; i < nodeNum; i++) {
    this->poles.emplace_back(new UCNode(i + 1, POLE));
  }
  inputs = 0;
  outputs = 0;
  gates = 0;

  this->eugs.emplace_back(new ValiantEUG (poles, 0, k, zhao, hybrid_choice));
  this->eugs.emplace_back(new ValiantEUG (poles, 1, k, zhao, hybrid_choice));

#ifdef STATE
  std::cout << "2. UC created" << std::endl;
#endif // STATE

  this->calculateSize();
  topOrderedNodes.resize(size);
}

/**
 * start the process to generate the two output files having the input Gamma_2 graph
 * @param graph Gamma_2 graph (supergraph)
 * @param inputs number of inputs
 * @param outputs number of outputs
 * @param name name of the output files (without suffixes)
 * @param zhao true if Zhao's construction (4-way) is used
 */
void ValiantUC::start(DAG_Gamma2 *graph, uint32_t inputs, uint32_t outputs, string name, bool zhao) {
  // Hint: edge embedding has to be done before setting inputs and outputs!
  // only for Zhao
	std::map<uint32_t, uint32_t> mapping;
  if (zhao) {
    mapping = getMapping();
  }

  this->setGamma2(graph);
  this->edgeEmbedding(mapping);
#ifdef STATE
  std::cout << "3. Edge-Embedding done" << std::endl;
#endif // STATE
  this->setInputsAndOutputs(inputs, outputs, zhao);
#ifdef STATE
  std::cout << "4. Merged both universal graphs to the UC" << std::endl;
#endif // STATE
  this->writeOnFile(name);
}

/**
 * get the left EUG
 * @return left EUG
 */
ValiantEUG* ValiantUC::left() { return this->eugs[0]; }

/**
 * get the right EUG
 * @return right EUG
 */
ValiantEUG* ValiantUC::right() { return this->eugs[1]; }

/**
 * calculate the size of the uc
 */
void ValiantUC::calculateSize() {
  this->size = (uint32_t) (2 * eugs[0]->getSize() - poles.size());
}

/**
 * Get the size of the uc
 * @return size
 */
uint32_t ValiantUC::getSize() { return this->size; }

/**
 * set the Gamma_2 supergraph
 * @param dag supergraph
 */
void ValiantUC::setGamma2(DAG_Gamma2 *dag) { this->dag = dag; }

/**
 * initiates the edge-embedding process
 * @param mapping programming bits for Zhao's construction
 */
void ValiantUC::edgeEmbedding(const std::map<uint32_t, uint32_t>& mapping) {
  DAG_Gamma1 *leftG1 = dag->gamma1_left;
  DAG_Gamma2 *leftG2 = dag->sub_left;

  DAG_Gamma1 *rightG1 = dag->gamma1_right;
  DAG_Gamma2 *rightG2 = dag->sub_right;

  eugs[0]->edgeEmbedding(leftG1, leftG2, mapping);
  eugs[1]->edgeEmbedding(rightG1, rightG2, mapping);
}

/**
 * remove incoming inputs [outgoing outputs] of real inputs [outputs]
 * @param inputs number of inputs
 * @param outputs number of outputs
 * @param zhao true if Zhao's 4-way construction is used
 */
void ValiantUC::setInputsAndOutputs(uint32_t inputs, uint32_t outputs, bool zhao) {
  this->inputs = inputs;
  this->outputs = outputs;
  this->gates = (int) poles.size() - inputs - outputs;
  for (int i = 0; i < inputs; i++) {
    this->poles[i]->setPoleType(INPUT);
    if(i > 3 && zhao && this->eugs[0]->getK() == 4){
    	this->poles[i]->swapParentsProg(); //ONLY FOR ZHAO and not for the head block
    }
    this->poles[i]->clearParents();
  }
  for (int i = inputs; i < poles.size() - outputs; i++) {
    this->poles[i]->setPoleType(GATE);
    auto currentNode = this->dag->node_array[i];
    uint16_t tmp0 = currentNode->function_bits[0];
    uint16_t tmp1 = currentNode->function_bits[1];
    uint16_t tmp2 = currentNode->function_bits[2];
    uint16_t tmp3 = currentNode->function_bits[3];

    if (tmp1 != tmp2) {
      auto leftParent = currentNode->left_parent;
      auto rightParent = currentNode->right_parent;
      auto leftGamma = this->dag->gamma1_left;
      auto rightGamma = this->dag->gamma1_right;

      if (leftParent && !rightParent || rightParent && !leftParent) {
        auto parent = leftParent;
        if (rightParent) {
          parent = rightParent;
        }
        if (leftGamma && leftGamma->node_array[parent->number - 1] && leftGamma->node_array[parent->number - 1]->child
            && leftGamma->node_array[parent->number - 1]->child->number == currentNode->number ) {
          if(tmp0 != tmp1) std::cout << "Something is wrong!" << std::endl;
        } else if (rightGamma && rightGamma->node_array[parent->number - 1] && rightGamma->node_array[parent->number - 1]->child
            && rightGamma->node_array[parent->number - 1]->child->number == currentNode->number) {
          auto referenceBit = tmp0;
          tmp1 = 1 - referenceBit;
          tmp2 = referenceBit;
        }
      } else if (rightGamma->node_array[rightParent->number - 1]->child) {
        if (leftGamma && leftParent && leftGamma->node_array[leftParent->number - 1]
            && (!leftGamma->node_array[leftParent->number - 1]->child || (
                leftGamma->node_array[leftParent->number - 1]->child
                    && leftGamma->node_array[leftParent->number - 1]->child->number != currentNode->number))
            ||
                rightGamma && rightParent && rightGamma->node_array[rightParent->number - 1]
                    && (!rightGamma->node_array[rightParent->number - 1]->child ||
                        rightGamma->node_array[rightParent->number - 1]->child->number != currentNode->number)) {
          auto tmp = tmp2;
          tmp2 = tmp1;
          tmp1 = tmp;
        }
      } else {
        if (leftGamma && leftParent && leftGamma->node_array[leftParent->number - 1]
            && (!leftGamma->node_array[leftParent->number - 1]->child || (
                leftGamma->node_array[leftParent->number - 1]->child
                    && leftGamma->node_array[leftParent->number - 1]->child->number != currentNode->number))
            ||
                rightGamma && rightParent && rightGamma->node_array[rightParent->number - 1]
                    && (!rightGamma->node_array[rightParent->number - 1]->child ||
                        false)) {
          auto tmp = tmp2;
          tmp2 = tmp1;
          tmp1 = tmp;
        }
      }
    }

    std::vector<unsigned int> result = {tmp0, tmp1, tmp2, tmp3};
    this->poles[i]->setControlBits(result);
  }
  for (auto i = (int) (poles.size() - outputs); i < poles.size(); i++) {
    this->poles[i]->setPoleType(OUTPUT);
    this->poles[i]->clearChildren();
  }

  topologicalOrdering(inputs);

  int counter = 0;
  for (auto node : this->topOrderedNodes) {
    int temp = node->getTopologicalNumber();
    node->setNumberOfAdditionalWires(counter);
    NodeType t = node->getNodeType();
    if (node->getParents().size() == 2 && node->getChildren().size() == 2 && node->getPoleType() == NO_POLE) {
      counter++;
    } else if (node->getParents().size() == 1) {
      counter--;
    }
  }
}

/**
 * order all nodes of the uc topological
 * @inputs number of real inputs
 */
void ValiantUC::topologicalOrdering (uint32_t inputs) {
  uint32_t topValue = this->size - 1;
  std::stack<UCNode*> dfs;

  for (int i = 0; i < inputs; i++) {
    this->poles[i]->setTopologicalVisited (true);
    this->poles[i]->setTopologicalNumber(i);
    this->topOrderedNodes[i] = this->poles[i];
  }

  UCNode *currentNode;
  dfs.push(this->poles[0]->getChildren()[0]);
  dfs.push(this->poles[0]->getChildren()[1]);
  while (!dfs.empty()) {
    currentNode = dfs.top();
    if (currentNode->getTopologicalVisited()) {
      bool foundSomething = false;
      for (auto child : currentNode->getChildren()) {
        if (!child->getTopologicalVisited()) {
          dfs.push(child);
          foundSomething = true;
          break;
        }
      }
      if (foundSomething) {
        continue;
      }
      currentNode->setTopologicalNumber (topValue);
      topOrderedNodes[topValue] = currentNode;
      topValue--;
      dfs.pop();
    } else {
      currentNode->setTopologicalVisited (true);
      for (auto child : currentNode->getChildren()) {
        if (!child->getTopologicalVisited()) {
          dfs.push(child);
          break;
        }
      }
    }
  }
}

/**
 * create the two output files
 * @name file name without suffix
 */
void ValiantUC::writeOnFile(string name) {
  string realName = OUTPUT_DIRECTORY;
  realName.append(name);
  string circName = realName;
  string programmingName = realName;
  circName.append(CIRCUIT_FILE_FORMAT);
  programmingName.append(PROGRAMMING_FILE_FORMAT);
  std::ofstream circ (circName);
  std::ofstream prog (programmingName);
  std::string tmp = "C ";
  for (int i = 0; i < inputs; i++) {
    tmp += std::to_string(i) + " ";
  }
  circ << tmp << std::endl;

  for (auto node : topOrderedNodes) {
    int parents = node->getParents().size();
    if (parents == 2) {
      if (node->getPoleType() != INPUT) {
        circ << node->getNodeDescription() << std::endl;
        prog << node->getNodeProgramming(dag) << std::endl;
      }
    }
  }

  tmp = "O ";
  for (int i = inputs + gates; i < poles.size(); i++) {
    tmp += std::to_string(poles[i]->getInputWire(poles[i])) + " ";
  }
  circ << tmp << std::endl;
}

/**
 * return list of nodes topological ordered
 * @return list of nodes topological ordered
 */
std::vector <UCNode*> ValiantUC::getTopOrderedNodes() { return this->topOrderedNodes; }

/**
 * returns poles of the UC
 * @return poles of the UC
 */
std::vector<UCNode *> ValiantUC::getPoles() { return this->poles; }
