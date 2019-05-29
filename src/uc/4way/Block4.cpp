/**
 \file 		Block4.cpp
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
 \brief		Implementation of the Block4 class part of Valiant's 4-way Split Construction.
 */


#include <utility>
#include "Block4.h"

/**
 * Construct a Block4
 * @param poles vector of all poles of the uc
 * @param poleIndex index of the first pole occurring in this block
 * @param recursionSteps vector of recursion step numbers
 * @param position position within the recursion step chain
 * @param blockType type of the block
 * @param recursionPoints list of recursion points of the block
 * @param true if Zhao's 4-way split block is used
 */
Block4::Block4(std::vector<UCNode*> poles, uint32_t poleIndex, std::vector<uint32_t> recursionSteps, uint32_t position,
		BlockType blockType, std::vector<std::vector<UCNode*>> recursionPoints, bool zhao) {
	this->zhao = zhao;
	this->blockType = blockType;
	this->recursionSteps = std::move(recursionSteps);
	this->position = position;
	this->recursionPoints = std::move(recursionPoints);

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
	case NORMAL:
		if (zhao) {
			createNormalBlock();
		} else {
			createNormalBlockValiant();
		}
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

/**
 * Deconstruct block
 */
Block4::~Block4() {}

/**
 * Create a head block
 */
void Block4::createHeadBlock() {
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

	this->createLowerPermutationNetwork();
}

/**
 * create a normal block with Valiant's construction
 */
void Block4::createNormalBlockValiant() {
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

  this->createLowerPermutationNetwork();

}

/**
 * creates a normal block with Zhao's construction
 */
void Block4::createNormalBlock() {
	// create normal block
	createNodesFromTo(1, 13, X_SWITCH);

	createEdge(recursionPoints[0][0], nodes[1]);
	createEdge(recursionPoints[1][0], nodes[1]);
	createEdge(recursionPoints[2][0], nodes[2]);
	createEdge(recursionPoints[3][0], nodes[2]);

	createEdge(nodes[1], nodes[3]);
	createEdge(nodes[1], nodes[4]);
	createEdge(nodes[2], nodes[3]);
	createEdge(nodes[2], nodes[4]);

	createEdge(nodes[3], nodes[6]);
	createEdge(nodes[3], nodes[7]);
	createEdge(nodes[4], poles[0]);
	createEdge(poles[0], nodes[5]);
	createEdge(nodes[4], nodes[5]);

	createEdge(nodes[5], nodes[6]);
	createEdge(nodes[5], nodes[7]);

	createEdge(nodes[6], nodes[10]);
	createEdge(nodes[6], nodes[11]);
	createEdge(nodes[7], poles[1]);
	createEdge(poles[1], nodes[8]);
	createEdge(nodes[7], nodes[8]);

	createEdge(nodes[8], poles[2]);
	createEdge(poles[2], nodes[9]);
	createEdge(nodes[8], nodes[9]);

	createEdge(nodes[9], nodes[10]);
	createEdge(nodes[9], nodes[11]);

	createEdge(nodes[10], nodes[13]);
	createEdge(nodes[11], poles[3]);
	createEdge(poles[3], nodes[12]);
	createEdge(nodes[11], nodes[12]);

	createEdge(nodes[12], nodes[13]);

	int recursionPointNumber = 4;
	if (recursionPoints[2].size() == 1) {
		recursionPointNumber = 1;
	} else if (recursionPoints[1].size() == 1) {
		recursionPointNumber = 2;
	} else if (recursionPoints[3].size() == 1) {
		recursionPointNumber = 3;
	}

	if (recursionPointNumber == 1) {
		nodes[13]->setNodeType(Y_SWITCH);
		nodes[10]->setNodeType(Y_SWITCH);
		nodes[12]->setNodeType(Y_SWITCH);
		createEdge(nodes[13], recursionPoints[0][1]); // q
		return;
	}

	nodes[14] = new UCNode(14, X_SWITCH);
	createEdge(nodes[10], nodes[14]);
	createEdge(nodes[12], nodes[14]);

	if (recursionPointNumber == 2) {
		nodes[13]->setNodeType(Y_SWITCH);
		nodes[14]->setNodeType(Y_SWITCH);
		createEdge(nodes[13], recursionPoints[0][1]); // q
		createEdge(nodes[14], recursionPoints[2][1]); // s
		return;
	}

	createEdge(nodes[13], recursionPoints[0][1]); // q
	createEdge(nodes[13], recursionPoints[1][1]); // r

	if (recursionPointNumber == 3) {
		nodes[14]->setNodeType(Y_SWITCH);
		createEdge(nodes[14], recursionPoints[2][1]); // s
		return;
	}
	createEdge(nodes[14], recursionPoints[2][1]); // s
	createEdge(nodes[14], recursionPoints[3][1]); // t
}

/**
 * creates a tail block
 * @param numberOfRemainingPoles number of poles in the tail block
 */
void Block4::createTailBlock(uint32_t numberOfRemainingPoles) {
	switch (numberOfRemainingPoles) {
	case 1:createEdge(recursionPoints[0][0], poles[0]);  // q
	break;
	case 2:nodes[35] = new UCNode(35, X_SWITCH);
	nodes[36] = new UCNode(36, Y_SWITCH);
	createEdge(recursionPoints[0][0], nodes[35]);  // q
	createEdge(recursionPoints[2][0], nodes[35]);  // s

	createEdge(nodes[35], poles[0]);
	createEdge(poles[0], nodes[36]);
	createEdge(nodes[35], nodes[36]);
	createEdge(nodes[36], poles[1]);
	break;
	case 3:createTail3Block();
	break;
	case 4:createTail4Block();
	break;
	default:break;
	}
}

/**
 * creates a tail block with 4 poles
 */
void Block4::createTail4Block() {
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

/**
 * creates a tail block with 3 nodes
 */
void Block4::createTail3Block() {
	nodes[5] = new UCNode(5, X_SWITCH);
	createNodesFromTo(30, 32, X_SWITCH);
	createNodesFromTo(33, 34, Y_SWITCH);

	createEdge(recursionPoints[0][0], nodes[30]);
	createEdge(recursionPoints[1][0], nodes[30]);

	createEdge(nodes[30], nodes[32]);
	createEdge(nodes[30], nodes[31]);
	createEdge(recursionPoints[2][0], nodes[31]);
	createEdge(nodes[31], nodes[32]);
	createEdge(nodes[32], nodes[34]);
	createEdge(nodes[32], poles[0]);
	createEdge(poles[0], nodes[5]);
	createEdge(nodes[31], nodes[5]);
	createEdge(nodes[5], nodes[33]);
	createEdge(nodes[5], poles[1]);
	createEdge(poles[1], nodes[33]);
	createEdge(nodes[33], nodes[34]);
	createEdge(nodes[34], poles[2]);
}

/**
 * creates the lower permutation network of the block
 */
void Block4::createLowerPermutationNetwork() {
	int recursionPointNumber = 4;
	int index = blockType == HEAD ? 0 : 1;
	if (recursionPoints[2].size() == index) {
		recursionPointNumber = 1;
	} else if (recursionPoints[1].size() == index) {
		recursionPointNumber = 2;
	} else if (recursionPoints[3].size() == index) {
		recursionPointNumber = 3;
	}

	if (recursionPointNumber == 1) {
		if (blockType == NORMAL) {
			createEdge(poles[3], nodes[11]);
			createEdge(nodes[10], nodes[12]);
			createEdge(nodes[8], nodes[12]);
		}
		nodes[11]->setNodeType(Y_SWITCH);
		nodes[12]->setNodeType(Y_SWITCH);
		nodes[14]->setNodeType(Y_SWITCH);
		createEdge(nodes[11], nodes[14]);
		createEdge(nodes[12], nodes[14]);
		createEdge(nodes[14], recursionPoints[0][index]); // q
		return;
	}

	if (blockType == NORMAL) {
		nodes[15] = new UCNode(15, X_SWITCH);
		createEdge(poles[3], nodes[15]);
		createEdge(nodes[10], nodes[15]);
		createEdge(nodes[15], nodes[11]);
		createEdge(nodes[15], nodes[12]);
		createEdge(nodes[8], nodes[12]);
	}
	nodes[13] = new UCNode(13, X_SWITCH);
	createEdge(nodes[11], nodes[13]);
	createEdge(nodes[12], nodes[13]);
	createEdge(nodes[11], nodes[14]);
	createEdge(nodes[12], nodes[14]);

	if (recursionPointNumber == 2) {
		nodes[13]->setNodeType(Y_SWITCH);
		nodes[14]->setNodeType(Y_SWITCH);
		createEdge(nodes[13], recursionPoints[0][index]); // q
		createEdge(nodes[14], recursionPoints[2][index]); // s
		return;
	}

	createEdge(nodes[13], recursionPoints[0][index]); // q
	createEdge(nodes[13], recursionPoints[1][index]); // r

	if (recursionPointNumber == 3) {
		createEdge(nodes[14], recursionPoints[2][index]); // s
		nodes[14]->setNodeType(Y_SWITCH);
		return;
	}
	createEdge(nodes[14], recursionPoints[2][index]); // s
	createEdge(nodes[14], recursionPoints[3][index]); // t
}

/**
 * block edge-embedding
 * @param mapping mapping for Zhao
 */
void Block4::edgeEmbedding(const std::map<uint32_t, uint32_t>& mapping) {

	// number of recursion points at tail / head of the block
	int nrRecursionPoints = 0;
	if (blockType != TAIL) {
		for (int i = 0; i < 4; i++) {
			if (blockType == NORMAL && recursionPoints[i].size() == 2) {
				nrRecursionPoints++;
			} else if (blockType == HEAD && recursionPoints[i].size() == 1) {
				nrRecursionPoints++;
			}
		}
	}

	if (poles.size() == 1) return;


	if (blockType == NORMAL && zhao) {
		// edge embedding of Zhao
		uint32_t element = inputPermutation[0] * 10000000 + inputPermutation[1] * 1000000 + inputPermutation[2] * 100000 + inputPermutation[3] * 10000 +
				outputPermutation[0] * 1000 + outputPermutation[1] * 100 + outputPermutation[2] * 10 + outputPermutation[3];
		auto search = mapping.find(element);
		uint32_t binary = 8192;
		if( search != mapping.end() ){
			for(uint32_t k = 1; k < 15; ++k){
				if (!(recursionPoints[2].size() == 1 && k == 14)) { //nodes[14] does not exist when there is only one recursion point
					nodes[k]->setControlBit(search->second / binary % 2);
					binary /= 2;
				}
			}
		}
		else{
			std::cerr << "Not found, problem with Zhao's lookup table! " <<
								inputPermutation[0] << inputPermutation[1] << inputPermutation[2] << inputPermutation[3] <<
								outputPermutation[0] << outputPermutation[1] << outputPermutation[2] << outputPermutation[3] << " " << endl;
		}
		return;
	}

	std::vector<uint32_t> helpVector = {inputPermutation[0], inputPermutation[1],
			inputPermutation[2], inputPermutation[3]};
	this->fillDummyPermutations(helpVector);

	if (this->blockType != RECURSION_BASE && (this->blockType != TAIL || this->poles.size() == 4)) {
		if (this->blockType != HEAD) {
			// UCNode 1
			if (helpVector[0] == 0 || helpVector[1] == 1) {
				nodes[1]->setControlBit(0);
			} else {
				nodes[1]->setControlBit(1);
			}

			// UCNode 2
			if (helpVector[2] == 0 || helpVector[3] == 1) {
				nodes[2]->setControlBit(0);
			} else {
				nodes[2]->setControlBit(1);
			}


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
		}

		uint32_t left7;  // first input of node 7
		uint32_t right8;  // right input of node 8
		if (this->blockType != HEAD) {
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
					if ((outputPermutation[0] + outputPermutation[3] == 9 && outputPermutation[0] != DUMMY && outputPermutation[3] != DUMMY)
							|| (outputPermutation[0] + outputPermutation[3] == 13 && outputPermutation[0] != DUMMY && outputPermutation[3] != DUMMY)
							|| (outputPermutation[1] + outputPermutation[2] == 9 && outputPermutation[2] > 3 && outputPermutation[1] != DUMMY && outputPermutation[2] != DUMMY)
							|| outputPermutation[1] + outputPermutation[2] == 13 && outputPermutation[1] != DUMMY && outputPermutation[2] != DUMMY) {
						nodes[6]->setControlBit(1);
					} else {
						nodes[6]->setControlBit(0);
					}
				} else if ((outputPermutation[0] + outputPermutation[3] == 9 && outputPermutation[0] != DUMMY && outputPermutation[3] != DUMMY) ||
						outputPermutation[0] + outputPermutation[3] == 13 && outputPermutation[0] != DUMMY && outputPermutation[3] != DUMMY) {
					nodes[6]->setControlBit(1);
				} else {
					nodes[6]->setControlBit(0);
				}
			} else if (outputPermutation[1] > 3 && outputPermutation[1] != DUMMY) {
				if ((outputPermutation[1] + outputPermutation[3] == 9 && outputPermutation[1] != DUMMY && outputPermutation[3] != DUMMY) ||
                        (outputPermutation[1] + outputPermutation[3] == 13 && outputPermutation[1] != DUMMY && outputPermutation[3] != DUMMY)) {
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

		if (this->blockType == HEAD || nrRecursionPoints < 3) {
			right11 = outputPermutation[3];
			left11 = right7;
			right12 = left8;
			left12 = rightOutput10;
		} else {  // NORMAL block with at least 3 recursion points
			left11 = nodes[7]->getControlBits()[0] == 1 ? right7 : left7;
			right12 = nodes[8]->getControlBits()[0] == 1 ? left8 : right8;

			// outputs (4, 5) and (6, 7) shall not be at the same side!
			if ((rightOutput10 + left11 == 9 && rightOutput10 != DUMMY && left11 != DUMMY) ||
                    (rightOutput10 + left11 == 13 && rightOutput10 != DUMMY && left11 != DUMMY) ||
                    (outputPermutation[3] + right12 == 9 && outputPermutation[3] != DUMMY && right12 != DUMMY)||
			        (outputPermutation[3] + right12 == 13 && outputPermutation[3] != DUMMY && right12 != DUMMY)) {
				nodes[15]->setControlBit(0);
				right11 = outputPermutation[3];
				left12 = rightOutput10;
			} else {
				nodes[15]->setControlBit(1);
				right11 = rightOutput10;
				left12 = outputPermutation[3];
			}
		}

		uint32_t left13, right13, left14, right14;

		// UCNode 11 and 12
		if(nrRecursionPoints != 1) {
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

			left13 = nodes[11]->getControlBits()[0] == 1 ? right11 : left11;
			right13 = nodes[12]->getControlBits()[0] == 1 ? right12 : left12;
			left14 = nodes[11]->getControlBits()[0] == 1 ? left11 : right11;
			right14 = nodes[12]->getControlBits()[0] == 1 ? left12 : right12;
		} else {
			// UCNode 11
			if (left11 == 4) {
				nodes[11]->setControlBit(0);
				left14 = left11;
			} else {
				nodes[11]->setControlBit(1);
				left14 = right11;
			}
			// UCNode 12
			if(left12 == 4) {
				nodes[12]->setControlBit(0);
				right14 = left12;
			} else {
				nodes[12]->setControlBit(1);
				right14 = right12;
			}
		}

		// UCNode 13
		if (nrRecursionPoints != 1) {
			if (left13 == 4  || right13 == 5) {
				nodes[13]->setControlBit(0);
			} else {
				nodes[13]->setControlBit(1);
			}
		}

		// UCNode 14
		if (nrRecursionPoints != 1) {
			if ((nrRecursionPoints == 4 && (left14 == 6 || right14 == 7)) || (nrRecursionPoints < 4 && left14 == 6)) {
				nodes[14]->setControlBit(0);
			} else {
				nodes[14]->setControlBit(1);
			}
		} else {
			if (left14 == 4) {
				nodes[14]->setControlBit(0);
			} else {
				nodes[14]->setControlBit(1);
			}
		}

	} else if (this->blockType == RECURSION_BASE){ // handle recursion base
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
	} else if (poles.size() == 3) {
		int left31, left32, right32, left34;
		// UCNode 30
		if (inputPermutation[0] == 0 || inputPermutation[0] == 2 || inputPermutation[1] == 1) {
			nodes[30]->setControlBit(0);
			left31 = inputPermutation[1];
			left32 = inputPermutation[0];
		} else {
			nodes[30]->setControlBit(1);
			left31 = inputPermutation[0];
			left32 = inputPermutation[1];
		}

		// UCNode 31
		if (left31 == 0 || left31 == 2 || inputPermutation[2] == 1) {
			nodes[31]->setControlBit(0);
			right32 = left31;
		} else {
			nodes[31]->setControlBit(1);
			right32 = inputPermutation[2];
		}

		// UCNode 32
		if (left32 == 2 || right32 == 0) {
			nodes[32]->setControlBit(0);
			left34 = left32;
		} else {
			nodes[32]->setControlBit(1);
			left34 = right32;
		}

		// UCNode 5
		if (outputPermutation[0] == 1) {
			nodes[5]->setControlBit(1);
		} else {
			nodes[5]->setControlBit(0);
		}

		// UCNode 33
		if (outputPermutation[1] == 2) {
			nodes[33]->setControlBit(1);
		} else {
			nodes[33]->setControlBit(0);
		}

		// UCNode 34
		if (left34 == 2) {
			nodes[34]->setControlBit(0);
		} else {
			nodes[34]->setControlBit(1);
		}

	} else { // TAIL 2
		// UCNode 35
		if (inputPermutation[0] == 0 || inputPermutation[2] == 1) {
			nodes[35]->setControlBit(0);
		} else {
			nodes[35]->setControlBit(1);
		}

		// UCNode 36
		if(outputPermutation[0] == 1) {
			nodes[36]->setControlBit(0);
		} else {
			nodes[36]->setControlBit(1);
		}
	}
}

/**
 * replace dummy values of the input vector with concrete values. This step makes the edge-embedding easier
 * @param helpVector the current input vector
 */
void Block4::fillDummyPermutations(std::vector<uint32_t> &helpVector) {
	if (this->blockType != HEAD && this->blockType != RECURSION_BASE) {
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

/**
 * test, if the edge-embedding algorithm worked for this block
 * @param eugNumber left or right EUG
 */
bool Block4::validateEdgeEmbedding(int eugNumber) {
	if (this->blockType == RECURSION_BASE && this->poles.size() == 3) return true;
	int maxDepth = 8;
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
		for (int i = 1; i < 8; i++) {
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
			if (i < 4) {
				lastNode = poles[i];
			} else {
				lastNode = recursionPoints[i - 4][index];
			}
			int index2 = eugNumber;
			if (lastNode->getNodeType() == RECURSION_POINT && i > 3) {
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

/**
 * Prints the embedding of the block
 */
void Block4::printEmbedding() {
	std::cout << std::endl << "Input Permutation:\t(" << inputPermutation[0] << ", "
			<< inputPermutation[1] << ", " << inputPermutation[2] << ", " << inputPermutation[3]
																							  << ")" << std::endl;
	std::cout << "Output Permutation:\t(" << outputPermutation[0] << ", " << outputPermutation[1] << ", "
			<< outputPermutation[2] << ", " << outputPermutation[3] << ")" << std::endl;

	std::cout << this->blockType << " " << this->position << std::endl;
	std::cout << this->recursionPoints.size() << std::endl;

	for (auto node : nodes) {
		std::string temp = "-";
		if(node.second->getNodeType() != I_SWITCH && node.second->getNodeType() != REVERSE_Y_SWITCH) {
			temp = std::to_string(node.second->getControlBits()[0]);
		}
		std::cout << "Node " << node.first << ":\t" << temp << std::endl;
	}
	std::cout << std::endl;
}

