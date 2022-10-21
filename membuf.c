#include "membuf.h"
#include <stdlib.h>
#include <string.h>
#include <luaconf.h>
#include <ctype.h>

void membuf_init(Membuf *buf)
{
    buf->buf = malloc(16);
    buf->len = 0;
    buf->cap = 16;
}


void membuf_free(Membuf *buf)
{
    free(buf->buf);
    buf->len = 0;
    buf->cap = 0;
}

void membuf_grow(Membuf *buf, size_t size)
{
    size_t new_cap = buf->cap;
    while(new_cap < size) {
        new_cap *= 2;
    }
    buf->buf = realloc(buf->buf, new_cap);
    buf->cap = new_cap;
}

void membuf_append(Membuf *buf, const char *data, size_t len)
{
    if(buf->len + len > buf->cap) {
        membuf_grow(buf, buf->len + len);
    }
    memcpy(buf->buf + buf->len, data, len);
    buf->len += len;
}

void membuf_append_str(Membuf *buf, const char *str)
{
    membuf_append(buf, str, strlen(str));
}

void membuf_append_fmt(Membuf *buf, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    membuf_append_vfmt(buf, fmt, ap);
    va_end(ap);
}

void membuf_append_vfmt(Membuf *buf, const char *fmt, va_list ap)
{
    va_list ap2;
    va_copy(ap2, ap);
    int len = vsnprintf(NULL, 0, fmt, ap2);
    va_end(ap2);
    if(buf->len + len > buf->cap) {
        membuf_grow(buf, buf->len + len);
    }
    vsnprintf(buf->buf + buf->len, len + 1, fmt, ap);
    buf->len += len;
}

void membuf_append_char(Membuf *buf, char c)
{
    if(buf->len + 1 > buf->cap) {
        membuf_grow(buf, buf->len + 1);
    }
    buf->buf[buf->len] = c;
    buf->len += 1;
}

const char* membuf_to_string(Membuf *buf)
{
    membuf_append_char(buf, '\0');
    return buf->buf;
}


void  membuf_addquoted (Membuf *buf, const char *s, size_t len) {
  membuf_append_char(buf, '"');
  while (len--) {
    if (*s == '"' || *s == '\\' || *s == '\n') {
      membuf_append_char(buf, '\\');
      membuf_append_char(buf, *s);
    }
    else if (iscntrl(*s)) {
      char buff[10];
      if (!isdigit(*(s+1)))
        l_sprintf(buff, sizeof(buff), "\\%d", (int)(*s));
      else
        l_sprintf(buff, sizeof(buff), "\\%03d", (int)(*s));
      membuf_append(buf, buff, strlen(buff));
    }
    else
      membuf_append_char(buf, *s);
    s++;
  }
  membuf_append_char(buf, '"');
}