#include <utility>

#ifndef PROJECT02_QUACK_FIELD_H
#define PROJECT02_QUACK_FIELD_H

#include <utility>
#include <cstring>
#include <assert.h>

#include "container_templates.h"

namespace Quack {
  // Forward declaration
  class Class;

  /**
   * Encapsulates Quack a single class field.
   */
  struct Field {
    struct Container : public MapContainer<Field> {
     /**
       * Prints the user defined classes only.  It will raise a runtime error if called.  Never
       * need to print the fields.
       *
       * @param indent_depth Depth to tab the contents.
       */
      const void print_original_src(unsigned int indent_depth) {
        // Do not implement.  Makes no sense since not needed.
        assert(false);
      }
      /**
       * Modified version of the field that automates boxing the field objects.
       *
       * @param name Name of the field to add.
       */
      void add_by_name(const std::string &name) {
        add(new Field(name));
      }
    };

    explicit Field(std::string name) : name_(std::move(name)), type_(nullptr) { }

    std::string name_;
    Class* type_ = nullptr;
  };
}

#endif //PROJECT02_QUACK_FIELD_H
