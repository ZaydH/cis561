//
// Created by Zayd Hammoudeh on 9/21/18.
//

#ifndef AST_Q_CLASS_H
#define AST_Q_CLASS_H

#include <vector>

class QuackClass {
private:
  std::string
};

class QuackClass {
 public:
  class Method {
    /**
     * Checks the name of the method matches the specified name.
     *
     * @param name Method name to check.
     * @return True if the Method has the specified name
     */
    const bool is_name(const std::string &name) { return name_ == name; }

   private:
    const std::string name_;
    QuackClass * return_type_;
  };

  class Field {
    private:
      const std::string name;
  };

  /**
   * Checks if the specified method name exists for this class.
   *
   * @param name Method name to check.
   * @return True if the class has the specified method.
   */
  const bool has_method(const std::string & name) {
    return methods_.find(name) == methods_.end();
  }

 private:
  const QuackClass * super_ = nullptr;
  const std::string name_;
  std::map<std::string, QuackClass::Method> methods_;

};

#endif //AST_Q_CLASS_H
