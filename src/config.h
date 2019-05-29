/**
 \file 		config.h
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
 \brief		Configuration File of Valiant's UC Constructions
 */

#ifndef VALIANTUC_CONFIG_H_H
#define VALIANTUC_CONFIG_H_H

#define TIME // measure time for building UC
#define STATE // prints out the current state
//#define CIRCUIT_DEBUG // prints out debug information of the circuit
#define DEBUG_GRAPH //HAS TO BE UNDEFINED WHEN MEASURING PERFORMANCE!!! Prints out the graphvizz graphs
#define DEBUG_CORRECTNESS // run some corrctness checks

#define SHDL_CIRCUIT_FILE_FORMAT ("")
#define SHDL_MOD_CIRCUIT_FILE_FORMAT ("_Mod_SHDL.circuit")
#define CIRCUIT_FILE_FORMAT ("_circ.txt")
#define PROGRAMMING_FILE_FORMAT ("_prog.txt")
#define CIRCUIT_DIRECTORY ("circuits/")
#define OUTPUT_DIRECTORY ("output/")

#endif //VALIANTUC_CONFIG_H_H
