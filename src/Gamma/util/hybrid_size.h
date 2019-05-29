/**
 \file 		hybrid_size.h
 \author 	agnes.kiss@crisp-da.de
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
 \brief     Header of the hybrid choices.
 */

#ifndef VALIANTUC_HYBRID_SIZE_H
#define VALIANTUC_HYBRID_SIZE_H

#include <cstdint>
#include <vector>

void Dynamic_Hybrid( std::vector<uint64_t>& hybrid_choice, uint32_t n, bool zhao );

uint32_t nextK (uint32_t size, std::vector<uint64_t>& hybrid_choice);

#endif //VALIANTUC_HYBRID_SIZE_H
