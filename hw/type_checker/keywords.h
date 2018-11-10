//
// Created by Zayd Hammoudeh on 10/4/18.
//

#ifndef PROJECT02_KEYWORDS_H
#define PROJECT02_KEYWORDS_H

#define KEY_CLASS "class"
#define KEY_DEF "def"
#define KEY_EXTENDS "extends"
#define KEY_TYPECASE "typecase"

#define CLASS_INT "Int"
#define CLASS_STR "String"
#define CLASS_OBJ "Obj"
#define CLASS_BOOL "Boolean"
#define CLASS_NOTHING "Nothing"

#define UNARY_OP_NOT "not"
#define UNARY_OP_NEG "-"

#define FIELD_OTHER_LIT_NAME "other"

#define METHOD_MAIN "main"
//#define METHOD_CONSTRUCTOR "__CONSTRUCTOR__"

#define METHOD_PRINT "PRINT"
#define METHOD_STR "STR"

#define METHOD_EQUALITY "EQUALS"

#define METHOD_ADD "PLUS"
#define METHOD_SUBTRACT "MINUS"
#define METHOD_MULTIPLY "TIMES"
#define METHOD_DIVIDE "DIVIDE"

#define METHOD_GT "MORE"
#define METHOD_LT "LESS"
#define METHOD_GEQ "ATLEAST"
#define METHOD_LEQ "ATMOST"

#define METHOD_AND "and"
#define METHOD_OR "or"

#define OBJECT_SELF "this"
#define OBJECT_NOT_FOUND nullptr
#define BASE_CLASS nullptr

#define EXIT_SCANNER 4
#define EXIT_PARSER 8
#define EXIT_CLASS_HIERARCHY 16
#define EXIT_INITIALIZE_BEFORE_USE 32
#define EXIT_TYPE_INFERENCE 64

#endif //PROJECT02_KEYWORDS_H
