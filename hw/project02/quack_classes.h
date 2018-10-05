//
// Created by Zayd Hammoudeh on 10/4/18.
//

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "container_templates.h"
#include "quack_methods.h"
#include "keywords.h"

#ifndef PROJECT02_QUACK_CLASSES_H
#define PROJECT02_QUACK_CLASSES_H

namespace Quack {

  class Class {
   public:
    class Container : public MapContainer<Class> {
     public:
      /**
       * Singleton object containing all of the class information.
       *
       * @return Pointer to a set of classes.
       */
      static Container* singleton() {
        static Container all_classes_;
        if (all_classes_.empty()) {
          // ToDo Initialize Class Container
        }
        return &all_classes_;
      }

      const void print_original_src(unsigned int indent_depth = 0) {
        MapContainer<Class>::print_original_src_(indent_depth, "\n\n");
      }

      Container(Container const&) = delete;       // Don't Implement
      Container& operator=(Container const&) = delete;  // Don't implement
     private:
      /**
       * Private constructor since it is a singleton.
       */
      Container() : MapContainer<Class>() {};          // Don't implement
    };

    Class(char* name, char * super_type, Param::Container* params,
          AST::Block* constructor, Method::Container* methods)
      : name_(name), super_type_name_(strcmp(super_type, "") == 0 ? CLASS_OBJ : super_type),
        constructor_params_(params), constructor_(constructor), methods_(methods) {
      Container* classes = Container::singleton();
      if (classes->exists(name))
        throw("Duplicate class named " + name_);
      // ToDo Handle Obj class
      if (!classes->exists(super_type_name_) && super_type_name_ != CLASS_OBJ)
        throw("Unknown super class: " + super_type_name_);
      if (super_type_name_ != CLASS_OBJ)
        super_ = classes->get(super_type_name_);
    }
    /**
     * Clear all dynamic memory in the object.
     */
    ~Class() {
      delete constructor_;
      delete constructor_params_;
      delete methods_;
    }
    /**
     * Checks if this class (or any of its super classes) has the specified method.
     *
     * @param name Name of the method to check
     * @return True if the class has a method with the specified name
     */
    bool has_method(const std::string &name) {
      if (methods_->exists(name))
        return true;
      if (super_)
        return super_->has_method(name);
      return false;
    }
    /**
     * Check if the class is of the specified type.
     *
     * @param name Name of the type
     * @return True if the class is of the specified type.
     */
    bool is_type(const std::string &name) {
      if (name_ == name)
        return true;
      if (super_)
        return super_->is_type(name);
      return false;
    }
    /** Name of the class */
    const std::string name_;

    void print_original_src(unsigned int indent_depth) {
      std::string indent_str = std::string(indent_depth, '\t');
      std::cout << indent_str << KEY_CLASS << " " << name_ << "(";

      constructor_params_->print_original_src(0);
      std::cout << ")";
      if (!super_type_name_.empty())
        std::cout << " " << KEY_EXTENDS << " " << super_type_name_;
      std::cout << " {\n";

      constructor_->print_original_src(indent_depth + 1);
      if (!constructor_->empty() && !methods_->empty()) {
        std::cout << "\n\n";
      }
      methods_->print_original_src(indent_depth + 1);

      std::cout << "\n" << indent_str << "}";
    }
   private:
    /** Name of the super class of this type */
    const std::string super_type_name_;
    /** Pointer to the super class of this class. */
    Class *super_;
    Param::Container* constructor_params_;
    /** Statements in the constructor */
    AST::Block* constructor_;

    /** All methods supported by the class */
    Method::Container* methods_;
  };

  class ObjectClass : public Class {
    ObjectClass()
        : Class(strdup(CLASS_OBJ), strdup(""), new Param::Container(), nullptr, nullptr) { }
  };
}

#endif //PROJECT02_QUACK_CLASSES_H
