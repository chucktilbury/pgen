/**
 * @file tokens.h
 *
 * @brief
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-01
 * @copyright Copyright (c) 2025
 */
#ifndef _TOKENS_H_
#define _TOKENS_H_

typedef struct {
    const char* str;
    int type;
} token_t;

token_t* create_token(const char* str, int type);
const char* tok_to_str(int type);

#endif /* _TOKENS_H_ */
