//
// Created by Zayd Hammoudeh on 11/7/18.
//

#ifndef PROJECT02_EXCEPTIONS_H
#define PROJECT02_EXCEPTIONS_H

#include <exception>
#include <string>
#include <sstream>


class UnknownBinOpException : public std::exception
{
 public:
  explicit UnknownBinOpException(const std::string &op) {
    std::stringstream ss;
    ss << "(UnknownOp): Unknown binary operator \"" << op << "\"";
    msg_ = ss.str();
  };
  virtual const char *what() const throw() {
    return msg_.c_str();
  };

 protected:
  std::string msg_;
};

class TypeCheckerException : public std::exception
{
 public:
  TypeCheckerException(const char *type, const char *error) {
    std::stringstream ss;
    ss << "(" << type << "): " << error;
    msg_ = ss.str();
  }

  TypeCheckerException(const char *type, const std::string &error)
          : TypeCheckerException(type, error.c_str()) {}

  virtual const char *what() const throw() {
    return msg_.c_str();
  };

 protected:
  std::string msg_;
};


class UnknownTypeException : public TypeCheckerException {
 public:
  explicit UnknownTypeException(const std::string &class_name)
      : TypeCheckerException("TypeError", build_error_msg(class_name)) {}
 private:
  static const char* build_error_msg(const std::string &class_name) {
    return ("Unknown class \"" + class_name + "\"").c_str();
  }
};


class UnknownSymbolException : public TypeCheckerException {
 public:
  explicit UnknownSymbolException(const std::string &symbol_name)
      : TypeCheckerException("SymbolError", build_error_msg(symbol_name)) {}
 private:
  static const char* build_error_msg(const std::string &symbol_name) {
    return ("Unknown symbol \"" + symbol_name + "\"").c_str();
  }
};


class AmbiguousInferenceException : public std::exception
{
 public:
  AmbiguousInferenceException(const char *type, const char *error) {
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


class MissingSuperFieldsException : public TypeCheckerException {
 public:
  explicit MissingSuperFieldsException(const std::string &class_name)
                            : TypeCheckerException("MissingField", build_error_msg(class_name)) { }
 private:
  static const char* build_error_msg(const std::string &class_name) {
    return ("Class " + class_name + " missing fields from its super class.").c_str();
  }
};

class SubTypeFieldTypeException : public TypeCheckerException {
 public:
  explicit SubTypeFieldTypeException(const std::string &class_name, const std::string &field_name)
      : TypeCheckerException("SubtypeFieldType", build_error_msg(class_name, field_name)) { }
 private:
  static const char* build_error_msg(const std::string &class_name, const std::string &field_name) {
    return ("Class " + class_name + " field \"" + field_name +
            "\" type not subtype of super class.").c_str();
  }
};


class DuplicateParamException : public TypeCheckerException {
 public:
  explicit DuplicateParamException(const std::string &param_name)
                  : TypeCheckerException("DuplicateParam",
                                         ("Duplicate parameter \"" + param_name + "\"").c_str()) {}
};

class UnknownConstructorException : TypeCheckerException {
 public:
  explicit UnknownConstructorException(const std::string &type_name)
    : TypeCheckerException("ClassError",
                           ("Unknown class for constructor \"" + type_name + "\"").c_str()) {}
};


class TypeInferenceException : TypeCheckerException {
 public:
  explicit TypeInferenceException(const char* error_name, const char* msg)
      : TypeCheckerException(error_name, msg) {}

  explicit TypeInferenceException(const char* error_name, const std::string &msg)
      : TypeCheckerException(error_name, msg.c_str()) {}
};

#endif //PROJECT02_EXCEPTIONS_H
