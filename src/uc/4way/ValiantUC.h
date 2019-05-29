/**
 \file 		ValiantUC.h
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
 \brief		Header of the ValiantUC class part of Valiant's 4-way Split Construction.
 */

#ifndef VALIANTUC4WAYSPLIT_VALIANTUC_H
#define VALIANTUC4WAYSPLIT_VALIANTUC_H

#include <vector>
#include <math.h>
#include "ValiantEUG.h"
#include "../../Gamma/util/SHDL_to_SHDL.h"

class ValiantUC {
 public:
  ValiantUC(uint32_t nodeNum, int k, bool zhao, std::vector<uint64_t>& hybrid_choice);

  ValiantEUG* left ();
  ValiantEUG* right();

  void start(DAG_Gamma2 *graph, uint32_t inputs, uint32_t outputs, string name, bool zhao);

  void setGamma2 (DAG_Gamma2* dag);
  uint32_t getSize();

  void setInputsAndOutputs (uint32_t inputs, uint32_t outputs, bool zhao);
  void edgeEmbedding(const std::map<uint32_t, uint32_t>& mapping);

  std::vector<UCNode*> getPoles ();

  void writeOnFile (string name);

  std::vector<UCNode*> getTopOrderedNodes();

 private:
  std::vector <UCNode*> poles;
  std::vector <UCNode*> topOrderedNodes;
  std::vector <ValiantEUG*> eugs;
  uint32_t size;
  uint32_t inputs;
  uint32_t outputs;
  uint32_t gates;

  DAG_Gamma2* dag;

  void calculateSize ();
  void topologicalOrdering (uint32_t inputs);
};

#endif //VALIANTUC4WAYSPLIT_VALIANTUC_H
