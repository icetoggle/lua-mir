#ifndef MEMBUF_H
#define MEMBUF_H
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

typedef struct {
    char *buf;
    size_t len;
    size_t cap;
}Membuf;

void membuf_init(Membuf *mb);
void membuf_free(Membuf *mb);
void membuf_clear(Membuf *mb);
void membuf_grow(Membuf *mb, size_t n);
void membuf_append(Membuf *mb, const char *data, size_t n);
void membuf_append_str(Membuf *mb, const char *str);
void membuf_append_fmt(Membuf *mb, const char *fmt, ...);
void membuf_append_vfmt(Membuf *mb, const char *fmt, va_list ap);
void membuf_append_char(Membuf *mb, char c);
const char* membuf_to_string(Membuf *mb);
void membuf_addquoted(Membuf *mb, const char *s, size_t len);

#define MC(str) if(!ctx->is_scan_jump) {membuf_append(&ctx->buf, str, sizeof(str) - 1);}
#define MCF(str, ...) if(!ctx->is_scan_jump) {membuf_append_fmt(&ctx->buf, str, ##__VA_ARGS__);};

#endif