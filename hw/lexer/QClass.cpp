//
// Created by Zayd Hammoudeh on 9/24/18.
//
#include <set>

#include "QClass.h"
#include "keywords.h"

std::set<std::string> AST::QuackClass::reserved_class_names_ = {KEY_OBJ, KEY_INT,
                                                                KEY_BOOL, KEY_STRING};
