/**
 \file 		Block4.h
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
 \brief		Header of the Block4 class part of Valiant's 4-way Split Construction.
 */

#ifndef VALIANTUC_BLOCK4_H
#define VALIANTUC_BLOCK4_H


#include <cstdint>
#include <fstream>
#include <vector>
#include <math.h>
#include "UCNode.h"
#include "BlockType.h"
#include "Block.h"

class Block4 : public Block {

public:
    Block4(std::vector<UCNode*> poles, uint32_t poleIndex, std::vector<uint32_t> recursionSteps, uint32_t position,
           BlockType blockType, std::vector<std::vector<UCNode*>> recursionPoints, bool zhao);
    ~Block4();

    void edgeEmbedding(const std::map<uint32_t, uint32_t>& mapping) override;
    void printEmbedding() override;
    bool validateEdgeEmbedding(int eugNumber) override;

protected:
    void createTail4Block();
    void createNormalBlockValiant();
    void createNormalBlock();
    void createHeadBlock();
    void createTailBlock(uint32_t numberOfRemainingPoles);
    void createTail3Block();
    void createLowerPermutationNetwork();
    void fillDummyPermutations(std::vector<uint32_t>&);

    bool zhao;
};


#endif //VALIANTUC_BLOCK4_H
