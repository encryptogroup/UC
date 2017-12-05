/**
 \file 		NodeType.h
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
 \brief		Definitions of enumerations of the differen Node types of Valiant's 4-way UC
 */

#ifndef VALIANTUC4WAYSPLIT_NODETYPE_H
#define VALIANTUC4WAYSPLIT_NODETYPE_H

enum NodeType {
  POLE,
  X_SWITCH,
  Y_SWITCH,
  REVERSE_Y_SWITCH,
  I_SWITCH,
  RECURSION_POINT
};

enum PoleType {
  INPUT,
  GATE,
  OUTPUT,
  NO_POLE
};

#endif //VALIANTUC4WAYSPLIT_NODETYPE_H
