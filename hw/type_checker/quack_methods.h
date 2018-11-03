//
// Created by Zayd Hammoudeh on 10/4/18.
//

#include <map>
#include <string>
#include <iostream>

#include "keywords.h"
#include "ASTNode.h"
#include "quack_params.h"

#ifndef PROJECT02_QUACK_METHODS_H
#define PROJECT02_QUACK_METHODS_H

namespace Quack {
  class Method {
   public:
    class Container : public MapContainer<Method> {
     public:
      const void print_original_src(unsigned int indent_depth) override {
        MapContainer<Method>::print_original_src_(indent_depth, "\n");
      }
    };

    Method(const std::string &name, const std::string &return_type,
           Param::Container* params, AST::Block* block)
      : name_(name), return_type_name_(return_type), params_(params), block_(block) { };

    ~Method() {
      delete params_;
      delete block_;
    }
    /**
     * Checks that all variables in the method are initialized before use.
     *
     * @return True if the initialized before use test passes
     */
    inline bool check_initialize_before_use() {
      return block_->check_initialize_before_use();
    }
    /**
     * Debug method used to print the original source code.
     * @param indent_depth Amount of tabs to indent the block.
     */
    void print_original_src(unsigned int indent_depth = 0) {
      std::string indent_str = std::string(indent_depth, '\t');
      std::cout << indent_str << KEY_DEF << " " << name_ << "(" <<  std::flush;
      params_->print_original_src(0);
      std::cout << ")";

      if (return_type_name_ != "")
        std::cout << " " << ": " << return_type_name_;

      std::cout << " {\n";
      block_->print_original_src(indent_depth + 1);
      std::cout << (!block_->empty() ? "\n" : "") << indent_str << "}";
    }

    /** Name of the method */
    std::string name_;
    /** Name of the return type of the method (if any) */
    std::string return_type_name_;
    /** Type of the return object */
    Class* return_type_;

    Param::Container* params_;
   private:
    /** Statements (if any) to perform in method */
    AST::Block* block_;
  };
}

#endif //PROJECT02_QUACK_METHODS_H
