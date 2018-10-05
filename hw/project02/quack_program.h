//
// Created by Zayd Hammoudeh on 10/4/18.
//

#include <vector>
#include <string>
#include <iostream>

#include "ASTNode.h"
#include "quack_classes.h"

#ifndef PROJECT02_QUACK_PROGRAM_H
#define PROJECT02_QUACK_PROGRAM_H

namespace Quack {

  class Program {
   public:
    explicit Program(Class::Container *classes, AST::Block *block) :
       classes_(classes), block_(block) {};

    ~Program() {
      delete classes_;
      delete block_;
    }

    void print_original_src() {
      if (classes_)
        classes_->print_original_src(0);
      std::cout << "\n";
      if (block_)
        block_->print_original_src(0);
      std::cout << std::flush;
    }

   private:
    Class::Container *classes_;
    AST::Block* block_;
  };
}

#endif //PROJECT02_QUACK_PROGRAM_H
