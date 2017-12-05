/**
 \file 		BlockType.h
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
 \brief		Definition of an enumeration of the different block types in Valiant's 4-way UC
 */

#ifndef VALIANTUC4WAYSPLIT_BLOCKTYPE_H
#define VALIANTUC4WAYSPLIT_BLOCKTYPE_H

enum BlockType{
  NORMAL,
  HEAD,
  TAIL,
  RECURSION_BASE
};

#endif //VALIANTUC4WAYSPLIT_BLOCKTYPE_H
