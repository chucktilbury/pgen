#ifndef _EMIT_PARSE_HEADER_H_
#define _EMIT_PARSE_HEADER_H_


#include "ast.h"
//#include "templates.h"
#include "ast_format.h"

#include "errors.h"
extern Errors errors;
#include "logger.h"
extern Logger logger;

using namespace std;

class _emit_parse_header {

    public:
    _emit_parse_header(Rule* rules): rules(rules) {}
    void emit();

    private:
    Rule* rules;
};



#endif /* _EMIT_PARSE_HEADER_H_ */
