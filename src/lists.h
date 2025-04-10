/**
 * @file lists.h
 *
 * @brief Traverse the AST to create the raw lists
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-06
 * @copyright Copyright (c) 2025
 */
#ifndef _LISTS_H_
#define _LISTS_H_

#include <stdio.h>
#include "ast.h"
#include "strg_list.h"

#include "nterm_list.h"
#include "term_list.h"
typedef struct {
    nterm_list_t* nterm_list;
    term_list_t* term_list;
    int nterm_idx;
    int term_idx;
} master_list_t;

extern master_list_t* master_list;

master_list_t* create_master_list(void);
void destroy_master_list(master_list_t* lst);
void make_raw_lists(grammar_t* node);

#endif /* _LISTS_H_ */
