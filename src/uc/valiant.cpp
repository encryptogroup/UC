/**
 \file 		valiant.cpp
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
 \brief		Implementation of Valiant_DAG class for universal graphs with at most two incoming and at most
            two outgoing edge for each node.
 */

#include "valiant.h"

/**
 * Valiant_DAG contructor, creating list of nodes and poles for a given size
 * @param k number of gates of universal circuit
 * @param u number of inputs of universal circuit
 * @param v number of outputs of universal circuit
 * @param side default side in which edges should be added
 * @param outest true if it is the outest universal graph and not a subgraph
 */
Valiant_DAG::Valiant_DAG(uint32_t k, uint32_t u, uint32_t v, bool side, bool outest)
 : pole_number(k + u + v)
 , sub_left(0)
 , sub_right(0)
 , sub_left2(0)
 , sub_right2(0){

    // Creating poles
    this->create_poles(pole_number, outest);
    // Base cases special (2, 3, 4)
    if(pole_number == 2 || pole_number == 3){
        node_number = 0;
        add_edge(pole_array[0], pole_array[1], side);
        if(outest){
            add_edge(pole_array[0], pole_array[1], !side);
        }
    }
    if (pole_number == 3){
        node_number = 0;
        add_edge(pole_array[1], pole_array[2], side);
        if(outest){
            add_edge(pole_array[1], pole_array[2], !side);
        }
    }
    if(pole_number == 4){
        node_number = 3;
        node_array = new Node*[node_number];
        for (uint32_t i = 0; i < node_number; ++i){
            node_array[i] = new Node(i + 1);
        }
        this->node4(node_array, pole_array, side);

        if(outest){
            node_array_outest = new Node*[node_number];
            for (uint32_t i = 0; i < node_number; ++i){
                node_array_outest[i] = new Node(i + 1);
                node_array_outest[i]->is_node2 = true;
            }
            this->node4(node_array_outest, pole_array, !side);
        }
    }
    // Not outest, does not have to deal with input and output nodes!
    if(!outest){
        // Generic case
        if(pole_number > 4){
            // Odd case
            if(pole_number % 2 == 1){
                node_number = 3 * ((pole_number-2)/2) + 2;
            }
            // Even case
            else{
                node_number = 3 * ((pole_number-2)/2);
            }
            node_array = new Node*[node_number];
            for(uint32_t i = 0; i < node_number; ++i){
                node_array[i] = new Node(i + 1);
            }
            // Paths for all nodes
            uint32_t i = 3;
            for(uint32_t j = 3; j <= pole_number - 2; j = j + 2){
                this->generic_node_path(i, j, node_array, pole_array, side);
                i += 3;
            }
            // End nodes
            if(pole_number % 2 == 0){
                add_edge(pole_array[pole_number - 2], node_array[i - 1], side);
                add_edge(node_array[i - 1], pole_array[pole_number - 1], side);
            }
        }
    }
    // Outest, it has to deal with input and output nodes!
    else{
        Valiant_DAG::Node* zero_node;
        Valiant_DAG::Node* zero_node_outest;
        Valiant_DAG::Node* first_node;
        Valiant_DAG::Node* first_node_outest;
        Valiant_DAG::Node* zero_pole;
        Valiant_DAG::Node* first_pole;

        Valiant_DAG::Node* j_pole;
        Valiant_DAG::Node* j1_pole;
        Valiant_DAG::Node* i1_node;
        Valiant_DAG::Node* i1_node_outest;

        // Generic case
        if(pole_number > 4){
            // Output parity
            if((pole_number % 2 == 0 && v % 2 == 1) || (pole_number % 2 == 1 && v % 2 == 0)){
                v--;
                k++;
            }
            // Odd case
            if(pole_number % 2 == 1){
                node_number = 3 * ((pole_number - 2)/2) + 2 - u + 2 - v + 1;
            }
            // Even case
            else{
                node_number = 3 * ((pole_number - 2)/2) - u + 2 - v + 1;
            }
            node_array = new Node*[node_number];
            node_array_outest = new Node*[node_number];
            for(uint32_t i = 0; i < node_number; ++i){
                node_array[i] = new Node(i + 1);
                node_array_outest[i] = new Node(i + 1);
                node_array_outest[i]->is_node2 = true;
            }
            // Input nodes case initialize nodes
            uint32_t i = 2;
            zero_node = node_array[0];
            zero_node_outest = node_array_outest[0];
            first_node = node_array[1];
            first_node_outest = node_array_outest[1];
            zero_pole = pole_array[0];
            first_pole = pole_array[1];
            // Dealing with inputs
            add_edge(zero_pole, zero_node, side); //p1-> 1
            add_edge(zero_pole, zero_node_outest, !side); //p1-> 1
            if(u < 2){
                add_edge(zero_node, first_pole, side); //1 -> p2
                add_edge(zero_node, first_node, !side); //1 -> 2
                add_edge(first_pole, first_node, side); //p2-> 2
                add_edge(zero_node_outest, first_pole, !side); //1 -> p2
                add_edge(zero_node_outest, first_node_outest, side); //1 -> 2
                add_edge(first_pole, first_node_outest, !side); //p2-> 2
                i = 3;
            }
            else{
                add_edge(first_pole, zero_node, !side); //p2-> 2
                add_edge(first_pole, node_array_outest[0], side); //p2-> 2
            }
            // Nodes in the middle
            for(uint32_t j = 3; j < pole_number - 1; j += 2){
                j1_pole = pole_array[j - 1];
                j_pole = pole_array[j];
                i1_node = node_array[i - 1];
                i1_node_outest = node_array_outest[i - 1];

                // Inputs
                if(j < u){
                    add_edge(j1_pole, i1_node, !side);
                    add_edge(j_pole, i1_node, side);
                    add_edge(j1_pole, i1_node_outest, side);
                    add_edge(j_pole, i1_node_outest, !side);
                    i += 1;
                }
                // Outputs
                else if(j > u + k - 1){
                    add_edge(i1_node, j1_pole, side);
                    add_edge(i1_node, j_pole, !side);
                    add_edge(i1_node_outest, j1_pole, !side);
                    add_edge(i1_node_outest, j_pole, side);
                    i += 1;
                }
                // Paths for all nodes
                else{
                    this->generic_node_path(i, j, node_array, pole_array, side);
                    this->generic_node_path(i, j, node_array_outest, pole_array, !side);
                    i += 3;
                }
            }
            // Somewhere srpX --> pj has to be added in all other cases
            if(pole_number % 2 == 0 && v < 2){
                // Initializes
                zero_pole = pole_array[pole_number - 2];
                first_pole = pole_array[pole_number - 1];
                i1_node = node_array[i - 1];
                i1_node_outest = node_array_outest[i - 1];

                // Add edges
                add_edge(zero_pole, i1_node, side);
                add_edge(i1_node, first_pole, side);
                add_edge(zero_pole, i1_node_outest, !side);
                add_edge(i1_node_outest, first_pole, !side);
            }
        }
    }
}

/**
 * Creates the array of poles given the number of poles
 * @param pole_num number of poles expected
 * @param outest is true if the Valiant_DAG is the outest universal graph
 */
void Valiant_DAG::create_poles(uint32_t pole_num, bool outest){
    this->pole_array = new Node*[pole_number];
    for(uint32_t i = 0; i < pole_num; ++i){
		this->pole_array[i] = new Node(i + 1);
		this->pole_array[i]->is_pole = true;
		// For outest poles store that they are outest, i.e., inputs, outputs or gates
		if(outest){
            this->pole_array[i]->is_outest_pole = true;
		}
	}
}

/**
 * A pathfinder between pole[j - 1] and pole[j] according to Valiant's skeleton of universal graph, using
 * nodes i - 1, i and i + 1.
 * @param i index specifying the nodes to be used
 * @param j index specifying the poles to be used
 * @param node_arr Node array from which we take nodes
 * @param node_arr Node array from which we take poles
 * @param side default side in which edges should be added
 */
void Valiant_DAG::generic_node_path(uint32_t i, uint32_t j, Node** node_arr, Node** pole_arr, bool side){
    Valiant_DAG::Node* first_pole = pole_arr[j - 1];
    Valiant_DAG::Node* second_pole = pole_arr[j];
    Valiant_DAG::Node* first_node = node_arr[i - 1];
    Valiant_DAG::Node* second_node = node_arr[i];
    Valiant_DAG::Node* third_node = node_arr[i + 1];
    // Path from pole j - 1 to pole j
    add_edge(first_node, first_pole, side);
    add_edge(first_pole, second_node, side);
    add_edge(first_node, second_node, !side);
    add_edge(second_node, second_pole, side);
    add_edge(second_node, third_node, !side);
    add_edge(second_pole, third_node, side);
}

/**
 * A pathfinder between pole[0] and pole[3] according to Valiant's skeleton 4-node universal graph, using
 * additional nodes.
 * @param node_arr Node array from which we take nodes
 * @param node_arr Node array from which we take poles
 * @param side default side in which edges should be added
 */
void Valiant_DAG::node4(Node** node_arr, Node** pole_arr, bool side){
    // Used more than once
    Valiant_DAG::Node* first_pole = pole_arr[1];
    Valiant_DAG::Node* second_pole = pole_arr[2];
    Valiant_DAG::Node* zero_node = node_arr[0];
    Valiant_DAG::Node* first_node = node_arr[1];
    Valiant_DAG::Node* second_node = node_arr[2];
    // Path from 0 to 3 in Valiant's base case with n = 4
    add_edge(pole_arr[0], zero_node, side);
    add_edge(zero_node, first_pole, side);
	add_edge(zero_node, first_node, !side);
	add_edge(first_pole, first_node, side);
	add_edge(first_node, second_pole, side);
	add_edge(first_node, second_node, !side);
	add_edge(second_pole, second_node, side);
	add_edge(second_node, pole_arr[3], side);
}

/**
 * Add an edge to the graph given two nodes, with special property that if a x->left = y then y->left_parent=x and
 * if x->right=y then y->right_parent=x!
 * @param n1 first Node
 * @param n2 second Node
 * @param side which side it should be added on: side true = left, false = right
 */
void Valiant_DAG::add_edge(Node* n1, Node* n2, bool side){
    if(side == true && !(n1->left) && !(n2->left_parent)){
        n1->left = n2;
        n2->left_parent = n1;
    }
	else if(side == false && !(n1->right) && !(n2->right_parent)){
		n1->right = n2;
		n2->right_parent = n1;
	}
	else{
		cerr << "PROBLEM in valiant::add_edge: Failed edge insert ";
		if(n1->is_pole)
            cerr << "p";
		else
            cerr << "n" << n1->number-1 << " , ";
		if(n2->is_pole)
            cerr << "p";
		else
            cerr << "n" << n2->number-1 << endl;
	}
}


/**
 * Set random control bits after the others are set
 * @param outest true if we are in outest graph
 */
void Valiant_DAG::set_random_control_bits(bool outest){
    for(uint32_t i = 0; i < this->node_number; ++i){
        if(node_array[i] != 0 && !node_array[i]->is_set){
            node_array[i]->is_set = true;
            node_array[i]->control_num = rand() % 2 == 1;
        }
        if(outest){
            if(!node_array_outest[i]->is_set){
                node_array_outest[i]->is_set = true;
                node_array_outest[i]->control_num = rand() % 2 == 1;
            }
        }
    }
    if(this->sub_left){
        this->sub_left->set_random_control_bits(false);
    }
    if(this->sub_right){
        this->sub_right->set_random_control_bits(false);
    }
    if(outest){
        if(this->sub_left2){
            this->sub_left2->set_random_control_bits(false);
        }
        if(this->sub_right2){
            this->sub_right2->set_random_control_bits(false);
        }
    }
}

/**
 * Unified init part for the inner universal graph construction, with only small differences for the
 * even and odd cases.
 * @param n size of universal graph
 * @param side default side in which edges should be added
 * @param even true if we are in the even case and false if we are in the odd case
 */
void Valiant_DAG::init_even_odd(uint32_t n, bool side, bool even){
    Valiant_DAG::Node* nodel;
    Valiant_DAG::Node* nodel1;
    Valiant_DAG::Node* pole_left_k1;
    Valiant_DAG::Node* pole_right_k1;
	uint32_t l = 2;
	for (uint32_t k = 1; k < (n - 2)/2 + 1; ++k){
        nodel = node_array[l];
        nodel1 = node_array[l - 1];
        pole_left_k1 = sub_left->pole_array[k - 1];
        pole_right_k1 = sub_right->pole_array[k - 1];

		add_edge(nodel1, pole_left_k1, side);
		add_edge(nodel1, pole_right_k1, !side);
		#ifdef DEBUG_GRAPH
		if(nodel1->left == pole_left_k1){
            nodel1->is_left_in_subleft = true;
		}
		else if(nodel1->right == pole_left_k1){
            nodel1->is_right_in_subleft = true;
		}
        if(nodel1->left == pole_right_k1){
            nodel1->is_left_in_subright = true;
		}
		else if(nodel1->right == pole_right_k1){
            nodel1->is_right_in_subright = true;
		}
		#endif // DEBUG_GRAPH

		if(k != sub_left->pole_number || !even){
			add_edge(pole_left_k1, nodel, side);
			#ifdef DEBUG_GRAPH
            if(pole_left_k1->left == nodel){
               pole_left_k1->is_left_outer = true;
            }
            else if(pole_left_k1->right == nodel){
                pole_left_k1->is_right_outer = true;
            }
            #endif

		} //if the (k-1)th node is not the last subpole
		else{
            add_edge(pole_left_k1, pole_array[pole_number - 2], side);
            #ifdef DEBUG_GRAPH
            if(pole_left_k1->left == pole_array[pole_number - 2]){
                pole_left_k1->is_left_outer = true;
            }
            else if(pole_left_k1->right == pole_array[pole_number - 2]){
                pole_left_k1->is_right_outer = true;
            }
            #endif
		}
		add_edge(pole_right_k1, nodel, !side);
		#ifdef DEBUG_GRAPH
        if(pole_right_k1->left == nodel){
            pole_right_k1->is_left_outer = true;
		}
		else if(pole_right_k1->right == nodel){
            pole_right_k1->is_right_outer = true;
		}
		#endif // DEBUG_GRAPH

		l += 3;
	}
	if(!even){
        nodel1 = node_array[l - 1];
        pole_left_k1 = sub_left->pole_array[sub_left->pole_number - 1];
        pole_right_k1 = pole_array[pole_number - 1];

        add_edge(nodel1, pole_left_k1, side);
        #ifdef DEBUG_GRAPH
        if(nodel1->left == pole_left_k1){
            nodel1->is_left_in_subleft  = true;
        }
        else if(nodel1->right == pole_left_k1){
            nodel1->is_right_in_subleft  = true;
        }
        #endif // DEBUG_GRAPH

        add_edge(pole_left_k1, pole_right_k1, side);
        #ifdef DEBUG_GRAPH
        if(pole_left_k1->left == pole_right_k1){
            pole_left_k1->is_left_outer = true;
        }
        else if(pole_left_k1->right == pole_right_k1){
            pole_left_k1->is_right_outer = true;
        }
        #endif
	}
}

/**
 * Common part of init_merged for odd and even case
 * @param side default side in which edges should be added
 * @param l first index
 * @param k second index
 */
void Valiant_DAG::gates(bool side, uint32_t l, uint32_t k){
    add_edge(node_array[l - 1], sub_left->pole_array[k - 1], side);
    add_edge(node_array[l - 1], sub_right->pole_array[k - 1], !side);
    #ifdef DEBUG_GRAPH
    if(node_array[l - 1]->left == sub_left->pole_array[k - 1]){
        node_array[l - 1]->is_left_in_subleft  = true;
    }
    else if(node_array[l - 1]->right == sub_left->pole_array[k - 1]){
        node_array[l - 1]->is_right_in_subleft  = true;
    }
    if(node_array[l - 1]->left == sub_right->pole_array[k - 1]){
        node_array[l - 1]->is_left_in_subright  = true;
    }
    else if(node_array[l - 1]->right == sub_right->pole_array[k - 1]){
        node_array[l - 1]->is_right_in_subright  = true;
    }
    #endif // DEBUG_GRAPH

    add_edge(node_array_outest[l - 1], sub_left2->pole_array[k - 1], !side);
    add_edge(node_array_outest[l - 1], sub_right2->pole_array[k - 1], side);
    #ifdef DEBUG_GRAPH
    if(node_array_outest[l - 1]->left == sub_left2->pole_array[k - 1]){
        node_array_outest[l - 1]->is_left_in_subleft  = true;
    }
    else if(node_array_outest[l - 1]->right == sub_left2->pole_array[k - 1]){
        node_array_outest[l - 1]->is_right_in_subleft  = true;
    }
    if(node_array_outest[l - 1]->left == sub_right2->pole_array[k - 1]){
        node_array_outest[l - 1]->is_left_in_subright  = true;
    }
    else if(node_array_outest[l - 1]->right == sub_right2->pole_array[k - 1]){
        node_array_outest[l - 1]->is_right_in_subright  = true;
    }
    #endif // DEBUG_GRAPH
}

/**
 * Creating the actual instance of Valiant's DAG
 * @param k number of gates of universal circuit
 * @param u number of inputs of universal circuit
 * @param v number of outputs of universal circuit
 * @param side default side in which edges should be added
 * @param outest true if it is the outest universal graph and not a subgraph
 * @return universal graph
 */
Valiant_DAG* init_merged(uint32_t gate, uint32_t u, uint32_t v, bool side, bool outest){
    uint32_t n = gate + u + v;
    Valiant_DAG* g = new Valiant_DAG(gate, u, v, side, outest);
	if(n <= 4){
		return g;
	}
    if((v % 2 == 0 && v != 0 && n % 2 == 1) || (v % 2 == 1 && n % 2 == 0)){
        v--;
        gate++;
    }
    uint32_t anum = (n-2)/2;
    if(n % 2 == 1){
        anum++;
    }
    Valiant_DAG* a = init(anum, !side);
    Valiant_DAG* b = init((n-2)/2, side);
    Valiant_DAG* c = init(anum, side);
    Valiant_DAG* d = init((n-2)/2, !side);

    g->sub_left = a;
    g->sub_right = b;
    g->sub_left2 = c;
    g->sub_right2 = d;
    for(uint32_t i = 0; i < anum; ++i){
        c->pole_array[i]->is_node2 = true;
    }
    for(uint32_t i = 0; i < (n-2)/2; ++i){
        d->pole_array[i]->is_node2 = true;
    }

	uint32_t l = 1;
	if(u < 2){
        l = 2;
	}
	if(n % 2 == 1){
		for (uint32_t k = 1; k < (n - 2)/2 + 1; ++k){
            if(k <= (u + gate)/2){
                g->gates(side, l, k);
            }

            if(l >= u/2){
                g->add_edge(g->sub_left->pole_array[k - 1], g->node_array[l], side);
                g->add_edge(g->sub_right->pole_array[k - 1], g->node_array[l], !side);
                #ifdef DEBUG_GRAPH
                if(g->sub_left->pole_array[k - 1]->left == g->node_array[l]){
                    g->sub_left->pole_array[k - 1]->is_left_outer = true;
                }
                else if(g->sub_left->pole_array[k - 1]->right == g->node_array[l]){
                    g->sub_left->pole_array[k - 1]->is_right_outer = true;
                }
                if(g->sub_right->pole_array[k - 1]->left == g->node_array[l]){
                    g->sub_right->pole_array[k - 1]->is_left_outer = true;
                }
                else if(g->sub_right->pole_array[k - 1]->right == g->node_array[l]){
                    g->sub_right->pole_array[k - 1]->is_right_outer = true;
                }
                #endif

                g->add_edge(g->sub_left2->pole_array[k - 1], g->node_array_outest[l], !side);
                g->add_edge(g->sub_right2->pole_array[k - 1], g->node_array_outest[l], side);

                #ifdef DEBUG_GRAPH
                if(g->sub_left2->pole_array[k - 1]->left == g->node_array_outest[l]){
                    g->sub_left2->pole_array[k - 1]->is_left_outer = true;
                }
                else if(g->sub_left2->pole_array[k - 1]->right == g->node_array_outest[l]){
                    g->sub_left2->pole_array[k - 1]->is_right_outer = true;
                }
                if(g->sub_right2->pole_array[k - 1]->left == g->node_array_outest[l]){
                    g->sub_right2->pole_array[k - 1]->is_left_outer = true;
                }
                else if(g->sub_right2->pole_array[k - 1]->right == g->node_array_outest[l]){
                    g->sub_right2->pole_array[k - 1]->is_right_outer = true;
                }
                #endif // DEBUG_GRAPH

                if(k < (u + gate)/2){
                    l += 3;
                }
                else{
                    l += 1;
                }
            }
            else{
                l += 1;
            }
		}
		if(v < 2){
            g->add_edge(g->node_array[l - 1], g->sub_left->pole_array[g->sub_left->pole_number - 1], side);
            g->add_edge(g->node_array_outest[l - 1], g->sub_left2->pole_array[g->sub_left2->pole_number - 1], !side);
            #ifdef DEBUG_GRAPH
            if(g->node_array[l - 1]->left == g->sub_left->pole_array[g->sub_left->pole_number - 1]){
                g->node_array[l - 1]->is_left_in_subleft  = true;
            }
            else if(g->node_array[l - 1]->right == g->sub_left->pole_array[g->sub_left->pole_number - 1]){
                g->node_array[l - 1]->is_right_in_subleft  = true;
            }
            if(g->node_array_outest[l - 1]->left == g->sub_left2->pole_array[g->sub_left2->pole_number - 1]){
                g->node_array_outest[l - 1]->is_left_in_subleft  = true;
            }
            else if(g->node_array_outest[l - 1]->right == g->sub_left2->pole_array[g->sub_left2->pole_number - 1]){
                g->node_array_outest[l - 1]->is_right_in_subleft  = true;
            }
            #endif // DEBUG_GRAPH
		}

		g->add_edge(g->sub_left->pole_array[g->sub_left->pole_number - 1], g->pole_array[g->pole_number - 1], side);
		g->add_edge(g->sub_left2->pole_array[g->sub_left2->pole_number - 1], g->pole_array[g->pole_number - 1], !side);

		#ifdef DEBUG_GRAPH
        if(g->sub_left->pole_array[g->sub_left->pole_number - 1]->left == g->pole_array[g->pole_number - 1]){
            g->sub_left->pole_array[g->sub_left->pole_number - 1]->is_left_outer = true;
		}
		else if(g->sub_left->pole_array[g->sub_left->pole_number - 1]->right == g->pole_array[g->pole_number - 1]){
            g->sub_left->pole_array[g->sub_left->pole_number - 1]->is_right_outer = true;
		}
        if(g->sub_left2->pole_array[g->sub_left2->pole_number - 1]->left == g->pole_array[g->pole_number - 1]){
            g->sub_left2->pole_array[g->sub_left2->pole_number - 1]->is_left_outer = true;
		}
		else if(g->sub_left2->pole_array[g->sub_left2->pole_number - 1]->right == g->pole_array[g->pole_number - 1]){
            g->sub_left2->pole_array[g->sub_left2->pole_number - 1]->is_right_outer = true;
		}
		#endif
	}
	// Even case
	else{
		for (uint32_t k = 1; k < (n - 2)/2 + 1; ++k){
            if(k <= (u + gate)/2){
                g->gates(side, l, k);
            }

            if(l >= u/2){
                if(k != g->sub_left->pole_number){
                    g->add_edge(g->sub_left->pole_array[k - 1], g->node_array[l], side);
                    g->add_edge(g->sub_left2->pole_array[k - 1], g->node_array_outest[l], !side);

                    #ifdef DEBUG_GRAPH
                    if(g->sub_left->pole_array[k - 1]->left == g->node_array[l]){
                        g->sub_left->pole_array[k - 1]->is_left_outer = true;
                    }
                    else if(g->sub_left->pole_array[k - 1]->right == g->node_array[l]){
                        g->sub_left->pole_array[k - 1]->is_right_outer = true;
                    }
                    if(g->sub_left2->pole_array[k - 1]->left == g->node_array_outest[l]){
                        g->sub_left2->pole_array[k - 1]->is_left_outer = true;
                    }
                    else if(g->sub_left2->pole_array[k - 1]->right == g->node_array_outest[l]){
                        g->sub_left2->pole_array[k - 1]->is_right_outer = true;
                    }
                    #endif // DEBUG_GRAPH

                } //if the (k-1)th node is not the last subpole
                else{
                    g->add_edge(g->sub_left->pole_array[k - 1], g->pole_array[g->pole_number - 2], side);
                    g->add_edge(g->sub_left2->pole_array[k - 1], g->pole_array[g->pole_number - 2], !side);

                    #ifdef DEBUG_GRAPH
                    if(g->sub_left->pole_array[k - 1]->left == g->pole_array[g->pole_number - 2]){
                        g->sub_left->pole_array[k - 1]->is_left_outer = true;
                    }
                    else if(g->sub_left->pole_array[k - 1]->right == g->pole_array[g->pole_number - 2]){
                        g->sub_left->pole_array[k - 1]->is_right_outer = true;
                    }
                    if(g->sub_left2->pole_array[k - 1]->left == g->pole_array[g->pole_number - 2]){
                        g->sub_left2->pole_array[k - 1]->is_left_outer = true;
                    }
                    else if(g->sub_left2->pole_array[k - 1]->right == g->pole_array[g->pole_number - 2]){
                        g->sub_left2->pole_array[k - 1]->is_right_outer = true;
                    }
                    #endif // DEBUG_GRAPH
                }
                if(v > 1 && k == g->sub_right->pole_number){ //NEW_OUTPUT
                    g->add_edge(g->sub_right->pole_array[k - 1], g->pole_array[g->pole_number - 1], !side);
                    g->add_edge(g->sub_right2->pole_array[k - 1], g->pole_array[g->pole_number - 1], side);

                    #ifdef DEBUG_GRAPH
                    if(g->sub_right->pole_array[k - 1]->left == g->pole_array[g->pole_number - 1]){
                        g->sub_right->pole_array[k - 1]->is_left_outer = true;
                    }
                    else if(g->sub_right->pole_array[k - 1]->right == g->pole_array[g->pole_number - 1]){
                        g->sub_right->pole_array[k - 1]->is_right_outer = true;
                    }
                    if(g->sub_right2->pole_array[k - 1]->left == g->pole_array[g->pole_number - 1]){
                        g->sub_right2->pole_array[k - 1]->is_left_outer = true;
                    }
                    else if(g->sub_right2->pole_array[k - 1]->right == g->pole_array[g->pole_number - 1]){
                        g->sub_right2->pole_array[k - 1]->is_right_outer = true;
                    }
                    #endif // DEBUG_GRAPH
                }
                else{ //(v <= 1 || k != g->sub_right->pole_number){ //NEW_OUTPUT
                    g->add_edge(g->sub_right->pole_array[k - 1], g->node_array[l], !side);
                    g->add_edge(g->sub_right2->pole_array[k - 1], g->node_array_outest[l], side);

                    #ifdef DEBUG_GRAPH
                    if(g->sub_right->pole_array[k - 1]->left == g->node_array[l]){
                        g->sub_right->pole_array[k - 1]->is_left_outer = true;
                    }
                    else if(g->sub_right->pole_array[k - 1]->right == g->node_array[l]){
                        g->sub_right->pole_array[k - 1]->is_right_outer = true;
                    }
                    if(g->sub_right2->pole_array[k - 1]->left == g->node_array_outest[l]){
                        g->sub_right2->pole_array[k - 1]->is_left_outer = true;
                    }
                    else if(g->sub_right2->pole_array[k - 1]->right == g->node_array_outest[l]){
                        g->sub_right2->pole_array[k - 1]->is_right_outer = true;
                    }
                    #endif // DEBUG_GRAPH
                }

                if(k < (u + gate)/2){
                    l += 3;
                }
                else{
                    l += 1;
                }
            }
            else{
                l += 1;
            }
		}
	}
	return g;
}


/**
 * Creating the actual instance of Valiant's DAG.
 * @param n size of universal graph
 * @param side default side in which edges should be added
 * @param outest true if it is the outest universal graph and not a subgraph
 * @return universal graph
 */
Valiant_DAG* init(uint32_t n, bool side){
	Valiant_DAG* g = new Valiant_DAG(n, 0, 0, side, false);
	uint32_t anum = (n-2)/2;
	if(n <= 4){
		return g;
	}
	else{
        if(n % 2 == 1){
            anum++;
        }
        Valiant_DAG* a = init(anum, !side);
        Valiant_DAG* b = init((n-2)/2, side);
        g->sub_left = a;
        g->sub_right = b;

        // First edges
        Valiant_DAG::Node* first_pole = g->pole_array[1];
        Valiant_DAG::Node* zero_node = g->node_array[0];
        Valiant_DAG::Node* first_node = g->node_array[1];
        g->add_edge(g->pole_array[0], zero_node, side); //p1-> 1
        g->add_edge(zero_node, first_pole, side); //1 -> p2
        g->add_edge(zero_node, first_node, !side); //1 -> 2
        g->add_edge(first_pole, first_node, side); //p2-> 2
	}
	if(n % 2 == 1){
        g->init_even_odd(n, side, false);
	}
	else{
        g->init_even_odd(n, side, true);
	}

	return g;
}

/**
 * Destructor universal graph.
 */
Valiant_DAG::~Valiant_DAG(){
    for (uint32_t i = 0; i < pole_number; i++){
        delete pole_array[i];
    }
    for (uint32_t j = 0; j < node_number; j++){
        delete node_array[j];
    }
}

/**
 * Delete universal graph.
 * @param g universal graph to delete
 */
void clearup(Valiant_DAG* g){
	if(g->sub_left){
		clearup(g->sub_left);
	}
	if(g->sub_right){
		clearup(g->sub_right);
	}
	g->~Valiant_DAG();
}
