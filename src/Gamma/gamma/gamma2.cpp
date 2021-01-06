/**
 \file 		gamma2.cpp
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
 \brief		Implementation of Gamma2 class for graphs with at most two incoming and at most two
            outgoing edge for each node.
 */

#include "gamma2.h"
#include "../util/hybrid_size.h"

//#define DEBUG_GAMMA2

/**
 * Node constructor that takes its number num, by default it has no children
 * and parents, is not colored.
 * @param num the number that identifies the node
 */
DAG_Gamma2::Node::Node(uint32_t num)
    : number(num),
      left(0),
      right(0),
      left_parent(0),
      right_parent(0),
      colored(false),
      output(false),
      is_bitchanged(false) {
  this->set_function_bits(2, 2, 2, 2);
}

/**
 * Node cunstructor that takes another node
 * @param other other node
 */
DAG_Gamma2::Node::Node(const Node &other) {
  number = other.number;
  left = new Node(*(other.left));
  right = new Node(*(other.right));
  left_parent = new Node(*(other.left_parent));
  right_parent = new Node(*(other.right_parent));
  colored = other.colored;
  output = other.output;
  set_function_bits(other.function_bits[0], other.function_bits[1], other.function_bits[2], other.function_bits[3]);
  is_bitchanged = other.is_bitchanged;
}

/**
 * Check that the node exists and is not colored, returns true if so.
 */
bool DAG_Gamma2::Node::node_not_colored() {
  if (this && !(this->colored)) {
    return true;
  }
  return false;
}

/**
 * Find the other child or parent of a node given the first one as parameter.
 * @param other_node the already known child or parent Node
 * @param left_node the first child or parent Node
 * @param right_node the second child or parent Node
 * @return Node which is not the already known child or parent
 */
DAG_Gamma2::Node *DAG_Gamma2::Node::other_child_or_parent(DAG_Gamma2::Node *other_node,
                                                          DAG_Gamma2::Node *left_node,
                                                          DAG_Gamma2::Node *right_node) {
  if (left_node && right_node && left_node == other_node && right_node->node_not_colored()) {
    return right_node;
  } else if (left_node && right_node && right_node == other_node && left_node->node_not_colored()) {
    return left_node;
  }
  return 0;
}

/**
 * Set the function bits of the Node to the 4 bits specified by the inputs
 * @param c0 1st bit of function table
 * @param c1 2nd bit of function table
 * @param c2 3rd bit of function table
 * @param c3 4th bit of function table
 */
void DAG_Gamma2::Node::set_function_bits(uint16_t c0, uint16_t c1, uint16_t c2, uint16_t c3) {
  this->function_bits[0] = c0;
  this->function_bits[1] = c1;
  this->function_bits[2] = c2;
  this->function_bits[3] = c3;
}

/**
 * Gamma2 constructor that creates a Gamma2 graph with specified number of Nodes
 * @param num the number specifying the number of Nodes
 */
DAG_Gamma2::DAG_Gamma2(uint32_t num)
    : node_number(num), sub_left(0), sub_right(0), gamma1_left(0), gamma1_right(0) {
  node_array = new Node *[node_number];
  for (uint32_t i = 0; i < node_number; ++i) {
    node_array[i] = new Node(i + 1);
  }
}

/**
 * Gamma2 constructor that creates a Gamma2 graph from another Gamma2 graph
 * @param other other Gamma2 graph
 */
DAG_Gamma2::DAG_Gamma2(const DAG_Gamma2 &other) {
  node_number = other.node_number;
  sub_left = new DAG_Gamma2(*(other.sub_left)); //recursive
  sub_right = new DAG_Gamma2(*(other.sub_right)); //recursive
  gamma1_left = new DAG_Gamma1(*(other.gamma1_left));
  gamma1_right = new DAG_Gamma1(*(other.gamma1_right));
  node_array = new Node *[node_number];
  for (uint32_t i = 0; i < node_number; ++i) {
    node_array[i] = new Node(*(other.node_array[i]));
  }
}

/**
 * Add an edge between two nodes
 * @param n1 the starting Node of the edge
 * @param n2 the ending Node of the edge
 */
void DAG_Gamma2::add_edge(DAG_Gamma2::Node *n1, DAG_Gamma2::Node *n2) {
  if (!(n1->left) && !(n2->left_parent)) {
    n1->left = n2;
    n2->left_parent = n1;
  } else if (!(n1->left) && !(n2->right_parent)) {
    n1->left = n2;
    n2->right_parent = n1;
  } else if (!(n1->right) && !(n2->left_parent)) {
    n1->right = n2;
    n2->left_parent = n1;
  } else if (!(n1->right) && !(n2->right_parent)) {
    n1->right = n2;
    n2->right_parent = n1;
  } else {
    cerr << "PROBLEM in gamma2::add_edge: failed edge insert " << n1->number << " , " << n2->number << endl;
  }
}

/**
 * Adds edge to bipartite graph which is used for coloring.
 * @param tmp1 the first Gamma2 graph building up the bipartite graph
 * @param tmp2 the second Gamma2 graph building up the bipartite graph
 * @param i index of node
 * @param edge_num number of edges calculated so far
 */
void DAG_Gamma2::Node::prepare_bipartite(DAG_Gamma2 *tmp1, DAG_Gamma2 *tmp2, uint32_t i, uint32_t &edge_num) {
  if (this) {
    tmp1->add_edge(tmp1->node_array[i], tmp2->node_array[this->number - 1]);
    edge_num++;
    if (this->output) {
      tmp2->node_array[this->number - 1]->output = true;
    }
  }
}

/**
 * Find the smallest uncolored node that has a child
 * @return the Node for which colored = false and its number is the smallest (first in node_array) with child
 */
DAG_Gamma2::Node *DAG_Gamma2::smallest_uncolored_with_child() {
  DAG_Gamma2::Node *current;
  for (uint32_t i = 0; i < this->node_number; ++i) {
    current = this->node_array[i];
    if (!(current->colored) && (current->left || current->right)) {
      return current;
    }
  }
  return 0;
}

/**
 * Draw the edges of the two Gamma1 subgraphs of the Gamma2 graph
 * @param gamma1_graph the Gamma1 into which the edges have to be added based on the Gamma2 graph
 * @param preprocessing trueif we are in a preprocessing step of 4-way split
 * @param four_way true it the edges are added in a 4-way split
 */
void DAG_Gamma2::add_edges(DAG_Gamma1 *gamma1_graph, bool preprocessing, bool four_way) {
  DAG_Gamma1::Node *first = 0;
  DAG_Gamma1::Node *second = 0;
  for (uint32_t i = 0; i < gamma1_graph->node_number; i += 2) {
    first = gamma1_graph->node_array[i];
    if (i + 1 < gamma1_graph->node_number) {
      second = gamma1_graph->node_array[i + 1];
    }
    if (first && first->child && first->child != first && first->child != second) {
#ifdef DEBUG_GAMMA2
      cout << "edge " << i/2 << "  " << (first->child->number - 1)/2-1 << endl;
#endif
      if (preprocessing && four_way) {
        this->add_edge(this->node_array[i / 2], this->node_array[(first->child->number - 1) / 2]);
      } else {
        this->add_edge(this->node_array[i / 2], this->node_array[(first->child->number - 1) / 2 - 1]);
      }
    }
    if (i + 1 < gamma1_graph->node_number && second && second->child && second->child != second) {
#ifdef DEBUG_GAMMA2
      cout << "edge2 " << i/2 << "  " << (second->child->number - 1)/2 -1 << endl;
#endif
      if (preprocessing && four_way) {
        this->add_edge(this->node_array[i / 2], this->node_array[(second->child->number - 1) / 2]);
      } else {
        this->add_edge(this->node_array[i / 2], this->node_array[(second->child->number - 1) / 2 - 1]);
      }
    }
  }
}

/**
 * Create Gamma1 subgraphs of the Gamma2 graph
 * @param previous_node_num number of nodes in the previous graph
 * @param preprocessing true fi we are in a preprocessing step of a 4-way split
 * @param four_way true if we create the subgraphs in a 4-way split
 * @param hybrid true if we use the hybrid construction
 * @param hybrid_choice mapping of number of nodes and next splits
 */
void DAG_Gamma2::create_subgraphs(uint32_t previous_node_num, bool preprocessing, bool four_way, bool hybrid, std::vector<uint64_t>& hybrid_choice) {
  // Split the Gamma2 graph into two Gamma1s
  pair<DAG_Gamma1 *, DAG_Gamma1 *> p = create_from_Gamma2(this, previous_node_num);
  this->gamma1_right = p.first;
  this->gamma1_left = p.second;

  bool fourWayLeft = four_way;
  bool fourWayRight = four_way;
  bool preprocessingLeft = preprocessing;
  bool preprocessingRight = preprocessing;

  //p.first->~DAG_Gamma1();
  //p.second->~DAG_Gamma1();

  uint32_t left_node_num = node_number / 2;
  uint32_t right_node_num = node_number / 2;

  if (node_number <= 4 && (preprocessing || !four_way)) {
    this->sub_left = 0;
    this->sub_right = 0;
    return;
  }

  // Because of parity it can vary between node_number/2 or node_number/2 - 1, here it is set to be correct
  uint32_t left_num = this->gamma1_left->node_number;
  uint32_t right_num = this->gamma1_right->node_number;

  if (hybrid && four_way && preprocessing) {
    if (nextK(left_num, hybrid_choice) == 2) {
      fourWayLeft = false;
    }
    if (nextK(right_num, hybrid_choice) == 2) {
      fourWayRight = false;
    }
  }

  if (left_num % 2 == 0) {
    left_node_num--;
    right_node_num--;
  }
  if (left_num % 2 == 1) {
    if (right_num % 2 == 0)
      right_node_num--;
  }
  if (preprocessingLeft && fourWayLeft) {
    left_node_num++;
  }
  if (preprocessingRight && fourWayRight) {
    right_node_num++;
  }

  bool nextPreprocessingLeft = !preprocessingLeft;
  bool nextPreprcoessingRight = !preprocessingRight;
  bool nextFourWayLeft = fourWayLeft;
  bool nextFourWayRight = fourWayRight;

  if (hybrid) {
    if (!fourWayLeft || !preprocessingLeft) {
      if (nextK(left_node_num, hybrid_choice) == 4) {
        nextFourWayLeft = true;
        nextPreprocessingLeft = true;
      } else {
        nextFourWayLeft = false;
        nextPreprocessingLeft = true;
      }
    }
    if (!fourWayRight || !preprocessingRight) {
      if (nextK(right_node_num, hybrid_choice) == 4) {
        nextFourWayRight = true;
        nextPreprcoessingRight = true;
      } else {
        nextFourWayRight = false;
        nextPreprcoessingRight = true;
      }
    }
  }

  if ((left_num > 4 && (preprocessingLeft || !four_way || !hybrid)) || (!preprocessingLeft && four_way)) {
    this->sub_left = new DAG_Gamma2(left_node_num);
    this->sub_left->add_edges(this->gamma1_left, preprocessingLeft, fourWayLeft);
    this->sub_left->create_subgraphs(left_num, nextPreprocessingLeft, nextFourWayLeft, hybrid, hybrid_choice);
  } else {
    this->sub_left = 0;
  }

  if ((right_num > 4 && (preprocessingRight || !four_way || !hybrid)) || (!preprocessingRight && four_way)) {
    this->sub_right = new DAG_Gamma2(right_node_num);
    this->sub_right->add_edges(this->gamma1_right, preprocessingRight, fourWayRight);
    this->sub_right->create_subgraphs(right_num, nextPreprcoessingRight, nextFourWayRight, hybrid, hybrid_choice);
  } else {
    this->sub_right = 0;
  }

}

/**
 * Check one Gamma2 graph if it is correct (parity-wise) and change if not
 * @param gamma1 Gamma1 graph that it checks based on the Gamma2 graph
 */
void DAG_Gamma2::check_one_subgraph(DAG_Gamma1 *gamma1) {
  uint32_t node_num = gamma1->node_number;
  uint32_t index1;
  uint32_t index2;
  if (gamma1 && node_num % 2 == 0) {
    if (gamma1->node_array[node_num - 1]->parent) {
      index2 = node_num - 1;
      index1 = gamma1->node_array[index2]->parent->number - 1;

      if (index1 / 2 <= index2 / 2 - 1 && this->gamma1_left->node_array[index1 / 2]->child &&
          this->gamma1_left->node_array[index1 / 2]->child == this->gamma1_left->node_array[index2 / 2 - 1] &&
          this->gamma1_right->node_array[index1 / 2]->child != this->gamma1_right->node_array[index2 / 2 - 1]) {

        swap(this->gamma1_right, this->gamma1_left);
        swap(this->sub_right, this->sub_left);
      }
    }
    if (gamma1->node_array[node_num - 2]->parent) {
      index2 = node_num - 2;

      index1 = gamma1->node_array[index2]->parent->number - 1;

      if (index1 / 2 <= index2 / 2 - 1 && this->gamma1_right->node_array[index1 / 2]->child &&
          this->gamma1_right->node_array[index1 / 2]->child == this->gamma1_right->node_array[index2 / 2 - 1] &&
          this->gamma1_left->node_array[index1 / 2]->child != this->gamma1_left->node_array[index2 / 2 - 1]) {

        swap(this->gamma1_right, this->gamma1_left);
        swap(this->sub_right, this->sub_left);
      }
    }
  }
}

/**
 * Check Gamma1 subgraphs of the Gamma2 graph and correct it if it is not correctly set
 */
void DAG_Gamma2::check_correct_subgraphs() {
  // Base cases
  if (this->node_number <= 4) {
    return;
  }
  uint32_t left_node_num = this->gamma1_left->node_number;
  uint32_t right_node_num = this->gamma1_right->node_number;
  if (this->gamma1_left && left_node_num % 2 == 0) {
    if (this->sub_left) {
      this->sub_left->check_one_subgraph(this->gamma1_left);
    }
  }
  if (this->gamma1_right && right_node_num % 2 == 0) {
    if (this->sub_right) {
      this->sub_right->check_one_subgraph(this->gamma1_right);
    }
  }
  // Recursively do the same for the subgraphs
  if (this->sub_right) {
    this->sub_right->check_correct_subgraphs();
  }
  if (this->sub_left) {
    this->sub_left->check_correct_subgraphs();
  }
}


/**
 * Create a random, topologically ordered DAG in Gamma2 with given number of nodes
 * @param n number of Nodes
 */
DAG_Gamma2 *random_init(uint32_t n) {
  DAG_Gamma2 *g = new DAG_Gamma2(n);
  uint32_t j;

  for (uint32_t i = 0; i < g->node_number; i++) {
    g->node_array[i]->set_function_bits(0, 1, 1, 0);
    j = rand() % (g->node_number - i);
    if (!(g->node_array[i]->right) && !(g->node_array[i + j]->right_parent) && i != i + j) {
      g->add_edge(g->node_array[i], g->node_array[i + j]);
#ifdef RANDOM_GRAPH
      file << "m" << g->node_array[i]->number << " -> m" << g->node_array[i + j]->number << ";" << endl;
#endif // RANDOM_GRAPH
    }
    j = rand() % (g->node_number - i);
    if (!(g->node_array[i]->right) && !(g->node_array[i + j]->right_parent) && i != i + j) {
      g->add_edge(g->node_array[i], g->node_array[i + j]);
#ifdef RANDOM_GRAPH
      file << "m" << g->node_array[i]->number << " -> m" << g->node_array[i + j]->number << ";" << endl;
#endif // RANDOM_GRAPH
    }
  }
  return g;
}

/**
 * Coloring of the edges, i.e., the edge from Node parent to Node child goes into the Gamma1 instance
 * defined by color, g1 or g2
 * @param parent Node from where the edge starts
 * @param child Node where the edge arrives
 * @param color specifies which color it should be colored
 * @param g1 color defines which Gamma1 graph will have the edge
 * @param g2 color defines which Gamma1 graph will have the edge
 * @return true if coloring happened and false if nothing happened
 */
bool coloring(DAG_Gamma2::Node *parent, DAG_Gamma2::Node *child, bool color, DAG_Gamma1 *g1, DAG_Gamma1 *g2) {
  DAG_Gamma1 *current;
  if (color) {
    current = g1;
  } else {
    current = g2;
  }
  if (child && parent) {
    if (child->output) {
#ifdef DEBUG_OUTPUT
      cout << "is_output g1" << endl;
#endif // DEBUG_OUTPUT
      current->node_array[child->number - 1]->is_output = true;
    }
    current->add_edge(current->node_array[parent->number - 1], current->node_array[child->number - 1]);
    return true;
  }
  return false;
}

/**
 * Actual coloring given the Nodes in question.
 * @param parent Node from where the edge starts
 * @param child Node where the edge arrives
 * @param old_parent previous parent in coloring
 * @param old_child previous child in coloring
 * @param color specifies which color it should be colored
 * @param b1 true if coloring happens in the first direction
 * @param b2 true if coloring happens in the second direction
 * @param g1 color defines which Gamma1 graph will have the edge
 * @param g2 color defines which Gamma1 graph will have the edge
 * @param edge_num number of edges minus the so far colored ones
 */
void color_parent_child(DAG_Gamma2::Node *&parent,
                        DAG_Gamma2::Node *&child,
                        DAG_Gamma2::Node *&old_parent,
                        DAG_Gamma2::Node *&old_child,
                        bool &color,
                        bool &b1,
                        bool &b2,
                        DAG_Gamma1 *g1,
                        DAG_Gamma1 *g2,
                        uint32_t &edge_num) {
  DAG_Gamma2::Node *old_child_parent = old_parent;
  DAG_Gamma2::Node *old_parent_child = old_child;
  old_child = child;
  old_parent = parent;
  DAG_Gamma2::Node *tmp_node;

  if (!(old_child_parent) && old_parent && old_child) {
    parent = old_child->other_child_or_parent(old_parent, old_child->left_parent, old_child->right_parent);
  } else if (old_child) {
    parent = old_child->other_child_or_parent(old_child_parent, old_child->left_parent, old_child->right_parent);
  } else {
    parent = 0;
  }

  if (!(old_parent_child) && old_child && old_parent) {
    tmp_node = old_parent->other_child_or_parent(old_child, old_parent->left, old_parent->right);
    if (tmp_node != old_child) {
      // For the case when there are two parallel edges from the same node to the same other node in the graph
      // (there is nothing else in that case, so it is safe this way, but it has to run once!)
      child = tmp_node;
    } else {
      child = 0;
    }
  } else if (old_parent) {
    child = old_parent->other_child_or_parent(old_parent_child, old_parent->left, old_parent->right);
  } else {
    child = 0;
  }

  color = !color;
  b1 = coloring(parent, old_child, color, g1, g2);
  if (b1 == true) {
    old_child->colored = true;
    edge_num--;
  }
  // do not color the same two times in one round
  if (!(old_parent == parent && old_child == child)) {
    b2 = coloring(old_parent, child, color, g1, g2);
    if (b2 == true) {
      old_parent->colored = true;
      edge_num--;
    }
  }
  if (!(parent) && old_child) {
    old_child->colored = true;
  }
  if (!(child) && old_parent) {
    old_parent->colored = true;
  }
}

/**
 * The separation of the edges in the Gamma2 graph into two returned Gamma1 instances
 * The bipartite graph is two Gamma2 DAGs, where tmp1 is one set, tmp2 is the other
 * (and thus, there are no edges within tmp1 and within tmp2 either)
 */
pair<DAG_Gamma1 *, DAG_Gamma1 *> create_from_Gamma2(DAG_Gamma2 *g, uint32_t previous_node_num) {
  // Creating the Gamma2 graphs building up the "bipartite" graph
  DAG_Gamma2 *tmp1 = new DAG_Gamma2(g->node_number);
  DAG_Gamma2 *tmp2 = new DAG_Gamma2(g->node_number);

  uint32_t edge_num = 0;
  DAG_Gamma2::Node *current;
  for (uint32_t i = 0; i < g->node_number; ++i) {
    current = g->node_array[i];
    // Add edges to the "bipartite" graph
	if (current->left)
      current->left->prepare_bipartite(tmp1, tmp2, i, edge_num);
	if (current->right)
	  current->right->prepare_bipartite(tmp1, tmp2, i, edge_num);
  }

  DAG_Gamma1 *g1 = new DAG_Gamma1(g->node_number);
  DAG_Gamma1 *g2 = new DAG_Gamma1(g->node_number);

  bool color = true;
  DAG_Gamma2::Node *parent;
  DAG_Gamma2::Node *child;
  DAG_Gamma2::Node *old_child;
  DAG_Gamma2::Node *old_parent;
  // The two directions in the coloring path, if both are false, we can stop, there is no more path
  bool b1, b2;
  while (edge_num > 0) {
    old_child = 0;
    old_parent = 0;
    parent = tmp1->smallest_uncolored_with_child();
    if (parent->left) {
      child = parent->left;
    }
      // We know that it has to have one child from smallest_uncolored_with_child
    else {
      child = parent->right;
    }

    b1 = coloring(parent, child, color, g1, g2);
    edge_num--;

    b2 = false;
    while (b1 || b2) {
      color_parent_child(parent, child, old_parent, old_child, color, b1, b2, g1, g2, edge_num);
    }
  }

  pair<DAG_Gamma1 *, DAG_Gamma1 *> p;
  // Depending on the parity, which side
  if (previous_node_num % 2 == 1) {
    if (g1->node_array[g1->node_number - 1]->parent == 0) {
      g1->delete_last_node();
      p.first = g1;
      p.second = g2;
    } else if (g2->node_array[g2->node_number - 1]->parent == 0) {
      g2->delete_last_node();
      p.first = g2;
      p.second = g1;
    } else {
      cerr << "PROBLEM in gamma:create_from_Gamma2: coloring did not work" << endl;
    }
  } else {
    p.first = g1;
    p.second = g2;
  }

  // tmp1->~DAG_Gamma2();
  // tmp2->~DAG_Gamma2();
  delete tmp1;
  delete tmp2;

  return p;
}

/**
 * Destructor Gamma2
 */
DAG_Gamma2::~DAG_Gamma2() {
  if (this->sub_left) {
    this->sub_left->~DAG_Gamma2();
  }
  if (this->sub_right) {
    this->sub_right->~DAG_Gamma2();
  }
  if (this->gamma1_left) {
    this->gamma1_left->~DAG_Gamma1();
  }
  if (this->gamma1_right) {
    this->gamma1_right->~DAG_Gamma1();
  }
  if (node_number > 0) {
    for (uint32_t i = 0; i < node_number; i++) {
      delete node_array[i];
    }
    delete[] node_array;
    node_number = 0;
  }
}
