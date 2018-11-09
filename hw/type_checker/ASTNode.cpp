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

  bool If::perform_type_inference(Symbol::Table *st, Quack::Class * return_type,
                                  Quack::Class * parent_type) {
//    cond_->set_node_type(Quack::Class::Container::Bool());
    bool success = cond_->perform_type_inference(st, return_type, nullptr);
    if (cond_->get_node_type() != Quack::Class::Container::Bool())
      throw TypeInferenceException("IfCondType", "If conditional not of type Bool");

    success = success && truepart_->perform_type_inference(st, return_type);
    success = success && falsepart_->perform_type_inference(st, return_type);

    return success;
  }

  bool IntLit::perform_type_inference(Symbol::Table *st, Quack::Class * return_type,
                                      Quack::Class * parent_type) {
    set_node_type(Quack::Class::Container::Bool());
    return true;
  }

  bool BoolLit::perform_type_inference(Symbol::Table *st, Quack::Class * return_type,
                                       Quack::Class * parent_type) {
    set_node_type(Quack::Class::Container::Bool());
    return true;
  }

  bool StrLit::perform_type_inference(Symbol::Table *st, Quack::Class * return_type,
                                      Quack::Class * parent_type) {
    set_node_type(Quack::Class::Container::Str());
    return true;
  }

  bool While::perform_type_inference(Symbol::Table *st, Quack::Class * return_type,
                                     Quack::Class * parent_type) {
    bool success = cond_->perform_type_inference(st, return_type, nullptr);
    if (cond_->get_node_type() != Quack::Class::Container::Bool())
      throw TypeInferenceException("WhileCondType", "While conditional not of type Bool");

    success = success && body_->perform_type_inference(st, return_type);

    return success;
  }

  bool FunctionCall::perform_type_inference(Symbol::Table *st, Quack::Class * return_type,
                                            Quack::Class * parent_type) {
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
    if (params->count() != args_->count()) {
      std::string msg = "Wrong arg count for method " + ident_;
      throw TypeInferenceException("FunctionCall", msg.c_str());
    }

    // Iterate through the arguments and set the
    for (int i = 0; i < args_->count(); i++) {
      ASTNode* arg = args_->args_[i];
      Quack::Param * param = (*params)[i];
      if (arg->get_node_type() == BASE_CLASS)
        arg->set_node_type(param->type_);

      if (!arg->get_node_type()->is_subtype(param->type_)) {
        std::string msg = "Param " + param->name_ + " type error";
        throw TypeInferenceException("FunctionCall", msg.c_str());
      }

      arg->perform_type_inference(st, return_type, nullptr);
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
                                      Quack::Class * parent_type) {
    type_ = return_type;
    if ((type_ == nullptr || right_ == nullptr) && (type_ != nullptr || right_ != nullptr))
      throw TypeInferenceException("ReturnNothing", "Mismatch of return " CLASS_NOTHING);

    right_->perform_type_inference(st, return_type, nullptr);

    Quack::Class * r_type = right_->get_node_type();
    if (r_type == nullptr)
      throw TypeCheckerException("ReturnType", "return has no type");
    if (!r_type->is_subtype(type_))
      throw TypeCheckerException("ReturnType", ("Invalid return type " + r_type->name_).c_str());

    return true;
  }

  bool UniOp::perform_type_inference(Symbol::Table *st, Quack::Class *return_type,
                                     Quack::Class *parent_type) {
    right_->perform_type_inference(st, return_type, nullptr);
    type_ = right_->get_node_type();

    if (type_ == BASE_CLASS)
      throw TypeInferenceException("UniOp", "Unary operator called on statement of no type");

    if ((opsym == UNARY_OP_NEG && type_ != Quack::Class::Container::Int())
        || (opsym == UNARY_OP_NOT && type_ != Quack::Class::Container::Bool())) {
      std::string msg = "Operator \"" + opsym + "\" does not match type " + type_->name_;
      throw TypeInferenceException("UniOp", msg.c_str());
    }
    return true;
  }
}
