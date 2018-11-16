#include <utility>

//
// Created by Michal Young on 9/12/18.
//

#ifndef ASTNODE_H
#define ASTNODE_H

#include <assert.h>

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include "keywords.h"
#include "initialized_list.h"
#include "exceptions.h"
#include "symbol_table.h"
#include "compiler_utils.h"


namespace Quack { class Class; }


namespace AST {
  // Abstract syntax tree.  ASTNode is abstract base class for all other nodes.
  // This is not the final AST for Quack ... it's a quick-and-dirty version
  // that I copied over from the calculator example, and it isn't even the
  // final version from the calculator example.

  struct ASTNode {
    virtual ~ASTNode() = default;

    virtual void print_original_src(unsigned int indent_depth = 0) = 0;
    virtual bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                             bool is_method) = 0;
    /**
     * Updates the initialized list in the initialized per use check.
     *
     * @param inits Set of initialized variables.
     * @param is_constructor True if the function being checked is a constructor.
     */
    virtual void update_initialized_list(InitializedList &inits, bool is_constructor) {}
    /**
     * Implements type inference for a single note in the AST.
     *
     * @param st Symbol table for the method
     * @param return_type Return type from the block
     * @param parent_type Type of the parent node.  If parent node has no type, then BASE_CLASS.
     *
     * @return True if type inference was successful.
     */
    virtual bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) {
      // ToDo Switch perform type inference to pure virtual
      throw ("Should not be here");
    };
    /**
     * Updates the symbol table and the nodes using an inferred type
     *
     * @param st Symbol table for the method
     * @param inferred_type Type inferred for updating
     * @param this_class Class type of the object
     *
     * @return True if the update is successful
     */
    virtual bool update_inferred_type(TypeCheck::Settings &settings, Quack::Class *inferred_type,
                                      bool is_field) {
      std::string msg = "Unpredicted node type encountered in inferred type update";
      throw TypeCheckerException("Unexpected", msg);
    }
    /**
     * Updates the type for the node.
     *
     * @param type New type for the node
     */
    inline void set_node_type(Quack::Class * new_type) { type_ = new_type; }
    /**
     * Accessor for the type of the node.
     * @return Node type
     */
    inline Quack::Class* get_node_type() { return type_; }

    static std::string indent_str(unsigned indent_level) {
      return std::string(indent_level, '\t');
    }
   protected:
    /** Type for the node */
    Quack::Class * type_ = nullptr;
    /**
     * Counter for label generator for GoTo's
     */
    static unsigned long label_counter_;
  };

  /* A block is a sequence of statements or expressions.
   * For simplicity we'll just make it a sequence of ASTNode,
   * and leave it to the parser to build valid structures.
   */
  class Block {
   public:
    Block() : stmts_{std::vector<ASTNode *>()} {}

    ~Block() {
      for (const auto &ptr : stmts_)
        delete ptr;
    }

    void append(ASTNode *stmt) { stmts_.push_back(stmt); }

    void print_original_src(unsigned int indent_depth) {
      bool is_first = true;
      std::string indent_str = std::string(indent_depth, '\t');
      for (const auto &stmt : stmts_) {
       if (!is_first)
         std::cout << "\n";
       is_first = false;

       std::cout << indent_str;
       stmt->print_original_src(indent_depth);
       std::cout << ";";
      }
    }

    bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                     bool is_method) {
      bool success = true;
      for (auto &stmt : stmts_)
        success = success && stmt->check_initialize_before_use(inits, all_inits, is_method);
      return success;
    }

    bool perform_type_inference(TypeCheck::Settings &settings) {
      for (auto * stmt : stmts_)
        stmt->perform_type_inference(settings, nullptr);
      return true;
    }

    bool empty() { return stmts_.empty(); }
   private:
    std::vector<ASTNode *> stmts_;
  };

  class If : public ASTNode {
   public:
    explicit If(ASTNode *cond, Block* truepart, Block* falsepart) :
        cond_{cond}, truepart_{truepart}, falsepart_{falsepart} {};

    ~If() {
      delete cond_;
      delete truepart_;
      delete falsepart_;
    }

    void print_original_src(unsigned int indent_depth = 0) override {
      std::string indent_str = std::string(indent_depth, '\t');
      std::cout << "if ";
      cond_->print_original_src();
      std::cout << " {\n";
      truepart_->print_original_src(indent_depth + 1);
      std::cout << (!truepart_->empty() ? "\n" : "") << indent_str << "}";

      if (!falsepart_->empty()) {
        std::cout << " else {\n";
        falsepart_->print_original_src(indent_depth + 1);
        std::cout << "\n" << indent_str << "}";
      }
    }
    /**
     * Specialized implementation of the initialize before use type check.  The initialized list
     * is modified by the function and represents the set of variables in the intersection of
     * the variables from the true and false blocks.
     *
     * @param inits Initialized list modified in place
     * @return True if the check passed.
     */
    bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                     bool is_method) override {
      InitializedList false_lits = inits;

      bool success = cond_->check_initialize_before_use(inits, all_inits, is_method)
                     && truepart_->check_initialize_before_use(inits, all_inits, is_method)
                     && falsepart_->check_initialize_before_use(false_lits, all_inits, is_method);
      // This is used in the constructor to make sure fields initialized on all paths
      if (all_inits != nullptr) {
        *all_inits = inits;
        all_inits->var_union(false_lits);
      }

      inits.var_intersect(false_lits);

      return success;
    }

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override;
   private:
    ASTNode *cond_; // The boolean expression to be evaluated
    Block *truepart_; // Execute this block if the condition is true
    Block *falsepart_; // Execute this block if the condition is false
  };

  /* Identifiers like x and literals like 42 are the
   * leaves of the AST.  A literal can only be evaluated
   * for value_ (the 'eval' method), but an identifier
   * can also be evaluated for location (when we want to
   * store something in it).
   */
  struct Ident : public ASTNode {
    explicit Ident(const char* txt) : text_{txt} {}

    void print_original_src(unsigned int indent_depth = 0) override { std::cout << text_; }

    bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                     bool is_method) override {
      return check_ident_initialized(inits, all_inits, false);
    }
    /**
     * Generalized function for checking identifiers including constructor identifiers.
     *
     * @param inits Initialized variable set
     * @param all_inits All initialized variables so far
     * @param is_field True if the identifier corresponds to a field
     * @return True if the identifier is initialized
     */
    bool check_ident_initialized(InitializedList &inits, InitializedList *all_inits,
                                 bool is_field=false) {
      if (text_ == OBJECT_SELF)
        return true;

      if (!inits.exists(text_, is_field)) {
        throw UnitializedVarException(typeid(this).name(), text_, is_field);
//        return false;
      }
      return true;
    }
    /**
     * Adds the implicit identifier to the initialized variable list.  If this function is called
     * directly, the initialized variable is marked as not a field of the class.
     *
     * @param inits Set of initialized variables.
     * @param is_constructor True if the function is a constructor.  It has no effect in this
     *                       function.
     */
    void update_initialized_list(InitializedList &inits, bool is_constructor) override {
      add_identifier_to_initialized(inits, false);
    }
    /**
     * Add the identifier to the initialized variable list.
     *
     * @param inits Set of initialized variables
     * @param is_field True if the identifier corresponds to a field.
     */
    void add_identifier_to_initialized(InitializedList &inits, bool is_field) {
      inits.add(text_, is_field);
    }
    /**
     * Updates the symbol table and the node of the symbol in the symbol table and the AST>
     *
     * @param st Symbol table
     * @param inferred_type Inferred type of the symbol
     *
     * @return True if the inferred type was successfully updated
     */
    bool update_inferred_type(TypeCheck::Settings &settings, Quack::Class *inferred_type,
                              bool is_field) override;

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override;
    /** Identifier name */
    const std::string text_;
  };

  template <typename _T>
  struct Literal : public ASTNode {
    explicit Literal(const _T &v) : value_{v} {}
    /**
     * No initialization before use check for a literal.  This function simply returns true.
     *
     * @param inits Set of initialized variables.  Not changed in the funciton.
     * @return True always.
     */
    bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                     bool is_method) override {
      return true;
    }

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override{
      throw AmbiguousInferenceException(typeid(this).name(), "Not able to infer type for literal");
    }

    /** Value of the literal */
    const _T value_;
  };

  struct IntLit : public Literal<int>{
    explicit IntLit(int v) : Literal(v) {};

    void print_original_src(unsigned int indent_depth = 0) override {
      std::cout << std::to_string(value_);
    }

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override;
  };

  struct BoolLit : public Literal<bool>{
    explicit BoolLit(bool v) : Literal(v) {};

    void print_original_src(unsigned int indent_depth = 0) override {
      std::cout << (value_ ? "true" : "false");
    }

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override;
  };

  struct StrLit : public Literal<std::string> {
    explicit StrLit(const char* v) : Literal<std::string>(std::string("")) {}

    void print_original_src(unsigned int indent_depth = 0) override {
      std::cout  << "\"" << value_ << "\"";
    }

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override;
  };

  struct BinOp : public ASTNode {
    std::string opsym;
    ASTNode *left_;
    ASTNode *right_;

    BinOp(const std::string &sym, ASTNode *l, ASTNode *r) : opsym{sym}, left_{l}, right_{r} {};

    ~BinOp() {
      delete left_;
      delete right_;
    }

    void print_original_src(unsigned int indent_depth = 0) override {
      std::cout << "(";
      left_->print_original_src();
      std::cout << " " << opsym << " ";
      right_->print_original_src();
      std::cout << ")";
    }
    /**
     * Checks if the initialize before use test on the two subexpressions passes.
     *
     * @param inits Set of initialized variables.
     * @return True if the initialized before use test passes for both subexpressions.
     */
    bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                     bool is_method) override {
      return left_->check_initialize_before_use(inits, all_inits, is_method)
             && right_->check_initialize_before_use(inits, all_inits, is_method);
    }
    /**
     * Helper function to get the method name that desugars the binary operator.
     *
     * @param op Binary operator value
     *
     * @return Desugared method name.
     */
    static const std::string op_lookup(const std::string &op) {
      if (op == "+")
        return METHOD_ADD;
      else if (op == "-")
        return METHOD_SUBTRACT;
      else if (op == "*")
        return METHOD_MULTIPLY;
      else if (op == "/")
        return METHOD_DIVIDE;
      else if (op == ">=")
        return METHOD_GEQ;
      else if (op == ">")
        return METHOD_GT;
      else if (op == "<=")
        return METHOD_LEQ;
      else if (op == "<")
        return METHOD_LT;
      else if (op == "==")
        return METHOD_EQUALITY;
      else if (op == "and")
        return METHOD_AND;
      else if (op == "or")
        return METHOD_OR;
      throw UnknownBinOpException(op);
    }

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override;
  };

  struct UniOp : public ASTNode {
    std::string opsym;
    ASTNode *right_;

    UniOp(const std::string &sym, ASTNode *r) : opsym{std::move(sym)}, right_{r} {};

    ~UniOp() { delete right_; }

    void print_original_src(unsigned int indent_depth = 0) override {
      std::cout << "(" << opsym << " ";
      right_->print_original_src();
      std::cout << ")";
    }
    /**
     * Checks if the initialize before use test passes on the right subexpression.
     *
     * @param inits Set of initialized variables.
     * @return True if the initialized before use test passes for the right subexpression.
     */
    bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                     bool is_method) override {
      return right_->check_initialize_before_use(inits, all_inits, is_method);
    }

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override;
  };

  struct Return : public ASTNode {
    ASTNode* right_;

    explicit Return(ASTNode* right) : right_(right) {}

    ~Return() {
      delete right_;
    }

    void print_original_src(unsigned int indent_depth = 0) override {
      std::cout << "return ";
      right_->print_original_src();
    }
    /**
     * Checks if the initialize before use test passes on the right subexpression.
     *
     * @param inits Set of initialized variables.
     * @return True if the initialized before use test passes for the right subexpression.
     */
    bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                     bool is_method) override {
      return right_->check_initialize_before_use(inits, all_inits, is_method);
    }

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override;
  };

  struct While : public ASTNode {
    ASTNode* cond_;
    Block* body_;

    explicit While(ASTNode* cond, Block* body) : cond_(cond), body_(body) {};

    ~While() {
      delete cond_;
      delete body_;
    }

    void print_original_src(unsigned int indent_depth = 0) override {
      std::string indent_str = std::string(indent_depth, '\t');
      std::cout << "While ";
      cond_->print_original_src();
      std::cout << " {\n";

      body_->print_original_src(indent_depth + 1);

      if (!body_->empty())
        std::cout << "\n";
      std::cout << indent_str << "}";
    }
    /**
     * Performs an initialize before use test on the while loop.  It checks the conditional
     * statement as well as the body of the while loop.
     *
     * @param inits Set of initialized variables.
     * @return True if the initialized before use test passes for the conditional statement
     *         as well as the body for the while loop
     */
    bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                     bool is_method) override {
      bool success = cond_->check_initialize_before_use(inits, all_inits, is_method);

      InitializedList body_temp_list = inits;
      success = success && body_->check_initialize_before_use(body_temp_list, all_inits, is_method);

      if (all_inits != nullptr)
        all_inits->var_union(body_temp_list);

      return success;
    }

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override;
  };

  struct RhsArgs : public ASTNode {
    std::vector<ASTNode*> args_;

    RhsArgs() {}

    ~RhsArgs() {
      for (const auto &arg: args_)
        delete arg;
    }
    /**
     * Accessor for number of arguments in the node.
     * @return Number of arguments
     */
    unsigned long count() { return args_.size(); }
//    /**
//     * Checks whether there are any input arguments
//     * @return True if there are no arguments
//     */
//    bool empty() { return args_.empty(); }

    void add(ASTNode* new_node) { args_.emplace_back(new_node); }

    void print_original_src(unsigned int indent_depth) override {
      bool is_first = true;
      for (const auto &arg : args_) {
        if (!is_first)
          std::cout << ", ";
        is_first = false;
        arg->print_original_src(indent_depth);
      }
    }

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override{
      std::string msg = "Type inference not valid for right hand side args";
      throw TypeInferenceException("UnexpectedStateReached", msg);
    }

    /**
     * Checks if the initialize before use test passes on the right subexpression.
     *
     * @param inits Set of initialized variables.
     * @return True if the initialized before use test passes for the right subexpression.
     */
    bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                     bool is_method) override {
      bool success = true;
      for (auto &arg : args_)
        success = success && arg->check_initialize_before_use(inits, all_inits, is_method);
      return success;
    }
  };

  struct ObjectCall : public ASTNode {
    ASTNode* object_;
    ASTNode* next_;

    ObjectCall(ASTNode* object, ASTNode* next) : object_(object), next_(next) {}

    ~ObjectCall() {
      delete object_;
      delete next_;
    }

    void print_original_src(unsigned int indent_depth) override {
      object_->print_original_src(indent_depth);
      std::cout << ".";
      next_->print_original_src(indent_depth);
    }
    /**
     * Checks if the initialize before use test passes on the right subexpression.
     *
     * @param inits Set of initialized variables.
     * @return True if the initialized before use test passes for the right subexpression.
     */
    bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                     bool is_method) override {
      if (auto obj = dynamic_cast<Ident*>(object_)) {
        if (all_inits != nullptr && obj->text_ == OBJECT_SELF) {
          if (auto next = dynamic_cast<Ident *>(next_))
            return next->check_ident_initialized(inits, all_inits, true);
        } else if(is_method && obj->text_ == OBJECT_SELF) {
          // No need to check fields for this class outside the constructor
          return true;
        }
      }

      bool success = object_->check_initialize_before_use(inits, all_inits, is_method);
      if (dynamic_cast<Ident*>(next_) != nullptr)
        return success;

      return success && next_->check_initialize_before_use(inits, all_inits, is_method);
    }

    void update_initialized_list(InitializedList &inits, bool is_field) override {
      if (auto obj = dynamic_cast<Ident*>(object_))
        if (is_field && obj->text_ == OBJECT_SELF)
          if (auto next = dynamic_cast<Ident*>(next_))
            next->add_identifier_to_initialized(inits, true);
    }

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override;

    bool update_inferred_type(TypeCheck::Settings &settings, Quack::Class *inferred_type,
                              bool is_field) override;
  };

  struct FunctionCall : public ASTNode {
    const std::string ident_;
    RhsArgs* args_;

    FunctionCall(char* ident, RhsArgs* args) : ident_(ident), args_(args) {}

    ~FunctionCall() {
      delete args_;
    }
    /**
     * Checks if the initialize before use test passes on the right subexpression.
     *
     * @param inits Set of initialized variables.
     * @return True if the initialized before use test passes for the right subexpression.
     */
    bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                     bool is_method) override {
      return args_->check_initialize_before_use(inits, all_inits, is_method);
    }

    void print_original_src(unsigned int indent_depth) override {
      std::cout << ident_ << "(";
      args_->print_original_src(indent_depth);
      std::cout << ")";
    }

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override;
  };

  struct Typing : public ASTNode {
    Typing(ASTNode* expr, const std::string &type_name) : expr_(expr), type_name_(type_name) {}

    ~Typing() {
      delete expr_;
    }

    ASTNode* expr_;
    std::string type_name_;

    void print_original_src(unsigned int indent_depth) override {
      expr_->print_original_src(indent_depth);
      if (!type_name_.empty())
        std::cout << " : " << type_name_;
    }
    /**
     * Helper function used to check if the specified type name actually exists.
     *
     * @param type_name Name of the type used
     * @return True if the type_name_ is a valid class.
     */
    bool check_type_name_exists(const std::string &type_name) const;

    bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                     bool is_method) override {
//      if (!check_type_name_exists(type_name_))
//        return false;

      return expr_->check_initialize_before_use(inits, all_inits, is_method);
    }
    /**
     * Used in an assignment statement to update the set of initialized variables.
     *
     * @param inits Set of initialized variables.
     * @param is_constructor True if the function is a constructor.
     */
    void update_initialized_list(InitializedList &inits, bool is_constructor) override {
      expr_->update_initialized_list(inits, is_constructor);
    }

    bool update_inferred_type(TypeCheck::Settings &settings, Quack::Class *inferred_type,
                              bool is_field) override;

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override{
      configure_initial_typing(BASE_CLASS);

      bool success = expr_->perform_type_inference(settings, parent_type);

      return success && verify_typing();
    }

   private:
    /**
     * Configures the initial typing of the node predicated on the specified type name.
     *
     * @param other_type Other type n
     * @return
     */
    bool configure_initial_typing(Quack::Class* other_type);
    /**
     * Checks that the typing of the expression is compatible with the typing of the node (if any)
     *
     * @return True if typing successfully verified
     */
    bool verify_typing();
  };

  class TypeAlternative {
   public:
    TypeAlternative(const std::string &t1, const std::string &t2, Block* block)
              : type_names_{t1,t2}, block_(block) {}

    ~TypeAlternative() { delete block_; }

    void print_original_src(unsigned int indent_depth) {
      std::cout << type_names_[0] << " : " << type_names_[1] << " {\n";
      block_->print_original_src(indent_depth + 1);

      std::cout << "\n" << std::string(indent_depth, '\t') << "}";
    }

    std::string type_names_[2];
    Block* block_;
  };


  struct Assn : public ASTNode {
    Typing* lhs_;
    ASTNode* rhs_;

    Assn(Typing* lhs, ASTNode* rhs) : lhs_(lhs), rhs_(rhs) {};

    ~Assn() {
      delete lhs_;
      delete rhs_;
    }

    void print_original_src(unsigned int indent_depth = 0) override {
      lhs_->print_original_src(indent_depth);
      std::cout << " = ";
      rhs_->print_original_src(indent_depth);
    }
    /**
     * Verifies that both the left and right hand side of statement pass the initialize before
     * use test.  It next adds the assigned variable to the initialized list.
     * @param inits Set of initialized variables
     * @return True if all initialized before use test passes.
     */
    bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                     bool is_method) override {
      bool success = rhs_->check_initialize_before_use(inits, all_inits, is_method);
      if (!success)
        return false;

      bool is_constructor = (all_inits != nullptr);
      lhs_->update_initialized_list(inits, is_constructor);
      if (is_constructor)
        all_inits->var_union(inits);

      return lhs_->check_initialize_before_use(inits, all_inits, is_method);
    }

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override{

      bool success = rhs_->perform_type_inference(settings, nullptr);

      success = success && lhs_->update_inferred_type(settings, rhs_->get_node_type(), false);

      success = success && lhs_->perform_type_inference(settings, nullptr);
      return success;
    }
  };

  struct Typecase : public ASTNode {
    // ToDo No typechecking in typecase
    Typecase(ASTNode* expr, std::vector<TypeAlternative*>* alts) : expr_(expr), alts_(alts) {}

    ~Typecase() {
      delete expr_;
      for (const auto &alt : *alts_)
        delete alt;
      delete alts_;
    }

    void print_original_src(unsigned int indent_depth) override {
      std::string indent_str = std::string(indent_depth, '\t');
      std::cout << KEY_TYPECASE << " ";
      expr_->print_original_src(0);
      std::cout << " {\n";
      bool is_first = true;

      // Print the alternate blocks
      for (const auto &alt : *alts_) {
        if (!is_first)
          std::cout << "\n";
        is_first = false;
        std::cout << indent_str << "\t";
        alt->print_original_src(indent_depth + 1);
      }

      if (!alts_->empty())
        std::cout << "\n";
      std::cout << indent_str << "}";
    }

    bool perform_type_inference(TypeCheck::Settings &settings, Quack::Class * parent_type) override;

    bool check_initialize_before_use(InitializedList &inits, InitializedList *all_inits,
                                     bool is_method) override {
      bool success = expr_->check_initialize_before_use(inits, all_inits, false);
      all_inits->var_intersect(inits);

      for (unsigned i = 0; i < alts_->size(); i++) {
        TypeAlternative * alt = (*alts_)[i];
        InitializedList type_init(inits);

        // Add the typed object
        type_init.add(alt->type_names_[0], false);
        success = success && alt->block_->check_initialize_before_use(type_init, all_inits, false);

        // Update the initialized variable lists.
        if (i == 0)
          inits = type_init;
        else
          inits.var_intersect(type_init);
        all_inits->var_union(type_init);
      }

      return success;
    }

   private:
    ASTNode* expr_;
    std::vector<TypeAlternative*>* alts_;
  };
}
#endif //ASTNODE_H
