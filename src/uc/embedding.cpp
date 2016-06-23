/**
 \file 		embedding.cpp
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
 \brief		Implementation of the functions in Valiant_DAG class which are corresponding to
            the embedding algorithm.
 */


#include "valiant.h"
#include "../util/print_graph.h"

//#define DEBUG_OUTPUT
#define STATE

/**
 * Embedding when node_number is 2
 * @param index1 first index
 * @param index2 second index
 * @param parent node
 * @return last parent node
 */
Valiant_DAG::Node* Valiant_DAG::node2(uint32_t index1, uint32_t index2, Valiant_DAG::Node* parent){
    if(index1 == 0 && index2 == 1){
        this->pole_array[0]->set_control_bit(this->pole_array[1], parent);
        return this->pole_array[0];
    }
    return 0;
}

/**
 * Embedding when node_number is 3
 * @param index1 first index
 * @param index2 second index
 * @param parent node
 * @return last parent node
 */
Valiant_DAG::Node* Valiant_DAG::node3(uint32_t index1, uint32_t index2, Valiant_DAG::Node* parent){
    if(index1 == 0 && index2 == 1){
        this->pole_array[0]->set_control_bit(this->pole_array[1], parent);
        return this->pole_array[0];
    }
    else if(index1 == 0 && index2 == 2){
        this->pole_array[0]->set_control_bit(this->pole_array[1], parent);
        this->pole_array[1]->set_control_bit(this->pole_array[2], this->pole_array[0]);
        return this->pole_array[1];
    }
    else if(index1 == 1 && index2 == 2){
        this->pole_array[1]->set_control_bit(this->pole_array[2], parent);
        return this->pole_array[1];
    }
    return 0;
}

/**
 * Embedding when node_number is 4
 * @param index1 first index
 * @param index2 second index
 * @param parent node
 * @return last parent node
 */
Valiant_DAG::Node* Valiant_DAG::node4(uint32_t index1, uint32_t index2, Valiant_DAG::Node* parent){
    if(index1 == 0 && index2 == 1){
        this->pole_array[0]->set_control_bit(this->node_array[0], parent);
        this->node_array[0]->set_control_bit(this->pole_array[1], this->pole_array[0]);
        return this->node_array[0];
    }
    else if(index1 == 0 && index2 == 2){
        this->pole_array[0]->set_control_bit(this->node_array[0], parent);
        this->node_array[0]->set_control_bit(this->node_array[1], this->pole_array[0]);
        this->node_array[1]->set_control_bit(this->pole_array[2], this->node_array[0]);
        return this->node_array[1];
    }
    else if(index1 == 0 && index2 == 3){
        this->pole_array[0]->set_control_bit(this->node_array[0], parent);
        this->node_array[0]->set_control_bit(this->node_array[1], this->pole_array[0]);
        this->node_array[1]->set_control_bit(this->node_array[2], this->node_array[0]);
        this->node_array[2]->set_control_bit(this->pole_array[3], this->node_array[1]);
        return this->node_array[2];
    }
    else if(index1 == 1 && index2 == 2){
        this->pole_array[1]->set_control_bit(this->node_array[1], parent);
        this->node_array[1]->set_control_bit(this->pole_array[2], this->pole_array[1]);
        return this->node_array[1];
    }
    else if(index1 == 1 && index2 == 3){
        this->pole_array[1]->set_control_bit(this->node_array[1], parent);
        this->node_array[1]->set_control_bit(this->node_array[2], this->pole_array[1]);
        this->node_array[2]->set_control_bit(this->pole_array[3], this->node_array[1]);
        return this->node_array[2];
    }
    else if(index1 == 2 && index2 == 3){
        this->pole_array[2]->set_control_bit(this->node_array[2], parent);
        this->node_array[2]->set_control_bit(this->pole_array[3], this->pole_array[2]);
        return this->node_array[2];
    }
    return 0;
}

/**
 * Set the indeces for embedding depending on the parity of the number of poles and the indeces
 * @param u number of inputs
 * @param v number of outputs
 * @param index1 first index
 * @param index2 second index
 * @param n_index1 first index for node
 * @param n_index2 second index for node
 */
void Valiant_DAG::set_indeces(uint32_t u, uint32_t v, uint32_t index1, uint32_t index2, int64_t& n_index1, int64_t& n_index2){
    if((pole_number % 2 == 0 && v % 2 == 1) || (pole_number % 2 == 1 && v % 2 == 0)){
        v--;
    }
    if(index2 > this->pole_number - v){
        n_index2 -= (v - (this->pole_number - index2 + 1));
        if(index2 % 2 == 0){
            n_index2--;
        }
    }
    if(u > 1){
        if(u % 2 == 0){
            n_index2 = n_index2 - u + 1;
            if(index1 < u){
                // For index1 % 2 == 0 and for index1 % 2 == 1 as well, in the even case, edge goes to n_index1 + 1, so both are fine
                n_index1 = n_index1 - index1;
            }
            else{ //index1 >= u
                n_index1 = n_index1 - u + 1;
            }
        }
        else{
            n_index2 = n_index2 - u + 2;
            if(index1 < u - 1){
                n_index1 = n_index1 - index1;
            }
            else{
                n_index1 = n_index1 - u + 2;
            }
        }
    }
}

/**
 * Check the side of the embedded edge
 * @param node_1 starting node
 * @param node_2 ending node
 * @param returned specifies if the check returns a value or not
 * @return bool true if side is left and false if right
 */
bool Valiant_DAG::check_side(Valiant_DAG::Node* node_1, Valiant_DAG::Node* node_2, bool& returned){
    if(node_1->left_parent == (node_2)){
        returned = true;
        return false;
    }
    else if(node_1->right_parent == (node_2)){
        returned = true;
        return true;
    }
    returned = false;
    cerr << "PROBLEM in embedding::check_side: outest pathfinder side check" << endl;
    return false;
}

/**
 * Pathfinder for inside universal graphs
 * @param index1 first index
 * @param index2 second index
 * @param parent node
 * @param sides vector of bools predefining the embedding
 * @return last parent node, which is needed for setting the control bits
 */
Valiant_DAG::Node* Valiant_DAG::pathfinder(uint32_t index1, uint32_t index2, Valiant_DAG::Node* parent, vector<bool>& sides){
    if(index1 == index2){
        return parent;
    }
    // Base cases
    if(this->pole_number == 1){
        // But index1 != index2
        return 0;
    }
	if(this->pole_number == 2){
        return node2(index1, index2, parent);
	}
	if(this->pole_number == 3){
		return node3(index1, index2, parent);
	}
	if(this->pole_number == 4){
        return node4(index1, index2, parent);
	}
	// Generic case
	if(this->pole_number > 4){
        Valiant_DAG* sub = 0;
        Valiant_DAG::Node* last = 0;
        int64_t n_index1;
        int64_t n_index2;

        n_index1 = 3*(index1/2);
        n_index2 = 3*(index2/2);

        // Starting pole for starting the path
        Valiant_DAG::Node* pole_index1 = this->pole_array[index1];
        Valiant_DAG::Node* node_index;
        Valiant_DAG::Node* node_index2;

        if (index2 % 2 == 1 && index2 == pole_number - 1 && index2 == index1 + 1){
            node_index = this->node_array[n_index2 - 1];
            #ifdef DEBUG_EMBEDDING
            cout << "p" << index1 << "n" << n_index2 - 1 << "p" << index2 << "  ";
            #endif // DEBUG_EMBEDDING
            pole_index1->set_control_bit(node_index, parent);
            node_index->set_control_bit(this->pole_array[index2], pole_index1);
            return node_index;
        }
        else if(index2 == (index1 + 1) && (index1 % 2 == 0)){ //this can only happen if index2 % 2 == 1
            node_index = this->node_array[n_index1];
            #ifdef DEBUG_EMBEDDING
            cout << "p" << index1 << "n" << n_index1 << "p" << index2 << "  ";
            #endif // DEBUG_EMBEDDING
            pole_index1->set_control_bit(node_index, parent);
            node_index->set_control_bit(this->pole_array[index2], pole_index1);
            return node_index;
        }
        else if(sides.back() == true){
            #ifdef DEBUG_EMBEDDING
            cout << "left" ;
            #endif // DEBUG_EMBEDDING
            sub = this->sub_left;
        }
        else if(sides.back() == false){
            #ifdef DEBUG_EMBEDDING
            cout << "right" ;
            #endif // DEBUG_EMBEDDING
            sub = this->sub_right;
        }
        else{
            cerr << "Problem at finding the path in a subgraph!" << endl;
        }
        sides.pop_back();

        if(index1 % 2 == 0){
            #ifdef DEBUG_EMBEDDING
            cout << "p" << index1 << "n" << n_index1 << "n" << n_index1 + 1 << "_subp" << index1/2;
            #endif // DEBUG_EMBEDDING
            node_index = this->node_array[n_index1];
            node_index2 = this->node_array[n_index1 + 1];
            pole_index1->set_control_bit(node_index, parent);
            node_index->set_control_bit(node_index2, pole_index1);
            node_index2->set_control_bit(sub->pole_array[index1/2], node_index);
        }
        else{ //(index1 % 2 == 1){
            #ifdef DEBUG_EMBEDDING
            cout << "p" << index1 << "n" << n_index1 + 1 << "_subp" << index1/2 << "  ";
            #endif // DEBUG_EMBEDDING
            node_index = this->node_array[n_index1 + 1];
            pole_index1->set_control_bit(node_index, parent);
            node_index->set_control_bit(sub->pole_array[index1/2], pole_index1);
        }

        last = sub->pathfinder(index1/2, index2/2 - 1, this->node_array[n_index1 + 1], sides);
        // Starting pole for ending the path
        pole_index1 = sub->pole_array[index2/2 - 1];

        // Cases in the very end - even
        if(index2 % 2 == 0){
            if(index2 == pole_number - 2 || index2 == pole_number - 1){ //first if the pole number is even, second if odd, but they act the same way
                #ifdef DEBUG_EMBEDDING
                cout << "_subp" << index2/2 - 1 << "p" << index2 << "  ";
                #endif // DEBUG_EMBEDDING
                pole_index1->set_control_bit(this->pole_array[index2], last);
                return pole_index1;
            }
            else{ //if it is not in the end
                #ifdef DEBUG_EMBEDDING
                cout << "_subp" << index2/2 - 1 << "n" << n_index2 - 1 << "p" << index2 << "  ";
                #endif // DEBUG_EMBEDDING
                node_index = this->node_array[n_index2 - 1];
                pole_index1->set_control_bit(node_index, last);
                node_index->set_control_bit(this->pole_array[index2], pole_index1);
                return node_index;
            }
        }
        // Cases in the very end - odd
        if(index2 % 2 == 1){
            if(index2 == pole_number - 1 && pole_number % 2 == 0){
                #ifdef DEBUG_EMBEDDING
                cout << "_subp" << index2/2 - 1 << "n" << n_index2 - 1 << "p" << index2 << "  ";
                #endif // DEBUG_EMBEDDING
                node_index = this->node_array[n_index2 - 1];
                pole_index1->set_control_bit(node_index, last);
                node_index->set_control_bit(this->pole_array[index2], pole_index1);
                return node_index;
            }
            else{
                #ifdef DEBUG_EMBEDDING
                cout << "_subp" << index2/2 - 1 << "n" << n_index2 - 1 << "n" << n_index2 << "p" << index2 << "  ";
                #endif // DEBUG_EMBEDDING
                node_index = this->node_array[n_index2 - 1];
                node_index2 = this->node_array[n_index2];
                pole_index1->set_control_bit(node_index, last);
                node_index->set_control_bit(node_index2, pole_index1);
                node_index2->set_control_bit(this->pole_array[index2], node_index);
                return node_index2;
            }
        }
    }
	return 0;
}

/**
 * Outest pathfinder for inside universal graphs, it returns a boolean value which corresponds to
 * if the last node going into the final pole was left parent (false) or right parent (true)
 * which will be used by the embedding algorithm to check if the middle bits need to be exchanged or not
 * @param index1 first index
 * @param index2 second index
 * @param parent node
 * @param sides vector of bools predefining the embedding
 * @param u number of inputs
 * @param v number of outputs
 * @param outest_first true if we are in the outest graph and it is the first case
 * @return last parent node, which is needed for setting the control bits
 */
bool Valiant_DAG::pathfinder(uint32_t index1, uint32_t index2, Valiant_DAG::Node* parent, vector<bool>& sides, uint32_t u, uint32_t v, bool outest_first){
    // Call simpler pathfinder that has the base cases
    if(this->pole_number <= 4){
        pathfinder(index1, index2, parent, sides);
    }
	Valiant_DAG* sub = 0;
	Valiant_DAG::Node* last = 0;
	// Always even number
	int64_t n_index1 = 3*(index1/2);
    int64_t n_index2 = 3*(index2/2);

    // Define node array for which we embed
    Node** actual = this->node_array;
    if(!outest_first){
        actual = this->node_array_outest;
    }
    bool returned = false;
    bool sidecheck;

    // Generic pathfinder
	if(this->pole_number > 4){
        // Set indeces based on inputs, outputs and gates numbers
        this->set_indeces(u, v, index1, index2, n_index1, n_index2);

        // Set starting pole
        Valiant_DAG::Node* node_index;
        Valiant_DAG::Node* pole_index1 = this->pole_array[index1];
        Valiant_DAG::Node* pole_index2 = this->pole_array[index2];

	    if(index1 == index2){
            cerr << "PROBLEM in embedding::pathfinder: This should not happen in the outest case, no path should be searched for from i to i" << endl;
            return false;
        }
        else if (index2 % 2 == 1 && index2 == pole_number - 1 && index2 == index1 + 1){
            #ifdef DEBUG_EMBEDDING
            cout << "p" << index1 << "n" << n_index2 - 1 << "p" << index2 << "  ";
            #endif // DEBUG_EMBEDDING
            node_index = *(actual + n_index2 - 1);
            pole_index1->set_control_bit(node_index, parent);
            node_index->set_control_bit(pole_index2, pole_index1);
            // Check side
            sidecheck = check_side(pole_index2, node_index, returned);
            if( returned ){
                return sidecheck;
            }
        }
        // This can only happen if index2 % 2 == 1, only n_index1 changes
        else if(index2 == (index1 + 1) && (index1 % 2 == 0)){
            #ifdef DEBUG_EMBEDDING
            cout << "p" << index1 << "n" << n_index1 << "p" << index2 << "  ";
            #endif // DEBUG_EMBEDDING
            node_index = *(actual + n_index1);
            pole_index1->set_control_bit(*(actual + n_index1), parent);
            node_index->set_control_bit(pole_index2, pole_index1);
            // Check side
            sidecheck = check_side(pole_index2, node_index, returned);
            if( returned ){
                return sidecheck;
            }
        }

        // Set which subgraph to use
        else if(sides.back() == true && outest_first){
            sub = this->sub_left;
        }
        else if(sides.back() == false && outest_first){
            sub = this->sub_right;
        }
        else if(sides.back() == true && !outest_first){
            sub = this->sub_left2;
        }
        else if(sides.back() == false && !outest_first){
            sub = this->sub_right2;
        }
        else{
            cerr << "PROBLEM in embedding::pathfinder: outest pathfinder!" << endl;
        }
        sides.pop_back();

        if(index1 % 2 == 0 && ((u % 2 == 0 && index1 < u) || (u % 2 == 1 && index1 < u - 1))){
            #ifdef DEBUG_EMBEDDING
            cout << "p" << index1 << "n" << n_index1 << "_subp" << index1/2;
            #endif // DEBUG_EMBEDDING
            node_index = *(actual + n_index1);
            pole_index1->set_control_bit(node_index, parent);
            node_index->set_control_bit(sub->pole_array[index1/2], pole_index1);
            parent = node_index;
        }
        else if(index1 % 2 == 0){
            #ifdef DEBUG_EMBEDDING
            cout << "p" << index1 << "n" << n_index1 << "n" << n_index1 + 1 << "_subp" << index1/2;
            #endif // DEBUG_EMBEDDING
            node_index = *(actual + n_index1);
            pole_index1->set_control_bit(node_index, parent);
            (node_index)->set_control_bit(*(actual + n_index1 + 1), pole_index1);
            (*(actual + n_index1 + 1))->set_control_bit(sub->pole_array[index1/2], node_index);
            parent = *(actual + n_index1 + 1);
        }
        else{ //(index1 % 2 == 1){
            #ifdef DEBUG_EMBEDDING
            cout << "p" << index1 << "n" << n_index1 + 1 << "_subp" << index1/2 << "  ";
            #endif // DEBUG_EMBEDDING
            node_index = *(actual + n_index1 + 1);
            pole_index1->set_control_bit(node_index, parent);
            (node_index)->set_control_bit(sub->pole_array[index1/2], pole_index1);
            parent = node_index;
        }

        last = sub->pathfinder(index1/2, index2/2 - 1, parent, sides);

        // Set indeces
        pole_index1 = sub->pole_array[index2/2 - 1];
        pole_index2 = this->pole_array[index2];

        if(index2 % 2 == 0){
            // End case
            if(index2 == pole_number - 2 || index2 == pole_number - 1){ //first if the pole number is even, second if odd, but they act the same way
                #ifdef DEBUG_EMBEDDING
                cout << "_subp" << index2/2 - 1 << "p" << index2 << "  ";
                #endif // DEBUG_EMBEDDING
                pole_index1->set_control_bit(pole_index2, last);
                // Check side
                sidecheck = check_side(pole_index2, pole_index1, returned);
                if( returned ){
                    return sidecheck;
                }
            }
            // If it is not in the end
            else{
                #ifdef DEBUG_EMBEDDING
                cout << "_subp" << index2/2 - 1 << "n" << n_index2 - 1 << "p" << index2 << "  ";
                #endif // DEBUG_EMBEDDING
                pole_index1->set_control_bit(*(actual + n_index2 - 1), last);
                (*(actual + n_index2 - 1))->set_control_bit(pole_index2, pole_index1);
                // Check side
                sidecheck = check_side(pole_index2, (*(actual + n_index2 - 1)), returned);
                if( returned ){
                    return sidecheck;
                }
            }
        }
        if(index2 % 2 == 1 && index2 > this->pole_number - v){
            // End case
            if(index2 == pole_number - 1 && pole_number % 2 == 0){
                #ifdef DEBUG_EMBEDDING
                cout << "_subp" << index2/2 - 1 << "p" << index2 << "  ";
                #endif // DEBUG_EMBEDDING
                pole_index1->set_control_bit(pole_index2, last);
                // Check side
                sidecheck = check_side(pole_index2, pole_index1, returned);
                if( returned ){
                    return sidecheck;
                }
            }
            // If it is not in the end
            else{
                #ifdef DEBUG_EMBEDDING
                cout << "_subp" << index2/2 - 1 << "n" << n_index2 - 1 << "p" << index2 << "  ";
                #endif // DEBUG_EMBEDDING
                pole_index1->set_control_bit(*(actual + n_index2 - 1), last);
                (*(actual + n_index2 - 1))->set_control_bit(pole_index2, pole_index1);
                // Check side
                sidecheck = check_side(pole_index2, (*(actual + n_index2 - 1)), returned);
                if( returned ){
                    return sidecheck;
                }
            }
        }
        else if(index2 % 2 == 1){
            // End case
            if(index2 == pole_number - 1 && pole_number % 2 == 0){
                #ifdef DEBUG_EMBEDDING
                cout << "_subp" << index2/2 - 1 << "n" << n_index2 - 1 << "p" << index2 << "  ";
                #endif // DEBUG_EMBEDDING
                pole_index1->set_control_bit(*(actual + n_index2 - 1), last);
                (*(actual + n_index2 - 1))->set_control_bit(pole_index2, pole_index1);
                // Check side
                sidecheck = check_side(pole_index2, (*(actual + n_index2 - 1)), returned);
                if( returned ){
                    return sidecheck;
                }
            }
            // If it is not in the end
            else{
                #ifdef DEBUG_EMBEDDING
                cout << "_subp" << index2/2 - 1 << "n" << n_index2 - 1 << "n" << n_index2 << "p" << index2 << "  ";
                #endif // DEBUG_EMBEDDING
                pole_index1->set_control_bit(*(actual + n_index2 - 1), last);
                (*(actual + n_index2 - 1))->set_control_bit(*(actual + n_index2), pole_index1);
                (*(actual + n_index2))->set_control_bit(pole_index2, *(actual + n_index2 - 1));
                // Check side
                sidecheck = check_side(pole_index2, (*(actual + n_index2)), returned);
                if( returned ){
                    return sidecheck;
                }
            }
        }
    }
    cerr << "PROBLEM in embedding::pathfinder: outest pathfinder!" << endl;
	return false;
}

/**
 * Embed according to the side, it defines the side of the embedding as well as if we expect the node to be node2 or not
 * @param gamma1 Gamma1 graph left or right
 * @param gamma2 Gamma2 graph supergraph
 * @param u number of inputs
 * @param v number of outputs
 * @param side defining if side is left or right
 */
void Valiant_DAG::embed_side(DAG_Gamma1* gamma1, DAG_Gamma2* gamma2, uint32_t u, uint32_t v, bool side){
    vector<bool> sides;
    bool is_right;
    DAG_Gamma2::Node* the_node = 0;
    for(uint32_t i = 0; i < gamma1->node_number; ++i){
        if(gamma1->node_array[i]->child){
        #ifdef DEBUG_EMBEDDING
        cout << "Embed " << i << "  " << gamma1->node_array[i]->child->number - 1 << endl;
        #endif // DEBUG_EMBEDDING
        embedding_with_supergraph(gamma2, i, gamma1->node_array[i]->child->number - 1, sides, side);
        /* Embedding via this->nodeA */
        is_right = this->pathfinder(i, gamma1->node_array[i]->child->number - 1, 0, sides, u, v, !side);
        if(gamma1->node_array[i]->child->is_output){
            #ifdef DEBUG_OUTPUT
            cout << "is_output is true set to 0 nodeA " << gamma1->node_array[i]->child->number - 1 << endl;
            #endif // DEBUG_OUTPUT
            if((this->pole_array[gamma1->node_array[i]->child->number - 1]->left_parent && (this->pole_array[gamma1->node_array[i]->child->number - 1]->left_parent->is_node2) == side)){
                    #ifdef DEBUG_OUTPUT
                    cout << "left parent, control bit set to 1" << endl;
                    #endif
                    gamma2->node_array[gamma1->node_array[i]->child->number - 1]->set_function_bits(1, 0, 0, 0);
            }
            else if((this->pole_array[gamma1->node_array[i]->child->number - 1]->right_parent && (this->pole_array[gamma1->node_array[i]->child->number - 1]->right_parent->is_node2) == side)){
                    #ifdef DEBUG_OUTPUT
                    cout << "right parent, control bit set to 0" << endl;
                    #endif
                    gamma2->node_array[gamma1->node_array[i]->child->number - 1]->set_function_bits(0, 0, 0, 0);
            }
            else{
                cerr << "none of the parents are node1" << endl;
            }
        }
        the_node = gamma2->node_array[gamma1->node_array[i]->child->number - 1];
        if(is_right && the_node->left_parent && !(the_node->is_bitchanged) &&
            the_node->left_parent->number == gamma1->node_array[i]->number){
            swap(the_node->function_bits[1], the_node->function_bits[2]);
            the_node->is_bitchanged = true;
        }
        if(!is_right && the_node->right_parent && !(the_node->is_bitchanged) &&
            the_node->right_parent->number == gamma1->node_array[i]->number){
            swap(the_node->function_bits[1], the_node->function_bits[2]);
            the_node->is_bitchanged = true;
        }
     }
  }
}

/**
 * Embedding algorithm given the Gamma2 supergraph
 * @param g Gamma2 supergraph
 * @param index1 first index
 * @param index2 second index
 * @param sides vector of bools predefining the embedding
 * @param side true if left and false if right (?)
 */
void embedding_with_supergraph(DAG_Gamma2* g, uint32_t index1, uint32_t index2, vector<bool>& sides, bool side){
    uint32_t new_index1 = index1/2;
    uint32_t new_index2 = index2/2-1;
    DAG_Gamma1* gamma1;

    if(index1 == index2){
        return;
    }
    else if(index2 == neighbouring_index(index1)){
        return;
    }
    else if(g->node_number <= 4){
        return;
    }
    DAG_Gamma2* sub;

    // Set subgraph accordingly
    if(side){
        sub = g->sub_left;
        gamma1 = g->gamma1_left;
    }
    else{
        sub = g->sub_right;
        gamma1 = g->gamma1_right;
    }

    // Right is fine
    if(sub && sub->gamma1_right->check_exist(new_index1) && sub->gamma1_right->check_right(new_index1, new_index2)){
        // Left is fine
        if(sub->gamma1_left->check_exist(new_index1) && sub->gamma1_left->check_right(new_index1, new_index2)){
            // Choose left based on parity
            if((gamma1->node_number % 2 == 0 && index2 == gamma1->node_number - 2) || (gamma1->node_number % 2 == 1 && index2 == gamma1->node_number - 1)){
                sub->gamma1_left->node_array[new_index1]->is_embedded = true;
                // Recursive
                embedding_with_supergraph(sub, new_index1, new_index2, sides, true);
                sides.push_back(true);
            }
            // Choose right based on parity
            else{
                sub->gamma1_right->node_array[new_index1]->is_embedded = true;
                // Recursive
                embedding_with_supergraph(sub, new_index1, new_index2, sides, false);
                sides.push_back(false);
            }
        }
        else{
            sub->gamma1_right->node_array[new_index1]->is_embedded = true;
            // Recursive
            embedding_with_supergraph(sub, new_index1, new_index2, sides, false);
            sides.push_back(false);
        }
        return;
    }
    // Left is fine
    if(sub && sub->gamma1_left->check_exist(new_index1) && sub->gamma1_left->check_right(new_index1, new_index2)){
        sub->gamma1_left->node_array[new_index1]->is_embedded = true;
        // Recursive
        embedding_with_supergraph(sub, new_index1, new_index2, sides, true);
        sides.push_back(true);
        return;
    }
    // If none was fine, return error
    cerr << "PROBLEM in embedding::embedding_with_supergraph " << index1 << "  " << index2 << endl;
}

/**
 * Embed according to the side, it defines the side of the embedding as well as if we expect the node to be node2 or not
 * @param g Gamma2 graph supergraph
 * @param u number of inputs
 * @param v number of outputs
 */
Valiant_DAG* embedding_merged(DAG_Gamma2* g, uint32_t u, uint32_t v){
  #ifdef DEBUG_GRAPH
  print_Gamma2_simple(g);
  #endif

  uint32_t node_num = g->node_number;
  DAG_Gamma2::Node* the_node = 0;

  // Create subgraphs for embedding
  g->create_subgraphs(node_num * 2);
  g->check_correct_subgraphs();
  #ifdef DEBUG_GRAPH
  print_gamma2_full(g);
  #endif // DEBUG_GRAPH

  #ifdef STATE
  cout << "2. Subgraphs created for Gamma2 graph" << endl;
  #endif // STATE

  // Create outest universal graph with u inputs, v outputs and node_num-u-v gates
  Valiant_DAG* g2 = init_merged(node_num - u - v, u, v, true, true);
  #ifdef STATE
  cout << "3. Merged universal graph initialized with " << node_num - u - v << " gates, " << u << " inputs and " << v << " outputs" << endl;
  #endif // STATE

  g2->embed_side(g->gamma1_right, g, u, v, false);
  #ifdef STATE
  cout << "4. MERGED right side embedded" << endl;
  #endif // STATE

  g2->embed_side(g->gamma1_left, g, u, v, true);
  #ifdef STATE
  cout << "5. MERGED left side embedded" << endl;
  #endif // STATE
  g2->topologically_sort(u);
  //g2->set_random_control_bits(true);

  for(uint32_t i = u; i < g2->pole_number; ++i){
        the_node = g->node_array[i];
        g2->pole_array[i]->control_num = the_node->function_bits[0] * 1 + the_node->function_bits[1] * 2 + the_node->function_bits[2] * 4 + the_node->function_bits[3] * 8;
  }

  #ifdef DEBUG_GRAPH
  print_Graph_full(g2, false, true);
  #endif // DEBUG_GRAPH

  return g2;
}

/**
 * Given an index, returns the neighbouring one
 * @param index initial index
 * @return neighbouring index
 */
uint32_t neighbouring_index(uint32_t index){
    if(index % 2 == 0){
        return index + 1;
    }
    else{
        return index - 1;
    }
}
