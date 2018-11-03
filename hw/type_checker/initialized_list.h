//
// Created by Zayd Hammoudeh on 11/3/18.
//

#ifndef PROJECT02_INITIALIZED_LIST_H
#define PROJECT02_INITIALIZED_LIST_H

#include <unordered_set>
#include <string>

class InitializedList {
 public:
  InitializedList() = default;
  /**
   * Copy constructor.  Only copys the initialized variables set.
   * @param other Object to be copied.
   */
  InitializedList(const InitializedList& other) { this->vars_ = other.vars_; }
  /**
   * Assignment operator overload.  Needed because of conditionals in the program.
   * @param other Object used to set the other object.
   * @return
   */
  InitializedList& operator=(const InitializedList &other){
    this->vars_ = other.vars_;
    return *this;
  }
  /**
   * Adds the specified variable name to the initialized variable list.
   * @param var_name Name of the variable to add
   */
  inline void add(const std::string &var_name) { vars_.insert(var_name); }
  /**
   * Checks whether the specified variable name exists in the initialized variable list.
   * @param var_name Name of the variable to check.
   */
  inline bool exists(const std::string &var_name) {
    return vars_.find(var_name) != vars_.end();
  }
  /**
   * Takes the intersection of the initialized variable list.
   *
   * @param other Other initialized list to take in the intersection
   */
  void intersect(const InitializedList& other) {
    std::unordered_set<std::string> intersect, large, small;

    // Iterate over the smaller list for better p
    small = (vars_.size() > other.vars_.size()) ? other.vars_ : vars_;
    large = (small == vars_) ? other.vars_ : vars_;
    for (const auto &var_name : small) {
      auto loc = large.find(var_name);
      if (loc != large.end())
        intersect.insert(var_name);
    }

    vars_ = intersect;
  }

 private:
  std::unordered_set<std::string> vars_;
};

#endif //PROJECT02_INITIALIZED_LIST_H
