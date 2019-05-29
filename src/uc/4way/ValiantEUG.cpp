/**
 \file                 ValiantEUG.cpp
 \author         guenther@rangar.de
 \copyright        Valiant's Universal Circuit Construction Optimized for Private Function Evaluation
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
 \brief                Implementation of the ValiantEUG class part of Valiant's 4-way Split Construction.
 */
#include "ValiantEUG.h"
#include <math.h>
#include "../../Gamma/util/hybrid_size.h"

/**
 * Construct a ValiantEUG
 * @param poles poles of the eug
 * @param eugNum denotes the left and the right eug of the uc
 * @param k 2 or 4 for k-way split
 * @param zhao true if Zhao's 4-way construction is used
 * @param hybrid_choice mapping between number of poles and next k
 */
ValiantEUG::ValiantEUG(const std::vector<UCNode *> &poles, int eugNum, int k, bool zhao, std::vector<uint64_t>& hybrid_choice) {

  if (k == 0) {
    this->hybrid = true;
    this->k = nextK(poles.size(), hybrid_choice);
  } else {
    this->k = k;
    this->hybrid = false;
  }
  this->eugNum = eugNum;
  uint32_t n = (uint32_t) poles.size();
  if (n == 0) return;
  std::vector<uint32_t> recursionSteps;
  constructUC(poles, recursionSteps, zhao, hybrid_choice);
}
/**
 * Construct a ValiantEUG
 * @param poles poles of the eug
 * @param recursionSteps recursion steps to go to this chain
 * @param eugNum denotes the left and the rigth eug of the uc
 * @param k 2 or 4 for k-way split
 * @param hybrid true if our hybrid construction is used
 * @param zhao true if Zhao's 4-way construction is used
 * @param hybrid_choice mapping between number of poles and next k
 */
ValiantEUG::ValiantEUG(const std::vector<UCNode *> &poles,
                       std::vector<uint32_t> recursionSteps,
                       int eugNum,
                       int k,
                       bool hybrid,
                       bool zhao,
					   std::vector<uint64_t>& hybrid_choice) {
  this->hybrid = hybrid;
  this->eugNum = eugNum;
  this->k = k;
  uint32_t n = (uint32_t) poles.size();
  if (n == 0) return;
  constructUC(poles, recursionSteps, zhao, hybrid_choice);
}

/**
 * deconstruct Valiant's EUG
 */
ValiantEUG::~ValiantEUG() {}

/**
 * construct a uc with parity optimization
 * @param poles poles of the eug
 * @param recursionSteps recursion steps of the valiantEUG within the chains
 * @param zhao true if Zhao's construction is used
 * @param hybrid_choice mapping between number of poles and next k
 */
void ValiantEUG::constructUC(const std::vector<UCNode *> &poles, std::vector<uint32_t> recursionSteps, bool zhao, std::vector<uint64_t>& hybrid_choice) {
  double n = (uint32_t) poles.size(); // number of poles
  std::vector<std::vector<UCNode *>> rp(k), rpL(k);
  if (n <= 4) {
    this->blocks.emplace_back(createBlock(poles, 0, recursionSteps, 0, RECURSION_BASE, rpL, k, zhao));
  } else {
    double blocks = ceil(n / k);
    int restPoles = ((int) (n - 1) % k) + 1;

    for (auto i = 0; i < k; i++) {
      int tmp = i;
      if (k == 4) {
        if (i == 1) {
          tmp = 2;
        } else if (i == 2) {
          tmp = 1;
        }
      }
      if (blocks != 2 || restPoles >= tmp + 1) {
        auto recP = new UCNode(tmp, RECURSION_POINT);
        rp[i].emplace_back(recP);
        rpL[i].emplace_back(recP);
      }
    }

    Block *UCHead = createBlock(poles, 0, recursionSteps, 0, HEAD, rpL, k, zhao);
    this->blocks.emplace_back(UCHead);
    uint32_t i;

    for (i = 1; i < blocks - 1; i++) {
      for (int j = 0; j < k; j++) {
        int tmp = j;
        if (k == 4) {
          if (j == 1) {
            tmp = 2;
          } else if (j == 2) {
            tmp = 1;
          }
        }
        if (i != blocks - 2 || restPoles >= tmp + 1) {
          auto recP = new UCNode(tmp, RECURSION_POINT);
          rp[j].emplace_back(recP);
          rpL[j].emplace_back(recP);
        }
      }

      if (i != 1) {
        for (int j = 0; j < k; j++) {
          rpL[j].erase(rpL[j].begin());
        }
      }

      this->blocks.emplace_back(createBlock(poles, i * k, recursionSteps, i, NORMAL, rpL, k, zhao));
    }

    if (i != 1) {
      for (int j = 0; j < k; j++) {
        rpL[j].erase(rpL[j].begin());
      }
    }

    this->blocks.emplace_back(createBlock(poles, i * k, recursionSteps, i, TAIL, rpL, k, zhao));
    recursionSteps.emplace_back(0);
    int recursionIndex = (int) recursionSteps.size() - 1;

    for (int i = 0; i < k; i++) {
      auto nk = k;
      if (hybrid) {
        nk = nextK(rp[i].size(), hybrid_choice);
      }
      auto ruc = new ValiantEUG(rp[i], recursionSteps, eugNum, nk, this->hybrid, zhao, hybrid_choice);
      recursionSteps[recursionIndex]++;
      this->children.emplace_back(ruc);
    }
  }
}

/**
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
/**
 * recursion point edge-embedding
 * @param graph Gamma_1 graph
 * @param graphG2 Gamma_2 graph
 * @param mapping programming bits for Zhao
 */
void ValiantEUG::edgeEmbedding(DAG_Gamma1 *graph, DAG_Gamma2 *graphG2, const std::map<uint32_t, uint32_t>& mapping) {
  if (!graphG2 && graph) edgeEmbedding(graph, mapping);
  if (!graph || !graphG2) return;
  DAG_Gamma1 *left = graphG2->gamma1_left;
  DAG_Gamma1 *right = graphG2->gamma1_right;
  DAG_Gamma2 *leftG2 = graphG2->sub_left;
  DAG_Gamma2 *rightG2 = graphG2->sub_right;

  std::vector<DAG_Gamma1 *> g1(k);
  if (k == 2) {
    g1[0] = left;
    g1[1] = right;
  } else { // k == 4
    if (!leftG2 && !rightG2) return;
    if (leftG2) {
      g1[0] = leftG2->gamma1_left;
      g1[1] = leftG2->gamma1_right;
    }
    if (rightG2) {
      g1[2] = rightG2->gamma1_left;
      g1[3] = rightG2->gamma1_right;
    }
  }

  for (int i = 0; i < graph->node_number; i++) {
    DAG_Gamma1::Node *currentNode = graph->node_array[i];
    int id = currentNode->number;
    int currentBlock = (int) floor(float((id - 1) / k));
    int currentBlockPosition = (id - 1) % k;

    // only for k == 4
    int currentNodePreprocessingPosition = (int) floor(float((id - 1) / 2));

    DAG_Gamma1::Node *destinationNode = currentNode->child;
    if (!destinationNode) continue;
    int destId = destinationNode->number;
    int destBlock = (int) floor(float((destId - 1) / k));
    int destBlockPosition = (destId - 1) % k;
    int destNodePreProcessingPosition = (int) floor(float((destId - 1) / 2));

    if (k == 4) {
      destNodePreProcessingPosition++;
    }

    if (currentBlock == destBlock) {
      if (currentBlockPosition != destBlockPosition) {
        this->blocks[currentBlock]->setOutputPermutationBit(currentBlockPosition, destBlockPosition);
      }
    } else {
      DAG_Gamma1 *currentGamma, *currentGammaLeft, *currentGammaRight;
      int recursionPoint;
      if (left
          && left->node_array[currentNodePreprocessingPosition]->child
          && left->node_array[currentNodePreprocessingPosition]->child->number == destNodePreProcessingPosition
          && !left->node_array[currentNodePreprocessingPosition]->is_embedded) {
        if (k == 4) {
          currentGammaLeft = g1[0];
          currentGammaRight = g1[1];
        } else {
          currentGamma = g1[0];
        }
        recursionPoint = 0;
        left->node_array[currentNodePreprocessingPosition]->is_embedded = true;
      } else if (right
          && right->node_array[currentNodePreprocessingPosition]->child
          && right->node_array[currentNodePreprocessingPosition]->child->number == destNodePreProcessingPosition
          && !right->node_array[currentNodePreprocessingPosition]->is_embedded) {
        if (k == 4) {
          currentGammaLeft = g1[2];
          currentGammaRight = g1[3];
          recursionPoint = 2;
        } else {
          currentGamma = g1[1];
          recursionPoint = 1;
        }
        right->node_array[currentNodePreprocessingPosition]->is_embedded = true;
      } else {
        std::cout << "Something is wrong with preprocessing" << std::endl;
        continue;
      }
      if (k == 4) {
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
        } else {
          std::cout << "something is wrong" << std::endl;
          return;
        }
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
                                                              + k);
      this->blocks[destBlock]->setInputPermutationBit(recursionPoint, destBlockPosition);
    }
  }
  for (Block *b : this->blocks) {
    b->edgeEmbedding(mapping);
  }

  std::vector<DAG_Gamma2 *> g2(k);
  if (k == 2) {
    g2[0] = leftG2;
    g2[1] = rightG2;
  } else { // k == 4
    g2[0] = leftG2->sub_left;
    g2[1] = leftG2->sub_right;
    g2[2] = rightG2->sub_left;
    g2[3] = rightG2->sub_right;
  }

  for (int i = 0; i < k; i++) {
    if (g1[i] && g1[i]->node_number != 0) {
      if (g2[i]) {
        this->children[i]->edgeEmbedding(g1[i], g2[i], mapping);
      } else {
        this->children[i]->edgeEmbedding(g1[i], mapping);
      }
    }
  }
}

/**
 * recursion point edge-embedding for recursion bases
 * @param graph Gamma_1 graph
 * @param mapping Programming bits for Zhao
 */
void ValiantEUG::edgeEmbedding(DAG_Gamma1 *graph, const std::map<uint32_t, uint32_t>& mapping) {
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
      std::cout << "in single embedding is something wrong" << std::endl;
    }
  }
  for (Block *b : this->blocks) {
    b->edgeEmbedding(mapping);
  }
}

/**
 * Returns blocks of the EUG
 * @return blocks of the EUG
 */
std::vector<Block *> ValiantEUG::getBlocks() { return this->blocks; }

/**
 * Returns children EUGs
 * @return children EUGs
 */
std::vector<ValiantEUG *> ValiantEUG::getChildren() { return this->children; }

/**
 * returns k-way split
 * @return k-way split
 */
int ValiantEUG::getK() const { return this->k; }
