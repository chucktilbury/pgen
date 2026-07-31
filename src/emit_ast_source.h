#ifndef _EMIT_AST_SOURCE_H_
#define _EMIT_AST_SOURCE_H_


#include "ast.h"
//#include "templates.h"
#include "ast_format.h"

#include "errors.h"
extern Errors errors;
#include "logger.h"
extern Logger logger;

using namespace std;

class _emit_ast_source {

    public:
    _emit_ast_source(Rule* rules): rules(rules) {}
    void emit();

    private:
    Rule* rules;
};


#endif /* _EMIT_AST_SOURCE_H_ */
