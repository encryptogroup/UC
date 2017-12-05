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
 \brief     Header of debug functions of Valiant's 4-way split UC Construction
 */

#ifndef VALIANTUC_DEBUG_H
#define VALIANTUC_DEBUG_H

#include "uc/4way/ValiantUC.h"
bool validate_block_edge_embedding(ValiantUC* uc);
bool validate_block_edge_embedding(ValiantEUG* eug, int eugNumber, string path);

bool validate_recursion_point_edge_embedding(ValiantUC *uc, DAG_Gamma2* gamma);
bool validate_recursion_point_edge_embedding(DAG_Gamma1 *graph, ValiantEUG *uc, int eugNum);
#endif //VALIANTUC_DEBUG_H
