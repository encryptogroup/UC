/**
 \file 		print_graph.cpp
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
 \brief		Implementation of function for printing the graph for graphviz: this is strictly for debugging purposes and needs
            to be enabled by defining DEBUG_GRAPH. Then, the output file can be inputted in graphviz and the
            circuit can be debugged by means of the graph.
 */

//NOTE: NOT CHECKED, might have functions that do not work anymore
//print_Graph_full_merged_top_order() function is the best to be used and it works!

#include "print_graph.h"

/** Print simple Gamma2 graph into gamma2.txt file */
void print_Gamma2_simple(DAG_Gamma2* g){
    ofstream file;
    file.open ("../graphviz/gamma2.txt");
	file << "digraph{" << endl;

    for (uint32_t i = 0; i < g->node_number; i++){
        if(g->node_array[i]->right){
            file << "m" << g->node_array[i]->number << " -> m" << g->node_array[i]->right->number << ";" << endl;
        }
        if(g->node_array[i]->left){
            file << "m" << g->node_array[i]->number << " -> m" << g->node_array[i]->left->number << ";" << endl;
        }
    }
    file << "}" << endl;
	file.close();
}

/** Print Gamma2 supergraph into gamma2_super.txt file */
void print_gamma2_full(DAG_Gamma2* g){
    ofstream file;
	file.open ("../graphviz/gamma2_super.txt");
	file << "digraph {" << endl;
	file.close();

	string substring = "";
	print_gamma2(g, substring);

	file.open ("../graphviz/gamma2_super.txt", std::ofstream::app);
	file << "}" << endl;
	file.close();
}

/** Print Gamma2 supergraph into gamma2_super.txt file */
void print_gamma2(DAG_Gamma2* g, string substring){
    ofstream file;
	file.open ("../graphviz/gamma2_super.txt", std::ofstream::app);
	for(uint32_t i = 0; i < g->node_number; i++){
        if(g->node_array[i]->left){
            file << substring << g->node_array[i]->number << " -> " << substring << g->node_array[i]->left->number;
            if(g->gamma1_left->node_array[i]->child && g->gamma1_left->node_array[i]->child->number == g->node_array[i]->left->number){
                file << "[color = blue];";
            }
            file << endl;
        }
        if(g->node_array[i]->right){
            file << substring << g->node_array[i]->number << " -> " << substring << g->node_array[i]->right->number;
            if(g->gamma1_left->node_array[i]->child && g->gamma1_left->node_array[i]->child->number == g->node_array[i]->right->number && g->gamma1_left->node_array[i]->child->number != g->node_array[i]->left->number){
                file << "[color = blue];";
            }
            file << endl;
        }
	}
	string substring1 = substring + "sl";
	string substring2 = substring + "sr";
	if(g->sub_left){
		print_gamma2(g->sub_left, substring1);
	}
	if(g->sub_right){
		print_gamma2(g->sub_right, substring2);
	}
	file.close();
}

/** Print the full Valiant graph */
void print_Graph_full(Valiant_DAG* g, bool top_order, bool merged){
	ofstream file;
	file.open ("../graphviz/graph.txt");
	file << "digraph {" << endl;
	if(merged){
        file << "node [shape = circle, style = filled, fillcolor = white, width = 0.2];";
	}
	file.close();

	string substring = "";
	if(merged){
        print_labels_merged(g, substring, substring, true);
        substring = "";
        print_Graph_merged(g, substring, substring, true);
	}
	else if(top_order){
        print_Graph(g, substring, substring, true);
	}
    else{
        print_Graph(g, substring, substring, false);
	}

	file.open ("../graphviz/graph.txt", std::ofstream::app);
	file << "}" << endl;
	file.close();
}

uint32_t getnum(Valiant_DAG::Node* n, bool top_order){
    if(top_order){
        return n->top_order;
    }
    return n->number;
}

void print_Graph(Valiant_DAG* g, string substring1, string substring2, bool top_order){
	ofstream file;
	file.open ("../graphviz/graph.txt", std::ofstream::app);
	string str;
	for(uint32_t i = 0; i < g->pole_number; ++i){
		if(g->pole_array[i]->left){
			if(g->pole_array[i]->left->is_pole){
				if(g->pole_array[i]->is_left_in_subleft){
					file << substring1 << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring1 << "sl" << "p" << getnum(g->pole_array[i]->left, top_order) << ";" << endl;
				}
				else if(g->pole_array[i]->is_left_in_subright){
					file << substring1 << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring1 << "sr" << "p" << getnum(g->pole_array[i]->left, top_order) << ";" << endl;
				}
				else if(g->pole_array[i]->is_left_outer){
					str = substring1.substr(substring1.size()-2,2);
					substring1.erase(substring1.end()-2, substring1.end());
					file << substring1 << str << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring1 << "p" << getnum(g->pole_array[i]->left, top_order) << ";" << endl;
					substring1 += str;
				}
				else{
					file << substring1 << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring1 << "p" << getnum(g->pole_array[i]->left, top_order) << ";" << endl;
				}
			}
			else{
				if(g->pole_array[i]->is_left_in_subleft){
					file << substring1 << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring1 << "sl" << "n" << getnum(g->pole_array[i]->left, top_order) << ";" << endl;
				}
				else if(g->pole_array[i]->is_left_in_subright){
					file << substring1 << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring1 << "sr" << "n" << getnum(g->pole_array[i]->left, top_order) << ";" << endl;
				}
				else if(g->pole_array[i]->is_left_outer){
					str = substring1.substr(substring1.size()-2,2);
					substring1.erase(substring1.end()-2, substring1.end());
					file << substring1 << str << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring1 << "n" << getnum(g->pole_array[i]->left, top_order) << ";" << endl;
					substring1 += str;
				}
				else{
					file << substring1 << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring1 << "n" << getnum(g->pole_array[i]->left, top_order) << ";" << endl;
				}
			}
		}
		if(g->pole_array[i]->right){
			if(g->pole_array[i]->right->is_pole){
				if(g->pole_array[i]->is_right_in_subright){
					file << substring2 << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring2 << "sr" << "p" << getnum(g->pole_array[i]->right, top_order) << ";" << endl;
				}
				else if(g->pole_array[i]->is_right_in_subleft){
					file << substring2 << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring2 << "sl" << "p" << getnum(g->pole_array[i]->right, top_order) << ";" << endl;
				}
				else if(g->pole_array[i]->is_right_outer){
					str = substring2.substr(substring2.size()-2,2);
					substring2.erase(substring2.end()-2, substring2.end());
					file << substring2 << str << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring2 << "p" << getnum(g->pole_array[i]->right, top_order) << ";" << endl;
					substring2 += str;
				}
				else{
					file << substring2 << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring2 << "p" << getnum(g->pole_array[i]->right, top_order) << ";" << endl;
				}
			}
			else{
				if(g->pole_array[i]->is_right_in_subright){
					file << substring2 << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring2 << "sr" << "n" << getnum(g->pole_array[i]->right, top_order) << ";" << endl;
				}
				else if(g->pole_array[i]->is_right_in_subleft){
					file << substring2 << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring2 << "sl" << "n" << getnum(g->pole_array[i]->right, top_order) << ";" << endl;
				}
				else if(g->pole_array[i]->is_right_outer){
					str = substring2.substr(substring2.size()-2,2);
					substring2.erase(substring2.end()-2, substring2.end());
					file << substring2 << str << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring2 << "n" << getnum(g->pole_array[i]->right, top_order) << ";" << endl;
					substring2 += str;
				}
				else{
					file << substring2 << "p" << getnum(g->pole_array[i], top_order) << " -> " << substring2 << "n" << getnum(g->pole_array[i]->right, top_order) << ";" << endl;
				}
			}
		}
	}
	for(uint32_t i = 0; i < g->node_number; ++i){
		if(g->node_array[i]->left){
			if(g->node_array[i]->left->is_pole){
				if(g->node_array[i]->is_left_in_subleft){
					file << substring1 << "n" << getnum(g->node_array[i], top_order) << " -> " << substring1 << "sl" << "p" << getnum(g->node_array[i]->left, top_order) << ";" << endl;
				}
				else if(g->node_array[i]->is_left_in_subright){
					file << substring1 << "n" << getnum(g->node_array[i], top_order) << " -> " << substring1 << "sr" << "p" << getnum(g->node_array[i]->left, top_order) << ";" << endl;
				}
				else if(g->node_array[i]->is_left_outer){
					str = substring1.substr(substring1.size()-2,2);
					substring1.erase(substring1.end()-2, substring1.end());
					file << substring1 << str << "n" << getnum(g->node_array[i], top_order) << " -> " << substring1 << "p" << getnum(g->node_array[i]->left, top_order) << ";" << endl;
					substring1 += str;
				}
				else{
					file << substring1 << "n" << getnum(g->node_array[i], top_order) << " -> " << substring1 << "p" << getnum(g->node_array[i]->left, top_order) << ";" << endl;
				}
			}
			else{
				if(g->node_array[i]->is_left_in_subleft){
					file << substring1 << "n" << getnum(g->node_array[i], top_order) << " -> " << substring1 << "sl" << "n" << getnum(g->node_array[i]->left, top_order) << ";" << endl;
				}
				else if(g->node_array[i]->is_left_in_subright){
					file << substring1 << "n" << getnum(g->node_array[i], top_order) << " -> " << substring1 << "sr" << "n" << getnum(g->node_array[i]->left, top_order) << ";" << endl;
				}
				else if(g->node_array[i]->is_left_outer){
					str = substring1.substr(substring1.size()-2,2);
					substring1.erase(substring1.end()-2, substring1.end());
					file << substring1 << str << "n" << getnum(g->node_array[i], top_order) << " -> " << substring1 << "n" << getnum(g->node_array[i]->left, top_order) << ";" << endl;
					substring1 += str;
				}
				else{
					file << substring1 << "n" << getnum(g->node_array[i], top_order) << " -> " << substring1 << "n" << getnum(g->node_array[i]->left, top_order) << ";" << endl;
				}
			}
		}
		if(g->node_array[i]->right){
			if(g->node_array[i]->right->is_pole){
				if(g->node_array[i]->is_right_in_subright){
					file << substring2 << "n" << getnum(g->node_array[i], top_order) << " -> " << substring2 << "sr" << "p" << getnum(g->node_array[i]->right, top_order) << ";" << endl;
				}
				else if(g->node_array[i]->is_right_in_subleft){
					file << substring2 << "n" << getnum(g->node_array[i], top_order) << " -> " << substring2 << "sl" << "p" << getnum(g->node_array[i]->right, top_order) << ";" << endl;
				}
				else if(g->node_array[i]->is_right_outer){
					str = substring2.substr(substring2.size()-2,2);
					substring2.erase(substring2.end()-2, substring2.end());
					file << substring2 << str << "n" << getnum(g->node_array[i], top_order) << " -> " << substring2 << "p" << getnum(g->node_array[i]->right, top_order) << ";" << endl;
					substring2 += str;
				}
				else{
					file << substring2 << "n" << getnum(g->node_array[i], top_order) << " -> " << substring2 << "p" << getnum(g->node_array[i]->right, top_order) << ";" << endl;
				}
			}
			else{
				if(g->node_array[i]->is_right_in_subright){
					file << substring2 << "n" << getnum(g->node_array[i], top_order) << " -> " << substring2 << "sr" << "n" << getnum(g->node_array[i]->right, top_order) << ";" << endl;
				}
				else if(g->node_array[i]->is_right_in_subleft){
					file << substring2 << "n" << getnum(g->node_array[i], top_order) << " -> " << substring2 << "sl" << "n" << getnum(g->node_array[i]->right, top_order) << ";" << endl;
				}
				else if(g->node_array[i]->is_right_outer){
					str = substring2.substr(substring2.size()-2,2);
					substring2.erase(substring2.end()-2, substring2.end());
					file << substring2 << str << "n" << getnum(g->node_array[i], top_order) << " -> " << substring2 << "n" << getnum(g->node_array[i]->right, top_order) << ";" << endl;
					substring2 += str;
				}
				else{
					file << substring2 << "n" << getnum(g->node_array[i], top_order) << " -> " << substring2 << "n" << getnum(g->node_array[i]->right, top_order) << ";" << endl;
				}
			}
		}
	}
	if(g->sub_left){
		substring1 += "sl";
		print_Graph(g->sub_left, substring1, substring1, top_order);
	}
	if(g->sub_right){
		substring2 += "sr";
		print_Graph(g->sub_right, substring2, substring2, top_order);
	}
	file.close();
}

void print_labels_merged(Valiant_DAG* g, string substring1, string substring2, bool outest){
    ofstream file;
	file.open ("../graphviz/graph.txt", std::ofstream::app);
	string str;
	file << endl;

	for(uint32_t i = 0; i < g->pole_number; ++i){
        if(g->pole_array[i]->is_set){
            file << "node [label = \"" << substring1 << "p" << g->pole_array[i]->top_order << " " << g->pole_array[i]->control_num << "\"];"  << endl;
            file << substring1 << "p" << g->pole_array[i]->top_order << ";" << endl;
        }
        else{
            file << "node [label = \"" << substring1 << "p" << g->pole_array[i]->top_order << "\"];" << endl;
            file << substring1 << "p" << g->pole_array[i]->top_order << ";" << endl;
        }
	}
	for(uint32_t i = 0; i < g->node_number; ++i){
	    if(g->node_array[i]->is_set){
            file << "node [label = \"" << substring1 << "n" << g->node_array[i]->top_order << " " << g->node_array[i]->control_num << "\"];" << endl;
            file << substring1 << "n" << g->node_array[i]->top_order << ";" << endl;
	    }
	    else{
            file << "node [label = \"" << substring1 << "n" << g->node_array[i]->top_order << "\"];" << endl;
            file << substring1 << "n" << g->node_array[i]->top_order << ";" << endl;
	    }
	}
	if(outest){
        for(uint32_t i = 0; i < g->node_number; ++i){
            if(g->node_array_outest[i]->is_set){
                file << "node [label = \"" << substring1 << "n" << g->node_array_outest[i]->top_order << " " << g->node_array_outest[i]->control_num << "\"];" << endl;
                file << substring1 << "n" << g->node_array_outest[i]->top_order << ";" << endl;
            }
            else{
                file << "node [label = \"" << substring1 << "n" << g->node_array_outest[i]->top_order << "\"];" << endl;
                file << substring1 << "n" << g->node_array_outest[i]->top_order << ";" << endl;
            }
        }
	}
	if(g->sub_left){
		substring1 += "sl";
		print_labels_merged(g->sub_left, substring1, substring1, false);
		if(outest){
            print_labels_merged(g->sub_left2, substring1, substring1, false);
		}
	}
	if(g->sub_right){
		substring2 += "sr";
		print_labels_merged(g->sub_right, substring2, substring2, false);
		if(outest){
            print_labels_merged(g->sub_right2, substring2, substring2, false);
		}
	}

	file.close();
}

void print_Graph_merged(Valiant_DAG* g, string substring1, string substring2, bool outest){
	ofstream file;
	file.open ("../graphviz/graph.txt", std::ofstream::app);
	string str;
	file << endl;

	for(uint32_t i = 0; i < g->pole_number; ++i){
		if(g->pole_array[i]->left){
            if(g->pole_array[i]->left->is_pole){
				if(g->pole_array[i]->is_left_in_subleft){
					file << substring1 << "p" << g->pole_array[i]->top_order << " -> " << substring1 << "sl" << "p" << g->pole_array[i]->left->top_order;
				}
				else if(g->pole_array[i]->is_left_in_subright){
					file << substring1 << "p" << g->pole_array[i]->top_order << " -> " << substring1 << "sr" << "p" << g->pole_array[i]->left->top_order;
				}
				else if(g->pole_array[i]->is_left_outer){
					str = substring1.substr(substring1.size()-2,2);
					substring1.erase(substring1.end()-2, substring1.end());
					file << substring1 << str << "p" << g->pole_array[i]->top_order << " -> " << substring1 << "p" << g->pole_array[i]->left->top_order;
					substring1 += str;
				}
				else{
					file << substring1 << "p" << g->pole_array[i]->top_order << " -> " << substring1 << "p" << g->pole_array[i]->left->top_order;
				}
			}
			else{
				if(g->pole_array[i]->is_left_in_subleft){
					file << substring1 << "p" << g->pole_array[i]->top_order << " -> " << substring1 << "sl" << "n" << g->pole_array[i]->left->top_order;
				}
				else if(g->pole_array[i]->is_left_in_subright){
					file << substring1 << "p" << g->pole_array[i]->top_order << " -> " << substring1 << "sr" << "n" << g->pole_array[i]->left->top_order;
				}
				else if(g->pole_array[i]->is_left_outer){
					str = substring1.substr(substring1.size()-2,2);
					substring1.erase(substring1.end()-2, substring1.end());
					file << substring1 << str << "p" << g->pole_array[i]->top_order << " -> " << substring1 << "n" << g->pole_array[i]->left->top_order;
					substring1 += str;
				}
				else{
					file << substring1 << "p" << g->pole_array[i]->top_order << " -> " << substring1 << "n" << g->pole_array[i]->left->top_order;
				}
			}
			/*Write out if it is the first or second input of the next node*/
            if(g->pole_array[i]->left->left_parent && g->pole_array[i]->left->left_parent == g->pole_array[i])
                file << "[label=\"1\"];" << endl;
            else
                file << "[label=\"2!!!\"];" << endl;
		}
		if(g->pole_array[i]->right){
			if(g->pole_array[i]->right->is_pole){
				if(g->pole_array[i]->is_right_in_subright){
					file << substring2 << "p" << g->pole_array[i]->top_order << " -> " << substring2 << "sr" << "p" << g->pole_array[i]->right->top_order;
				}
				else if(g->pole_array[i]->is_right_in_subleft){
					file << substring2 << "p" << g->pole_array[i]->top_order << " -> " << substring2 << "sl" << "p" << g->pole_array[i]->right->top_order;
				}
				else if(g->pole_array[i]->is_right_outer){
					str = substring2.substr(substring2.size()-2,2);
					substring2.erase(substring2.end()-2, substring2.end());
					file << substring2 << str << "p" << g->pole_array[i]->top_order << " -> " << substring2 << "p" << g->pole_array[i]->right->top_order;
					substring2 += str;
				}
				else{
					file << substring2 << "p" << g->pole_array[i]->top_order << " -> " << substring2 << "p" << g->pole_array[i]->right->top_order;
				}
			}
			else{
				if(g->pole_array[i]->is_right_in_subright){
					file << substring2 << "p" << g->pole_array[i]->top_order << " -> " << substring2 << "sr" << "n" << g->pole_array[i]->right->top_order;
				}
				else if(g->pole_array[i]->is_right_in_subleft){
					file << substring2 << "p" << g->pole_array[i]->top_order << " -> " << substring2 << "sl" << "n" << g->pole_array[i]->right->top_order;
				}
				else if(g->pole_array[i]->is_right_outer){
					str = substring2.substr(substring2.size()-2,2);
					substring2.erase(substring2.end()-2, substring2.end());
					file << substring2 << str << "p" << g->pole_array[i]->top_order << " -> " << substring2 << "n" << g->pole_array[i]->right->top_order;
					substring2 += str;
				}
				else{
					file << substring2 << "p" << g->pole_array[i]->top_order << " -> " << substring2 << "n" << g->pole_array[i]->right->top_order;
				}
			}
			/*Write out if it is the first or second input of the next node*/
			if(g->pole_array[i]->right->left_parent && g->pole_array[i]->right->left_parent == g->pole_array[i])
                file << "[label=\"1!!!\"];" << endl;
            else
                file << "[label=\"2\"];" << endl;
		}


	}
	for(uint32_t i = 0; i < g->node_number; ++i){
		if(g->node_array[i]->left){
			if(g->node_array[i]->left->is_pole){
				if(g->node_array[i]->is_left_in_subleft){
					file << substring1 << "n" << g->node_array[i]->top_order << " -> " << substring1 << "sl" << "p" << g->node_array[i]->left->top_order;
				}
				else if(g->node_array[i]->is_left_in_subright){
					file << substring1 << "n" << g->node_array[i]->top_order << " -> " << substring1 << "sr" << "p" << g->node_array[i]->left->top_order;
				}
				else if(g->node_array[i]->is_left_outer){
					str = substring1.substr(substring1.size()-2,2);
					substring1.erase(substring1.end()-2, substring1.end());
					file << substring1 << str << "n" << g->node_array[i]->top_order << " -> " << substring1 << "p" << g->node_array[i]->left->top_order;
					substring1 += str;
				}
				else{
					file << substring1 << "n" << g->node_array[i]->top_order << " -> " << substring1 << "p" << g->node_array[i]->left->top_order;
				}
			}
			else{
				if(g->node_array[i]->is_left_in_subleft){
					file << substring1 << "n" << g->node_array[i]->top_order << " -> " << substring1 << "sl" << "n" << g->node_array[i]->left->top_order;
				}
				else if(g->node_array[i]->is_left_in_subright){
					file << substring1 << "n" << g->node_array[i]->top_order << " -> " << substring1 << "sr" << "n" << g->node_array[i]->left->top_order;
				}
				else if(g->node_array[i]->is_left_outer){
					str = substring1.substr(substring1.size()-2,2);
					substring1.erase(substring1.end()-2, substring1.end());
					file << substring1 << str << "n" << g->node_array[i]->top_order << " -> " << substring1 << "n" << g->node_array[i]->left->top_order;
					substring1 += str;
				}
				else{
					file << substring1 << "n" << g->node_array[i]->top_order << " -> " << substring1 << "n" << g->node_array[i]->left->top_order;
				}
			}
			/*Write out if it is the first or second input of the next node*/
            if(g->node_array[i]->left->left_parent && g->node_array[i]->left->left_parent == g->node_array[i])
                file << "[label=\"1\"];" << endl;
            else
                file << "[label=\"2!!!\"];" << endl;
		}
		if(g->node_array[i]->right){
			if(g->node_array[i]->right->is_pole){
				if(g->node_array[i]->is_right_in_subright){
					file << substring2 << "n" << g->node_array[i]->top_order << " -> " << substring2 << "sr" << "p" << g->node_array[i]->right->top_order;
				}
				else if(g->node_array[i]->is_right_in_subleft){
					file << substring2 << "n" << g->node_array[i]->top_order << " -> " << substring2 << "sl" << "p" << g->node_array[i]->right->top_order;
				}
				else if(g->node_array[i]->is_right_outer){
					str = substring2.substr(substring2.size()-2,2);
					substring2.erase(substring2.end()-2, substring2.end());
					file << substring2 << str << "n" << g->node_array[i]->top_order << " -> " << substring2 << "p" << g->node_array[i]->right->top_order;
					substring2 += str;
				}
				else{
					file << substring2 << "n" << g->node_array[i]->top_order << " -> " << substring2 << "p" << g->node_array[i]->right->top_order;
				}
			}
			else{
				if(g->node_array[i]->is_right_in_subright){
					file << substring2 << "n" << g->node_array[i]->top_order << " -> " << substring2 << "sr" << "n" << g->node_array[i]->right->top_order;
				}
				else if(g->node_array[i]->is_right_in_subleft){
					file << substring2 << "n" << g->node_array[i]->top_order << " -> " << substring2 << "sl" << "n" << g->node_array[i]->right->top_order;
				}
				else if(g->node_array[i]->is_right_outer){
					str = substring2.substr(substring2.size()-2,2);
					substring2.erase(substring2.end()-2, substring2.end());
					file << substring2 << str << "n" << g->node_array[i]->top_order << " -> " << substring2 << "n" << g->node_array[i]->right->top_order;
					substring2 += str;
				}
				else{
					file << substring2 << "n" << g->node_array[i]->top_order << " -> " << substring2 << "n" << g->node_array[i]->right->top_order;
				}
			}
			/*Write out if it is the first or second input of the next node*/
			if(g->node_array[i]->right->left_parent && g->node_array[i]->right->left_parent == g->node_array[i])
                file << "[label=\"1!!!\"];" << endl;
            else
                file << "[label=\"2\"];" << endl;
		}
	}
	if(outest){
        for(uint32_t i = 0; i < g->node_number; ++i){
            if(g->node_array_outest[i]->left){
                if(g->node_array_outest[i]->left->is_pole){
                    if(g->node_array_outest[i]->is_left_in_subleft){
                        file << substring1 << "n" << g->node_array_outest[i]->top_order << " -> " << substring1 << "sl" << "p" << g->node_array_outest[i]->left->top_order;
                    }
                    else if(g->node_array_outest[i]->is_left_in_subright){
                        file << substring1 << "n" << g->node_array_outest[i]->top_order << " -> " << substring1 << "sr" << "p" << g->node_array_outest[i]->left->top_order;
                    }
                    else if(g->node_array_outest[i]->is_left_outer){
                        str = substring1.substr(substring1.size()-2,2);
                        substring1.erase(substring1.end()-2, substring1.end());
                        file << substring1 << str << "n" << g->node_array_outest[i]->top_order << " -> " << substring1 << "p" << g->node_array_outest[i]->left->top_order;
                        substring1 += str;
                    }
                    else{
                        file << substring1 << "n" << g->node_array_outest[i]->top_order << " -> " << substring1 << "p" << g->node_array_outest[i]->left->top_order;
                    }
                }
                else{
                    if(g->node_array_outest[i]->is_left_in_subleft){
                        file << substring1 << "n" << g->node_array_outest[i]->top_order << " -> " << substring1 << "sl" << "n" << g->node_array_outest[i]->left->top_order;
                    }
                    else if(g->node_array_outest[i]->is_left_in_subright){
                        file << substring1 << "n" << g->node_array_outest[i]->top_order << " -> " << substring1 << "sr" << "n" << g->node_array_outest[i]->left->top_order;
                    }
                    else if(g->node_array_outest[i]->is_left_outer){
                        str = substring1.substr(substring1.size()-2,2);
                        substring1.erase(substring1.end()-2, substring1.end());
                        file << substring1 << str << "n" << g->node_array_outest[i]->top_order << " -> " << substring1 << "n" << g->node_array_outest[i]->left->top_order;
                        substring1 += str;
                    }
                    else{
                        file << substring1 << "n" << g->node_array_outest[i]->top_order << " -> " << substring1 << "n" << g->node_array_outest[i]->left->top_order;
                    }
                }
                /*Write out if it is the first or second input of the next node*/
                if(g->node_array_outest[i]->left->left_parent && g->node_array_outest[i]->left->left_parent == g->node_array_outest[i])
                    file << "[label=\"1\"];" << endl;
                else
                    file << "[label=\"2!!!\"];" << endl;
            }
            if(g->node_array_outest[i]->right){
                if(g->node_array_outest[i]->right->is_pole){
                    if(g->node_array_outest[i]->is_right_in_subright){
                        file << substring2 << "n" << g->node_array_outest[i]->top_order << " -> " << substring2 << "sr" << "p" << g->node_array_outest[i]->right->top_order;
                    }
                    else if(g->node_array_outest[i]->is_right_in_subleft){
                        file << substring2 << "n" << g->node_array_outest[i]->top_order << " -> " << substring2 << "sl" << "p" << g->node_array_outest[i]->right->top_order;
                    }
                    else if(g->node_array_outest[i]->is_right_outer){
                        str = substring2.substr(substring2.size()-2,2);
                        substring2.erase(substring2.end()-2, substring2.end());
                        file << substring2 << str << "n" << g->node_array_outest[i]->top_order << " -> " << substring2 << "p" << g->node_array_outest[i]->right->top_order;
                        substring2 += str;
                    }
                    else{
                        file << substring2 << "n" << g->node_array_outest[i]->top_order << " -> " << substring2 << "p" << g->node_array_outest[i]->right->top_order;
                    }
                }
                else{
                    if(g->node_array_outest[i]->is_right_in_subright){
                        file << substring2 << "n" << g->node_array_outest[i]->top_order << " -> " << substring2 << "sr" << "n" << g->node_array_outest[i]->right->top_order;
                    }
                    else if(g->node_array_outest[i]->is_right_in_subleft){
                        file << substring2 << "n" << g->node_array_outest[i]->top_order << " -> " << substring2 << "sl" << "n" << g->node_array_outest[i]->right->top_order;
                    }
                    else if(g->node_array_outest[i]->is_right_outer){
                        str = substring2.substr(substring2.size()-2,2);
                        substring2.erase(substring2.end()-2, substring2.end());
                        file << substring2 << str << "n" << g->node_array_outest[i]->top_order << " -> " << substring2 << "n" << g->node_array_outest[i]->right->top_order;
                        substring2 += str;
                    }
                    else{
                        file << substring2 << "n" << g->node_array_outest[i]->top_order << " -> " << substring2 << "n" << g->node_array_outest[i]->right->top_order;
                    }
                }
                /*Write out if it is the first or second input of the next node*/
                if(g->node_array_outest[i]->right->left_parent && g->node_array_outest[i]->right->left_parent == g->node_array_outest[i])
                    file << "[label=\"1!!!\"];" << endl;
                else
                    file << "[label=\"2\"];" << endl;
            }
        }
	}
	if(g->sub_left){
		substring1 += "sl";
		print_Graph_merged(g->sub_left, substring1, substring1, false);
		if(outest){
            print_Graph_merged(g->sub_left2, substring1, substring1, false);
		}
	}
	if(g->sub_right){
		substring2 += "sr";
		print_Graph_merged(g->sub_right, substring2, substring2, false);
		if(outest){
            print_Graph_merged(g->sub_right2, substring2, substring2, false);
		}
	}
	file.close();
}

