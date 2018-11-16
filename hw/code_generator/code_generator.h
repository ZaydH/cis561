//
// Created by Zayd Hammoudeh on 11/15/18.
//

#ifndef TYPE_CHECKER_CODE_GENERATOR_H
#define TYPE_CHECKER_CODE_GENERATOR_H

#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <boost/filesystem.hpp>

#include "quack_program.h"
#include "quack_class.h"
#include "quack_param.h"
#include "compiler_utils.h"
#include "ASTNode.h"

namespace filesys = boost::filesystem;

namespace CodeGen {
  class Gen {
   public:

    Gen(Quack::Program * prog, const std::string &quack_filename) : prog_(prog) {
      // Preserve path and filename for the exported quack
      filesys::path path(quack_filename);
      path.replace_extension(".c");
      output_file_path_ = path.string();
      fout_.open(output_file_path_);
    }

    ~Gen() {
      fout_.close();
    }
    /**
     * Generates the output file associated with the specified program.
     */
    void run() {
      export_includes();

      for (auto & class_pair : *Quack::Class::Container::singleton()) {
        Quack::Class * q_class = class_pair.second;
        if (!q_class->is_user_class())
          continue;
        generate_class(q_class);
      }

      export_main();
    }

   private:
    /** Write any includes to the beginning of the generated file. */
    void export_includes() {
      std::pair<std::string, bool> libs[] = {{"stdlib", false},
                                             {"stdlib", false},
                                             {"builtins", true}};
      for (auto &lib_pair : libs) {
        fout_ << "#include " << (lib_pair.second ? "\"" : "<")
              << lib_pair.first << ".h" << (lib_pair.second ? "\"" : ">") << "\n";
      }
      fout_ << std::endl;
    }

    void generate_class(Quack::Class * q_class) {
      assert(q_class->is_user_class());
      fout_ << "typedef struct {\n";
      fout_ << "\n} * " << q_class->export_name() << ";\n";

      fout_ << std::endl;
    }

    void generate_method(Quack::Class * q_class, Quack::Method* method,
                         const std::string &override_method_name = "") {
      if (q_class)
        fout_ << method->return_type_->export_name();
      else
        fout_ << "void";
      // Method names can take many forms
      fout_ << " ";
      if (!override_method_name.empty())
        fout_ << override_method_name;
      else
        fout_ << (q_class ? q_class->export_method_name(method) : method->name_);

      // Print parameters
      fout_<< "(";
      if (q_class)
        q_class->generate_params(method);
      fout_ << ") {";

      fout_ << "\n}\n";
    }

//    void generate_params(Quack::Param::Container * params) {
//      if (!params)
//        return;
//      for (auto * param : *params) {
//        fout_ << "," << param->type_->export_name() << " " << param->name_;
//      }
//    }
    /** Writes the main() function to the output file. */
    void export_main() {
      generate_method(nullptr, prog_->main_, METHOD_MAIN);

      fout_ << "\n\n" << "int main() {"
            << "\n" << AST::ASTNode::indent_str(1) << METHOD_MAIN << "();\n"
            << "}" << std::endl;
    }
    /** Location to which the generated code is written */
    std::string output_file_path_;
    /** Filestream where the generated code is written */
    std::ofstream fout_;

    const Quack::Program * prog_;
  };
}

#endif //TYPE_CHECKER_CODE_GENERATOR_H
