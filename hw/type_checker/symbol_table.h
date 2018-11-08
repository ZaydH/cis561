#ifndef PROJECT02_SYMBOL_TABLE_H
#define PROJECT02_SYMBOL_TABLE_H

#include <unordered_map>
#include "quack_param.h"
#include "quack_field.h"
#include "quack_classes.h"
#include "exceptions.h"

class Symbol {
 public:
  class Table {
   public:
    struct Key {
      std::string name_;
      bool is_field_;

      Key(const std::string &name, bool is_field) : name_(name), is_field_(is_field) {}
    };
    /**
     * Adds a new symbol the table.  The symbol is set to the base class of all classes.
     * @param symbol_name Name of the symbol
     * @param is_field True if the new symbol is a field
     */
    void add_new(const std::string &symbol_name, bool is_field) {
      add_new(symbol_name, is_field, BASE_CLASS);
    }
    /**
     * Updates the class of the specified
     * @param symbol
     * @param new_class
     */
    void add_new(const std::string &symbol_name, bool is_field, Quack::Class *new_class) {
      Key key(symbol_name, is_field);
      objs_[key] = new Symbol(symbol_name, is_field);
    }
    /**
     * Updates the class of the specified symbol.
     *
     * @param symbol_name Name of the symbol to update.
     * @param new_class True if the corresponding symbol is a class field.
     */
    void update(const std::string &symbol_name, bool is_field, Quack::Class *new_class) {
      Key key(symbol_name, is_field);

      assert(exists(key));

      objs_[key]->set_class(new_class);
    }
    /**
     * Adds the specified parameters to the symbol table.
     *
     * @param params Parameter objects to add.
     */
    void add_params(Quack::Param::Container *params) {
      for (auto *param : *params)
        // Parameters can never be fields.
        add_new(param->name_, false, param->type_class_);
    }
    /**
     * Adds a class's fields to the symbol table.
     *
     * @param fields Set of class fields.
     */
    void add_fields(Quack::Field::Container *fields) {
      for (auto field_info : *fields) {
        Quack::Field *field = field_info.second;
        update(field->name_, true, field->type_);
      }
    }
   private:
    /**
     * Checks whether the specified key exists in the symbol table.
     *
     * @param key Symbol key
     * @return True if the key exists.
     */
    bool exists(const Key &key) const {
      return objs_[key] != objs_.end();
    }


    std::unordered_map<Key,Symbol*> objs_;
  };
  /**
   * Initialize a new symbol.  The class is set to
   *
   * @param name Name of the symbol
   * @param is_field True if the symbol is a class field.
   */
  Symbol(const std::string &name, bool is_field) : Symbol(name, is_field, nullptr) {}

  Symbol(std::string name, bool is_field, Quack::Class* q_class)
      : name_(std::move(name)), is_field_(is_field), class_(q_class) {}

  bool has_method(const std::string &method_name) const {
    return class_->has_method(method_name);
  }
  /**
   * Updates the class of the symbol.
   *
   * @param q_class New class for the symbol.
   */
  void set_class(Quack::Class* q_class) { class_ = q_class; }

 private:
  std::string name_;
  bool is_field_;
  Quack::Class * class_;
};

#endif //PROJECT02_SYMBOL_TABLE_H
