/**
 \file 		Block.cpp
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
 \brief		Implementation of the Block class part of Valiant's 4-way Split Construction.
 */

#include <iostream>
#include "Block.h"

/*
 * Construct a Block
 * @param poles vector of all poles of the uc
 * @param poleIndex index of the first pole occurring in this block
 * @param recursionSteps vector of recursion step numbers
 * @param position position within the recursion step chain
 * @param blockType type of the block
 * @param q first recursion points
 * @param r second recursion points
 * @param s third recursion points
 * @param t fourth recursion points
 */
Block::Block(std::vector<UCNode *> poles, uint32_t poleIndex,
             std::vector<uint32_t> recursionSteps, uint32_t position, BlockType blockType,
             std::vector<UCNode *> q, std::vector<UCNode *> r, std::vector<UCNode *> s, std::vector<UCNode *> t) {
  this->blockType = blockType;
  this->recursionSteps = recursionSteps;
  this->position = position;
  this->recursionPoints.emplace_back(q);
  this->recursionPoints.emplace_back(r);
  this->recursionPoints.emplace_back(s);
  this->recursionPoints.emplace_back(t);

  int numberOfRemainingPoles = (int) poles.size() - poleIndex;
  int numberOfPoles = (numberOfRemainingPoles > 4) ? 4 : numberOfRemainingPoles;

  this->inputPermutation = {DUMMY, DUMMY, DUMMY, DUMMY};
  this->outputPermutation = std::vector<uint32_t>();

  for (int i = poleIndex; i < poleIndex + numberOfPoles; i++) {
    this->poles.emplace_back(poles[i]);
    this->outputPermutation.emplace_back(DUMMY);
  }

  // you have to switch these to createAlternativeNormalBlock() etc.
  switch (this->blockType) {
    case NORMAL:createNormalBlock();
      break;
    case HEAD:createHeadBlock();
      break;
    case TAIL:createTailBlock(numberOfRemainingPoles);
      break;
    case RECURSION_BASE:
      if (numberOfRemainingPoles == 2 || numberOfRemainingPoles == 3) {
        createEdge(poles[0], poles[1]);
      }
      if (numberOfRemainingPoles == 3) {
        createEdge(poles[1], poles[2]);
      }
      if (numberOfRemainingPoles == 4) {
        nodes[18] = new UCNode(18, REVERSE_Y_SWITCH);
        nodes[19] = new UCNode(19, X_SWITCH);
        nodes[20] = new UCNode(20, Y_SWITCH);
        createEdge(poles[0], nodes[18]);
        createEdge(nodes[18], nodes[19]);
        createEdge(nodes[18], poles[1]);
        createEdge(poles[1], nodes[19]);
        createEdge(nodes[19], nodes[20]);
        createEdge(nodes[19], poles[2]);
        createEdge(poles[2], nodes[20]);
        createEdge(nodes[20], poles[3]);
      }
      break;
    default: break;
  }
}

// Deconstruct block
Block::~Block() {}

// create a head block
void Block::createHeadBlock() {
  nodes[5] = new UCNode(5, REVERSE_Y_SWITCH);
  nodes[6] = new UCNode(6, X_SWITCH);
  createNodesFromTo(7, 8, REVERSE_Y_SWITCH);
  createNodesFromTo(9, 14, X_SWITCH);

  createEdge(poles[0], nodes[5]);
  createEdge(nodes[5], nodes[6]);
  createEdge(nodes[5], poles[1]);
  createEdge(poles[1], nodes[6]);
  createEdge(nodes[6], nodes[7]);
  createEdge(nodes[6], nodes[8]);
  createEdge(nodes[7], nodes[11]);
  createEdge(nodes[7], nodes[9]);
  createEdge(nodes[8], nodes[9]);
  createEdge(nodes[9], nodes[10]);
  createEdge(nodes[9], poles[2]);
  createEdge(poles[2], nodes[10]);
  createEdge(nodes[10], poles[3]);
  createEdge(nodes[10], nodes[12]);
  createEdge(nodes[8], nodes[12]);
  createEdge(poles[3], nodes[11]);
  createEdge(nodes[11], nodes[13]);
  createEdge(nodes[11], nodes[14]);
  createEdge(nodes[12], nodes[13]);
  createEdge(nodes[12], nodes[14]);

  createEdge(nodes[13], recursionPoints[0][0]); // q
  createEdge(nodes[13], recursionPoints[1][0]); // r
  createEdge(nodes[14], recursionPoints[2][0]); // s
  createEdge(nodes[14], recursionPoints[3][0]); // t
}

// create a head block with parity optimization
void Block::createAlternativeHeadBlock() {
  nodes[5] = new UCNode(5, REVERSE_Y_SWITCH);
  nodes[6] = new UCNode(6, X_SWITCH);
  createNodesFromTo(7, 8, REVERSE_Y_SWITCH);
  createNodesFromTo(9, 12, X_SWITCH);
  nodes[14] = new UCNode(14, X_SWITCH);

  createEdge(poles[0], nodes[5]);
  createEdge(nodes[5], nodes[6]);
  createEdge(nodes[5], poles[1]);
  createEdge(poles[1], nodes[6]);
  createEdge(nodes[6], nodes[7]);
  createEdge(nodes[6], nodes[8]);
  createEdge(nodes[7], nodes[11]);
  createEdge(nodes[7], nodes[9]);
  createEdge(nodes[8], nodes[9]);
  createEdge(nodes[9], nodes[10]);
  createEdge(nodes[9], poles[2]);
  createEdge(poles[2], nodes[10]);
  createEdge(nodes[10], poles[3]);
  createEdge(nodes[10], nodes[12]);
  createEdge(nodes[8], nodes[12]);
  createEdge(poles[3], nodes[11]);
  createEdge(nodes[11], nodes[14]);
  createEdge(nodes[12], nodes[14]);

  this->createLowerPermutationNetwork();
}

// create a normal block
void Block::createNormalBlock() {
  createNodesFromTo(1, 15, X_SWITCH);

  createEdge(recursionPoints[0][0], nodes[1]);
  createEdge(recursionPoints[1][0], nodes[1]);
  createEdge(recursionPoints[2][0], nodes[2]);
  createEdge(recursionPoints[3][0], nodes[2]);

  createEdge(nodes[1], nodes[3]);
  createEdge(nodes[1], nodes[4]);
  createEdge(nodes[2], nodes[3]);
  createEdge(nodes[2], nodes[4]);
  createEdge(nodes[3], nodes[7]);
  createEdge(nodes[3], poles[0]);
  createEdge(poles[0], nodes[5]);
  createEdge(nodes[4], nodes[5]);
  createEdge(nodes[5], nodes[6]);
  createEdge(nodes[5], poles[1]);
  createEdge(poles[1], nodes[6]);
  createEdge(nodes[6], nodes[7]);
  createEdge(nodes[6], nodes[8]);
  createEdge(nodes[4], nodes[8]);
  createEdge(nodes[7], nodes[11]);
  createEdge(nodes[7], nodes[9]);
  createEdge(nodes[8], nodes[9]);
  createEdge(nodes[9], nodes[10]);
  createEdge(nodes[9], poles[2]);
  createEdge(poles[2], nodes[10]);
  createEdge(nodes[10], poles[3]);
  createEdge(poles[3], nodes[15]);
  createEdge(nodes[10], nodes[15]);
  createEdge(nodes[15], nodes[11]);
  createEdge(nodes[15], nodes[12]);
  createEdge(nodes[8], nodes[12]);
  createEdge(nodes[11], nodes[13]);
  createEdge(nodes[11], nodes[14]);
  createEdge(nodes[12], nodes[13]);
  createEdge(nodes[12], nodes[14]);

  createEdge(nodes[13], recursionPoints[0][1]);
  createEdge(nodes[13], recursionPoints[1][1]);
  createEdge(nodes[14], recursionPoints[2][1]);
  createEdge(nodes[14], recursionPoints[3][1]);
}

// create a normal block with parity optimization
void Block::createAlternativeNormalBlock() {
  createNodesFromTo(1, 12, X_SWITCH);
  nodes[14] = new UCNode(14, X_SWITCH);

  createEdge(recursionPoints[0][0], nodes[1]);
  createEdge(recursionPoints[1][0], nodes[1]);
  createEdge(recursionPoints[2][0], nodes[2]);
  createEdge(recursionPoints[3][0], nodes[2]);

  createEdge(nodes[1], nodes[3]);
  createEdge(nodes[1], nodes[4]);
  createEdge(nodes[2], nodes[3]);
  createEdge(nodes[2], nodes[4]);
  createEdge(nodes[3], nodes[7]);
  createEdge(nodes[3], poles[0]);
  createEdge(poles[0], nodes[5]);
  createEdge(nodes[4], nodes[5]);
  createEdge(nodes[5], nodes[6]);
  createEdge(nodes[5], poles[1]);
  createEdge(poles[1], nodes[6]);
  createEdge(nodes[6], nodes[7]);
  createEdge(nodes[6], nodes[8]);
  createEdge(nodes[4], nodes[8]);
  createEdge(nodes[7], nodes[11]);
  createEdge(nodes[7], nodes[9]);
  createEdge(nodes[8], nodes[9]);
  createEdge(nodes[9], nodes[10]);
  createEdge(nodes[9], poles[2]);
  createEdge(poles[2], nodes[10]);
  createEdge(nodes[10], poles[3]);
  createEdge(nodes[8], nodes[12]);
  createEdge(nodes[11], nodes[14]);
  createEdge(nodes[12], nodes[14]);

  this->createLowerPermutationNetwork();
}

// create a tail block
void Block::createTailBlock(uint32_t numberOfRemainingPoles) {
  switch (numberOfRemainingPoles) {
    case 1:createNodesFromTo(1, 2, Y_SWITCH);
      nodes[17] = new UCNode(17, Y_SWITCH);

      createEdge(recursionPoints[0][0], nodes[1]);  // q
      createEdge(recursionPoints[1][0], nodes[1]);  // r
      createEdge(recursionPoints[2][0], nodes[2]);  // s
      createEdge(recursionPoints[3][0], nodes[2]);  // t

      createEdge(nodes[1], nodes[17]);
      createEdge(nodes[2], nodes[17]);
      createEdge(nodes[17], poles[0]);
      break;
    case 2:createNodesFromTo(1, 2, X_SWITCH);
      createNodesFromTo(3, 5, Y_SWITCH);

      createEdge(recursionPoints[0][0], nodes[1]);  // q
      createEdge(recursionPoints[1][0], nodes[1]);  // r
      createEdge(recursionPoints[2][0], nodes[2]);  // s
      createEdge(recursionPoints[3][0], nodes[2]);  // t

      createEdge(nodes[1], nodes[3]);
      createEdge(nodes[1], nodes[4]);
      createEdge(nodes[2], nodes[3]);
      createEdge(nodes[2], nodes[4]);
      createEdge(nodes[3], poles[0]);
      createEdge(nodes[4], nodes[5]);
      createEdge(poles[0], nodes[5]);
      createEdge(nodes[5], poles[1]);
      break;
    case 3:createTail3Block();
      break;
    case 4:createTail4Block();
      break;
    default:break;
  }
}

// create a tail block with parity optimization
void Block::createAlternativeTailBlock(uint32_t numberOfRemainingPoles) {
  switch (numberOfRemainingPoles) {
    case 1:
      createEdge(recursionPoints[3][0], poles[0]);  // t
      break;
    case 2:
      nodes[1] = new UCNode(1, X_SWITCH);
      nodes[5] = new UCNode(5, Y_SWITCH);
      createEdge(recursionPoints[1][0], nodes[1]);  // r
      createEdge(recursionPoints[3][0], nodes[1]);  // t

      createEdge(nodes[1], poles[0]);
      createEdge(nodes[1], nodes[5]);
      createEdge(poles[0], nodes[5]);
      createEdge(nodes[5], poles[1]);
      break;
    case 3:createAlternativeTail3Block();
      break;
    case 4:createTail4Block();
      break;
    default:break;
  }
}

// creae a tail block with 4 poles
void Block::createTail4Block() {
  createNodesFromTo(1, 6, X_SWITCH);
  createNodesFromTo(7, 8, Y_SWITCH);
  nodes[9] = new UCNode(9, X_SWITCH);
  nodes[10] = new UCNode(10, Y_SWITCH);

  createEdge(recursionPoints[0][0], nodes[1]);
  createEdge(recursionPoints[1][0], nodes[1]);
  createEdge(recursionPoints[2][0], nodes[2]);
  createEdge(recursionPoints[3][0], nodes[2]);

  createEdge(nodes[1], nodes[3]);
  createEdge(nodes[1], nodes[4]);
  createEdge(nodes[2], nodes[3]);
  createEdge(nodes[2], nodes[4]);
  createEdge(nodes[3], nodes[7]);
  createEdge(nodes[3], poles[0]);
  createEdge(poles[0], nodes[5]);
  createEdge(nodes[4], nodes[5]);
  createEdge(nodes[5], nodes[6]);
  createEdge(nodes[5], poles[1]);
  createEdge(poles[1], nodes[6]);
  createEdge(nodes[6], nodes[7]);
  createEdge(nodes[6], nodes[8]);
  createEdge(nodes[4], nodes[8]);
  createEdge(nodes[7], nodes[9]);
  createEdge(nodes[8], nodes[9]);
  createEdge(nodes[9], nodes[10]);
  createEdge(nodes[9], poles[2]);
  createEdge(poles[2], nodes[10]);
  createEdge(nodes[10], poles[3]);
}

// create a tail block with 3 poles
void Block::createTail3Block() {
  createNodesFromTo(1, 5, X_SWITCH);
  nodes[6] = new UCNode(6, Y_SWITCH);
  nodes[8] = new UCNode(8, Y_SWITCH);
  nodes[16] = new UCNode(16, Y_SWITCH);

  createEdge(recursionPoints[0][0], nodes[1]);
  createEdge(recursionPoints[1][0], nodes[1]);
  createEdge(recursionPoints[2][0], nodes[2]);
  createEdge(recursionPoints[3][0], nodes[2]);

  createEdge(nodes[1], nodes[3]);
  createEdge(nodes[1], nodes[4]);
  createEdge(nodes[2], nodes[3]);
  createEdge(nodes[2], nodes[4]);
  createEdge(nodes[3], nodes[16]);
  createEdge(nodes[3], poles[0]);
  createEdge(poles[0], nodes[5]);
  createEdge(nodes[4], nodes[5]);
  createEdge(nodes[5], nodes[6]);
  createEdge(nodes[5], poles[1]);
  createEdge(poles[1], nodes[6]);
  createEdge(nodes[6], nodes[8]);
  createEdge(nodes[4], nodes[8]);
  createEdge(nodes[8], nodes[16]);
  createEdge(nodes[16], poles[2]);
}

// create a tail block with 3 poles and parity optimization
void Block::createAlternativeTail3Block() {
  createNodesFromTo(30, 33, X_SWITCH);
  createNodesFromTo(34, 35, Y_SWITCH);

  createEdge(recursionPoints[1][0], nodes[30]);
  createEdge(recursionPoints[2][0], nodes[30]);

  createEdge(nodes[30], nodes[32]);
  createEdge(nodes[30], nodes[31]);
  createEdge(recursionPoints[3][0], nodes[31]);
  createEdge(nodes[31], nodes[32]);
  createEdge(nodes[31], poles[0]);
  createEdge(nodes[32], nodes[34]);
  createEdge(nodes[32], nodes[33]);
  createEdge(poles[0], nodes[33]);
  createEdge(nodes[33], nodes[34]);
  createEdge(nodes[33], poles[1]);
  createEdge(nodes[34], nodes[35]);
  createEdge(poles[1], nodes[35]);
  createEdge(nodes[35], poles[2]);
}

void Block::createLowerPermutationNetwork() {
  int recursionPointNumber = 4;
  if (recursionPoints[1].size() == 0) {
    recursionPointNumber = 1;
  } else if (recursionPoints[2].size() == 0) {
    recursionPointNumber = 2;
  } else if (recursionPoints[0].size() == 0) {
    recursionPointNumber = 3;
  }

  if (recursionPointNumber == 1) {
    if (blockType == NORMAL) {
      createEdge(poles[3], nodes[11]);
      createEdge(nodes[10], nodes[12]);
    }
    nodes[11]->setNodeType (Y_SWITCH);
    nodes[12]->setNodeType (Y_SWITCH);
    nodes[14]->setNodeType (Y_SWITCH);
    createEdge(nodes[14], recursionPoints[3][0]); // t
    return;
  }

  if (blockType == NORMAL) {
    nodes[15] = new UCNode(15, X_SWITCH);
    createEdge(poles[3], nodes[15]);
    createEdge(nodes[10], nodes[15]);
    createEdge(nodes[15], nodes[11]);
    createEdge(nodes[15], nodes[12]);
  }

  if (recursionPointNumber == 2) {
    nodes[11]->setNodeType (Y_SWITCH);
    nodes[12]->setNodeType (Y_SWITCH);
    createEdge (nodes[14], recursionPoints[1][0]); // r
    createEdge (nodes[14], recursionPoints[3][0]); // t
    return;
  }

  nodes[13] = new UCNode (13, X_SWITCH);
  createEdge(nodes[11], nodes[13]);
  createEdge(nodes[12], nodes[13]);

  createEdge(nodes[14], recursionPoints[2][0]); // s
  createEdge (nodes[14], recursionPoints[3][0]); // t

  if (recursionPointNumber == 3) {
    createEdge(nodes[13], recursionPoints[1][0]); // r
    nodes[13]->setNodeType(Y_SWITCH);
    return;
  }
  createEdge(nodes[13], recursionPoints[0][0]); // q
  createEdge(nodes[13], recursionPoints[1][0]); // r
}

/*
 * creates a set of nodes from an index to an index of the same node type
 * @param from first index
 * @param to second index
 * @param nodeType node type
 */
void Block::createNodesFromTo(uint32_t from, uint32_t to, NodeType nodeType) {
  while (from <= to) {
    nodes[from] = new UCNode(from, nodeType);
    from++;
  }
}

/*
 * create an edge between two nodes
 * @param parent parent node
 * @param child child node
 */
void Block::createEdge(UCNode *parent, UCNode *child) {
  parent->addChild(child);
  child->addParent(parent);
}

/*
 * Get the recursion points
 * @return vector of recursion points
 */
std::vector<std::vector<UCNode *>> &Block::getRecursionPoints() { return recursionPoints; }

/*
 * get the size of the block
 * @return size of the block
 */
uint32_t Block::getSize() const { return (uint32_t) (nodes.size() + poles.size()); }

/*
 * set an input permutation bit for block edge-embedding
 * @param input the input bit
 * @param position the position within the vector
 */
void Block::setInputPermutationBit(uint32_t input, uint32_t position) {
  this->inputPermutation[input] = position;
}

/*
 * set an output permutation bit for block edge-embedding
 * @param output the output bit
 * @param position the position within the vector
 */
void Block::setOutputPermutationBit(uint32_t output, uint32_t position) {
  this->outputPermutation[output] = position;
}

// block edge-embedding
void Block::edgeEmbedding() {
  std::vector<uint32_t> helpVector = {inputPermutation[0], inputPermutation[1],
                                     inputPermutation[2], inputPermutation[3]};
  this->fillDummyPermutations(helpVector);
  if (this->blockType != RECURSION_BASE) {
    if (this->blockType != HEAD) {
      // UCNode 1
      if (helpVector[0] == 0 || helpVector[1] == 1) {
        nodes[1]->setControlBit(0);
      } else  {
        nodes[1]->setControlBit(1);
      }

      // UCNode 2
      if (helpVector[2] == 0 || helpVector[3] == 1) {
        nodes[2]->setControlBit(0);
      } else {
        nodes[2]->setControlBit(1);
      }

      if (poles.size() != 1) {

        // UCNode 3
        if (helpVector[0] == 0 || helpVector[1] == 0) {
          int tmpControlBit = nodes[3]->getNodeType() == Y_SWITCH ? 0 : 1;
          nodes[3]->setControlBit(tmpControlBit);
        } else {
          int tmpControlBit = nodes[3]->getNodeType() == Y_SWITCH ? 1 : 0;
          nodes[3]->setControlBit(tmpControlBit);
        }

        // UCNode 4
        if (helpVector[0] == 1 || helpVector[1] == 1) {
          nodes[4]->setControlBit(0);
        } else {
          nodes[4]->setControlBit(1);
        }

        // UCNode 5
        if (this->outputPermutation[0] == 1) {
          nodes[5]->setControlBit(1);
        } else {
          nodes[5]->setControlBit(0);
        }
        if (this->poles.size() == 2) {
          return;  // finished TAIL_2 block
        }
      } else {  // handle TAIL_1 block
        // UCNode 17
        if (inputPermutation[0] == 0 || inputPermutation[1] == 0) {
          nodes[17]->setControlBit(0);
        } else {
          nodes[17]->setControlBit(1);
        }
        return;  // finished TAIL_1 block
      }
    }

    uint32_t left7;  // first input of node 7
    uint32_t right8;  // right input of node 8
    if (this->blockType != HEAD && this->poles.size() != 3) {
      // determine left7
      if (this->nodes[3]->getControlBits()[0] == 0) {
        if (this->nodes[1]->getControlBits()[0] == 0) {
          left7 = this->inputPermutation[0];
        } else {
          left7 = this->inputPermutation[1];
        }
      } else {
        if (this->nodes[2]->getControlBits()[0] == 0) {
          left7 = this->inputPermutation[2];
        } else {
          left7 = this->inputPermutation[3];
        }
      }
    }
    if(this->blockType != HEAD) {
      // determine right8
      if (this->nodes[4]->getControlBits()[0] == 0) {
        if (this->nodes[2]->getControlBits()[0] == 0) {
          right8 = this->inputPermutation[3];
        } else {
          right8 = this->inputPermutation[2];
        }
      } else {
        if (this->nodes[1]->getControlBits()[0] == 0) {
          right8 = this->inputPermutation[1];
        } else {
          right8 = this->inputPermutation[0];
        }
      }
    } else if (this->blockType == HEAD) {  // HEAD block
      left7 = DUMMY;
      right8 = DUMMY;
    }

    bool rightUpperNodeToLowerNode = false;  // a pole[0] or pole[1] have a path over node 8 to pole[2] or pole[3]
    bool leftUpperNodeToLowerNode = false;  // a pole[0] or pole[1] have a path over node 7 to pole[2] or pole[3]

    // UCNode 6
    if (this->blockType == HEAD) {
      if (outputPermutation[0] > 3 && outputPermutation[0] != DUMMY) {
        if (outputPermutation[1] > 3 && outputPermutation[1] != DUMMY) {
          if (outputPermutation[0] + outputPermutation[3] == 9
              || outputPermutation[0] + outputPermutation[3] == 13
              || (outputPermutation[1] + outputPermutation[2] == 9 && outputPermutation[2] > 3)
              || outputPermutation[1] + outputPermutation[2] == 13) {
            nodes[6]->setControlBit(1);
          } else {
            nodes[6]->setControlBit(0);
          }
        } else if ((outputPermutation[0] + outputPermutation[3] == 9) ||
            outputPermutation[0] + outputPermutation[3] == 13) {
          nodes[6]->setControlBit(1);
        } else {
          nodes[6]->setControlBit(0);
        }
      } else if (outputPermutation[1] > 3 && outputPermutation[1] != DUMMY) {
        if (outputPermutation[1] + outputPermutation[3] == 9 ||
            outputPermutation[1] + outputPermutation[3] == 13) {
          nodes[6]->setControlBit(0);
        } else {
          nodes[6]->setControlBit(1);
        }
      } else {
        nodes[6]->setControlBit(0);
      }
    } else {
      if (this->outputPermutation[0] == 2 || this->outputPermutation[0] == 3) {
        if (poles.size() == 3) {  // TAIL_3
          rightUpperNodeToLowerNode = true;
          nodes[6]->setControlBit(0);
        } else if (left7 == DUMMY) {
          leftUpperNodeToLowerNode = true;
          nodes[6]->setControlBit(0);
        } else {  // right8 must be a DUMMY
          rightUpperNodeToLowerNode = true;
          nodes[6]->setControlBit(1);
        }
      }
      if (this->outputPermutation[1] == 2 || this->outputPermutation[1] == 3) {
        if (poles.size() == 3) {  // TAIL_3
          rightUpperNodeToLowerNode = true;
          nodes[6]->setControlBit(1);
        } else if (left7 == DUMMY && !leftUpperNodeToLowerNode) {
          leftUpperNodeToLowerNode = true;
          nodes[6]->setControlBit(1);
        } else {
          rightUpperNodeToLowerNode = true;
          nodes[6]->setControlBit(0);
        }
      }
    }

    if (this->blockType != HEAD) {  // nodes 7 and 8 are REVERSE_Y_SWITCH
      // UCNode 8
      if (rightUpperNodeToLowerNode) {
        nodes[8]->setControlBit(0);
      } else {
        nodes[8]->setControlBit(1);
      }

      if (poles.size() == 3) {  // TAIL_3
        // UCNode 16
        if (rightUpperNodeToLowerNode || right8 == 2) {
          nodes[16]->setControlBit(1);
        } else {
          nodes[16]->setControlBit(0);
        }
        return;  // finished TAIL_3
      }

      // UCNode 7
      if (leftUpperNodeToLowerNode) {
        if (this->blockType != TAIL) {
          nodes[7]->setControlBit(0);
        } else {
          nodes[7]->setControlBit(1);
        }
      } else {
        if (this->blockType != TAIL) {
          nodes[7]->setControlBit(1);
        } else {
          nodes[7]->setControlBit(0);
        }
      }
    }  // now, HEAD is important

    uint32_t left6; // left input of node 6
    if (this->blockType == HEAD) {
      left6 = outputPermutation[0];
    } else if (nodes[5]->getControlBits()[0] == 0) {
      left6 = outputPermutation[0];
    } else {
      left6 = DUMMY;
    }

    uint32_t right7; // right input of node 7
    uint32_t left8;  // left input of node 8
    if (nodes[6]->getControlBits()[0] == 0) {
      right7 = left6;
      left8 = outputPermutation[1];
    } else {
      right7 = outputPermutation[1];
      left8 = left6;
    }

    uint32_t left9;  // left input of node 9
    uint32_t right9;  // right input of node 9

    if (this->blockType == HEAD) {
      left9 = right7;
      right9 = left8;
    } else {
      if (this->blockType == TAIL) {
        left9 = nodes[7]->getControlBits()[0] == 0 ? left7 : right7;
      } else {
        left9 = nodes[7]->getControlBits()[0] == 0 ? right7 : left7;
      }
      right9 = nodes[8]->getControlBits()[0] == 0 ? left8 : right8;
    }

    // UCNode 9
    if (left9 == 2) {
      nodes[9]->setControlBit(1);
    } else if (left9 == 3) {
      nodes[9]->setControlBit(0);
    } else if (right9 == 2) {
      nodes[9]->setControlBit(0);
    } else {
      nodes[9]->setControlBit(1);
    }

    // UCNode 10
    if (outputPermutation[2] == 3) {
      nodes[10]->setControlBit(1);
    } else {
      nodes[10]->setControlBit(0);
    }

    if (this->blockType == TAIL) {
      return;  // finished TAIL_4
    }

    // right output of 10 can only be from the third pole. If the third pole goes to pole 4, the output is DUMMY
    uint32_t rightOutput10 = nodes[10]->getControlBits()[0] == 1 ? DUMMY : outputPermutation[2];
    uint32_t right11;  // right input of node 11
    uint32_t left11;  // left input of node 11
    uint32_t right12;  // right input of node 12
    uint32_t left12;  // left input of node 12

    if (this->blockType == HEAD) {
      right11 = outputPermutation[3];
      left11 = right7;
      right12 = left8;
      left12 = rightOutput10;
    } else {  // NORMAL block
      left11 = nodes[7]->getControlBits()[0] == 1 ? right7 : left7;
      right12 = nodes[8]->getControlBits()[0] == 1 ? left8 : right8;

      // outputs (4, 5) and (6, 7) shall not be at the same side!
      if (rightOutput10 + left11 == 9 || rightOutput10 + left11 == 13 ||
          outputPermutation[3] + right12 == 9 || outputPermutation[3] + right12 == 13) {
        nodes[15]->setControlBit(0);
        right11 = outputPermutation[3];
        left12 = rightOutput10;
      } else {
        nodes[15]->setControlBit(1);
        right11 = rightOutput10;
        left12 = outputPermutation[3];
      }
    }

    // UCNode 11
    if (left11 == 4 || left11 == 5) {
      nodes[11]->setControlBit(0);
    } else if (right11 == 4 || right11 == 5 || left11 == 6 || left11 == 7) {
      nodes[11]->setControlBit(1);
    } else {
      nodes[11]->setControlBit(0);
    }

    // UCNode 12
    if (right12 == 6 || right12 == 7) {
      nodes[12]->setControlBit(0);
    } else if (left12 == 6 || left12 == 7 || right12 == 4 || right12 == 5) {
      nodes[12]->setControlBit(1);
    } else {
      nodes[12]->setControlBit(0);
    }

    uint32_t left13 = nodes[11]->getControlBits()[0] == 1 ? right11 : left11;
    uint32_t right13 = nodes[12]->getControlBits()[0] == 1 ? right12 : left12;
    uint32_t left14 = nodes[11]->getControlBits()[0] == 1 ? left11 : right11;
    uint32_t right14 = nodes[12]->getControlBits()[0] == 1 ? left12 : right12;

    // UCNode 13
    if (left13 == 4 || right13 == 5) {
      nodes[13]->setControlBit(0);
    } else {
      nodes[13]->setControlBit(1);
    }

    // UCNode 14
    if (left14 == 6 || right14 == 7) {
      nodes[14]->setControlBit(0);
    } else {
      nodes[14]->setControlBit(1);
    }

  } else { // handle recursion base
    if (poles.
        size()
        == 3) {  // handle recursion base with 3 poles
      // nothing to do here
    } else if (poles.
        size()
        == 4) {  // handle recursion base with 4 poles
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

/*
 * replace dummy values of the input vector with concrete values. This step makes the edge-embedding easier
 * @param helpVector the current input vector
 */
void Block::fillDummyPermutations(std::vector<uint32_t> &helpVector) {
  if(this->blockType != HEAD && this->blockType != RECURSION_BASE) {
    for (int j = 0; j < helpVector.size(); j++) {
      int currentValue = helpVector[j];
      if (currentValue == DUMMY) {
        for (int i = 0; i < helpVector.size(); i++) {
          bool found = false;
          for (int k = 0; k < helpVector.size(); k++) {
            if (helpVector[k] == i) {
              found = true;
              break;
            }
          }
          if (!found) {
            helpVector[j] = i;
            break;
          }
        }
      }
    }
  }
}

/*
 * test, if the edge-embedding algorithm worked for this block
 */
bool Block::validateEdgeEmbedding(int eugNumber) {
  int maxDepth = 8;
  if(blockType != HEAD) {
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
  if(blockType != TAIL) {
    for(int i = 1; i < 8; i++) {
      int index = 1;
      if(blockType == HEAD) {
        index = 0;
      }
      uint32_t targetPole = DUMMY;
      for (int j = 0; j < outputPermutation.size(); j++) {
        if (outputPermutation[j] == i) {
          targetPole = j;
          break;
        }
      }
      if(targetPole == DUMMY) {
        continue;
      }
      UCNode *lastNode;
      if(i < 4) {
        lastNode = poles[i];
      } else {
        lastNode = recursionPoints[i - 4][index];
      }
      int index2 = eugNumber;
      if(lastNode->getNodeType() == RECURSION_POINT && i > 3) {
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
          if(nextNode->getControlBits()[0] == 0) {
            if(nextNode->getChildren()[0] == lastNode) {
              nextNode = nextNode->getParents()[0];
            } else {
              nextNode = nextNode->getParents()[1];
            }
          } else {
            if(nextNode->getChildren()[0] == lastNode) {
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
void Block::printEmbedding() {
  std::cout << std::endl << "Input Permutation:\t(" << inputPermutation[0] << ", "
            << inputPermutation[1] << ", " << inputPermutation[2] << ", " << inputPermutation[3]
            << ")" << std::endl;
  std::cout << "Output Permutation:\t(" << outputPermutation[0] << ", " << outputPermutation[1] << ", "
            << outputPermutation[2] << ", " << outputPermutation[3] << ")" << std::endl;

  for (auto node : nodes) {
    std:: cout << "Node " << node.first << ":\t" << node.second->getControlBits()[0] << std::endl;
  }
  std::cout << std::endl;
}

vector<UCNode *> Block::getPoles() { return this->poles; }

