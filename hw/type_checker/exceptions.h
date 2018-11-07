//
// Created by Zayd Hammoudeh on 11/7/18.
//

#ifndef PROJECT02_EXCEPTIONS_H
#define PROJECT02_EXCEPTIONS_H

#include <exception>
#include <string>
#include <sstream>

class TypeCheckerException : public std::exception
{
 public:
  TypeCheckerException(const char *type, const char *error) {
    std::stringstream ss;
    ss << "(" << type << "): " << error;
    msg_ = ss.str();
  };
  virtual const char *what() const throw() {
    return msg_.c_str();
  };

 protected:
  std::string msg_;
};


class InitializeBeforeUseException : public TypeCheckerException {
 public:
  InitializeBeforeUseException(const char *type, const std::string &var_name,
                               bool is_field)
                               : TypeCheckerException(type, build_error_msg(var_name, is_field)) { }
 private:
  static const char* build_error_msg(const std::string &var_name, bool is_field) {
    std::stringstream ss;
    ss << (is_field ? "Field v" : "V") << "ariable " << var_name
       << " is used before initialization.";
    return ss.str().c_str();
  }
};


class DuplicateParamException : public TypeCheckerException {
 public:
  explicit DuplicateParamException(const std::string &param_name)
                  : TypeCheckerException("Unknown Class",
                                         ("Duplicate parameter \"" + param_name + "\"").c_str()) {}
};

#endif //PROJECT02_EXCEPTIONS_H
