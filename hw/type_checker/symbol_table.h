//
// Created by Zayd Hammoudeh on 11/2/18.
//

#ifndef PROJECT02_SYMBOL_TABLE_H
#define PROJECT02_SYMBOL_TABLE_H

#include <string>
#include <unordered_set>

namespace AST {
  class SymbolTable {
   public:

   private:

  };


  class InitializedList {
   public:
    /**
     * Adds the specified variable name into the initialized variables list.  If the name already
     * exists, this function has no effects.
     *
     * @param var_name Name of the variable to add.
     */
    inline void add(const std::string &var_name) { vars_.insert(var_name); }
    /**
     * Takes the intersection of the two initialized variable lists (including the implicit
     * parameter).
     *
     * @param other An initialized variable list to intersect with the implicit variable list.
     */
    void Intersect(const InitializedList &other) {
      std::unordered_set<std::string> new_vars;

      for (const auto &var : other.vars_) {
        auto got = vars_.find (var);
        if(got != vars_.end())
          new_vars.insert(var);
      }

      vars_ = new_vars;
    }
   private:
    std::unordered_set<std::string> vars_;
  };
}

#endif //PROJECT02_SYMBOL_TABLE_H
