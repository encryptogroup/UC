/**
 \file 		Block.cpp
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
 \brief		Implementation of the UCNode class part of Valiant's 4-way Split Construction.
 */

#include <iostream>
#include "UCNode.h"

/*
 * Construct a UCNode
 * @param id id of the node
 * @param type node type
 */
UCNode::UCNode(const uint32_t id, const NodeType type) {
  this->id = id;
  this->type = type;
  this->edgeEmbedded = false;
  this->topologicalNumber = 0;
  this->topologicalVisited = false;
  this->numberOfAdditionalWires = 0;
  this->poleType = NO_POLE;
  if(type == Y_SWITCH || type == X_SWITCH || type == RECURSION_POINT) {
    this->setControlBit(0);
  }
}

// Deconstructor
UCNode::~UCNode() {}

/*
 * get the node's id
 * @return id
 */
uint32_t UCNode::getID() const { return this->id; }

/*
 * get the node's type
 * @return type
 */
NodeType UCNode::getNodeType() const { return this->type; }

/*
 * set the node's type
 * @param type type
 */
void UCNode::setNodeType(NodeType type) { this->type = type; }

/*
 * add a child to the node
 * @param child child
 */
void UCNode::addChild(UCNode *child) {
  this->children.emplace_back(child);
}

/*
 * add a parent to the node
 * @param parent
 */
void UCNode::addParent(UCNode *parent) {
  this->parents.emplace_back(parent);
}

/*
 * get the node's children
 * @return children
 */
std::vector<UCNode*>& UCNode::getChildren() {
  return this->children;
}

/*
 * get the node's parents
 * @return parents
 */
std::vector<UCNode*>& UCNode::getParents() {
  return this->parents;
}

/*
 * get the control bits of the node
 * @return control bits
 */
std::vector<unsigned int> UCNode::getControlBits() { return this->controlBits; }

/*
 * set the pole type of the node
 * @param poleType pole type
 */
void UCNode::setPoleType(PoleType poleType) {
  this->poleType = poleType;
}

/*
 * get the pole type of the node
 * @return pole type
 */
PoleType UCNode::getPoleType() const { return this->poleType; }

/*
 * set the control bits of the node
 * @param control bits
 */
void UCNode::setControlBits(std::vector<unsigned int> controlBits) {
  this->controlBits.clear();
  this->controlBits = controlBits;
}

/*
 * set one control bit of the node
 * @param control bit
 */
void UCNode::setControlBit(unsigned int controlBit) {
  this->controlBits.clear();
  this->controlBits.emplace_back(controlBit);
}

/*
 * mark the node as edge-embedded
 * @param true if the node was edge-embedded
 */
void UCNode::setEdgeEmbedded(bool edgeEmbedded) {
  this->edgeEmbedded = edgeEmbedded;
}

/*
 * set the topological number of the node
 * @param topNum topological number
 */
void UCNode::setTopologicalNumber(uint32_t topNum) { this->topologicalNumber = topNum; }

/*
 * get the topological visited information
 * @return true if the node was topological visited
 */
bool UCNode::getTopologicalVisited() { return this->topologicalVisited; }

/*
 * mark the node as topological visited
 * @param tru if the node is topological visited
 */
void UCNode::setTopologicalVisited(bool visited) { this->topologicalVisited = visited; }

/*
 * set the number of additional wires due to the X and copy gates
 * @aram value number of additional wires
 */
void UCNode::setNumberOfAdditionalWires(int value) { this->numberOfAdditionalWires = value; }

/*
 * remove all parents of the node
 */
void UCNode::clearParents() {
  for (auto parent : this->parents) {
    parent->removeChild (this);
  }
  this->parents.clear();
}

/*
 * remove all children of the node
 */
void UCNode::clearChildren() {
  for (auto child : this->children) {
    child->removeParent (this);
  }
  this->children.clear();
}

/*
 * remove a pecific parent of the node
 * @param parent
 */
void UCNode::removeParent(UCNode *parent) {
  if (this->type == X_SWITCH) {
    this->type = REVERSE_Y_SWITCH;
  } else if (this->type == Y_SWITCH) {
    this->type = I_SWITCH;
  } else {
    std::cout << "Something is wrong with removeParent" << std::endl;
  }

  if (this->parents[0]->getNodeType() == POLE && this->parents[0]->getPoleType() == OUTPUT) {
    this->parents.erase (this->parents.begin());
  }
  if (this->parents[1]->getNodeType() == POLE && this->parents[1]->getPoleType() == OUTPUT) {
    this->parents.erase (this->parents.begin() + 1);
  }
}

/*
 * remove a specific child of the node
 * @param child child
 */
void UCNode::removeChild(UCNode *child) {
  if (this->type == X_SWITCH) {
    this->type = Y_SWITCH;
  } else if (this->type == REVERSE_Y_SWITCH) {
    this->type = I_SWITCH;
  } else {
    std::cout << "Something is wrong with removeChild" << std::endl;
  }

  if (this->id == 10) {
    this->controlBits[0] = 1 - this->controlBits[0];
  }

  if (this->children[0]->getNodeType() == POLE && this->children[0]->getPoleType() == INPUT) {
    this->children.erase (this->children.begin());
  }
  if (this->children[1]->getNodeType() == POLE && this->children[1]->getPoleType() == INPUT) {
    this->children.erase (this->children.begin() + 1);
  }
}

/*
 * get the input wire of the node depending on the child
 * @return input wire
 */
uint32_t UCNode::getInputWire(UCNode *child) {
  uint32_t wire = topologicalNumber + numberOfAdditionalWires;
  int outputs = (int) children.size();
  int inputs = (int) parents.size();
  if (inputs == 0) {
    return wire;
  }
  if (inputs == 1) {
    return this->parents[0]->getInputWire(this);
  }
  if (outputs == 0 || outputs == 1 || type == POLE) {
    return wire;
  }
  if (children[0]->getNodeType() == child->getNodeType() && children[0]->getPoleType() == child->getPoleType()
      && children[0]->getID() == child->getID()) {
    return wire;
  }
  return wire + 1;
}

/*
 * get the node desciption, i.e. the output of the circuit file related to this node
 */
std::string UCNode::getNodeDescription() {
  std::string result = "";
  int wire = topologicalNumber + numberOfAdditionalWires;
  int outputs = (int) children.size();
  int inputs = (int) parents.size();
  if (inputs == 0) {
    result += wire;
  } else if (inputs == 2) {
    if(outputs <= 1) {
      result += "Y ";
      // inputs have to be swapped
      result += std::to_string(this->parents[1]->getInputWire(this)) + " ";
      result += std::to_string(this->parents[0]->getInputWire(this)) + " ";
      result += std::to_string(wire);
    } else if(outputs == 2) {
      if (type == POLE) {
        result += "U ";
        result += std::to_string(this->parents[0]->getInputWire(this)) + " ";
        result += std::to_string(this->parents[1]->getInputWire(this)) + " ";
        result += std::to_string(wire);
      } else {
        result += "X ";
        result += std::to_string(this->parents[0]->getInputWire(this)) + " ";
        result += std::to_string(this->parents[1]->getInputWire(this)) + " ";
        result += std::to_string(wire) + " ";
        result += std::to_string(wire + 1);
      }
    }
  }
  return result;
}

/*
 * get the node's programming bit
 */
std::string UCNode::getNodeProgramming(DAG_Gamma2 *dag) {
  if (poleType != OUTPUT) {
    if (controlBits.size() == 1) {
      return std::to_string(this->controlBits[0]);
    }
    return std::to_string(controlBits[3] * 8 + controlBits[2] * 4 + controlBits[1] * 2 + controlBits[0]);
  }
  DAG_Gamma1 *left = dag->gamma1_left;
  DAG_Gamma1 *right = dag->gamma1_right;
  DAG_Gamma1::Node *currentNode = left->node_array[this->id - 1];
  if (currentNode && currentNode->parent) {
    return std::to_string(0);
  } else {
    currentNode = right->node_array[this->id - 1];
    if (currentNode && currentNode->parent) {
      return std::to_string(1);
    } else {
      std::cout << "Something is wrong" << std::endl;
      return std::to_string(0);
    }
  }
  /*
  for (int i = 0; i < left->node_number; i++) {
    currentNode = left->node_array[i];
    if (currentNode->child && currentNode->child->number == this->id) {
      return std::to_string(1);
    }
  }
  return std::to_string (0);*/
}

uint32_t UCNode::getTopologicalNumber() { return this->topologicalNumber; }
