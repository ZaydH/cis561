//
// Created by Michal Young on 9/12/18.
//

#include "ASTNode.h"
#include "quack_class.h"
#include "quack_param.h"
#include "quack_method.h"
#include "exceptions.h"
#include "keywords.h"
#include "compiler_utils.h"


namespace AST {

  unsigned long ASTNode::label_cnt_ = 0;
  unsigned long ASTNode::var_cnt_ = 0;

  bool Typing::check_type_name_exists(const std::string &type_name) const {
    if (!type_name.empty() && !Quack::Class::Container::singleton()->exists(type_name)) {
      throw UnknownTypeException(type_name);
//      return false;
    }
    return true;
  }

  bool Typing::update_inferred_type(TypeCheck::Settings &settings, Quack::Class *inferred_type,
                                    bool is_field) {
    // Handle the initial typing
    bool success = configure_initial_typing(inferred_type);

    type_ = type_->least_common_ancestor(inferred_type);

    success = success && expr_->update_inferred_type(settings, type_, is_field);

    type_ = Quack::Class::least_common_ancestor(type_, expr_->get_node_type());
    if (type_ == BASE_CLASS) {
      std::string msg = "Unable to reconcile types " + type_name_ + " and " + inferred_type->name_;
      throw TypeInferenceException("TypingError", msg);
    }

    success = success && verify_typing();
    return success;
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

  bool If::perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) {
//    cond_->set_node_type(Quack::Class::Container::Bool());
    bool success = cond_->perform_type_inference(settings, nullptr);
    if (cond_->get_node_type() != Quack::Class::Container::Bool())
      throw TypeInferenceException("IfCondType", "If conditional not of type Bool");

    success = success && truepart_->perform_type_inference(settings);
    success = success && falsepart_->perform_type_inference(settings);

    return success;
  }

  bool IntLit::perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) {
    set_node_type(Quack::Class::Container::Int());
    return true;
  }

  bool BoolLit::perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) {
    set_node_type(Quack::Class::Container::Bool());
    return true;
  }

  bool StrLit::perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) {
    set_node_type(Quack::Class::Container::Str());
    return true;
  }

  bool While::perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) {
    bool success = cond_->perform_type_inference(settings, nullptr);
    if (cond_->get_node_type() != Quack::Class::Container::Bool())
      throw TypeInferenceException("WhileCondType", "While conditional not of type Bool");

    success = success && body_->perform_type_inference(settings);

    return success;
  }

  bool FunctionCall::perform_type_inference(TypeCheck::Settings &settings,
                                            Quack::Class * parent_type) {
    // Only need to check node type once
    Quack::Method * method;
    if (parent_type == nullptr) {
      Quack::Class * obj_class = Quack::Class::Container::singleton()->get(ident_);
      if (obj_class == OBJECT_NOT_FOUND)
        throw UnknownConstructorException(ident_);

      method = obj_class->get_constructor();
    } else {
      method = parent_type->get_method(ident_);
      if (method == OBJECT_NOT_FOUND) {
        std::string msg = "Unknown method \"" + ident_ + "\" in cls \"" + parent_type->name_ + "\"";
        throw TypeInferenceException("MethodError", msg);
      }
    }

    // Verify that argument count is correct
    Quack::Param::Container* params = method->params_;
    if (params->count() != args_->count())
      throw TypeInferenceException("FunctionCall", "Wrong arg count for method " + ident_);

    // Iterate through the arguments and set the
    for (unsigned i = 0; i < args_->count(); i++) {
      ASTNode* arg = args_->args_[i];
      Quack::Param * param = (*params)[i];
      if (arg->get_node_type() == BASE_CLASS)
        arg->set_node_type(param->type_);

      if (!arg->get_node_type()->is_subtype(param->type_))
        throw TypeInferenceException("FunctionCall", "Param " + param->name_ + " type error");

      arg->perform_type_inference(settings, nullptr);
    }

    Quack::Class * rtrn_type = method->return_type_;

    if (rtrn_type != nullptr) {
      if (get_node_type() == BASE_CLASS) {
        set_node_type(rtrn_type);
      } else {
        set_node_type(rtrn_type->least_common_ancestor(get_node_type()));
        if (get_node_type() == BASE_CLASS)
          throw TypeInferenceException("FunctionCall", "Unable to reconcile FunctionCall return");
      }
    }
    return true;
  }

  bool Return::perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) {
    type_ = settings.return_type_;
    if (settings.is_constructor_) {
      bool success = right_->perform_type_inference(settings, parent_type);
      if (right_->get_node_type() != settings.this_class_)
        throw TypeInferenceException("InvalidReturn", "Constructor return must match class");
      return success;
    }

    if ((type_ == nullptr || right_ == nullptr) && (type_ != nullptr || right_ != nullptr))
      throw TypeInferenceException("ReturnNothing", "Mismatch of return " CLASS_NOTHING);

    right_->perform_type_inference(settings, nullptr);

    Quack::Class * right_type = right_->get_node_type();
    if (right_type == nullptr)
      throw TypeCheckerException("ReturnType", "Return has no type");
    if (!right_type->is_subtype(type_))
      throw TypeCheckerException("ReturnType", "Invalid return type \"" + right_type->name_ + "\"");

    return true;
  }

  bool UniOp::perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) {
    right_->perform_type_inference(settings, nullptr);
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

  bool BinOp::perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) {

    bool success = left_->perform_type_inference(settings, nullptr);

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
    success = success && right_->perform_type_inference(settings, parent_type);
    Quack::Class * r_type = right_->get_node_type();

    if (!r_type->is_subtype(param->type_)) {
      msg = "Invalid right type \"" + r_type->name_ + "\" for operator \"" + opsym + "\"";
      throw TypeInferenceException("BinOp", msg);
    }

    if (type_ == nullptr)
      type_ =  method->return_type_;
    else
      type_ = type_->least_common_ancestor(method->return_type_);

    // Reconcile binary operator return type
    if (type_ == BASE_CLASS) {
      msg = "Invalid return type for binary operator \"" + opsym + "\"";
      throw TypeInferenceException("BinOp", msg);
    }
    return success;
  }

  bool Ident::update_inferred_type(TypeCheck::Settings &settings, Quack::Class *inferred_type,
                                   bool is_field) {
    Symbol * sym = settings.st_->get(text_, is_field);
    assert(sym != OBJECT_NOT_FOUND);

    if (((is_field && settings.is_constructor_) || !is_field) && sym->get_type() == BASE_CLASS)
      settings.st_->update(sym, inferred_type);
    else
      settings.st_->update(sym, inferred_type->least_common_ancestor(sym->get_type()));

    type_ = (type_ == BASE_CLASS) ? sym->get_type() : sym->get_type()->least_common_ancestor(type_);
    if (type_ == BASE_CLASS)
      throw TypeInferenceException("TypeUpdateFail",
                                   "Type inference failed for variable " + text_);
    return true;
  }

  bool Ident::perform_type_inference(TypeCheck::Settings &settings, Quack::Class *parent_type) {
    // If the identifier is this, then mark as the type of this
    if (settings.this_class_ != nullptr && text_ == OBJECT_SELF) {
      Quack::Class * tc = settings.this_class_;
      type_ = (type_ == nullptr) ? tc : type_->least_common_ancestor(tc);

      if (type_ == nullptr)
        throw TypeInferenceException("ThisError", "Invalid type for \"" OBJECT_SELF "\" class");
      return true;
    }

    if (parent_type != nullptr && !parent_type->has_field(text_)) {
      std::string msg = "Unknown field \"" + text_ + "\" for type \"" + parent_type->name_ + "\"";
      throw TypeInferenceException("FieldError", msg);
    }

    Symbol * sym = settings.st_->get(text_, parent_type != nullptr);
    type_ = (type_ == nullptr) ? sym->get_type() : type_->least_common_ancestor(sym->get_type());
    return true;
  }

  bool ObjectCall::update_inferred_type(TypeCheck::Settings &settings, Quack::Class *inferred_type,
                                        bool is_field) {
    if (auto obj = dynamic_cast<Ident *>(object_)) {
      if (obj->text_ == OBJECT_SELF) {
        if (auto next = dynamic_cast<Ident *>(next_)) {
          if (settings.is_constructor_) {
            // Only in a constructor can the field type be change
            next->update_inferred_type(settings, inferred_type, true);
          } else {
            // Outside the constructor, use the field type
            Symbol *sym = settings.st_->get(next->text_, true);
            if (!inferred_type->is_subtype(sym->get_type()))
              throw TypeInferenceException("InferenceError", "Type error for field " + next->text_);
            next->update_inferred_type(settings, sym->get_type(), true);
          }

          type_ = next->get_node_type();
          return true;
        }
      }
    }

    type_ = inferred_type;
    return true;
  }

  bool ObjectCall::perform_type_inference(TypeCheck::Settings &settings, Quack::Class *parent_type){
    bool success = true;
    Quack::Class * next_class;

    // Process the object. Could be either a "this", symbol, or expression
    if (auto obj = dynamic_cast<Ident *>(object_)) {
      if (obj->text_ == OBJECT_SELF)
        next_class = settings.this_class_;
      else
        next_class = settings.st_->get(obj->text_, false)->get_type();
    } else {
      // Type infer the object then pass that information to
      success = object_->perform_type_inference(settings, nullptr);
      next_class = object_->get_node_type();
    }

    // Infer the call then update the node type
    success = success && next_->perform_type_inference(settings, next_class);
    if (type_ == nullptr)
      type_ = next_->get_node_type();
    else
      type_ = type_->least_common_ancestor(next_->get_node_type());

    // Some methods (e.g., print) return Nothing
//    if (type_ == BASE_CLASS)
//      throw TypeInferenceException("ObjectCall", "Unable to resolve object call");
    return success;
  }

  const std::string
  ObjectCall::process_object_call(const std::string &left_obj, CodeGen::Settings &settings,
                                  unsigned indent_lvl) const {
    if (auto next = dynamic_cast<FunctionCall*>(next_)) {
      return next->generate_object_call(left_obj, settings, indent_lvl);
    } else if (auto next = dynamic_cast<Ident*>(next_)) {
      std::string new_var = define_new_temp_var();
      PRINT_INDENT(indent_lvl);
      settings.fout_ << type_->generated_object_type_name() << " "
                     << new_var << " = " << left_obj << "->" << next->text_ << ";\n";
      return new_var;
    }
    throw std::runtime_error("Unexpected bottoming out of ObjectCall code generation");
  }

  bool Typecase::perform_type_inference(TypeCheck::Settings &settings, Quack::Class *parent_type) {
    // type case does not have a type
    type_ = Quack::Class::Container::singleton()->get(CLASS_NOTHING);
    Quack::Class::Container * all_classes = Quack::Class::Container::singleton();

    bool success = expr_->perform_type_inference(settings, nullptr);

    for (auto * alt : *alts_) {
      std::string new_sym_name = alt->type_names_[0];
      std::string new_sym_type_name = alt->type_names_[1];

      Quack::Class * new_sym_type = all_classes->get(new_sym_type_name);
      if (new_sym_type == OBJECT_NOT_FOUND)
        throw UnknownTypeException(new_sym_type_name);

      // Check and configure the type for the typecase symbol
      Symbol * sym = settings.st_->get(new_sym_name, false);
      if (sym->get_type() != nullptr && !new_sym_type->is_subtype(sym->get_type())) {
        std::string msg = "Cannot reconcile type for variable " + new_sym_name;
        throw TypeInferenceException("TypecaseError", msg);
      }
      settings.st_->update(new_sym_name, false, new_sym_type);

      success = success && alt->block_->perform_type_inference(settings);

      // Verify the symbol name did not change after inference
      if (sym->get_type() != new_sym_type) {
        std::string msg = "Typecase var \"" + new_sym_name + "\" inferred type mismatch.";
        throw TypeInferenceException("TypecaseMismatch", msg);
      }
    }

    return success;
  }


  //====================================================================//
  //                   Code Generation Related Method                   //
  //====================================================================//

  template<typename _T>
  std::string Literal<_T>::generate_lit_code(CodeGen::Settings &settings, unsigned indent_lvl,
                                             const std::string &gen_func_name_) const {
    std::string temp_var_name = define_new_temp_var();

    PRINT_INDENT(indent_lvl);
    settings.fout_ << type_->generated_object_type_name() << " " << temp_var_name << " = "
                   << gen_func_name_ << "(" << value_ << ");\n";

    return temp_var_name;
  }

  std::string Typing::generate_code(CodeGen::Settings &settings, unsigned indent_lvl) const {

    std::string gen_var = expr_->generate_code(settings, indent_lvl);
    if (type_name_.empty())
      return gen_var;

    PRINT_INDENT(indent_lvl);
    std::string new_var = define_new_temp_var();
    settings.fout_ << type_->generated_object_type_name() << " " << new_var << " = "
                   << "(" << type_->generated_object_type_name() << ")" << gen_var << ";\n";

    return gen_var;
  }


  std::string FunctionCall::generate_code(CodeGen::Settings &settings, unsigned indent_lvl) const {
    Quack::Class * q_class = Quack::Class::Container::singleton()->get(ident_);
    assert(q_class);

    std::vector<std::string> * arg_vars = args_->generate_args(settings, indent_lvl);

    std::string new_var = define_new_temp_var();

    PRINT_INDENT(indent_lvl);
    settings.fout_ << q_class->generated_object_type_name() << " " << new_var << " = "
                   << q_class->generated_constructor_name() << "(";

    bool first_arg = true;
    for (const auto &arg : *arg_vars) {
      if (!first_arg)
        settings.fout_ << ", ";
      first_arg = false;
      settings.fout_ << arg;
    }
    delete arg_vars;

    settings.fout_ << ");\n";
    return new_var;
  }

  std::string FunctionCall::generate_object_call(std::string object_name,
                                                 CodeGen::Settings &settings,
                                                 unsigned indent_lvl) const {
    std::vector<std::string>* func_tmp_args = args_->generate_args(settings, indent_lvl);
    std::string new_var_name = define_new_temp_var();

    PRINT_INDENT(indent_lvl);
    settings.fout_ << type_->generated_object_type_name() << " " << new_var_name << " = "
                   << object_name << "->" << ident_ << "(" << object_name;
    for (const auto &arg : *func_tmp_args)
      settings.fout_ << ", " << arg;
    delete func_tmp_args;

    settings.fout_ << ");\n";
    return new_var_name;
  }

  std::string Assn::generate_code(CodeGen::Settings &settings, unsigned indent_lvl) const {
    std::string rhs_var = rhs_->generate_code(settings, indent_lvl);
    std::string lhs_var = lhs_->generate_code(settings, indent_lvl);

    PRINT_INDENT(indent_lvl);
    settings.fout_ << rhs_->get_node_type()->generated_object_type_name() << " " << lhs_var
                   << " = " << rhs_var << ";\n";

    return NO_RETURN_VAR;
  }
}
