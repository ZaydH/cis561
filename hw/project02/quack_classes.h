//
// Created by Zayd Hammoudeh on 10/4/18.
//

#include <map>
#include <string>
#include <vector>

#ifndef PROJECT02_QUACK_CLASSES_H
#define PROJECT02_QUACK_CLASSES_H

namespace Quack {
  class Class {
   public:
    class Container {
      public:
      Container() = default;
      /** Clean up all classes. */
      ~Container() {
        for (const auto &pair : classes_)
          delete pair.second;
      }
      /**
       * Check if the key already is used for another clsass.
       */
      bool exists(const std::string &class_name) {
        return !(classes_.find(class_name) == classes_.end());
      }
      /**
       * Add the class to the container.
       */
      void add(const Class *new_class) {
        if (exists(new_class->name_))
          throw("Duplicate class: " + new_class->name_);
        classes_[new_class->name_] = new_class;
      }
      /**
       * Extract the specified class by its class name. If the specified class name does not
       * exist, then return nullptr.
       *
       * @param str Name of the class to be extracted.
       * @return A pointer to the class with the specified name if it exists and
       * nullptr otherwise.
       */
      const Class* get(const std::string &str) {
        auto itr = classes_.find(str);
        if (itr == classes_.end())
          return nullptr;
        return itr->second;
      }
     private:
      std::map<std::string,const Class*> classes_;
    };

    Class(char* name) : name_(name) {}

   private:
    /** Name of the class */
    const std::string name_;
  };
}

#endif //PROJECT02_QUACK_CLASSES_H
