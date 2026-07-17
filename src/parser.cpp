
#include <iostream>
#include "parser.h"

#include "logger.h"
extern Logger logger;
#include "errors.h"
extern Errors errors;

enum parser_states {
    START_STATE = 0,
    REPEAT_STATE = 100,
    RETURN_MATCH = 200,
    RETURN_NO_MATCH = 300,
    RETURN_ERROR = 400,
};

#define TOKEN state->scanner->token()
#define TOKEN_STR TOKEN->get_text()
#define TOKEN_TYPE TOKEN->get_type()
#define ADVANCE state->scanner->advance()
#define STATE_NAME(s) ( \
    (s == START_STATE)? "START_STATE" : \
    (s == REPEAT_STATE)? "REPEAT_STATE" : \
    (s == RETURN_MATCH)? "RETURN_MATCH" : \
    (s == RETURN_NO_MATCH)? "RETURN_NO_MATCH" : \
    (s == RETURN_ERROR)? "RETURN_ERROR" : to_string(s))

// convert the entire set of expected punctuation or string
static string* convert_str(string& str) {

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
            case '|':
                return new string("TOK_PIPE");
            case '!':
                return new string("TOK_BANG");
            default:
                errors.error(format("unknown punctuation token: {:c}", str[0]));
                return nullptr;
        }
    }
    else
        return new string("TOK_" + str);
}

/*
    grammar ( + rule )
 */
AstGrammar* ParseGrammar(ParserState* state) {

    ENTER;
    START(" parser ");
    bool finished = false;
    int s = START_STATE;
    state->scanner->mark_queue();
    AstGrammar* node = nullptr;
    AstRule* rule = nullptr;
    vector<AstRule*>* list;

    while(!finished) {
        switch(s) {
            case START_STATE:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(TOKEN_TYPE == TOK_END_OF_FILE) {
                    errors.error("at least one rule is required in a grammar");
                    s = RETURN_ERROR;
                }
                else if(nullptr != (rule = ParseRule(state))) {
                    list = new vector<AstRule*>;
                    list->push_back(rule);
                    s = REPEAT_STATE;
                }
                else {
                    errors.error(format("expected a rule but got {}", TOKEN_STR));
                    s = RETURN_ERROR;
                }
                break;

            case REPEAT_STATE:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(TOKEN_TYPE == TOK_END_OF_FILE)
                    s = RETURN_MATCH;
                else if(nullptr != (rule = ParseRule(state)))
                    list->push_back(rule);
                else {
                    errors.error(format("expected a rule but got {}", TOKEN_STR));
                    s = RETURN_ERROR;
                }
                break;

            case RETURN_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                node = new AstGrammar(TOKEN);
                node->set_list(list);
                state->scanner->flush_queue();
                finished = true;
                break;

            case RETURN_NO_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                state->scanner->reset_queue();
                finished = true;
                break;

            case RETURN_ERROR:
                TRACE(format("state: {}", STATE_NAME(s)));
                // TODO: Implement error recovery
                finished = true;
                break;

            default:
                TRACE(format("state: {}", STATE_NAME(s)));
                errors.fatal(format("unknown state in ParseGrammar(): {}", s));
        }
    }

    END(" parser ");
    RETURN(node);
}

/*
    rule ( NON_TERMINAL grouping_expr )
 */
AstRule* ParseRule(ParserState* state) {

    ENTER;
    bool finished = false;
    int s = START_STATE;
    state->scanner->mark_queue();
    AstRule* node = nullptr;
    string* non_term = nullptr;
    AstGrouping* group = nullptr;

    while(!finished) {
        switch(s) {
            case START_STATE:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(TOKEN_TYPE == TOK_NON_TERMINAL) {
                    non_term = new string(TOKEN_STR);
                    TRACE(format("non terminal: {}", *non_term));
                    ADVANCE;
                    s = 1;
                }
                else
                    s = RETURN_NO_MATCH;
                break;

            case 1:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(nullptr != (group = ParseGroup(state))) {
                    s = RETURN_MATCH;
                }
                else {
                    errors.error("expected a grouped expression");
                    s = RETURN_ERROR;
                }
                break;

            case RETURN_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                node = new AstRule(TOKEN);
                node->set_non_terminal(non_term);
                node->set_expr(group);
                state->scanner->flush_queue();
                finished = true;
                break;

            case RETURN_NO_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                state->scanner->reset_queue();
                finished = true;
                break;

            case RETURN_ERROR:
                TRACE(format("state: {}", STATE_NAME(s)));
                // TODO: Implement error recovery
                finished = true;
                break;

            default:
                TRACE(format("state: {}", STATE_NAME(s)));
                errors.fatal(format("unknown state in ParseRule(): {}", s));
        }
    }

    RETURN(node);
}

/*
    The order of these operations matters.

    primary_expr (
        QSTRG | // creates a TERMINAL symbol
        TERMINAL |
        NON_TERMINAL |
        grouping_expr |
        zero_or_one_expr |
        zero_or_more_expr |
        one_or_more_expr |
        not_expr |
        select_expr
    )
 */
AstPrimary* ParsePrimary(ParserState* state) {

    ENTER;
    bool finished = false;
    int s = START_STATE;
    state->scanner->mark_queue();
    AstPrimary* node = nullptr;
    string* symbol = nullptr;
    AstNode* item = nullptr;

    while(!finished) {
        switch(s) {
            case START_STATE:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(TOKEN_TYPE == TOK_QSTR) {
                    symbol = convert_str(TOKEN_STR);
                    if(symbol) {
                        ADVANCE;
                        s = 9;
                    }
                    else {
                        errors.error(format("unknown operator: {}", TOKEN_STR));
                        s = RETURN_ERROR;
                    }
                }
                else
                    s = 1;
                break;

            case 1:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(TOKEN_TYPE == TOK_TERMINAL) {
                    symbol = new string(TOKEN_STR);
                    ADVANCE;
                    s = 9;
                }
                else
                    s = 2;
                break;

            case 2:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(TOKEN_TYPE == TOK_NON_TERMINAL) {
                    symbol = new string(TOKEN_STR);
                    ADVANCE;
                    s = 10;
                }
                else
                    s = 3;
                break;

            case 3:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(nullptr != (item = ParseGroup(state)))
                    s = 11;
                else
                    s = 4;
                break;

            case 4:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(nullptr != (item = ParseZeroOrOne(state)))
                    s = 11;
                else
                    s = 5;
                break;

            case 5:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(nullptr != (item = ParseZeroOrMore(state)))
                    s = 11;
                else
                    s = 6;
                break;

            case 6:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(nullptr != (item = ParseOneOrMore(state)))
                    s = 11;
                else
                    s = 7;
                break;

            case 7:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(nullptr != (item = ParseNot(state)))
                    s = 11;
                else
                    s = 8;
                break;

            case 8:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(nullptr != (item = ParseSelect(state)))
                    s = 11;
                else
                    s = RETURN_NO_MATCH;
                break;

            case 9:
                TRACE(format("state: {}", STATE_NAME(s)));
                node = new AstPrimary(TOKEN);
                node->set_terminal(symbol);
                node->set_non_terminal(nullptr);
                node->set_node(nullptr);
                s = RETURN_MATCH;
                break;

            case 10:
                TRACE(format("state: {}", STATE_NAME(s)));
                node = new AstPrimary(TOKEN);
                node->set_terminal(nullptr);
                node->set_non_terminal(symbol);
                node->set_node(nullptr);
                s = RETURN_MATCH;
                break;

            case 11:
                TRACE(format("state: {}", STATE_NAME(s)));
                node = new AstPrimary(TOKEN);
                node->set_terminal(nullptr);
                node->set_non_terminal(nullptr);
                node->set_node((AstNode*)item);
                s = RETURN_MATCH;
                break;

            case RETURN_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                state->scanner->flush_queue();
                finished = true;
                break;

            case RETURN_NO_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                state->scanner->reset_queue();
                finished = true;
                break;

            case RETURN_ERROR:
                TRACE(format("state: {}", STATE_NAME(s)));
                // TODO: Implement error recovery
                finished = true;
                break;

            default:
                TRACE(format("state: {}", STATE_NAME(s)));
                errors.fatal(format("unknown state in ParsePrimary(): {}", s));
        }
    }

    RETURN(node);
}

/*
    select_expr ( primary_expr '|' primary_expr )
 */
AstSelect* ParseSelect(ParserState* state) {

    ENTER;
    bool finished = false;
    int s = START_STATE;
    state->scanner->mark_queue();
    AstSelect* node = nullptr;
    AstPrimary* item = nullptr;

    while(!finished) {
        switch(s) {
            case START_STATE:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(TOKEN_TYPE == TOK_PIPE) {
                    ADVANCE;
                    s = 1;
                }
                else
                    s = RETURN_NO_MATCH;
                break;

            case 1:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(nullptr != (item = ParsePrimary(state)))
                    s = RETURN_MATCH;
                else
                    s = RETURN_NO_MATCH;
                break;

            case RETURN_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                node = new AstSelect(TOKEN);
                node->set_item(item);
                state->scanner->flush_queue();
                finished = true;
                break;

            case RETURN_NO_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                state->scanner->reset_queue();
                finished = true;
                break;

            case RETURN_ERROR:
                // TODO: Implement error recovery
                TRACE(format("state: {}", STATE_NAME(s)));
                finished = true;
                break;

            default:
                TRACE(format("state: {}", STATE_NAME(s)));
                errors.fatal(format("unknown state in ParseSelect(): {}", s));
        }
    }

    RETURN(node);
}

/*
    grouping_expr ( '(' + primary_expr ')' )
 */
AstGrouping* ParseGroup(ParserState* state) {

    ENTER;
    bool finished = false;
    int s = START_STATE;
    state->scanner->mark_queue();
    AstGrouping* node = nullptr;
    AstPrimary* item = nullptr;
    vector<AstPrimary*> list;

    while(!finished) {
        switch(s) {
            case START_STATE:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(TOKEN_TYPE == TOK_OPAREN) {
                    ADVANCE;
                    s = 1;
                }
                else
                    s = RETURN_NO_MATCH;
                break;

            case 1:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(nullptr != (item = ParsePrimary(state))) {
                    //list = new vector<AstPrimary*>;
                    list.push_back(item);
                    s = REPEAT_STATE;
                }
                else {
                    errors.error(format("expected at least one primary expression but got {}", TOKEN_STR));
                    s = RETURN_ERROR;
                }
                break;

            case 2:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(TOKEN_TYPE == TOK_CPAREN) {
                    ADVANCE;
                    s = RETURN_MATCH;
                }
                else {
                    errors.error(format("expected a ')' but got {}", TOKEN_STR));
                    s = RETURN_ERROR;
                }
                break;

            case REPEAT_STATE:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(nullptr != (item = ParsePrimary(state)))
                    list.push_back(item);
                else {
                    s = 2;
                }
                break;

            case RETURN_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                node = new AstGrouping(TOKEN);
                TRACE(format("size of the list: {}", list.size()));
                node->set_list(list);
                state->scanner->flush_queue();
                finished = true;
                break;

            case RETURN_NO_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                state->scanner->reset_queue();
                finished = true;
                break;

            case RETURN_ERROR:
                TRACE(format("state: {}", STATE_NAME(s)));
                // TODO: Implement error recovery
                finished = true;
                break;

            default:
                TRACE(format("state: {}", STATE_NAME(s)));
                errors.fatal(format("unknown state in ParseGrouping(): {}", s));
        }
    }

    RETURN(node);
}

/*
    zero_or_one_expr ( '?' primary_expr )
 */
AstZeroOrOne* ParseZeroOrOne(ParserState* state) {

    ENTER;
    bool finished = false;
    int s = START_STATE;
    state->scanner->mark_queue();
    AstZeroOrOne* node = nullptr;
    AstPrimary* item = nullptr;

    while(!finished) {
        switch(s) {
            case START_STATE:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(TOKEN_TYPE == TOK_QUESTION) {
                    ADVANCE;
                    s = 1;
                }
                else
                    s = RETURN_NO_MATCH;
                break;

            case 1:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(nullptr != (item = ParsePrimary(state)))
                    s = RETURN_MATCH;
                else {
                    errors.error(format("expected a primary expression but got {}", TOKEN_STR));
                    s = RETURN_ERROR;
                }
                break;

            case RETURN_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                node = new AstZeroOrOne(TOKEN);
                node->set_item(item);
                state->scanner->flush_queue();
                finished = true;
                break;

            case RETURN_NO_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                state->scanner->reset_queue();
                finished = true;
                break;

            case RETURN_ERROR:
                TRACE(format("state: {}", STATE_NAME(s)));
                // TODO: Implement error recovery
                finished = true;
                break;

            default:
                TRACE(format("state: {}", STATE_NAME(s)));
                errors.fatal(format("unknown state in ParseZeroOrOne(): {}", s));
        }
    }

    RETURN(node);
}

/*
    zero_or_more_expr ( '*' primary_expr )
 */
AstZeroOrMore* ParseZeroOrMore(ParserState* state) {

    ENTER;
    bool finished = false;
    int s = START_STATE;
    state->scanner->mark_queue();
    AstZeroOrMore* node = nullptr;
    AstPrimary* item = nullptr;

    while(!finished) {
        switch(s) {
            case START_STATE:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(TOKEN_TYPE == TOK_STAR) {
                    ADVANCE;
                    s = 1;
                }
                else
                    s = RETURN_NO_MATCH;
                break;

            case 1:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(nullptr != (item = ParsePrimary(state)))
                    s = RETURN_MATCH;
                else {
                    errors.error(format("expected a primary expression but got {}", TOKEN_STR));
                    s = RETURN_ERROR;
                }
                break;

            case RETURN_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                node = new AstZeroOrMore(TOKEN);
                node->set_item(item);
                state->scanner->flush_queue();
                finished = true;
                break;

            case RETURN_NO_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                state->scanner->reset_queue();
                finished = true;
                break;

            case RETURN_ERROR:
                TRACE(format("state: {}", STATE_NAME(s)));
                // TODO: Implement error recovery
                finished = true;
                break;

            default:
                TRACE(format("state: {}", STATE_NAME(s)));
                errors.fatal(format("unknown state in ParseZeroOrMore(): {}", s));
        }
    }

    RETURN(node);
}

/*
    one_or_more_expr ( '+' primary_expr )
 */
AstOneOrMore* ParseOneOrMore(ParserState* state) {

    ENTER;
    bool finished = false;
    int s = START_STATE;
    state->scanner->mark_queue();
    AstOneOrMore* node = nullptr;
    AstPrimary* item = nullptr;

    while(!finished) {
        switch(s) {
            case START_STATE:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(TOKEN_TYPE == TOK_PLUS) {
                    ADVANCE;
                    s = 1;
                }
                else
                    s = RETURN_NO_MATCH;
                break;

            case 1:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(nullptr != (item = ParsePrimary(state)))
                    s = RETURN_MATCH;
                else {
                    errors.error(format("expected a primary expression but got {}", TOKEN_STR));
                    s = RETURN_ERROR;
                }
                break;

            case RETURN_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                node = new AstOneOrMore(TOKEN);
                node->set_item(item);
                state->scanner->flush_queue();
                finished = true;
                break;

            case RETURN_NO_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                state->scanner->reset_queue();
                finished = true;
                break;

            case RETURN_ERROR:
                TRACE(format("state: {}", STATE_NAME(s)));
                // TODO: Implement error recovery
                finished = true;
                break;

            default:
                TRACE(format("state: {}", STATE_NAME(s)));
                errors.fatal(format("unknown state in ParseOneOrMore(): {}", s));
        }
    }

    RETURN(node);
}

/*
    not_expr ( '!' primary_expr )
 */
AstNot* ParseNot(ParserState* state) {

    ENTER;
    bool finished = false;
    int s = START_STATE;
    state->scanner->mark_queue();
    AstNot* node = nullptr;
    AstPrimary* item = nullptr;

    while(!finished) {
        switch(s) {
            case START_STATE:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(TOKEN_TYPE == TOK_BANG) {
                    ADVANCE;
                    s = 1;
                }
                else
                    s = RETURN_NO_MATCH;
                break;

            case 1:
                TRACE(format("state: {}", STATE_NAME(s)));
                if(nullptr != (item = ParsePrimary(state)))
                    s = RETURN_MATCH;
                else {
                    errors.error(format("expected a primary expression but got {}", TOKEN_STR));
                    s = RETURN_ERROR;
                }
                break;

            case RETURN_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                node = new AstNot(TOKEN);
                node->set_item(item);
                state->scanner->flush_queue();
                finished = true;
                break;

            case RETURN_NO_MATCH:
                TRACE(format("state: {}", STATE_NAME(s)));
                state->scanner->reset_queue();
                finished = true;
                break;

            case RETURN_ERROR:
                TRACE(format("state: {}", STATE_NAME(s)));
                // TODO: Implement error recovery
                finished = true;
                break;

            default:
                TRACE(format("state: {}", STATE_NAME(s)));
                errors.fatal(format("unknown state in ParseNot(): {}", s));
        }
    }

    RETURN(node);
}




