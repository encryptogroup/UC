/**
 \file 		UCNode.h
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
 \brief		Header of the UCNode class part of Valiant's 4-way Split Construction.
 */

#ifndef VALIANTUC4WAYSPLIT_NODE_H
#define VALIANTUC4WAYSPLIT_NODE_H

#include <stdint.h>
#include <vector>
#include <string>
#include "NodeType.h"
#include "../../Gamma/gamma/gamma2.h"

class UCNode {
 public:
  UCNode(const uint32_t id, const NodeType type);
  ~UCNode();

  uint32_t getID() const;
  NodeType getNodeType() const;
  void setNodeType(NodeType type);

  void addChild(UCNode *child);
  void addParent(UCNode *parent);
  std::vector<UCNode*>& getChildren();
  std::vector<UCNode*>& getParents();
  std::vector<unsigned int> getControlBits();

  void setPoleType(PoleType poleType);
  PoleType getPoleType() const;

  void setNumberOfAdditionalWires (int value);
  void setControlBits(std::vector<unsigned int> controlBits);
  void setControlBit(unsigned int controlBit);

  void setEdgeEmbedded (bool edgeEmbedded);

  void setTopologicalNumber (uint32_t topNum);
  uint32_t getTopologicalNumber ();
  void setTopologicalVisited (bool visited);
  bool getTopologicalVisited ();

  uint32_t getInputWire (UCNode *child);

  void clearParents ();
  void clearChildren ();
  void removeParent (UCNode *parent);
  void removeChild (UCNode *child);

  std::string getNodeDescription ();
  std::string getNodeProgramming (DAG_Gamma2 *dag);

 private:
  uint32_t id;
  NodeType type;
  PoleType poleType;
  int numberOfAdditionalWires;

  uint32_t topologicalNumber;
  bool topologicalVisited;

  std::vector<UCNode*> children;
  std::vector<UCNode*> parents;
  std::vector<unsigned int> controlBits;
  bool edgeEmbedded;
};


#endif //VALIANTUC4WAYSPLIT_NODE_H
