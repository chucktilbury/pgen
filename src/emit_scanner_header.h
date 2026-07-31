#ifndef _EMIT_SCANNER_HEADER_H_
#define _EMIT_SCANNER_HEADER_H_

#include "ast.h"
//#include "templates.h"
#include "ast_format.h"

#include "errors.h"
extern Errors errors;
#include "logger.h"
extern Logger logger;

using namespace std;

class _emit_scanner_header {

    public:
    _emit_scanner_header(Rule* rules): rules(rules) {}
    void emit();

    private:
    Rule* rules;
};

#endif /* _EMIT_SCANNER_HEADER_H_ */
