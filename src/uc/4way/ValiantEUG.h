/**
 \file 		ValiantEUG.h
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
 \brief		Header of the ValiantEUG class part of Valiant's 4-way Split Construction.
 */

#ifndef VALIANTUC4WAYSPLIT_VALIANTEUG_H
#define VALIANTUC4WAYSPLIT_VALIANTEUG_H

#include <vector>
#include "Block.h"
#include "UCNode.h"
#include "NodeType.h"
#include "../../Gamma/gamma/gamma2.h"
#include "../../Gamma/gamma/gamma1.h"
#include "../../Gamma/util/read_SHDL.h"

class ValiantEUG {
 public:
  ValiantEUG(const std::vector<UCNode*> &poles, int eugNum, int k, bool zhao, std::vector<uint64_t>& hybrid_choice);
  ~ValiantEUG();

  ValiantEUG* goToChildrenUC(std::vector<uint32_t> recursionPositions);

  uint32_t getSize() const;

  void edgeEmbedding(DAG_Gamma1 *graph, DAG_Gamma2 *graphG2, const std::map<uint32_t, uint32_t> & mapping);

  std::vector<Block*> getBlocks();
  std::vector<ValiantEUG*> getChildren();

  int getK() const;

 private:
  std::vector<Block*> blocks;
  std::vector<ValiantEUG*> children;

  int k;
  int eugNum;
  bool hybrid;

  ValiantEUG(const std::vector<UCNode*> &poles, std::vector<uint32_t> recursionSteps, int eugNum, int k, bool hybrid, bool zhao, std::vector<uint64_t>& hybrid_choice);
  void edgeEmbedding (DAG_Gamma1 *graph, const std::map<uint32_t, uint32_t>& mapping);

  void constructUC(const std::vector<UCNode *> &poles, std::vector<uint32_t> recursionSteps, bool zhao, std::vector<uint64_t>& hybrid_choice);
};

#endif //VALIANTUC4WAYSPLIT_VALIANTEUG_H
