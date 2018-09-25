//
// Created by Michal Young on 9/12/18.
//

#ifndef REFLEXIVE_ASTNODE_H
#define REFLEXIVE_ASTNODE_H

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include "keywords.h"
#include "CodegenContext.h"
#include "EvalContext.h"


namespace AST {
  // Abstract syntax tree.  ASTNode is abstract base class for all other nodes.

  class ASTNode {
   public:
    virtual ~ASTNode() {};

    virtual std::string STR() = 0;

    virtual int eval(EvalContext &ctx) = 0;        // Immediate evaluation
    void PRINT() {
      std::cout << STR();
    }
    // virtual std::string c_gen(CodegenContext &context) = 0;
  };

  /* A block is a sequence of statements or expressions.
   * For simplicity we'll just make it a sequence of ASTNode,
   * and leave it to the parser to build valid structures.
   */
  class Block : public ASTNode {
    std::vector<ASTNode *> stmts_;
   public:
    explicit Block() : stmts_{std::vector<ASTNode *>()} {}

    ~Block() {
      for (auto &stmt : stmts_)
        delete stmt;
    }

    void append(ASTNode *stmt) { stmts_.push_back(stmt); }

    int eval(EvalContext &ctx) override;

    std::string STR() override {
      std::stringstream ss;
      for (ASTNode *stmt: stmts_) {
        ss << stmt->STR() << std::endl;
      }
      return ss.str();
    }
  };

//  /* L_Expr nodes are AST nodes that can be evaluated for location.
//   * Most can also be evaluated for value_.  An example of an L_Expr
//   * is an identifier, which can appear on the left_ hand or right_ hand
//   * side of an assignment.  For example, in x = y, x is evaluated for
//   * location and y is evaluated for value_.
//   *
//   * For now, a location is just a name, because that's what we index
//   * the symbol table with.  In a full compiler, locations can be
//   * more complex, and typically in code generation we would have
//   * LExpr evaluate to an address in a register.
//   *
//   * LExpr is abstract.  It's only concrete subclass for now is Ident,
//   * but in a full OO language we would have LExprs that look like
//   * a.b and a[2].
//   */
//  class LExpr : public ASTNode {
//   public:
////    virtual std::string l_eval(EvalContext &ctx) = 0;
//  };

//  /* An assignment has an lvalue (location to be assigned to)
//   * and an expression.  We evaluate the expression and place
//   * the value_ in the variable.
//   */
//
//  class Assign : public ASTNode {
//    LExpr &lexpr_;
//    ASTNode &rexpr_;
//   public:
//    Assign(LExpr &lexpr, ASTNode &rexpr) :
//        lexpr_{lexpr}, rexpr_{rexpr} {}
//
//    std::string STR() override {
//      std::stringstream ss;
//      ss << lexpr_.STR() << " = "
//         << rexpr_.STR() << ";";
//      return ss.STR();
//    }
//
//    int eval(EvalContext &ctx) override;
//  };

  class If : public ASTNode {
    /** The boolean expression to be evaluated */
    ASTNode &cond_;
    /** Block exevuted if the condition is TRUE */
    Block &truepart_;
    /** Block exevuted if the condition is FALSE */
    Block &falsepart_;
   public:
    explicit If(ASTNode &cond, Block &truepart, Block &falsepart) :
        cond_{cond}, truepart_{truepart}, falsepart_{falsepart} {};

    std::string STR() override {
      return "if (" + cond_.STR() + ") {\n" +
             truepart_.STR() +
             "} else {\n" +
             falsepart_.STR() +
             "}\n";
    }

    int eval(EvalContext &ctx) override;

  };

  class While : public ASTNode {
    /** Boolean condition checked before execution a loop */
    ASTNode &cond_;
    /** Executed while condition is true */
    Block &body_;
   public:
    explicit While(ASTNode &cond, Block &body) :
        cond_{cond}, body_(body) {};

    std::string STR() override {
      return "while (" + cond_.STR() + ") {\n" + body_.STR() + "}\n";
    }

    // ToDO Write eval for While Loop
    int eval(EvalContext &ctx) override;
  };

//  /* Identifiers like x and literals like 42 are the
//   * leaves of the AST.  A literal can only be evaluated
//   * for value_ (the 'eval' method), but an identifier
//   * can also be evaluated for location (when we want to
//   * store something in it).
//   */
//  class Ident : public LExpr {
//    const std::string text_;
//   public:
//    explicit Ident(const char *txt) : text_(txt) {}
//
//    std::string STR() override { return text_; }
//
//    int eval(EvalContext &ctx) override;
//
//    std::string l_eval(EvalContext &ctx) override { return text_; }
//  };

  class Return : public AST::ASTNode {
   public:
    explicit Return(ASTNode * stmt) : stmt_(stmt) {}

    ASTNode * stmt_;

    std::string STR() override {
      std::stringstream ss("return (");
      ss << stmt_->STR() << ");";
      return ss.str();
    }

    // ToDo Define eval for Return
    int eval(EvalContext &ctx) override { return -1; }
  };

  class IntConst : public ASTNode {
    const int value_;
   public:
    explicit IntConst(const int v) : value_{v} {}

    std::string STR() override { return std::to_string(value_); }

    int eval(EvalContext &ctx) override { return value_; }
  };

  /** Stores Boolean constants */
  class BoolConst : public ASTNode {
    const int value_;
   public:
    explicit BoolConst(const bool v) : value_(v ? BOOL_TRUE: BOOL_FALSE) {}

    std::string STR() override { return (value_ == BOOL_TRUE) ? "true" : "false"; }

    int eval(EvalContext &ctx) override { return value_; }
  };

  class StringConst : public ASTNode {
    const std::string value_;
   public:
    explicit StringConst(const std::string &v) : value_(v) {}

    std::string STR() override { return value_; }

    // ToDo Define eval for a String Constant
    int eval(EvalContext &ctx) override { return -1; }
  };

  /**
   * Virtual base class for not etc
   */
  class UniOp : public ASTNode {
   public:
    // each subclass must override the inherited
    // eval() method

   protected:
    ASTNode &expr_;
    const std::string opsym;

    UniOp(const std::string &sym, ASTNode &expr) :
        opsym(sym), expr_(expr) {};
   public:
    std::string STR() override {
      std::stringstream ss;
      ss << opsym << " " << "(" << expr_.STR() << ")";
      return ss.str();
    }
  };

  //ToDo ensure NOT type checks correctly
  class Not : public UniOp {
   public:
    int eval(EvalContext &ctx) override {
      return (expr_.eval(ctx) == BOOL_TRUE) ? BOOL_FALSE : BOOL_TRUE;
    }
    explicit Not(ASTNode &expr) : UniOp(std::string("not"), expr) {};
  };

  /** Negates an integer value */
  class Negate : public UniOp {
   public:
    int eval(EvalContext &ctx) override {
      return -expr_.eval(ctx);
    }
    explicit Negate(ASTNode &expr) : UniOp(std::string("-"), expr) {};
  };

  // Virtual base class for +, -, *, /, etc
  class BinOp : public ASTNode {
   public:
    // each subclass must override the inherited
    // eval() method

   protected:
    ASTNode &left_;
    ASTNode &right_;
    const std::string opsym_;

    BinOp(const std::string &sym, ASTNode &l, ASTNode &r) :
        opsym_{sym}, left_{l}, right_{r} {};
   public:
    std::string STR() override {
      std::stringstream ss;
      ss << "(" << left_.STR() << " " << opsym_ << " "
         << right_.STR() << ")";
      return ss.str();
    }
  };

  class Plus : public BinOp {
   public:
    int eval(EvalContext &ctx) override;

    Plus(ASTNode &l, ASTNode &r) :
        BinOp(std::string("+"), l, r) {};
  };

  class Minus : public BinOp {
   public:
    int eval(EvalContext &ctx) override;

    Minus(ASTNode &l, ASTNode &r) :
        BinOp(std::string("-"), l, r) {};
  };

  class Times : public BinOp {
   public:
    int eval(EvalContext &ctx) override;

    Times(ASTNode &l, ASTNode &r) :
        BinOp(std::string("*"), l, r) {};
  };

  class Div : public BinOp {
   public:
    int eval(EvalContext &ctx) override;

    Div(ASTNode &l, ASTNode &r) :
        BinOp(std::string("/"), l, r) {};
  };

  class And : public BinOp {
   public:
    int eval(EvalContext &ctx) override {
      // Support short circuit compare
      if (left_.eval(ctx) == BOOL_FALSE) return BOOL_FALSE;
      return right_.eval(ctx);
    };
    And(ASTNode &l, ASTNode &r) : BinOp(std::string("and"), l, r) {};
  };

  class Or : public BinOp {
   public:
    int eval(EvalContext &ctx) override {
      // Support short circuit compare
      if (left_.eval(ctx) == BOOL_TRUE) return BOOL_TRUE;
      return right_.eval(ctx);
    };
    Or(ASTNode &l, ASTNode &r) : BinOp(std::string("or"), l, r) {};
  };

  class GT : public BinOp {
   public:
    int eval(EvalContext &ctx) override {
      return (left_.eval(ctx) > right_.eval(ctx)) ? BOOL_TRUE : BOOL_FALSE;
    }
    GT(ASTNode &l, ASTNode &r) : BinOp(std::string(">"), l, r) {};
  };

  class LT : public BinOp {
   public:
    int eval(EvalContext &ctx) override {
      return (left_.eval(ctx) < right_.eval(ctx)) ? BOOL_TRUE : BOOL_FALSE;
    }
    LT(ASTNode &l, ASTNode &r) : BinOp(std::string("<"), l, r) {};
  };

  class LEQ : public BinOp {
   public:
    int eval(EvalContext &ctx) override {
      return (left_.eval(ctx) <= right_.eval(ctx)) ? BOOL_TRUE : BOOL_FALSE;
    }
    LEQ(ASTNode &l, ASTNode &r) : BinOp(std::string("<="), l, r) {};
  };

  class GEQ : public BinOp {
   public:
    int eval(EvalContext &ctx) override {
      return (left_.eval(ctx) >= right_.eval(ctx)) ? BOOL_TRUE : BOOL_FALSE;
    }
    GEQ(ASTNode &l, ASTNode &r) : BinOp(std::string(">="), l, r) {};
  };

  //ToDo ensure EQ type checks correctly
  class EQ : public BinOp {
   public:
    int eval(EvalContext &ctx) override {
      return (left_.eval(ctx) == right_.eval(ctx)) ? BOOL_TRUE : BOOL_FALSE;
    }
    EQ(ASTNode &l, ASTNode &r) : BinOp(std::string("=="), l, r) {};
  };
}
#endif //REFLEXIVE_ASTNODE_H
