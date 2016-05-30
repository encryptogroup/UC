/**
 \file 		node.cpp
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
 \brief		Implementation of the Node class in Valiant_DAG class which are nodes of universal graphs
            with at most two incoming and at most two outgoing edge for each node.
 */

/**
 * Valiant_DAG::Node constructor
 * @param num the number identifying the node, everything else is set to false or null
 */
Valiant_DAG::Node::Node(uint32_t num)
  : number(num)
  , top_order(0)

  , top_ordered(false)
  , is_x(false)
  , is_node2(false)

  , is_first(false)
  , is_first2(false)
  , is_second(false)
  , is_second2(false)

  , firstof(0)
  , firstof2(0)
  , secondof(0)
  , secondof2(0)

  , left(0)
  , right(0)
  , left_parent(0)
  , right_parent(0)
  , next_top_order(0)

#ifdef DEBUG_GRAPH
  , is_left_outer(false)
  , is_right_outer(false)
  , is_left_in_subleft(false)
  , is_left_in_subright(false)
  , is_right_in_subleft(false)
  , is_right_in_subright(false)
  #endif // DEBUG_GRAPH

  , is_pole(false)
  , is_outest_pole(false)

  , control_num(0)
  , is_set(false)
  {}

/**
 * Setting the node's control_bit according to the embedding, i.e., according if the path goes further on the right or on the left
 * @param next next node in the path
 * @param previous previous node in the path
 */
void Valiant_DAG::Node::set_control_bit(Node* next, Node* previous){
    // This and next cannot be null -> error
	if(!(next) || !(this)){
		cerr << "PROBLEM in node::set_control_bit: failed set_control_bit (either next or this is null)" << this->number-1 << endl;
	}
	// Previous node is null, if we have the starting node, i.e., programming of poles, which are universal gates or outputs
	// or inputs, happens outside of this function
	else if(!previous){}
	// Previous node is not null
	else{
        if(previous == this->left_parent && this->left == next){
			if(this->is_set == false){
                //0: stays
				control_num = 0;
				this->is_set = true;
			}
			// Set nothing, already set well
			else if(this->is_set == true && control_num == 0) {}
			else{
                cerr << "PROBLEM in node::set_control_bit: failed set_control_bit (already set to opposite)  " << this->number-1 << endl;
			}
		}
		else if(previous == this->right_parent && this->right == next){
			if(this->is_set == false){
                //0: stays
				control_num = 0;
				this->is_set = true;
			}
			// Set nothing, already set well
			else if(this->is_set == true && control_num == 0) {}
			else{
                cerr << "PROBLEM in node::set_control_bit: failed set_control_bit (already set to opposite)  " << this->number-1 << endl;
			}
		}
		else if(previous == this->left_parent && this->right == next){
			if(this->is_set == false){
                //1: switch
				control_num = 1;
				this->is_set = true;
			}
			// Set nothing, already set well
			else if(this->is_set == true && control_num == 1) {}
			else{
				cerr << "PROBLEM in node::set_control_bit: failed set_control_bit (already set to opposite)  " << this->number-1 << endl;
			}
		}
		else if(previous == this->right_parent && this->left == next){
			if(this->is_set == false){
                //1: switch
				control_num = 1;
				this->is_set = true;
			}
			// Set nothing, already set well
			else if(this->is_set == true && control_num == 1) {}
			else{
				cerr << "PROBLEM in node::set_control_bit: failed set_control_bit (already set to opposite)  " << this->number-1 << endl;
			}
		}
		else{
            cerr << "PROBLEM in node::set_control_bit: something went wrong while trying to set control bit, either previous is not parent of this node or next is not child";
		}
	}
}

/**
 * Deleting all edges going in the node in the graph (used for input nodes)
 */
void Valiant_DAG::Node::delete_edge_to_node(){
    if(this->left_parent){
        this->left_parent->left = 0;
        this->left_parent = 0;
    }
    if(this->right_parent){
        this->right_parent->right = 0;
        this->right_parent = 0;
    }
}

