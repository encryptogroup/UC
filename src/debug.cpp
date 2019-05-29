/**
 \file 		debug.cpp
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
 \brief     Implementation of debug functions of Valiant's 4-way split UC Construction
 */

#include <cmath>
#include "debug.h"
#include "Gamma/util/hybrid_size.h"

using namespace std;

/**
 * validates the block edge embedding for the whole UC
 * @param uc uc to check
 * @return true if all blocks are embedded correctly
 */
bool validate_block_edge_embedding(ValiantUC* uc) {
  ValiantEUG* eugs[] = {uc->left(), uc->right()};
  for (int j = 0; j < 2; j++) {
    if(!validate_block_edge_embedding(eugs[j], j, "Path: " + std::to_string(j))) {
      return false;
    }
  }
  return true;
}

/**
 * validates the block edge-embedding of all blocks in a EUG
 * @param eug current EUG
 * @param eugNumber left or right EUG
 * @param path path of th EUG
 * @return true if all blocks in this EUG are embedded correctly
 */
bool validate_block_edge_embedding(ValiantEUG* eug, int eugNumber, string path) {
  // std::cout << path << std::endl;
  for (int i = 0; i < eug->getBlocks().size(); i++) {
    auto block = eug->getBlocks()[i];
    if(!block->validateEdgeEmbedding(eugNumber)) {
      block->printEmbedding();
      return false;
    }
  }
  for (int i = 0; i < eug->getChildren().size(); i++) {
    if(!validate_block_edge_embedding(eug->getChildren()[i], 1, path + " " + std::to_string(i))) {
      return false;
    }
  }
  return true;
}

/**
 * validates if the rcursion points are embedded correctly (requires that block edge-embedding is correct)
 * @param uc UC
 * @param gg Gamma2 graph
 * @param k k-way split
 * @param hybrid_choice mapping between number of poles and next k
 * @return true if recursion point edge-mebdding is correct
 */
bool validate_recursion_point_edge_embedding (ValiantUC *uc, DAG_Gamma2 *gg, int k, std::vector<uint64_t>& hybrid_choice) {
  ValiantEUG* eugs[] = {uc->left(), uc->right()};
  DAG_Gamma1* graphs[] = {gg->gamma1_left, gg->gamma1_right};
  if (k == 0) {
    k = nextK(gg->node_number, hybrid_choice);
  }
  for (int i = 0; i < 2; i++) {
    if(!validate_recursion_point_edge_embedding(graphs[i], eugs[i], i, k)) {
      return false;
    }
  }
  return true;
}

/**
 * validats recursion point edge-embedding on a single EUG
 * @param graph Gamma1 graph
 * @param uc EUG
 * @param eugNum left or right eug
 * @param k k-way split
 * @return true if recursion point edge-embedding is correct
 */
 bool validate_recursion_point_edge_embedding (DAG_Gamma1 *graph, ValiantEUG *uc, int eugNum, int k) {
  int wrongCounter = 0;
  for (int i = 0; i < graph->node_number; i++) {
    DAG_Gamma1::Node *currentNode = graph->node_array[i];
    int id = currentNode->number;

    DAG_Gamma1::Node *destNode = currentNode->child;
    if (!destNode) continue;
    int destId = destNode->number;
    int destBlock = (int) floor(float((destId - 1) / k));
    int destBlockPosition = (destId - 1) % k;

    UCNode *lastNode = uc->getBlocks()[destBlock]->getPoles()[destBlockPosition];
    UCNode *nextNode = lastNode->getParents()[eugNum];
    nextNode = find_next_pole(lastNode, nextNode);
    int calculatedStartId = nextNode->getID();
    if(calculatedStartId != id) {
      std::cout << "should be " << id << " -> " << destId << "\tbut is: " << calculatedStartId << " -> " << destId << std::endl;
      wrongCounter++;
    } //else {
      //std::cout << id << " -> " << destId << " correct" << std::endl;
    //}
  }
  if (wrongCounter == 0) {
    return true;
  }
  return false;
}

UCNode* find_next_pole (UCNode *pole, UCNode *nextNode) {
  UCNode *lastNode = pole;
  while (nextNode->getNodeType() != POLE) {
    UCNode *tmpNode = nextNode;
    NodeType type = nextNode->getNodeType();
    if (type == X_SWITCH || (type == RECURSION_POINT && nextNode->getParents().size() == 2 && nextNode->getChildren().size() == 2)) {
      if (nextNode->getControlBits()[0] == 0) {
        if (nextNode->getChildren()[0] == lastNode) {
          nextNode = nextNode->getParents()[0];
        } else {
          nextNode = nextNode->getParents()[1];
        }
      } else {
        if (nextNode->getChildren()[0] == lastNode) {
          nextNode = nextNode->getParents()[1];
        } else {
          nextNode = nextNode->getParents()[0];
        }
      }
    } else if (type == Y_SWITCH || (type == RECURSION_POINT && nextNode->getChildren().size() == 1)) {
      nextNode = nextNode->getParents()[nextNode->getControlBits()[0]];
    } else if (type == I_SWITCH || type == REVERSE_Y_SWITCH || (type == RECURSION_POINT && nextNode->getParents().size() == 1)) {
      nextNode = nextNode->getParents()[0];
    }
    lastNode = tmpNode;
  }
  return nextNode;
}
