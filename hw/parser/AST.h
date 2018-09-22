//
// Created by Zayd Hammoudeh on 9/21/18.
//

#ifndef AST_AST_H
#define AST_AST_H

#include <string>
#include <vector>

#include "quack\class.h"
#include "quack\variable.h"
#include "quack\statement.h"

class AST {

 private:
  /**
   * Supported classes
   */
  std::map<std::string, QuackClass> classes_;
  std::map<std::string, QuackVar> vars_;
  std::vector<QuackBlock> blocks_;
};

#endif //AST_AST_H
