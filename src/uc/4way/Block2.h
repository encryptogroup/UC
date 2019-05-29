/**
 \file 		Block2.h
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
 \brief		Header of the Block2 class part of Valiant's 4-way Split Construction.
 */


#ifndef VALIANTUC_BLOCK2_H
#define VALIANTUC_BLOCK2_H

#include <vector>
#include <map>
#include <stdint.h>
#include <algorithm>
#include "UCNode.h"
#include "NodeType.h"
#include "BlockType.h"
#include "Block.h"

class Block2 : public Block {
 public:
  Block2(std::vector<UCNode*> poles, uint32_t poleIndex, std::vector<uint32_t> recursionSteps, uint32_t position,
         BlockType blockType, std::vector<std::vector<UCNode*>> recursionPoints);
  ~Block2();

  void edgeEmbedding(const std::map<uint32_t, uint32_t>& mapping) override;
  void printEmbedding() override;
  bool validateEdgeEmbedding(int eugNumber) override;

};

#endif //VALIANTUC_BLOCK2_H
