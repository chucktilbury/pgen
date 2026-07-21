#include <iostream>
#include "parser.h"

#include "logger.h"
extern Logger logger;
#include "errors.h"
extern Errors errors;

enum parser_states {
    START_STATE = 100,
    REPEAT_STATE = 200,
    RETURN_MATCH = 300,
    RETURN_NO_MATCH = 400,
    RETURN_ERROR = 500,
};

#define TOKEN scanner.token()
#define TOKEN_STR TOKEN->get_text()
#define TOKEN_TYPE TOKEN->get_type()
#define ADVANCE scanner.advance()
#define STATE_NAME(s) (                                      \
        (s == START_STATE)             ? "START_STATE" :     \
                (s == REPEAT_STATE)    ? "REPEAT_STATE" :    \
                (s == RETURN_MATCH)    ? "RETURN_MATCH" :    \
                (s == RETURN_NO_MATCH) ? "RETURN_NO_MATCH" : \
                (s == RETURN_ERROR)    ? "RETURN_ERROR" :    \
                                         to_string(s))

// convert the entire set of expected punctuation or string
string* Parser::_convert_str(string& str) {

    if(ispunct(str[0])) {
        string val("");
        // the pgen grammar only has simple single character punct.
        switch(str[0]) {
            case '(':
                return new string("TOK_OPAREN");
            case ')':
                return new string("TOK_CPAREN");
            case '?':
                return new string("TOK_OPAREN");
            case '*':
                return new string("TOK_STAR");
            case '+':
                return new string("TOK_PLUS");
            case '/':
                return new string("TOK_SLASH");
            case '-':
                return new string("TOK_MINUS");
            case '%':
                return new string("TOK_PERCENT");
            case '|':
                return new string("TOK_PIPE");
            case '{':
                return new string("TOK_OCURLY");
            case '}':
                return new string("TOK_CCURLY");
            case '.':
                return new string("TOK_DOT");
            case ',':
                return new string("TOK_COMMA");
            case '&':
                return new string("TOK_AMPERSAND");
            case '^':
                return new string("TOK_CARAT");
            case '[':
                return new string("TOK_OSQUARE");
            case ']':
                return new string("TOK_CSQUARE");
            case '<':
                if(str.size() > 1) {
                    if(str[1] == '=')
                        return new string("TOK_LTE");
                    else {
                        _error(TOKEN, format("unknown operator: {}", str));
                        return nullptr;
                    }
                }
                else
                    return new string("TOK_LT");
            case '>':
                if(str.size() > 1) {
                    if(str[1] == '=')
                        return new string("TOK_GTE");
                    else {
                        _error(TOKEN, format("unknown operator: {}", str));
                        return nullptr;
                    }
                }
                else
                    return new string("TOK_GT");
            case '!':
                if(str.size() > 1) {
                    if(str[1] == '=')
                        return new string("TOK_NEQU");
                    else {
                        _error(TOKEN, format("unknown operator: {}", str));
                        return nullptr;
                    }
                }
                else
                    return new string("TOK_BANG");
            case '=':
                if(str.size() > 1) {
                    if(str[1] == '=')
                        return new string("TOK_ISEQU");
                    else {
                        _error(TOKEN, format("unknown operator: {}", str));
                        return nullptr;
                    }
                }
                else
                    return new string("TOK_ASSIGN");
            default:
                _error(TOKEN, format("unknown punctuation token: {}", str));
                return nullptr;
        }
    }
    else {
        string tmp = string(str);
        for(auto& c : tmp) {
            c = toupper(c);
        }
        return new string("TOK_" + tmp);
    }
}

_ast_node* Parser::parse() {
    ENTER;
    // logger.push_level(Logger::WARNING);
    _ast_node* node = (_ast_node*)_parse_grammar();
    // logger.pop_level();
    RETURN(node);
}

_ast_grammar* Parser::_parse_grammar() {
    ENTER;

    vector<_ast_rule*> list;
    _ast_rule* rule = nullptr;
    _ast_grammar* node = nullptr;

    bool finished = false;
    int state = START_STATE;
    scanner.mark_queue();

    while(!finished) {
        switch(state) {
            case START_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(TOKEN_TYPE == TOK_END_OF_FILE) {
                    _error(TOKEN, "expected at least one rule for the grammar");
                    state = RETURN_ERROR;
                }
                else if(nullptr != (rule = _parse_rule())) {
                    list.push_back(rule);
                    state = REPEAT_STATE;
                }
                else {
                    _error(TOKEN, format("expected a rule but got {}", TOKEN_STR));
                    state = RETURN_ERROR;
                }
            } break;

            case REPEAT_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(nullptr != (rule = _parse_rule())) {
                    list.push_back(rule);
                    state = REPEAT_STATE;
                }
                else {
                    state = RETURN_MATCH;
                }
            } break;

            case RETURN_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                node = new _ast_grammar(TOKEN);
                node->set_list(list);
                scanner.flush_queue();
                finished = true;
            } break;

            case RETURN_NO_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                scanner.reset_queue();
                finished = true;
            } break;

            default: {
                TRACE(format("state: {}", STATE_NAME(state)));
                _fatal(format("unknown state in {}(): {}", __PRETTY_FUNCTION__, state));
            }
        }
    }

    RETURN(node);
}

_ast_rule* Parser::_parse_rule() {
    ENTER;

    string* nterm;
    _ast_group* group = nullptr;
    _ast_rule* node = nullptr;

    bool finished = false;
    int state = START_STATE;
    scanner.mark_queue();

    while(!finished) {
        switch(state) {
            case START_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(TOKEN_TYPE == TOK_NON_TERMINAL) {
                    state = 10;
                    nterm = new string(TOKEN_STR);
                    ADVANCE;
                }
                else
                    state = RETURN_NO_MATCH;
            } break;

            case 10: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(nullptr != (group = _parse_group())) {
                    state = RETURN_MATCH;
                }
                else {
                    _error(TOKEN, format("expected a group definition but got {}", TOKEN_STR));
                    state = RETURN_ERROR;
                }
            } break;


            case REPEAT_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
            } break;

            case RETURN_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                node = new _ast_rule(TOKEN);
                node->group = group;
                node->nt = nterm;
                scanner.flush_queue();
                finished = true;
            } break;

            case RETURN_NO_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                scanner.reset_queue();
                finished = true;
            } break;

            default: {
                TRACE(format("state: {}", STATE_NAME(state)));
                _fatal(format("unknown state in {}(): {}", __PRETTY_FUNCTION__, state));
            }
        }
    }

    RETURN(node);
}

/*
    primary_expr (
        QSTRG | // creates a TERMINAL symbol
        TERMINAL |
        NON_TERMINAL |
        group_expr |
        zero_or_one_expr |
        zero_or_more_expr |
        one_or_more_expr |
        select_expr
    )
*/
_ast_primary* Parser::_parse_primary() {
    ENTER;

    _ast_group* group = nullptr;
    _ast_zero_or_one* zero_or_one = nullptr;
    _ast_zero_or_more* zero_or_more = nullptr;
    _ast_one_or_more* one_or_more = nullptr;
    _ast_select* select = nullptr;
    string* nterm = nullptr;
    string* term = nullptr;
    _ast_node* item = nullptr;
    _ast_primary* node = nullptr;

    bool finished = false;
    int state = START_STATE;
    scanner.mark_queue();

    while(!finished) {
        switch(state) {
            case START_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(TOKEN_TYPE == TOK_QSTR) {
                    term = _convert_str(TOKEN_STR);
                    ADVANCE;
                    state = RETURN_MATCH;
                }
                else {
                    state = 1;
                }
            } break;

            case 1: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(TOKEN_TYPE == TOK_TERMINAL) {
                    term = _convert_str(TOKEN_STR);
                    ADVANCE;
                    state = RETURN_MATCH;
                }
                else {
                    state = 2;
                }
            } break;

            case 2: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(TOKEN_TYPE == TOK_NON_TERMINAL) {
                    nterm = new string(TOKEN_STR);
                    ADVANCE;
                    state = RETURN_MATCH;
                }
                else {
                    state = 3;
                }
            } break;

            case 3: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(nullptr != (group = _parse_group())) {
                    item = (_ast_node*)group;
                    state = RETURN_MATCH;
                }
                else {
                    state = 4;
                }
            } break;

            case 4: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(nullptr != (zero_or_one = _parse_zero_or_one())) {
                    item = (_ast_node*)zero_or_one;
                    state = RETURN_MATCH;
                }
                else {
                    state = 5;
                }
            } break;

            case 5: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(nullptr != (zero_or_more = _parse_zero_or_more())) {
                    item = (_ast_node*)zero_or_more;
                    state = RETURN_MATCH;
                }
                else {
                    state = 6;
                }
            } break;

            case 6: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(nullptr != (one_or_more = _parse_one_or_more())) {
                    item = (_ast_node*)one_or_more;
                    state = RETURN_MATCH;
                }
                else {
                    state = 7;
                }
            } break;

            case 7: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(nullptr != (select = _parse_select())) {
                    item = (_ast_node*)select;
                    state = RETURN_MATCH;
                }
                else {
                    state = RETURN_NO_MATCH;
                }
            } break;

            case REPEAT_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
            } break;

            case RETURN_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                node = new _ast_primary(TOKEN);
                node->terminal = term;
                node->non_terminal = nterm;
                node->node = item;
                scanner.flush_queue();
                finished = true;
            } break;

            case RETURN_NO_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                scanner.reset_queue();
                finished = true;
            } break;

            default: {
                TRACE(format("state: {}", STATE_NAME(state)));
                _fatal(format("unknown state in {}(): {}", __PRETTY_FUNCTION__, state));
            }
        }
    }

    RETURN(node);
}

_ast_select* Parser::_parse_select() {
    ENTER;

    _ast_primary* primary = nullptr;
    _ast_select* node = nullptr;

    bool finished = false;
    int state = START_STATE;
    scanner.mark_queue();

    while(!finished) {
        switch(state) {
            case START_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(TOKEN_TYPE == TOK_PIPE) {
                    ADVANCE;
                    state = 1;
                }
                else {
                    state = RETURN_NO_MATCH;
                }
            } break;

            case 1: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(nullptr != (primary = _parse_primary())) {
                    state = RETURN_MATCH;
                }
                else {
                    _error(TOKEN, format("expected a primary expression but got {}", TOKEN_STR));
                    state = RETURN_ERROR;
                }
            } break;

            case REPEAT_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
            } break;

            case RETURN_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                node = new _ast_select(TOKEN);
                node->item = primary;
                scanner.flush_queue();
                finished = true;
            } break;

            case RETURN_NO_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                scanner.reset_queue();
                finished = true;
            } break;

            default: {
                TRACE(format("state: {}", STATE_NAME(state)));
                _fatal(format("unknown state in {}(): {}", __PRETTY_FUNCTION__, state));
            }
        }
    }

    RETURN(node);
}

/*
    group_expr ( '(' + primary_expr ')' )
*/
_ast_group* Parser::_parse_group() {
    ENTER;

    vector<_ast_primary*> list;
    _ast_primary* primary = nullptr;
    _ast_group* node = nullptr;

    bool finished = false;
    int state = START_STATE;
    scanner.mark_queue();

    while(!finished) {
        switch(state) {
            case START_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(TOKEN_TYPE == TOK_OPAREN) {
                    ADVANCE;
                    state = 1;
                }
                else {
                    state = RETURN_NO_MATCH;
                }
            } break;

            // one or more
            case 1: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(nullptr != (primary = _parse_primary())) {
                    list.push_back(primary);
                    state = REPEAT_STATE;
                }
                else {
                    _error(TOKEN, format("expected a primary expression but got {}", TOKEN_STR));
                    state = RETURN_ERROR;
                }
            } break;

            case REPEAT_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(nullptr != (primary = _parse_primary())) {
                    list.push_back(primary);
                    state = REPEAT_STATE;
                }
                else {
                    state = 2;
                }
            } break;

            case 2: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(TOKEN_TYPE == TOK_CPAREN) {
                    ADVANCE;
                    state = RETURN_MATCH;
                }
                else {
                    _error(TOKEN, format("expected a ')' but got a {}", TOKEN_STR));
                    state = RETURN_ERROR;
                }
            } break;

            case RETURN_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                node = new _ast_group(TOKEN);
                node->set_list(list);
                scanner.flush_queue();
                finished = true;
            } break;

            case RETURN_NO_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                scanner.reset_queue();
                finished = true;
            } break;

            default: {
                TRACE(format("state: {}", STATE_NAME(state)));
                _fatal(format("unknown state in {}(): {}", __PRETTY_FUNCTION__, state));
            }
        }
    }

    RETURN(node);
}

_ast_zero_or_one* Parser::_parse_zero_or_one() {
    ENTER;

    _ast_primary* primary = nullptr;
    _ast_zero_or_one* node = nullptr;

    bool finished = false;
    int state = START_STATE;
    scanner.mark_queue();

    while(!finished) {
        switch(state) {
            case START_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(TOKEN_TYPE == TOK_QUESTION) {
                    ADVANCE;
                    state = 1;
                }
                else {
                    state = RETURN_NO_MATCH;
                }
            } break;

            case 1: {
                if(nullptr != (primary = _parse_primary())) {
                    state = RETURN_MATCH;
                }
                else {
                    _error(TOKEN, format("expected a primary expression but got {}", TOKEN_STR));
                    state = RETURN_ERROR;
                }
            } break;

            case REPEAT_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
            } break;

            case RETURN_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                node = new _ast_zero_or_one(TOKEN);
                node->item = primary;
                scanner.flush_queue();
                finished = true;
            } break;

            case RETURN_NO_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                scanner.reset_queue();
                finished = true;
            } break;

            default: {
                TRACE(format("state: {}", STATE_NAME(state)));
                _fatal(format("unknown state in {}(): {}", __PRETTY_FUNCTION__, state));
            }
        }
    }

    RETURN(node);
}

_ast_zero_or_more* Parser::_parse_zero_or_more() {
    ENTER;

    _ast_primary* primary = nullptr;
    _ast_zero_or_more* node = nullptr;

    bool finished = false;
    int state = START_STATE;
    scanner.mark_queue();

    while(!finished) {
        switch(state) {
            case START_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(TOKEN_TYPE == TOK_STAR) {
                    ADVANCE;
                    state = 1;
                }
                else {
                    state = RETURN_NO_MATCH;
                }
            } break;

            case 1: {
                if(nullptr != (primary = _parse_primary())) {
                    state = RETURN_MATCH;
                }
                else {
                    _error(TOKEN, format("expected a primary expression but got {}", TOKEN_STR));
                    state = RETURN_ERROR;
                }
            } break;

            case REPEAT_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
            } break;

            case RETURN_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                node = new _ast_zero_or_more(TOKEN);
                node->item = primary;
                scanner.flush_queue();
                finished = true;
            } break;

            case RETURN_NO_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                scanner.reset_queue();
                finished = true;
            } break;

            default: {
                TRACE(format("state: {}", STATE_NAME(state)));
                _fatal(format("unknown state in {}(): {}", __PRETTY_FUNCTION__, state));
            }
        }
    }

    RETURN(node);
}

_ast_one_or_more* Parser::_parse_one_or_more() {
    ENTER;

    _ast_primary* primary = nullptr;
    _ast_one_or_more* node = nullptr;

    bool finished = false;
    int state = START_STATE;
    scanner.mark_queue();

    while(!finished) {
        switch(state) {
            case START_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
                if(TOKEN_TYPE == TOK_PLUS) {
                    ADVANCE;
                    state = 1;
                }
                else {
                    state = RETURN_NO_MATCH;
                }
            } break;

            case 1: {
                if(nullptr != (primary = _parse_primary())) {
                    state = RETURN_MATCH;
                }
                else {
                    _error(TOKEN, format("expected a primary expression but got {}", TOKEN_STR));
                    state = RETURN_ERROR;
                }
            } break;

            case REPEAT_STATE: {
                TRACE(format("state: {}", STATE_NAME(state)));
            } break;

            case RETURN_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                node = new _ast_one_or_more(TOKEN);
                node->item = primary;
                scanner.flush_queue();
                finished = true;
            } break;

            case RETURN_NO_MATCH: {
                TRACE(format("state: {}", STATE_NAME(state)));
                scanner.reset_queue();
                finished = true;
            } break;

            default: {
                TRACE(format("state: {}", STATE_NAME(state)));
                _fatal(format("unknown state in {}(): {}", __PRETTY_FUNCTION__, state));
            }
        }
    }

    RETURN(node);
}

void Parser::_error(string msg) {

    cerr << "syntax error: " << msg << endl;
    errors++;
}

void Parser::_warning(string msg) {

    cerr << "syntax warning: " << msg << endl;
    warnings++;
}

void Parser::_error(Token* tok, string msg) {

    cerr << tok->fname << ": "
         << tok->line_no << ": "
         << tok->col_no << ": "
         << "syntax error: " << msg << endl;
    errors++;
}

void Parser::_warning(Token* tok, string msg) {

    cerr << tok->fname << ": "
         << tok->line_no << ": "
         << tok->col_no << ": "
         << "syntax warning: " << msg << endl;
    warnings++;
}

void Parser::_fatal(string msg) {

    cerr << "fatal error: " << msg << endl;
    exit(1);
}
