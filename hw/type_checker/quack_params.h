//
// Created by Zayd Hammoudeh on 10/4/18.
//

#include <iostream>
#include <string>

#include "container_templates.h"

#ifndef PROJECT02_QUACK_PARAMS_H
#define PROJECT02_QUACK_PARAMS_H

namespace Quack {

  /**
   * Forward declaration of a Quack Class.  This is to allow links between parameters
   * and their actual type definition.
   */
  class Class;

  /**
   * Parameter to a class constructor or a method.
   */
  class Param {
   public:
    class Container : public VectorContainer<Param> {
     public:
      const void print_original_src(unsigned int indent_depth) {
        VectorContainer<Param>::print_original_src_(indent_depth, ", ");
      }
    };

    Param(const std::string &name, const std::string &type_name = "")
        : name_(name), type_name_(type_name) {
      type_class_ = nullptr;
    }

    void print_original_src(unsigned int indent_depth = 0) {
      std::string indent_str = "";
      if (indent_depth > 0)
        indent_str = std::string(indent_depth, '\t');

      std::cout << indent_str << name_;
      if (!type_name_.empty())
        std::cout << " : " << type_name_;
    }

    std::string name_;
    std::string type_name_;
    Class * type_class_;
  };
}

#endif //PROJECT02_QUACK_PARAMS_H