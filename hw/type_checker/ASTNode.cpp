//
// Created by Michal Young on 9/12/18.
//

#include "ASTNode.h"
#include "quack_class.h"
#include "exceptions.h"
#include "keywords.h"

namespace AST {
  // Abstract syntax tree.  ASTNode is abstract base class for all other nodes.

  // Each node type needs a str method -- most are in .h file for inlining,
  // a few require more code.

  // Nothing here yet.

  bool Typing::check_type_name_exists(const std::string &type_name) const {
    if (!type_name.empty() && !Quack::Class::Container::singleton()->exists(type_name)) {
      throw UnknownTypeException(type_name);
//      return false;
    }
    return true;
  }

  bool If::perform_type_inference(Symbol::Table *st) {
    // ToDo Ensure conditional type inference is correct
    cond_->set_node_type(Quack::Class::Container::singleton()->get(CLASS_BOOL));
    bool success = cond_->perform_type_inference(st);

    success = success && truepart_->perform_type_inference(st);
    success = success && falsepart_->perform_type_inference(st);

    return success;
  }

  bool IntLit::perform_type_inference(Symbol::Table *st) {
    set_node_type(Quack::Class::Container::singleton()->get(CLASS_INT));
    return true;
  }

  bool BoolLit::perform_type_inference(Symbol::Table *st) {
    set_node_type(Quack::Class::Container::singleton()->get(CLASS_BOOL));
    return true;
  }

  bool StrLit::perform_type_inference(Symbol::Table *st) {
    set_node_type(Quack::Class::Container::singleton()->get(CLASS_STR));
    return true;
  }

  bool While::perform_type_inference(Symbol::Table *st) {
    cond_->set_node_type(Quack::Class::Container::singleton()->get(CLASS_BOOL));
    bool success = cond_->perform_type_inference(st);

    success = success && body_->perform_type_inference(st);

    return success;
  }
}
