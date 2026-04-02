#include "json_token.h"
#include <assert.h>

int main(void) {
    const char *json = "{}";
    json_tokenizer_t t;
    json_tokenizer_init(&t, json, 2);

    json_token_t tok1 = json_tokenizer_next(&t);
    assert(tok1.type == JSON_TOKEN_OBJECT_BEGIN);

    json_token_t tok2 = json_tokenizer_next(&t);
    assert(tok2.type == JSON_TOKEN_OBJECT_END);

    json_token_t tok3 = json_tokenizer_next(&t);
    assert(tok3.type == JSON_TOKEN_EOF);

    return 0;
}