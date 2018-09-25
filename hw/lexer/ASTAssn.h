//
// Created by Zayd Hammoudeh on 9/24/18.
//

#ifndef AST_ASTASSN_H
#define AST_ASTASSN_H

#include <sstream>

#include "ASTNode.h"
#include "QClass.h"

namespace AST {

  /**
 * Named object.  This can be a variable or an object.
 */
  class NamedObject : public ASTNode {
   public:
    explicit NamedObject(const std::string &name) : name_(name) { }

    std::string str() { return name_; }

    const std::string name_;

    // Identifiers live in symtab and default to 0.
    // Taken from Young's original code
    int eval(EvalContext &ctx) {
      if (ctx.symtab.count(name_) == 1)
        return ctx.symtab[name_];
      else
        return 0;
    }
  };

  class ObjectCall : public NamedObject {
   public:
    /**
     * Object call represented as a linked list.  Each element in the list
     * represents one element in the call.
     *
     * @param name Name of the object/field.
     * @param next Next element in the object call.
     */
    explicit ObjectCall(const std::string &name, NamedObject* next)
        : AST::NamedObject(name), next_(next) { }

    ~ObjectCall() {
      delete next_;
    }

    std::string str() {
      std::stringstream ss(name_);
      if (next_)
        ss << "." << const_cast<NamedObject*>(next_)->str();
      return ss.str();
    }
    // ToDo Implement eval for ObjectCall
    int eval(EvalContext &ctx) { return -1; };

    /** Next parameter in the object call */
    const NamedObject* next_ = nullptr;
  };

  class ObjectMethodCall : public ObjectCall {
   public:
    explicit ObjectMethodCall(const std::string &name,
                              const QuackClass::Parameter::Container *params,
                              NamedObject* next=nullptr)
                              : ObjectCall(name, next), params_(params) {}

    ~ObjectMethodCall() {
      delete params_;

      // No need to call destructor of base class
      // See: https://stackoverflow.com/questions/677620/do-i-need-to-explicitly-call-the-base-virtual-destructor
    }

    std::string str() {
      std::stringstream ss;
      ss << name_ << "(" << QuackClass::Parameter::print_container(params_) << ")" << ".";
      if (next_)
        ss << const_cast<NamedObject*>(next_)->str();
      return ss.str();
    }

    // ToDo Implement eval for ObjectMethodCall
    int eval(EvalContext &ctx) { return -1; };

    /** Parameters for the function call */
    const AST::QuackClass::Parameter::Container* params_;
  };

  class AssnVar : public ASTNode {
   public:
    explicit AssnVar(const AST::NamedObject* var_info, ASTNode* assn_stmt,
                     const std::string &type_name="")
        : var_info_(var_info), type_name_(type_name), assn_stmt_(assn_stmt){
      // ToDo Decide what to do about type
      if (type_name.empty()) {
        var_type_ = nullptr;
      } else {

      }
    }

    std::string str() {
      std::stringstream ss;
      ss << const_cast<AST::NamedObject*>(var_info_)->str();
      if (!type_name_.empty())
        ss << " : " << type_name_;
      return ss.str();
    }

    int eval(EvalContext &ctx) {
      std::string loc = const_cast<AST::NamedObject*>(var_info_)->str();
      int rvalue = assn_stmt_->eval(ctx);
      ctx.symtab[loc] = rvalue;
      return rvalue;
    }

    const AST::NamedObject* var_info_;
    const std::string type_name_;
    /** Type associated with the assignment */
    QuackClass * var_type_;
    /** Statement of the variable assignment */
    ASTNode* assn_stmt_;
  };

}

#endif //AST_ASTASSN_H
