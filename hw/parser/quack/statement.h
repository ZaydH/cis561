//
// Created by Zayd Hammoudeh on 9/21/18.
//

#ifndef AST_Q_STATEMENT_BLOCK_H
#define AST_Q_STATEMENT_BLOCK_H

class QuackBlock {
 public:
  class Statement {
  };

  /**
   * Adds a statement to the block.
   */
  void add_statement(const QuackBlock::statement &statement) {
    statements_.push_back(statement);
  }

 private:
  /**
   * All statements in the block
   */
  std::vector<QuackBlock::Statement> statements_;
};



#endif //AST_Q_STATEMENT_BLOCK_H
