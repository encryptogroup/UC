/**
 \file 		ValiantEUG.cpp
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
 \brief		Implementation of the ValiantEUG class part of Valiant's 4-way Split Construction.
 */
#include "ValiantEUG.h"
#include <math.h>

/*
 * Construct a ValiantEUG
 * @param poles poles of the eug
 * @param eugNum denotes the left and the right eug of the uc
 */
ValiantEUG::ValiantEUG(const std::vector<UCNode *> &poles, int eugNum) {
  this->eugNum = eugNum;
  uint32_t n = (uint32_t) poles.size();
  if (n == 0) return;
  std::vector<uint32_t> recursionSteps;

  // switch these lines if you want to use the more efficient uc construction (hint: the edge-embedding is missing here)
  constructUC(poles, recursionSteps);
  // modifiedConstructedUC(poles, recursionSteps);
}

/*
 * Construct a ValiantEUG
 * @param poles poles of the eug
 * @param recursionSteps recursion steps to go to this chain
 * @param eugNum denotes the left and the rigth eug of the uc
 */
ValiantEUG::ValiantEUG(const std::vector<UCNode *> &poles, std::vector<uint32_t> recursionSteps, int eugNum) {
  this->eugNum = eugNum;
  uint32_t n = (uint32_t) poles.size();
  if (n == 0) return;
  constructUC(poles, recursionSteps);
}

// deconstruct ValiantEUG
ValiantEUG::~ValiantEUG() {};

/*
 * construct a uc with parity optimization
 * @param poles poles of the eug
 * @param recursionSteps recursion steps of the valiantEUG within the chains
 */
void ValiantEUG::modifiedConstructedUC(const std::vector<UCNode *> &poles, std::vector<uint32_t> recursionSteps) {
  double n = (uint32_t) poles.size(); // number of poles

  std::vector<UCNode *> q, r, s, t, qL, rL, sL, tL;

  if (n <= 4) {
    this->blocks.emplace_back(new Block(poles, 0, recursionSteps, 0, RECURSION_BASE, q, r, s, t));
  } else {
    double blocks = ceil(n / 4);
    int restPoles = ((int)(n - 1) % 4) + 1;
    if (blocks == 2) {
      if (restPoles == 4) {
        UCNode *q1 =  new UCNode(0, RECURSION_POINT);
        q.emplace_back(q1);
        qL.emplace_back(q1);
      }
      if (restPoles >= 3) {
        UCNode *s1 = new UCNode(2, RECURSION_POINT);
        s.emplace_back(s1);
        sL.emplace_back(s1);
      }
      if (restPoles >= 2) {
        UCNode *r1 = new UCNode(1, RECURSION_POINT);
        r.emplace_back(r1);
        rL.emplace_back(r1);
      }
      UCNode *t1 = new UCNode(3, RECURSION_POINT);
      t.emplace_back(t1);
      tL.emplace_back(t1);
    } else {
      UCNode *q1 = new UCNode(0, RECURSION_POINT);
      UCNode *r1 = new UCNode(1, RECURSION_POINT);
      UCNode *s1 = new UCNode(2, RECURSION_POINT);
      UCNode *t1 = new UCNode(3, RECURSION_POINT);
      q.emplace_back(q1);
      r.emplace_back(r1);
      s.emplace_back(s1);
      t.emplace_back(t1);
      qL.emplace_back(q1);
      rL.emplace_back(r1);
      sL.emplace_back(s1);
      tL.emplace_back(t1);
    }
    Block *UCHead = new Block(poles, 0, recursionSteps, 0, HEAD, q, r, s, t);
    this->blocks.emplace_back(UCHead);
    uint32_t i;
    for (i = 1; i < blocks - 1; i++) {
      if (i == blocks - 2) {
        if (restPoles == 4) {
          UCNode *qi =  new UCNode(0, RECURSION_POINT);
          q.emplace_back(qi);
          qL.emplace_back(qi);
        }
        if (restPoles >= 3) {
          UCNode *si = new UCNode(2, RECURSION_POINT);
          s.emplace_back(si);
          sL.emplace_back(si);
        }
        if (restPoles >= 2) {
          UCNode *ri = new UCNode(1, RECURSION_POINT);
          r.emplace_back(ri);
          rL.emplace_back(ri);
        }
        UCNode *ti = new UCNode(3, RECURSION_POINT);
        t.emplace_back(ti);
        tL.emplace_back(ti);
      } else {
        UCNode *q_i = new UCNode(0, RECURSION_POINT);
        UCNode *r_i = new UCNode(1, RECURSION_POINT);
        UCNode *s_i = new UCNode(2, RECURSION_POINT);
        UCNode *t_i = new UCNode(3, RECURSION_POINT);
        q.emplace_back(q_i);
        r.emplace_back(r_i);
        s.emplace_back(s_i);
        t.emplace_back(t_i);
        qL.emplace_back(q_i);
        rL.emplace_back(r_i);
        sL.emplace_back(s_i);
        tL.emplace_back(t_i);
      }
      if (i != 1) {
        qL.erase(qL.begin());
        rL.erase(rL.begin());
        sL.erase(sL.begin());
        tL.erase(tL.begin());
      }
      this->blocks.emplace_back(new Block(poles, i * 4, recursionSteps, i, NORMAL, qL, rL, sL, tL));
    }
    if (i != 1) {
      qL.erase(qL.begin());
      rL.erase(rL.begin());
      sL.erase(sL.begin());
      tL.erase(tL.begin());
    }

    this->blocks.emplace_back(new Block(poles, i * 4, recursionSteps, i, TAIL, qL, rL, sL, tL));

    recursionSteps.emplace_back(0);
    int recursionIndex = (int) recursionSteps.size() - 1;
    ValiantEUG *qUC = new ValiantEUG(q, recursionSteps, eugNum);
    recursionSteps[recursionIndex]++;
    ValiantEUG *rUC = new ValiantEUG(r, recursionSteps, eugNum);
    recursionSteps[recursionIndex]++;
    ValiantEUG *sUC = new ValiantEUG(s, recursionSteps, eugNum);
    recursionSteps[recursionIndex]++;
    ValiantEUG *tUC = new ValiantEUG(t, recursionSteps, eugNum);
    this->children.emplace_back(qUC);
    this->children.emplace_back(rUC);
    this->children.emplace_back(sUC);
    this->children.emplace_back(tUC);
  }
}

/*
 * construct a eug
 * @param poles poles of the eug
 * @aram recursionSteps recursion steps of the eug within the chains
 */
void ValiantEUG::constructUC(const std::vector<UCNode *> &poles, std::vector<uint32_t> recursionSteps) {
  uint32_t n = (uint32_t) poles.size(); // number of poles

  std::vector<UCNode *> q;
  std::vector<UCNode *> r;
  std::vector<UCNode *> s;
  std::vector<UCNode *> t;

  // recursion base
  if (n <= 4) {
    this->blocks.emplace_back(new Block(poles, 0, recursionSteps, 0, RECURSION_BASE, q, r, s, t));
  } else {
    UCNode *q1 = new UCNode(0, RECURSION_POINT);
    UCNode *r1 = new UCNode(1, RECURSION_POINT);
    UCNode *s1 = new UCNode(2, RECURSION_POINT);
    UCNode *t1 = new UCNode(3, RECURSION_POINT);
    q.emplace_back(q1);
    r.emplace_back(r1);
    s.emplace_back(s1);
    t.emplace_back(t1);
    Block *UCHead = new Block(poles, 0, recursionSteps, 0, HEAD, q, r, s, t);
    this->blocks.emplace_back(UCHead);

    std::vector<UCNode *> qL, rL, sL, tL;
    qL.emplace_back(q1);
    rL.emplace_back(r1);
    sL.emplace_back(s1);
    tL.emplace_back(t1);

    uint32_t i;
    for (i = 1; 4 * (i + 1) < n; i++) {
      UCNode *q_i = new UCNode(0, RECURSION_POINT);
      UCNode *r_i = new UCNode(1, RECURSION_POINT);
      UCNode *s_i = new UCNode(2, RECURSION_POINT);
      UCNode *t_i = new UCNode(3, RECURSION_POINT);
      q.emplace_back(q_i);
      r.emplace_back(r_i);
      s.emplace_back(s_i);
      t.emplace_back(t_i);
      qL.emplace_back(q_i);
      rL.emplace_back(r_i);
      sL.emplace_back(s_i);
      tL.emplace_back(t_i);
      if (i != 1) {
        qL.erase(qL.begin());
        rL.erase(rL.begin());
        sL.erase(sL.begin());
        tL.erase(tL.begin());
      }
      this->blocks.emplace_back(new Block(poles, i * 4, recursionSteps, i, NORMAL, qL, rL, sL, tL));
    }
    if (i != 1) {
      qL.erase(qL.begin());
      rL.erase(rL.begin());
      sL.erase(sL.begin());
      tL.erase(tL.begin());
    }
    this->blocks.emplace_back(new Block(poles, i * 4, recursionSteps, i, TAIL, qL, rL, sL, tL));

    recursionSteps.emplace_back(0);
    int recursionIndex = (int) recursionSteps.size() - 1;
    ValiantEUG *qUC = new ValiantEUG(q, recursionSteps, eugNum);
    recursionSteps[recursionIndex]++;
    ValiantEUG *rUC = new ValiantEUG(r, recursionSteps, eugNum);
    recursionSteps[recursionIndex]++;
    ValiantEUG *sUC = new ValiantEUG(s, recursionSteps, eugNum);
    recursionSteps[recursionIndex]++;
    ValiantEUG *tUC = new ValiantEUG(t, recursionSteps, eugNum);
    this->children.emplace_back(qUC);
    this->children.emplace_back(rUC);
    this->children.emplace_back(sUC);
    this->children.emplace_back(tUC);
  }
}

/*
 * go to a children chain
 * @param recursionPositions way to the target chain
 */
ValiantEUG *ValiantEUG::goToChildrenUC(std::vector<uint32_t> recursionPositions) {
  if (recursionPositions.size() == 0) {
    return this;
  }
  uint32_t recursionPosition = recursionPositions[0];
  recursionPositions.erase(recursionPositions.begin());
  return this->children[recursionPosition]->goToChildrenUC(recursionPositions);
}

/*
 * Get the size of the EUG
 * @return size
 */
uint32_t ValiantEUG::getSize() const {
  uint32_t size = 0;
  for (Block *b : this->blocks) {
    size += b->getSize();
  }
  for (ValiantEUG *uc : this->children) {
    size += uc->getSize();
  }
  return size;
}

/*
 * recursion point edge-embedding
 * @param graph Gamma_1 graph
 * @param graphG2 Gamma_2 graph
 */
void ValiantEUG::edgeEmbedding(DAG_Gamma1 *graph, DAG_Gamma2 *graphG2) {
  if (!graphG2 && graph) edgeEmbedding(graph);
  if (!graph || !graphG2) return;
  DAG_Gamma1 *left = graphG2->gamma1_left;
  DAG_Gamma1 *right = graphG2->gamma1_right;
  DAG_Gamma2 *leftG2 = graphG2->sub_left;
  DAG_Gamma2 *rightG2 = graphG2->sub_right;

  if (!leftG2 && !rightG2) return;

  DAG_Gamma1 *leftLeft, *leftRight, *rightLeft, *rightRight;
  if (leftG2) {
    leftLeft = leftG2->gamma1_left;
    leftRight = leftG2->gamma1_right;
  }
  if (rightG2) {
    rightLeft = rightG2->gamma1_left;
    rightRight = rightG2->gamma1_right;
  }

  for (int i = 0; i < graph->node_number; i++) {
    DAG_Gamma1::Node *currentNode = graph->node_array[i];
    int id = currentNode->number;
    int currentBlock = (int) floor(float((id - 1) / 4));
    int currentBlockPosition = (id - 1) % 4;

    int currentNodePreprocessingPosition = (int) floor(float((id - 1) / 2));

    DAG_Gamma1::Node *destinationNode = currentNode->child;
    if (!destinationNode) continue;
    int destId = destinationNode->number;
    int destBlock = (int) floor(float((destId - 1) / 4));
    int destBlockPosition = (destId - 1) % 4;

    int destNodePreProcessingPosition = (int) floor(float((destId - 1) / 2)) + 1;

    if (currentBlock == destBlock) {
      if(currentBlockPosition != destBlockPosition) {
        this->blocks[currentBlock]->setOutputPermutationBit(currentBlockPosition, destBlockPosition);
      }
    } else {
      DAG_Gamma1 *currentGamma, *currentGammaLeft, *currentGammaRight;
      int recursionPoint;
      if (left
          && left->node_array[currentNodePreprocessingPosition]->child
          && left->node_array[currentNodePreprocessingPosition]->child->number == destNodePreProcessingPosition
          && !left->node_array[currentNodePreprocessingPosition]->is_embedded) {
        currentGammaLeft = leftLeft;
        currentGammaRight = leftRight;
        recursionPoint = 0;
        left->node_array[currentNodePreprocessingPosition]->is_embedded = true;
      } else if (right
          && right->node_array[currentNodePreprocessingPosition]->child
          && right->node_array[currentNodePreprocessingPosition]->child->number == destNodePreProcessingPosition
          && !right->node_array[currentNodePreprocessingPosition]->is_embedded) {
        currentGammaLeft = rightLeft;
        currentGammaRight = rightRight;
        recursionPoint = 2;
        right->node_array[currentNodePreprocessingPosition]->is_embedded = true;
      } else {
          std::cout << "Something is wrong with preprocessing" << std::endl;
          continue;
        }
      if (currentGammaLeft
          && currentGammaLeft->node_array[currentBlock]->child
          && currentGammaLeft->node_array[currentBlock]->child->number == destBlock
          && !currentGammaLeft->node_array[currentBlock]->is_embedded) {
        currentGamma = currentGammaLeft;
      } else if (currentGammaRight
          && currentGammaRight->node_array[currentBlock]->child
          && currentGammaRight->node_array[currentBlock]->child->number == destBlock
          && !currentGammaRight->node_array[currentBlock]->is_embedded) {
        currentGamma = currentGammaRight;
        recursionPoint += 1;
      }  else {
        std::cout << "something is wrong" << std::endl;
        return;
      }
      currentGamma->node_array[currentBlock]->is_embedded = true;
      int tmp = currentBlock == 0 ? 0 : 1;
      this->blocks[currentBlock]->getRecursionPoints()[recursionPoint][tmp]->setEdgeEmbedded(true);
      if (destBlock == currentBlock + 1) {
        this->blocks[currentBlock]->getRecursionPoints()[recursionPoint][tmp]->setControlBit(0);
      } else {
        this->blocks[currentBlock]->getRecursionPoints()[recursionPoint][tmp]->setControlBit(1);
        this->blocks[destBlock]->getRecursionPoints()[recursionPoint][0]->setControlBit(1);
        this->blocks[destBlock]->getRecursionPoints()[recursionPoint][0]->setEdgeEmbedded(true);
      }
      this->blocks[currentBlock]->setOutputPermutationBit(currentBlockPosition,
                                                          recursionPoint
                                                              + 4);
      this->blocks[destBlock]->setInputPermutationBit(recursionPoint, destBlockPosition);
    }
  }
  for (Block* b : this->blocks) {
    b->edgeEmbedding();
  }
  if (leftLeft) {
    if (leftG2->sub_left) {
      this->children[0]->edgeEmbedding(leftLeft, leftG2->sub_left);
    } else {
      this->children[0]->edgeEmbedding(leftLeft);
    }
  }
  if (leftRight) {
    if (leftG2->sub_right) {
      this->children[1]->edgeEmbedding(leftRight, leftG2->sub_right);
    } else {
      this->children[1]->edgeEmbedding(leftRight);
    }
  }
  if (rightLeft) {
    if(rightG2->sub_left) {
      this->children[2]->edgeEmbedding(rightLeft, rightG2->sub_left);
    } else {
      this->children[2]->edgeEmbedding(rightLeft);
    }
  }
  if (rightRight) {
    if (rightG2->sub_right) {
      this->children[3]->edgeEmbedding(rightRight, rightG2->sub_right);
    } else {
      this->children[3]->edgeEmbedding(rightRight);
    }
  }
}

/*
 * recursion point edge-embedding for recursion bases
 * @param graph Gamma_1 graph
 */
void ValiantEUG::edgeEmbedding(DAG_Gamma1 *graph) {
  for (int i = 0; i < graph->node_number; i++) {
    DAG_Gamma1::Node *currentNode = graph->node_array[i];
    int id = currentNode->number;
    int currentBlock = (int) floor(float((id - 1) / 4));
    int currentBlockPosition = (id - 1) % 4;

    DAG_Gamma1::Node *destinationNode = currentNode->child;
    if (!destinationNode) continue;
    int destId = destinationNode->number;
    int destBlock = (int) floor(float((destId - 1) / 4));
    int destBlockPosition = (destId - 1) % 4;

    if (currentBlock == destBlock) {
      if (currentBlockPosition == destBlockPosition) {
        // do nothing?
      } else {
        this->blocks[currentBlock]->setOutputPermutationBit(currentBlockPosition, destBlockPosition);
      }
    } else {
      std::cout << "in singel embedding is something wrong" << std::endl;
    }
  }
  for (Block* b : this->blocks) {
    b->edgeEmbedding();
  }
}

std::vector<Block*> ValiantEUG::getBlocks() { return this->blocks; }
std::vector<ValiantEUG *> ValiantEUG::getChildren() { return this->children; }
