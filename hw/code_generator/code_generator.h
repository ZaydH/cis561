//
// Created by Zayd Hammoudeh on 11/15/18.
//

#ifndef TYPE_CHECKER_CODE_GENERATOR_H
#define TYPE_CHECKER_CODE_GENERATOR_H

#include <string>
#include <iomanip>
#include <sstream>
#include <boost/filesystem.hpp>

#include "quack_program.h"
#include "compiler_utils.h"
#include "ASTNode.h"

namespace filesys = boost::filesystem;

namespace CodeGen {
  class Gen {
   public:

    Gen(Quack::Program * prog, const std::string &quack_filename) : prog_(prog) {
      // Preserve path and filename for the exported quack
      filesys::path pathObj(quack_filename);
      pathObj.replace_extension(".c");
      output_file_path_ = pathObj.string();
      fout_.open(output_file_path_);
    }

    ~Gen() {
      fout_.close();
    }

    void run() {
      export_includes();
    }

   private:
    /** Write any includes to the beginning of the generated file. */
    void export_includes() {
      std::string libs[] = {"builtins", "stdlib", "stdio"};
      for (auto &lib : libs)
        fout_ << "#include<" << lib << ".h>\n";
      fout_ << std::endl;
    }

    void generate_class(Quack::Class * q_class) {
      fout_ << "typedef struct {\n"
      fout_ << "\n} " << q_class->export_name() << ";\n";
    }

    void generate_method(Quack::Class * q_class, Quack::Method* method) {
      if (method->return_type_)
        fout_ << method->return_type_->export_name();
      else
        fout_ << "void";
      fout_ << " " << (q_class) ? q_class->export_method_name(method) : method->name_ << " ";

      fout_ << " {";

      fout_ << "\n}\n\n";
    }

    void export_main() {
      Quack::Method * main = ;
      generate_method(nullptr, prog_->main_);

      fout_ << "\n\n" << "int main() { "
            << AST::ASTNode::indent_str(1) << METHOD_MAIN << "(NULL);\n"
            << "}" << std::endl;
    }

    std::string output_file_path_;
    /** Filestream where the generated code is written */
    std::ofstream fout_;

    const Quack::Program * prog_;
  };
}

#endif //TYPE_CHECKER_CODE_GENERATOR_H
