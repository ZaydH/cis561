//
// Created by Michal Young on 9/12/18.
//

#include "ASTNode.h"
#include "quack_class.h"
#include "quack_param.h"
#include "quack_method.h"
#include "exceptions.h"
#include "keywords.h"


namespace AST {
  bool Typing::check_type_name_exists(const std::string &type_name) const {
    if (!type_name.empty() && !Quack::Class::Container::singleton()->exists(type_name)) {
      throw UnknownTypeException(type_name);
//      return false;
    }
    return true;
  }

  bool Typing::update_inferred_type(Symbol::Table *st, Quack::Class *inferred_type,
                                    bool is_field, Quack::Class * this_class) {
    // Handle the initial typing
    configure_initial_typing(inferred_type);

    type_ = Quack::Class::least_common_ancestor(type_, inferred_type);
    if (type_ == BASE_CLASS) {
      std::string msg = "Unable to reconcile types " + type_name_ + " and " + inferred_type->name_;
      throw TypeInferenceException("TypingError", msg);
    }

    expr_->update_inferred_type(st, type_, is_field, this_class);

    verify_typing();
    return true;
  }

  bool Typing::configure_initial_typing(Quack::Class *other_type) {
    // Handle the initial typing
    if (type_ == BASE_CLASS) {
      if (!type_name_.empty()) {
        type_ = Quack::Class::Container::singleton()->get(type_name_);
        if (type_ == OBJECT_NOT_FOUND)
          throw TypeInferenceException("TypingError", "Unknown type name \"" + type_name_ + "\"");
      } else {
        type_ = other_type;
      }
    }
//
//    if (type_ == BASE_CLASS)
//      throw TypeInferenceException("TypingError", "Unable to resolve initial type");
    return true;
  }

  bool Typing::verify_typing() {
    if (!expr_->get_node_type()->is_subtype(type_)) {
      std::string msg = "Unable to cast type " + expr_->get_node_type()->name_
                        + " to " + type_->name_;
      throw TypeInferenceException("TypingError", msg);
    }
    if (type_ == BASE_CLASS)
      type_ = expr_->get_node_type();
    return true;
  }

  bool If::perform_type_inference(Symbol::Table *st, Quack::Class * return_type,
                                  Quack::Class * parent_type, Quack::Class * this_class) {
//    cond_->set_node_type(Quack::Class::Container::Bool());
    bool success = cond_->perform_type_inference(st, return_type, nullptr, this_class);
    if (cond_->get_node_type() != Quack::Class::Container::Bool())
      throw TypeInferenceException("IfCondType", "If conditional not of type Bool");

    success = success && truepart_->perform_type_inference(st, return_type, this_class);
    success = success && falsepart_->perform_type_inference(st, return_type, this_class);

    return success;
  }

  bool IntLit::perform_type_inference(Symbol::Table *st, Quack::Class * return_type,
                                      Quack::Class * parent_type, Quack::Class * this_class) {
    set_node_type(Quack::Class::Container::Bool());
    return true;
  }

  bool BoolLit::perform_type_inference(Symbol::Table *st, Quack::Class * return_type,
                                       Quack::Class * parent_type, Quack::Class * this_class) {
    set_node_type(Quack::Class::Container::Bool());
    return true;
  }

  bool StrLit::perform_type_inference(Symbol::Table *st, Quack::Class * return_type,
                                      Quack::Class * parent_type, Quack::Class * this_class) {
    set_node_type(Quack::Class::Container::Str());
    return true;
  }

  bool While::perform_type_inference(Symbol::Table *st, Quack::Class * return_type,
                                     Quack::Class * parent_type, Quack::Class * this_class) {
    bool success = cond_->perform_type_inference(st, return_type, nullptr, this_class);
    if (cond_->get_node_type() != Quack::Class::Container::Bool())
      throw TypeInferenceException("WhileCondType", "While conditional not of type Bool");

    success = success && body_->perform_type_inference(st, return_type, this_class);

    return success;
  }

  bool FunctionCall::perform_type_inference(Symbol::Table *st, Quack::Class * return_type,
                                            Quack::Class * parent_type, Quack::Class * this_class) {
    // Only need to check node type once
    Quack::Method * method;
    Quack::Class * return_class;
    if (parent_type == BASE_CLASS) {
      return_class = Quack::Class::Container::singleton()->get(ident_);
      if (return_class == BASE_CLASS)
        throw UnknownConstructorException(ident_);
      method = return_class->get_constructor();
    } else {
      method = parent_type->get_method(ident_);
      return_class = method->return_type_;
    }

    // Verify that argument count is correct
    Quack::Param::Container* params = method->params_;
    if (params->count() != args_->count())
      throw TypeInferenceException("FunctionCall", "Wrong arg count for method " + ident_);

    // Iterate through the arguments and set the
    for (int i = 0; i < args_->count(); i++) {
      ASTNode* arg = args_->args_[i];
      Quack::Param * param = (*params)[i];
      if (arg->get_node_type() == BASE_CLASS)
        arg->set_node_type(param->type_);

      if (!arg->get_node_type()->is_subtype(param->type_))
        throw TypeInferenceException("FunctionCall", "Param " + param->name_ + " type error");

      arg->perform_type_inference(st, return_type, nullptr, this_class);
    }

    if (return_class != nullptr) {
      // ToDo think more about if this should be set to tightest type
      if (get_node_type() == BASE_CLASS) {
        set_node_type(return_class);
      } else {
        if (!return_class->is_subtype(get_node_type()))
          throw TypeInferenceException("FunctionCall", "Incompatible constructor type");
      }
    }
    return true;
  }

  bool Return::perform_type_inference(Symbol::Table *st, Quack::Class * return_type,
                                      Quack::Class * parent_type, Quack::Class * this_class) {
    type_ = return_type;
    if (this_class != BASE_CLASS) {
      if (right_ != nullptr)
        throw TypeInferenceException("InvalidReturn", "Cannot return anything in a constructor");
      return true;
    }

    if ((type_ == nullptr || right_ == nullptr) && (type_ != nullptr || right_ != nullptr))
      throw TypeInferenceException("ReturnNothing", "Mismatch of return " CLASS_NOTHING);

    right_->perform_type_inference(st, return_type, nullptr, this_class);

    Quack::Class * r_type = right_->get_node_type();
    if (r_type == nullptr)
      throw TypeCheckerException("ReturnType", "return has no type");
    if (!r_type->is_subtype(type_))
      throw TypeCheckerException("ReturnType", ("Invalid return type " + r_type->name_).c_str());

    return true;
  }

  bool UniOp::perform_type_inference(Symbol::Table *st, Quack::Class *return_type,
                                     Quack::Class * parent_type, Quack::Class * this_class) {
    right_->perform_type_inference(st, return_type, nullptr, this_class);
    type_ = right_->get_node_type();

    if (type_ == BASE_CLASS)
      throw TypeInferenceException("UniOp", "Unary operator called on statement of no type");

    if ((opsym == UNARY_OP_NEG && type_ != Quack::Class::Container::Int())
        || (opsym == UNARY_OP_NOT && type_ != Quack::Class::Container::Bool())) {
      std::string msg = "Operator \"" + opsym + "\" does not match type " + type_->name_;
      throw TypeInferenceException("UniOp", msg);
    }
    return true;
  }

  bool BinOp::perform_type_inference(Symbol::Table *st, Quack::Class *return_type,
                                     Quack::Class * parent_type, Quack::Class * this_class) {

    bool success = left_->perform_type_inference(st, return_type, parent_type, this_class);

    // Check the method information
    std::string msg, method_name = op_lookup(opsym);
    Quack::Class * l_type = left_->get_node_type();
    Quack::Method* method = l_type->get_method(method_name);
    if (method == OBJECT_NOT_FOUND) {
      msg = "Operator \"" + opsym + "\" does not exist for class " + l_type->name_;
      throw TypeInferenceException("BinOp", msg);
    }
    if (method->params_->count() != 1) {
      msg = "Binary operator \"" + opsym + "\" for class \"" + l_type->name_
            + "\" should take exactly one argument";
      throw TypeInferenceException("BinOp", msg);
    }

    // Verify the right type is valid
    Quack::Param* param = (*method->params_)[0];
    success = success && right_->perform_type_inference(st, return_type, parent_type, this_class);
    Quack::Class * r_type = right_->get_node_type();

    if (!r_type->is_subtype(param->type_)) {
      msg = "Invalid right type \"" + r_type->name_ + "\" for operator \"" + opsym + "\"";
      throw TypeInferenceException("BinOp", msg);
    }

    return success;
  }

  bool Ident::update_inferred_type(Symbol::Table *st, Quack::Class *inferred_type,
                                   bool is_field, Quack::Class * this_class) {
    Symbol * sym = st->get(text_, is_field);
    assert(sym != nullptr);

    // ToDo This is needed due to the constructor. May need to revisit
    if (is_field && this_class != BASE_CLASS && sym->get_class() == BASE_CLASS)
      sym->set_class(inferred_type);

    Quack::Class * updated_type = inferred_type->least_common_ancestor(sym->get_class());
    if (updated_type == BASE_CLASS)
      throw TypeInferenceException("TypeUpdateFail", "Type inference failed for variable " + text_);

    // Update the type of both the symbol and
    sym->set_class(updated_type);
    type_ = updated_type;
    return true;
  }

  bool ObjectCall::update_inferred_type(Symbol::Table *st, Quack::Class *inferred_type,
                                        bool is_field, Quack::Class *this_class) {
    if (auto obj = dynamic_cast<Ident *>(object_)) {
      if (obj->text_ == OBJECT_SELF) {
        if (auto next = dynamic_cast<Ident *>(next_)) {
          if (this_class != BASE_CLASS) {
            // Only in a constructor can the field type be change
            next->update_inferred_type(st, inferred_type, true, this_class);
          } else {
            // Outside the constructor, use the field type
            Symbol *sym = st->get(next->text_, true);
            if (!inferred_type->is_subtype(sym->get_class()))
              throw TypeInferenceException("InferenceError", "Type error for field " + next->text_);
            next->update_inferred_type(st, sym->get_class(), true, this_class);
          }

          type_ = next->get_node_type();
          return true;
        }
      }
    }

    type_ = inferred_type;
    return true;
  }
}
