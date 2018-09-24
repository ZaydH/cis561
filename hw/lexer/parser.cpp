//
// The parser driver just glues together a parser object
// and a lexer object.
//

#include "lex.yy.h"
#include "ASTNode.h"
#include "EvalContext.h"
#include "Messages.h"
#include "QClass.h"

class Driver {
public:
    explicit Driver(reflex::Input in) : lexer(in), parser(new yy::parser(lexer, &root))
       { root = nullptr; }
    ~Driver() { delete parser; }
    AST::Program* parse() {
        // parser->set_debug_level(1); // 0 = no debugging, 1 = full tracing
        // std::cout << "Running parser\n";
        int result = parser->parse();
        if (result == 0 && report::ok()) {  // 0 == success, 1 == failure
            // std::cout << "Extracting result\n";
            if (root == nullptr) {
                std::cout << "But I got a null result!  How?!\n";
            }
            return root;
        } else {
            std::cout << "Parse failed, no tree\n";
            return nullptr;
        }
    }
private:
    yy::Lexer   lexer;
    yy::parser *parser;
    AST::Program *root;
};

int main()
{
    Driver driver(std::cin);
    AST::Program* root = driver.parse();
    // ToDo Add handling fo the classes in the program
    if (root != nullptr) {
        std::cout << "Successfully parsed: \n" << root->block_->str() << std::endl;
        auto ctx = EvalContext();
        std::cout << "Evaluates to " << root->block_->eval(ctx) << std::endl;
    } else {
        std::cout << "Extracted root was nullptr" << std::endl;
    }
}
