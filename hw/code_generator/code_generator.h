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

      CodeGen::Settings settings(fout_);

      for (auto & class_pair : *Quack::Class::Container::singleton()) {
        Quack::Class * q_class = class_pair.second;
        if (!q_class->is_user_class())
          continue;
        q_class->generate_code(settings);
      }

      export_main(settings);
      std::cout << "Code generated completed successfully." << std::endl;
    }

   private:
    /** Write any includes to the beginning of the generated file. */
    void export_includes() {
      std::pair<std::string, bool> libs[] = {{"stdlib", false},
                                             {"stdio", false},
                                             {"builtins", true}};
      for (auto &lib_pair : libs) {
        fout_ << "#include " << (lib_pair.second ? "\"" : "<")
              << lib_pair.first << ".h" << (lib_pair.second ? "\"" : ">") << "\n";
      }
      fout_ << std::endl;
    }
    /**
     * Helper function to generate the C code associated with the main function call.
     *
     * @param main_subfunc_name Name of the function to be called inside main.
     */
    void generate_main(CodeGen::Settings settings, const std::string &main_subfunc_name) {
      Quack::Class * nothing_class = Quack::Class::Container::singleton()->get(CLASS_NOTHING);

      fout_ << "\n"
            << nothing_class->generated_object_type_name() << " " << main_subfunc_name << "() {\n";

      prog_->main_->block_->generate_code(settings);

      fout_ << AST::ASTNode::indent_str(1) << "return none;\n"
            << "}" << std::endl;
    }
    /** Writes the main() function to the output file. */
    void export_main(CodeGen::Settings settings) {
      generate_main(settings, METHOD_MAIN);

      fout_ << "\n" << "int main() {"
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
