//
// Created by Michal Young on 9/12/18.
//

#include "ASTNode.h"
#include "quack_classes.h"

namespace AST {
  // Abstract syntax tree.  ASTNode is abstract base class for all other nodes.

  // Each node type needs a str method -- most are in .h file for inlining,
  // a few require more code.

  // Nothing here yet.

  bool Typing::check_type_name_exists(const std::string &type_name) const {
    if (!type_name.empty() && !Quack::Class::Container::singleton()->exists(type_name)) {
      throw "Unknown type " + type_name + " used.";
//      return false;
    }
    return true;
  }

}
