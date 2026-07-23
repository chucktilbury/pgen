
#include <algorithm>

#include "logger.h"
extern Logger logger;

#include "ast_format.h"

void AstFormat::_traverse_grammar(_ast_grammar* node) {
    ENTER;
    START(" traverse ");

    for(auto item : node->items) {
        _traverse_rule(item);
    }

    END(" traverse ");
    RETURN();
}

void AstFormat::_traverse_rule(_ast_rule* node) {
    ENTER;

    TRACE(format("define non-terminal symbol: {}", *node->nt));
    crnt_line = new string("");
    emit(format("{} ", *node->nt));
    _traverse_group(node->group);
    store_rule(new Rule(node, crnt_line));

    RETURN();
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
void AstFormat::_traverse_primary(_ast_primary* node) {
    ENTER;

    // TRACE(format("this: {}: {}", (void*)this, typeid(node).name()));
    if(node->non_terminal != nullptr) {
        TRACE(format("non-terminal symbol: \"{}\"", *node->non_terminal));
        emit(format("{} ", *node->non_terminal));
    }
    else if(node->terminal != nullptr) {
        TRACE(format("terminal symbol: \"{}\"", *node->terminal));
        emit(format("{} ", *node->terminal));
        if(find(term_list.begin(), term_list.end(), *node->terminal) == term_list.end())
            term_list.push_back(*node->terminal);
    }
    else if(node->node != nullptr) {
        TRACE(format("traversing node type: {}", node->node->to_str()));
        if(node->node->type() == _ast_node::AST_GROUP) {
            _traverse_group((_ast_group*)node->node);
        }
        else if(node->node->type() == _ast_node::AST_SELECT) {
            _traverse_select((_ast_select*)node->node);
        }
        else if(node->node->type() == _ast_node::AST_ZERO_OR_ONE) {
            _traverse_zero_or_one((_ast_zero_or_one*)node->node);
        }
        else if(node->node->type() == _ast_node::AST_ZERO_OR_MORE) {
            _traverse_zero_or_more((_ast_zero_or_more*)node->node);
        }
        else if(node->node->type() == _ast_node::AST_ONE_OR_MORE) {
            _traverse_one_or_more((_ast_one_or_more*)node->node);
        }
        else
            errors.fatal(format("unknown primary type: {}", typeid(node->node).name()));
    }
    else
        errors.fatal("invalid primary type");

    RETURN();
}

void AstFormat::_traverse_group(_ast_group* node) {
    ENTER;

    TRACE(format("list size is {}", node->list.size()));
    emit("(\n");
    pad++;
    indent();
    for(auto item : node->list) {
        _traverse_primary(item);
    }
    pad--;
    emit("\n");
    indent();
    emit(")");

    RETURN();
}

void AstFormat::_traverse_select(_ast_select* node) {
    ENTER;

    emit("|\n");
    indent();
    _traverse_primary(node->item);

    RETURN();
}

void AstFormat::_traverse_zero_or_one(_ast_zero_or_one* node) {
    ENTER;

    emit("?");
    _traverse_primary(node->item);

    RETURN();
}

void AstFormat::_traverse_zero_or_more(_ast_zero_or_more* node) {
    ENTER;

    emit("*");
    _traverse_primary(node->item);

    RETURN();
}

void AstFormat::_traverse_one_or_more(_ast_one_or_more* node) {
    ENTER;

    emit("+");
    _traverse_primary(node->item);

    RETURN();
}
