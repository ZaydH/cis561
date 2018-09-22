//
// Created by Zayd Hammoudeh on 9/21/18.
//

#ifndef AST_Q_VARIABLE_H
#define AST_Q_VARIABLE_H

class QuackVar {
 public:
  QuackVar(const std::string &name, const QuackClass * type)
    : name_(name), type_(type) {
    // ToDo -- Add code to initialize the variable
  }
  /**
   * Checks whether this variable is of the specified type.
   *
   * @param type Type for comparison
   * @return True if this variable is of a matching type.
   */
  const bool is_type(const QuackClass * type) {
    assert(type);
    return type_ == type;
  }

 private:
  const QuackClass * type_;
  const std::string name;
  // ToDo Add an object for the value of the variable
};


#endif //AST_Q_VARIABLE_H
