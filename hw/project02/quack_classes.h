//
// Created by Zayd Hammoudeh on 10/4/18.
//

#include <map>
#include <string>
#include <vector>

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
        if (!all_classes_)
          all_classes_ = new Quack::Class::Container();
        return all_classes_;
      }

     private:
      /**
       * Private constructor since it is a singleton.
       */
      Container() : MapContainer<Class>() {}
      /**
       * Stores the information on
       */
      static Container* all_classes_;
    };

    Class(char* name, char * super_type, AST::Block* constructor, Method::Container* methods)
      : name_(name), super_type_name_(strcmp(super_type, "") == 0 ? KEY_OBJ : super_type),
        constructor_(constructor), methods_(methods) {

      Container* classes = Container::singleton();
      if (classes->exists(name))
        throw("Duplicate class named " + name_);
      if (!classes->exists(super_type_name_))
        throw("Unknown super class: " + super_type_name_);

      super_ = const_cast<Class*>(classes->get(super_type_name_));
    }
    /**
     * Clear all dynamic memory in the object.
     */
    ~Class() {
      delete constructor_;
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
   private:
    /** Name of the super class of this type */
    const std::string super_type_name_;
    /** Pointer to the super class of this class. */
    Class *super_;
    /** Statements in the constructor */
    AST::Block* constructor_;

    /** All methods supported by the class */
    Method::Container* methods_;
  };
}

#endif //PROJECT02_QUACK_CLASSES_H
