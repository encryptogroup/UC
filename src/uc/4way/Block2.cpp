/**
 \file 		Block2.cpp
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
 \brief		Implementation of the Block2 class part of Valiant's 4-way Split Construction.
 */


#include "Block2.h"

/**
 * Constructor of Valiant's 2-way split Block for hybrid construction
 * @param poles list of poles
 * @param poleIndex current pole index
 * @param recursionSteps list of recursion steps
 * @param position current position
 * @param blockType type of the block
 * @param recursionPoints list of recursion points
 */
Block2::Block2(std::vector<UCNode *> poles,
               uint32_t poleIndex,
               std::vector<uint32_t> recursionSteps,
               uint32_t position,
               BlockType blockType,
               std::vector<std::vector<UCNode *>> recursionPoints) {
  this->blockType = blockType;
  this->recursionSteps = recursionSteps;
  this->position = position;
  this->recursionPoints.emplace_back(recursionPoints[0]);
  this->recursionPoints.emplace_back(recursionPoints[1]);

  int poleSize = poles.size();
  int numberOfRemainingPoles = (int) poleSize - poleIndex;
  int numberOfPoles;
  if (poleSize > 4) {
    if (numberOfRemainingPoles > 2) {
      numberOfPoles = 2;
    } else {
      numberOfPoles = numberOfRemainingPoles;
    }
  } else {
    numberOfPoles = numberOfRemainingPoles;
  }

  this->inputPermutation = {DUMMY, DUMMY};
  this->outputPermutation = std::vector<uint32_t>();

  for (int i = poleIndex; i < poleIndex + numberOfPoles; i++) {
    this->poles.emplace_back(poles[i]);
    this->outputPermutation.emplace_back(DUMMY);
  }

  int recursionPointNumber = 2;
  int index = blockType == HEAD ? 0 : 1;
  if(recursionPoints[1].size() == index) {
    recursionPointNumber = 1;
  }

  switch (this->blockType) {
    case NORMAL:
      createNodesFromTo(1, 2, X_SWITCH);
      nodes[3] = new UCNode(3, recursionPointNumber == 2 ? X_SWITCH : Y_SWITCH);
      createEdge(this->recursionPoints[0][0], nodes[1]);
      createEdge(this->recursionPoints[1][0], nodes[1]);
      createEdge(nodes[1], nodes[2]);
      createEdge(nodes[1], this->poles[0]);
      createEdge(this->poles[0], nodes[2]);
      createEdge(nodes[2], nodes[3]);
      createEdge(nodes[2], this->poles[1]);
      createEdge(this->poles[1], nodes[3]);
      createEdge(nodes[3], this->recursionPoints[0][1]);
      if (recursionPointNumber == 2) {
        createEdge(nodes[3], this->recursionPoints[1][1]);
      }
      break;
    case HEAD:
      nodes[2] = new UCNode(2, REVERSE_Y_SWITCH);
      nodes[3] = new UCNode(3, recursionPointNumber == 2 ? X_SWITCH : Y_SWITCH);
      createEdge(this->poles[0], nodes[2]);
      createEdge(nodes[2], nodes[3]);
      createEdge(nodes[2], this->poles[1]);
      createEdge(this->poles[1], nodes[3]);
      createEdge(nodes[3], this->recursionPoints[0][0]);
      if (recursionPointNumber == 2) {
        createEdge(nodes[3], this->recursionPoints[1][0]);
      }
      break;
    case TAIL:
      if (numberOfRemainingPoles == 1) {
        createEdge(this->recursionPoints[0][0], this->poles[0]);
      } else {
        nodes[1] = new UCNode(1, X_SWITCH);
        nodes[2] = new UCNode(2, Y_SWITCH);
        createEdge(this->recursionPoints[0][0], nodes[1]);
        createEdge(this->recursionPoints[1][0], nodes[1]);
        createEdge(nodes[1], nodes[2]);
        createEdge(nodes[1], this->poles[0]);
        createEdge(this->poles[0], nodes[2]);
        createEdge(nodes[2], this->poles[1]);
      }
      break;
    case RECURSION_BASE:
      if (numberOfRemainingPoles == 2 || numberOfRemainingPoles == 3) {
        createEdge(this->poles[0], this->poles[1]);
      }
      if (numberOfRemainingPoles == 3) {
        createEdge(this->poles[1], this->poles[2]);
      }
      if (numberOfRemainingPoles == 4) {
        nodes[18] = new UCNode(18, REVERSE_Y_SWITCH);
        nodes[19] = new UCNode(19, X_SWITCH);
        nodes[20] = new UCNode(20, Y_SWITCH);
        createEdge(this->poles[0], nodes[18]);
        createEdge(nodes[18], nodes[19]);
        createEdge(nodes[18], this->poles[1]);
        createEdge(this->poles[1], nodes[19]);
        createEdge(nodes[19], nodes[20]);
        createEdge(nodes[19], this->poles[2]);
        createEdge(this->poles[2], nodes[20]);
        createEdge(nodes[20], this->poles[3]);
      }
      break;
    default: break;
  }

}

/**
 * Delete Block2
 */
Block2::~Block2() {}

/**
 * edge embedding of the block
 * @param mapping mapping between nodes (not used in the implementation, but necessary for interface)
 */
void Block2::edgeEmbedding(const std::map<uint32_t, uint32_t>& mapping) {
  int recursionPointNumber = 2;
  int index = blockType == HEAD ? 0 : 1;
  if(recursionPoints[1].size() == index) {
    recursionPointNumber = 1;
  }
  if (poles.size() == 1) return;

  if(this->blockType != RECURSION_BASE) {
    int left3;
    if (this->blockType != HEAD) {

      // UCNode 1
      if (inputPermutation[0] == 0 || inputPermutation[1] == 1) {
        nodes[1]->setControlBit(1);
      } else {
        nodes[1]->setControlBit(0);
      }

      // UCNode 2
      if(blockType == TAIL) {
        if (outputPermutation[0] == 1) {
          nodes[2]->setControlBit(1);
        } else {
          nodes[2]->setControlBit(0);
        }
      } else {
        if (outputPermutation[0] == 1) {
          nodes[2]->setControlBit(0);
          left3 = DUMMY;
        } else {
          nodes[2]->setControlBit(1);
          left3 = outputPermutation[0];
        }
      }
    }
    if(this->blockType == TAIL) return;
    if (blockType == HEAD) {
      left3 = outputPermutation[0];
    }

    //UCNode 3
    if (outputPermutation[1] == 2 || left3 == 3) {
      nodes[3]->setControlBit(1);
    } else {
      nodes[3]->setControlBit(0);
    }
  } else {
    if (poles.size() == 4) {  // handle recursion base with 4 poles
      // UCNode 19
      if (outputPermutation[0] == 3 || outputPermutation[1] == 2) {
        nodes[19]->setControlBit(0);
      } else {
        nodes[19]->setControlBit(1);
      }

      // UCNode 20
      if (outputPermutation[2] == 3) {
        nodes[20]->setControlBit(1);
      } else {
        nodes[20]->setControlBit(0);
      }
    }
  }
}

/**
 * prints the embedding of the block
 */
void Block2::printEmbedding() {
  std::cout << std::endl << "Input Permutation:\t(" << inputPermutation[0] << ", "
            << inputPermutation[1] << ")" << std::endl;
  std::cout << "Output Permutation:\t(" << outputPermutation[0] << ", " << outputPermutation[1]  << ")" << std::endl;
  std::cout << "Block Type:\t" << this->blockType << std::endl;

  for (auto node : nodes) {
    std::string temp = "-";
    if(node.second->getNodeType() != I_SWITCH && node.second->getNodeType() != REVERSE_Y_SWITCH) {
      temp = std::to_string(node.second->getControlBits()[0]);
    }
    std::cout << "Node " << node.first << ":\t" << temp << std::endl;
  }
  std::cout << std::endl;
}

/**
 * test, if the edge-embedding algorithm worked for this block
 * @param eugNumber left or right EUG
 */
bool Block2::validateEdgeEmbedding(int eugNumber) {
  if (this->blockType == RECURSION_BASE) return true;
  int maxDepth = 3;
  if (blockType != HEAD && blockType != RECURSION_BASE) {
    for (int i = 0; i < poles.size(); i++) {
      uint32_t targetInput = DUMMY;
      for (int j = 0; j < inputPermutation.size(); j++) {
        if (inputPermutation[j] == i) {
          targetInput = j;
          break;
        }
      }
      if (targetInput == DUMMY) {
        continue;
      }
      UCNode *lastNode = poles[i];
      int index = eugNumber;
      if (this->recursionSteps.empty()) {
        index = eugNumber;
      }
      UCNode *nextNode = lastNode->getParents()[index];

      int counter = 0;
      while (nextNode->getNodeType() != RECURSION_POINT && counter <= maxDepth) {
        UCNode *tmpNode = nextNode;
        NodeType type = nextNode->getNodeType();
        if (type == X_SWITCH) {
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
        } else if (type == Y_SWITCH) {
          nextNode = nextNode->getParents()[nextNode->getControlBits()[0]];
        } else if (type == I_SWITCH || type == REVERSE_Y_SWITCH) {
          nextNode = nextNode->getParents()[0];
        }
        lastNode = tmpNode;
        counter++;
      }
      if (recursionPoints[targetInput][0] != nextNode || counter > maxDepth) {
        return false;
      }
    }
  }
  if (blockType != TAIL) {
    for (int i = 1; i < 4; i++) {
      int index = 1;
      if (blockType == HEAD) {
        index = 0;
      }
      uint32_t targetPole = DUMMY;
      for (int j = 0; j < outputPermutation.size(); j++) {
        if (outputPermutation[j] == i) {
          targetPole = j;
          break;
        }
      }
      if (targetPole == DUMMY) {
        continue;
      }
      UCNode *lastNode;
      if (i == 1) {
        lastNode = poles[i];
      } else {
        lastNode = recursionPoints[i - 2][index];
      }
      int index2 = eugNumber;
      if (lastNode->getNodeType() == RECURSION_POINT && i > 1) {
        index2 = 0;
      }
      if (this->recursionSteps.empty() && lastNode->getNodeType() == POLE) {
        index2 = eugNumber;
      }
      UCNode *nextNode = lastNode->getParents()[index2];
      while (nextNode->getNodeType() != POLE && nextNode->getNodeType() != RECURSION_POINT) {
        UCNode *tmpNode = nextNode;
        NodeType type = nextNode->getNodeType();
        if (type == X_SWITCH) {
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
        } else if (type == Y_SWITCH) {
          nextNode = nextNode->getParents()[nextNode->getControlBits()[0]];
        } else if (type == I_SWITCH || type == REVERSE_Y_SWITCH) {
          nextNode = nextNode->getParents()[0];
        }
        lastNode = tmpNode;
      }
      if (poles[targetPole] != nextNode) {
        return false;
      }
    }
  }
  return true;
}
