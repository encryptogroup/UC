/**
 \file 		hybrid_size.cpp
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
 \brief     Implementation of the hybrid choices.
 */

#include "hybrid_size.h"

/**
 * Calculates for each number of poles if the 2-way or the 4-way construction would be the better choice
 * @param hybrid_choice mapping between poles and k-way choice
 * @param n number of poles
 * @param zhao true, if the Zhao optimization is considered in the 4-way split construction
 */
void Dynamic_Hybrid( std::vector<uint64_t>& hybrid_choice, uint32_t n, bool zhao ){
	std::vector<uint64_t> valiant_hybrid(n+1, 0);

	const uint32_t HEAD_2_s = 3;
	const uint32_t BODY_2_1_s = 5;
	const uint32_t BODY_2_2_s = 5;
	const uint32_t HEAD_4_s = 13;

	const uint32_t TAIL_1_s = 1;
	const uint32_t TAIL_2_s = 4;
	const uint32_t TAIL_3_s = 9;
	const uint32_t TAIL_4_s = 14;

	const uint32_t BODY_4_1_s = 17;
	const uint32_t BODY_4_2_s = 18;

	uint32_t BODY_4_3_s = 19;
	uint32_t BODY_4_4_s = 19;

	if(zhao){
		BODY_4_3_s = 18;
		BODY_4_4_s = 18;
	}

    uint32_t j;
    valiant_hybrid[4] = 6;
    valiant_hybrid[3] = 3;
    valiant_hybrid[2] = 2;
    valiant_hybrid[1] = 1;
    valiant_hybrid[0] = 0;

    hybrid_choice.push_back(2);
    hybrid_choice.push_back(2);
    hybrid_choice.push_back(2);
    hybrid_choice.push_back(2);
    hybrid_choice.push_back(2);

    uint32_t option_2way;
    uint32_t option_4way;
    for(j = 5; j <= n; j++){
        if( n == 5 ){
            option_4way = 13;
            option_2way = valiant_hybrid[ n/2 - 1 ] + valiant_hybrid[ n/2 ] +
                    BODY_2_2_s * (n/2 + 1 - 3) + BODY_2_1_s + HEAD_2_s + TAIL_1_s ; //-4 for Tail(1) and -3 for Head
        }
        if( n == 6 ){
            option_4way = 17;
            option_2way = 2 * valiant_hybrid[ n/2 - 1 ] +
                    BODY_2_2_s * (n/2 - 3) + BODY_2_2_s + TAIL_2_s + HEAD_2_s ; //-1 for Tail(2) and -3 for Head
        }
        if( n == 7 ){
            option_4way = 23;
            option_2way = valiant_hybrid[ n/2 - 1 ] + valiant_hybrid[ n/2 ] +
                    BODY_2_2_s * (n/2 + 1 - 3) + BODY_2_1_s + HEAD_2_s + TAIL_1_s ; //-4 for Tail(1) and -3 for Head
        }
        if( j % 4 == 0 ){ //2-way split: j % 2 == 0
            option_4way = 4 * valiant_hybrid[ j/4 - 1 ] +
                BODY_4_4_s * ( j/4 - 3 ) + BODY_4_4_s + HEAD_4_s + TAIL_4_s;; //-9 for Head and -5 for Tail(4)
            option_2way = 2 * valiant_hybrid[ j/2 - 1 ] +
                BODY_2_2_s * (j/2 - 3) + BODY_2_2_s + TAIL_2_s + HEAD_2_s ; //-1 for Tail(2) and -3 for Head
        }
        else if( j % 4 == 3 ){ //2-way split: j % 2 == 1
            option_4way = valiant_hybrid[ j/4 - 1 ] + 3 * valiant_hybrid[ j/4 ] +
                BODY_4_4_s * ( j/4 + 1 - 3 ) + BODY_4_3_s + HEAD_4_s + TAIL_3_s ; //-9 for Head and -10 for Tail(3)
            option_2way = valiant_hybrid[ j/2 - 1 ] + valiant_hybrid[ j/2 ] +
                BODY_2_2_s * (j/2 + 1 - 3) + BODY_2_1_s + HEAD_2_s + TAIL_1_s ; //-4 for Tail(1) and -3 for Head
        }
        else if( j % 4 == 2 ){ //2-way split: j % 2 == 0
            option_4way = 2* valiant_hybrid[ j/4 - 1 ] + 2 * valiant_hybrid[ j/4 ] +
                BODY_4_4_s * ( j/4 + 1 - 3 ) + BODY_4_2_s + HEAD_4_s + TAIL_2_s  ; //-9 for Head and -15 for Tail(2) and -2 for Body(2)
            option_2way = 2 * valiant_hybrid[ j/2 - 1 ] +
                BODY_2_2_s * (j/2 - 3) + BODY_2_2_s + TAIL_2_s + HEAD_2_s ; //-1 for Tail(2) and -3 for Head
        }
        else if( j % 4 == 1 ){ //2-way split: j % 2 == 1
            option_4way = 3 * valiant_hybrid[ j/4 - 1 ] + valiant_hybrid[ j/4 ] +
                BODY_4_4_s * ( j/4 + 1 - 3 ) + BODY_4_1_s + HEAD_4_s + TAIL_1_s ; //-9 for Head and -18 for Tail(1) and -2 for Body(2)
            option_2way = valiant_hybrid[ j/2 - 1 ] + valiant_hybrid[ j/2 ] +
                BODY_2_2_s * (j/2 + 1 - 3) + BODY_2_1_s + HEAD_2_s + TAIL_1_s; //-4 for Tail(1) and -3 for Head
        }
        if( option_2way < option_4way ){
            valiant_hybrid[j] = option_2way;
            //hybrid_choice[j] = 2;
            hybrid_choice.push_back(2);
        }
        else{
            valiant_hybrid[j] = option_4way;
            //hybrid_choice[j] = 4;
            hybrid_choice.push_back(4);
        }
    }
}

/**
 * Returns the smallest split for the given size
 * @param size number of poles
 * @param hybrid_choice mapping of poles and smallest splits
 * @return smallest split
 */
uint32_t nextK(uint32_t size, std::vector<uint64_t>& hybrid_choice) {
	return hybrid_choice[size];
}
