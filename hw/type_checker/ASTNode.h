#include <utility>

//
// Created by Michal Young on 9/12/18.
//

#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include "keywords.h"

namespace AST {
  // Abstract syntax tree.  ASTNode is abstract base class for all other nodes.
  // This is not the final AST for Quack ... it's a quick-and-dirty version
  // that I copied over from the calculator example, and it isn't even the
  // final version from the calculator example.

  class ASTNode {
   public:
    virtual ~ASTNode() = default;

    virtual void print_original_src(unsigned int indent_depth = 0) = 0;
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

    bool empty() { return stmts_.empty(); }
   private:
    std::vector<ASTNode *> stmts_;
  };

  class If : public ASTNode {
    ASTNode *cond_; // The boolean expression to be evaluated
    Block *truepart_; // Execute this block if the condition is true
    Block *falsepart_; // Execute this block if the condition is false
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
  };

  /* Identifiers like x and literals like 42 are the
   * leaves of the AST.  A literal can only be evaluated
   * for value_ (the 'eval' method), but an identifier
   * can also be evaluated for location (when we want to
   * store something in it).
   */
  class Ident : public ASTNode {
    std::string text_;
   public:
    explicit Ident(const char* txt) : text_{txt} {}

    void print_original_src(unsigned int indent_depth = 0) override { std::cout << text_; }
  };

  template <typename _T>
  class Literal : public ASTNode {
   public:
    explicit Literal(const _T &v) : value_{v} {}

    /** Value of the literal */
    const _T value_;
  };

  class IntLit : public Literal<int>{
   public:
    explicit IntLit(int v) : Literal(v) {};

    void print_original_src(unsigned int indent_depth = 0) override {
      std::cout << std::to_string(value_);
    }
  };

  class BoolLit : public Literal<bool>{
   public:
    explicit BoolLit(bool v) : Literal(v) {};

    void print_original_src(unsigned int indent_depth = 0) override {
      std::cout << (value_ ? "true" : "false");
    }
  };

  class StrLit : public Literal<std::string> {
   public:
    explicit StrLit(const char* v) : Literal<std::string>(std::string("")) {}

    void print_original_src(unsigned int indent_depth = 0) {
      std::cout  << "\"" << value_ << "\"";
    }
  };

  class BinOp : public ASTNode {
   public:
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
  };

  class UniOp : public ASTNode {
   public:
    std::string opsym;
    ASTNode *right_;

    UniOp(const std::string &sym, ASTNode *r) : opsym{std::move(sym)}, right_{r} {};

    ~UniOp() { delete right_; }

    void print_original_src(unsigned int indent_depth = 0) override {
      std::cout << "(" << opsym << " ";
      right_->print_original_src();
      std::cout << ")";
    }
  };

  class Return : public ASTNode {
   public:
    ASTNode* right_;

    explicit Return(ASTNode* right) : right_(right) {}

    ~Return() {
      delete right_;
    }

    void print_original_src(unsigned int indent_depth = 0) override {
      std::cout << "return ";
      right_->print_original_src();
    }
  };

  class While : public ASTNode {
   public:
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
  };

  class Assn : public ASTNode {
   public:
    ASTNode* lhs_;
    ASTNode* rhs_;

    Assn(ASTNode* lhs, ASTNode* rhs) : lhs_(lhs), rhs_(rhs) {};

    ~Assn() {
      delete lhs_;
      delete rhs_;
    }

    void print_original_src(unsigned int indent_depth = 0) {
      lhs_->print_original_src(indent_depth);
      std::cout << " = ";
      rhs_->print_original_src(indent_depth);
    }
  };

  class RhsArgs : public ASTNode {
   public:
    std::vector<ASTNode*> args_;

    RhsArgs() {}

    ~RhsArgs() {
      for (const auto &arg: args_)
        delete arg;
    }

    unsigned long count() { return args_.size(); }

    bool empty() { return args_.empty(); }

    void add(ASTNode* new_node) { args_.emplace_back(new_node); }

    void print_original_src(unsigned int indent_depth) {
      bool is_first = true;
      for (const auto &arg : args_) {
        if (!is_first)
          std::cout << ", ";
        is_first = false;
        arg->print_original_src(indent_depth);
      }
    }
  };

  class ObjectCall : public ASTNode {
   public:
    ASTNode* object_;
    ASTNode* next_;

    ObjectCall(ASTNode* object, ASTNode* next) : object_(object), next_(next) {}

    ~ObjectCall() {
      delete object_;
      delete next_;
    }

    void print_original_src(unsigned int indent_depth) {
      object_->print_original_src(indent_depth);
      std::cout << ".";
      next_->print_original_src(indent_depth);
    }
  };

  class FunctionCall : public ASTNode {
   public:
    std::string ident_;
    RhsArgs* args_;

    FunctionCall(char* ident, RhsArgs* args) : ident_(ident), args_(args) {}

    ~FunctionCall() {
      delete args_;
    }

    void print_original_src(unsigned int indent_depth) {
      std::cout << ident_ << "(";
      args_->print_original_src(indent_depth);
      std::cout << ")";
    }
  };

  class Typing : public ASTNode {
   public:
    Typing(ASTNode* expr, const std::string &type_name) : expr_(expr), type_name_(type_name) {}

    ~Typing() {
      delete expr_;
    }

    ASTNode* expr_;
    std::string type_name_;

    void print_original_src(unsigned int indent_depth) {
      expr_->print_original_src(indent_depth);
      if (!type_name_.empty())
        std::cout << " : " << type_name_;
    }

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

  class Typecase : public ASTNode {
   public:
    Typecase(ASTNode* expr, std::vector<TypeAlternative*>* alts) : expr_(expr), alts_(alts) {}

    ~Typecase() {
      delete expr_;
      for (const auto &alt : *alts_)
        delete alt;
      delete alts_;
    }

    void print_original_src(unsigned int indent_depth) {
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

   private:
    ASTNode* expr_;
    std::vector<TypeAlternative*>* alts_;
  };
}
#endif //ASTNODE_H
