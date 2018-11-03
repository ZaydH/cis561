//
// Created by Zayd Hammoudeh on 11/3/18.
//

#ifndef PROJECT02_SYMBOL_TABLE_H
#define PROJECT02_SYMBOL_TABLE_H

#include <unordered_map>

#include "quack_classes.h"

class SymbolTable {
  void update(const std::string &symbol, Class * new_class) {
    symbols_.insert(symbol, new_class);
  }

 private:
  std::unordered_map<std::string, Class*> symbols_;
};

#endif //PROJECT02_SYMBOL_TABLE_H
