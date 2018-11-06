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
    class Container : public MapContainer<Field> {
     public:
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
    };

    Field(std::string name, std::string type_name)
      : name_(std::move(name)), type_name_(std::move(type_name)) { };

    std::string name_;
    std::string type_name_;
    Class* type_ = nullptr;
  };
}

#endif //PROJECT02_QUACK_FIELD_H
