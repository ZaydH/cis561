//
// Created by Zayd Hammoudeh on 9/26/18.
//

#ifndef PROJECT00_TREE_NODE_H
#define PROJECT00_TREE_NODE_H

#include <string>
#include <iostream>

class ASTNode {
 public:
//  explicit ASTNode(const std::string &v = "") : value_(v) {
//    next_ = nullptr;
//  }

  explicit ASTNode(char* v) : value_(std::string(v)) {
    next_ = nullptr;
  }

  const void PRINT(std::ostream &out) {
    out << value_;
    if (next_)
      next_->PRINT(out);
  }
  std::string value_;
  ASTNode* next_;
};

#endif //PROJECT00_TREE_NODE_H
