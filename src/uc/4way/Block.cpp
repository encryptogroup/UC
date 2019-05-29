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

#include <utility>
#include <iostream>
#include "Block.h"
#include "Block4.h"
#include "Block2.h"

/**
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

/**
 * create an edge between two nodes
 * @param parent parent node
 * @param child child node
 */
void Block::createEdge(UCNode *parent, UCNode *child) {
  parent->addChild(child);
  child->addParent(parent);
}

/**
 * Get the recursion points
 * @return vector of recursion points
 */
std::vector<std::vector<UCNode *>> &Block::getRecursionPoints() { return recursionPoints; }

/**
 * get the size of the block
 * @return size of the block
 */
uint32_t Block::getSize() const { return (uint32_t) (nodes.size() + poles.size()); }

/**
 * set an input permutation bit for block edge-embedding
 * @param input the input bit
 * @param position the position within the vector
 */
void Block::setInputPermutationBit(uint32_t input, uint32_t position) {
  this->inputPermutation[input] = position;
}

/**
 * set an output permutation bit for block edge-embedding
 * @param output the output bit
 * @param position the position within the vector
 */
void Block::setOutputPermutationBit(uint32_t output, uint32_t position) {
  this->outputPermutation[output] = position;
}

/**
 * Return the poles of the block
 * @return poles of the block
 */
vector<UCNode *> Block::getPoles() { return this->poles; }

/**
 * creates a block with Valiant's 2 or 4-way split construcion or Zhao's 4-way split construction
 * or our hybrid construction (either 2-way or one of the 4-way constructions, depending on k)
 * @param poles list of poles
 * @param poleIndex index oft he first ple in the block
 * @param recursionSteps list of all recursion steps
 * @param position position in current EUG
 * @param blockType type of the block
 * @param recursionPoints list of recursion points of the block
 * @param k block with k-way split (2 or 4)
 * @param zhao true if Zhao's construction shall be used
 * @return Block under the given conditions
 */
Block* createBlock(std::vector<UCNode*> poles, uint32_t poleIndex, std::vector<uint32_t> recursionSteps, uint32_t position,
                   BlockType blockType, std::vector<std::vector<UCNode*>> recursionPoints, uint32_t k, bool zhao) {
  if (k == 4) {
    return new Block4(std::move(poles), poleIndex, std::move(recursionSteps), position, blockType,
                      std::move(recursionPoints), zhao);
  } else { // k = 2
    return new Block2(std::move(poles), poleIndex, std::move(recursionSteps), position, blockType,
                      std::move(recursionPoints));
  }
}
