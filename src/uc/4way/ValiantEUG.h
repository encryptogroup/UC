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

class ValiantEUG {
 public:
  ValiantEUG(const std::vector<UCNode*> &poles, int eugNum);
  ~ValiantEUG();

  ValiantEUG* goToChildrenUC(std::vector<uint32_t> recursionPositions);

  uint32_t getSize() const;

  void edgeEmbedding(DAG_Gamma1 *graph, DAG_Gamma2 *graphG2);

  std::vector<Block*> getBlocks();
  std::vector<ValiantEUG*> getChildren();

 private:
  std::vector<Block*> blocks;
  std::vector<ValiantEUG*> children;
  int eugNum;

  ValiantEUG(const std::vector<UCNode*> &poles, std::vector<uint32_t> recursionSteps, int eugNum);
  void constructUC(const std::vector<UCNode*> &poles, std::vector<uint32_t> recursionSteps);
  void edgeEmbedding (DAG_Gamma1 *graph);

  void modifiedConstructedUC(const std::vector<UCNode *> &poles, std::vector<uint32_t> recursionSteps);
};

#endif //VALIANTUC4WAYSPLIT_VALIANTEUG_H
