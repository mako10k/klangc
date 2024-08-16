#include "str.h"
#include "hash.h"
#include "input.h"
#include "malloc.h"
#include "output.h"
#include <assert.h>
#include <string.h>

struct klangc_str {
  const char *ks_str;
  unsigned int ks_len;
  char ks_strbuf[0];
};

static klangc_hash_t *klangc_str_hash = NULL;

__attribute__((constructor)) void klangc_str_init(void) {
  klangc_str_hash = klangc_hash_new(16);
}

const klangc_str_t *klangc_str_new(const char *str, unsigned int len) {
  assert(str != NULL);
  klangc_str_t str_tmp = {.ks_str = str, .ks_len = len};
  klangc_str_t *str_ptr;
  if (klangc_hash_get(klangc_str_hash, &str_tmp, (void **)&str_ptr))
    return str_ptr;
  str_ptr = klangc_malloc(sizeof(klangc_str_t) + len + 1);
  str_ptr->ks_str = str_ptr->ks_strbuf;
  str_ptr->ks_len = len;
  memcpy(str_ptr->ks_strbuf, str, len);
  str_ptr->ks_strbuf[len] = '\0';
  klangc_hash_put(klangc_str_hash, str_ptr, str_ptr, NULL);
  return str_ptr;
}

const char *klangc_str_get_str(const klangc_str_t *str) {
  assert(str != NULL);
  return str->ks_str;
}

unsigned int klangc_str_get_len(const klangc_str_t *str) {
  assert(str != NULL);
  return str->ks_len;
}

int klangc_str_cmp(const klangc_str_t *str1, const klangc_str_t *str2) {
  assert(str1 != NULL);
  assert(str2 != NULL);
  if (str1 == str2)
    return 0;
  if (str1->ks_len == str2->ks_len)
    return memcmp(str1->ks_str, str2->ks_str, str1->ks_len);
  if (str1->ks_len < str2->ks_len) {
    int cmp = memcmp(str1->ks_str, str2->ks_str, str1->ks_len);
    return cmp == 0 ? -1 : cmp;
  }
  int cmp = memcmp(str1->ks_str, str2->ks_str, str2->ks_len);
  return cmp == 0 ? 1 : cmp;
}

const char *klangc_str_get_cstr(const klangc_str_t *str) {
  assert(str != NULL);
  return str->ks_str;
}

static int klangc_parse_hexchar(klangc_input_t *input) {
  int hex = 0;
  klangc_ipos_t ipos = klangc_input_save(input);
  int c = klangc_getc(input);
  if (c == EOF)
    return EOF;

  if ('0' <= c && c <= '9')
    hex = c - '0';
  else if ('a' <= c && c <= 'f')
    hex = c - 'a' + 10;
  else if ('A' <= c && c <= 'F')
    hex = c - 'A' + 10;
  else {
    klangc_input_restore(input, ipos);
    return 'x';
  }
  klangc_ipos_t ipos2 = klangc_input_save(input);
  c = klangc_getc(input);
  if (c == EOF)
    return EOF;
  if ('0' <= c && c <= '9')
    hex = hex * 16 + (c - '0');
  else if ('a' <= c && c <= 'f')
    hex = hex * 16 + (c - 'a' + 10);
  else if ('A' <= c && c <= 'F')
    hex = hex * 16 + (c - 'A' + 10);
  else {
    klangc_input_restore(input, ipos2);
    return hex;
  }
  return hex;
}

static int klangc_parse_hexchar_str(const char *str, unsigned int *pi,
                                    unsigned int slen) {
  int hex = 0;
  int c = *pi < slen ? str[(*pi)++] : EOF;
  if (c == EOF)
    return EOF;

  if ('0' <= c && c <= '9')
    hex = c - '0';
  else if ('a' <= c && c <= 'f')
    hex = c - 'a' + 10;
  else if ('A' <= c && c <= 'F')
    hex = c - 'A' + 10;
  else
    return 'x';
  c = *pi < slen ? str[(*pi)++] : EOF;
  if (c == EOF)
    return EOF;
  if ('0' <= c && c <= '9')
    hex = hex * 16 + (c - '0');
  else if ('a' <= c && c <= 'f')
    hex = hex * 16 + (c - 'a' + 10);
  else if ('A' <= c && c <= 'F')
    hex = hex * 16 + (c - 'A' + 10);
  else
    return hex;
  return hex;
}

static int klangc_parse_octchar(klangc_input_t *input, int oct) {
  oct = oct - '0';
  klangc_ipos_t ipos = klangc_input_save(input);
  int c = klangc_getc(input);
  if (c == EOF)
    return EOF;
  if ('0' <= c && c <= '7')
    oct = oct * 8 + (c - '0');
  else {
    klangc_input_restore(input, ipos);
    return oct;
  }
  return oct;
}

static int klangc_parse_octchar_str(const char *str, unsigned int *pi,
                                    unsigned int slen, int oct) {
  oct = oct - '0';
  int c = *pi < slen ? str[(*pi)++] : EOF;
  if (c == EOF)
    return EOF;
  if ('0' <= c && c <= '7')
    oct = oct * 8 + (c - '0');
  else
    return oct;
  return oct;
}

static int klangc_parse_escchar(klangc_input_t *input) {
  assert(input != NULL);
  klangc_ipos_t ipos = klangc_input_save(input);
  int c = klangc_getc(input);
  switch (c) {
  case EOF:
    return EOF;
  case 'n':
  case 'N':
    c = '\n';
    break;
  case 't':
  case 'T':
    c = '\t';
    break;
  case 'r':
  case 'R':
    c = '\r';
    break;
  case '0':
    c = '\0';
    break;
  case '\\':
  case '"':
  case '\'':
    break;
  case 'a':
  case 'A':
    c = '\a';
    break;
  case 'b':
  case 'B':
    c = '\b';
    break;
  case 'f':
  case 'F':
    c = '\f';
    break;
  case 'v':
  case 'V':
    c = '\v';
    break;
  case 'x':
  case 'X':
    c = klangc_parse_hexchar(input);
  default:
    if ('0' <= c && c <= '7')
      c = klangc_parse_octchar(input, c);
  }
  return c;
}

static int klangc_parse_escchar_str(const char *str, unsigned int *pi,
                                    unsigned int slen) {
  int c = *pi < slen ? str[(*pi)++] : EOF;
  switch (c) {
  case EOF:
    return EOF;
  case 'n':
  case 'N':
    c = '\n';
    break;
  case 't':
  case 'T':
    c = '\t';
    break;
  case 'r':
  case 'R':
    c = '\r';
    break;
  case '0':
    c = '\0';
    break;
  case '\\':
  case '"':
  case '\'':
    break;
  case 'a':
  case 'A':
    c = '\a';
    break;
  case 'b':
  case 'B':
    c = '\b';
    break;
  case 'f':
  case 'F':
    c = '\f';
    break;
  case 'v':
  case 'V':
    c = '\v';
    break;
  case 'x':
  case 'X':
    c = klangc_parse_hexchar_str(str, pi, slen);
  default:
    if ('0' <= c && c <= '7')
      c = klangc_parse_octchar_str(str, pi, slen, c);
  }
  return c;
}

klangc_parse_result_t klangc_str_parse(klangc_input_t *input,
                                       const klangc_str_t **pstrval) {
  klangc_ipos_t ipos = klangc_input_save(input);
  klangc_skipspaces(input);
  int c = klangc_getc(input);
  if (c != '"') {
    klangc_input_restore(input, ipos);
    return KLANGC_PARSE_NOPARSE;
  }
  char *strval = klangc_malloc(16);
  size_t len = 0;
  size_t cap = 16;
  while (1) {
    klangc_ipos_t ipos2 = klangc_input_save(input);
    c = klangc_getc(input);
    if (c == '"')
      break;
    if (c == '\\') {
      ipos2 = klangc_input_save(input);
      c = klangc_parse_escchar(input);
    }
    if (c == EOF) {
      klangc_printf_ipos(kstderr, ipos2, "Unexpected EOF\n");
      klangc_input_restore(input, ipos);
      return KLANGC_PARSE_ERROR;
    }
    if (cap <= len + 1) {
      cap *= 2;
      strval = klangc_realloc(strval, cap);
    }
    strval[len++] = c;
  }
  strval[len] = '\0';
  strval = klangc_realloc(strval, len + 1);
  *pstrval = klangc_str_new(strval, len);
  return KLANGC_PARSE_OK;
}

const klangc_str_t *klangc_str_parse_str(const char *str, size_t slen) {
  unsigned int i = 0;
  int c;
  c = i < slen ? str[i++] : EOF;
  if (c != '"')
    return NULL;
  char *strval = klangc_malloc(16);
  size_t len = 0;
  size_t cap = 16;
  while (1) {
    c = i < len ? str[i++] : EOF;
    if (c == '"')
      break;
    if (c == '\\')
      c = klangc_parse_escchar_str(str, &i, slen);
    if (c == EOF)
      return NULL;
    if (cap <= len + 1) {
      cap *= 2;
      strval = klangc_realloc(strval, cap);
    }
    strval[len++] = c;
  }
  strval[len] = '\0';
  strval = klangc_realloc(strval, len + 1);
  return klangc_str_new(strval, len);
}

void klangc_str_print(klangc_output_t *output, const klangc_str_t *str) {
  assert(output != NULL);
  assert(str != NULL);
  klangc_printf(output, "\"");
  const char *cstr = klangc_str_get_str(str);
  unsigned int len = klangc_str_get_len(str);
  for (unsigned int i = 0; i < len; i++) {
    switch (cstr[i]) {
    case '\n':
      klangc_printf(output, "\\n");
      break;
    case '\t':
      klangc_printf(output, "\\t");
      break;
    case '\r':
      klangc_printf(output, "\\r");
      break;
    case '\0':
      klangc_printf(output, "\\0");
      break;
    case '\\':
      klangc_printf(output, "\\\\");
      break;
    case '"':
      klangc_printf(output, "\\\"");
      break;
    case '\'':
      klangc_printf(output, "\\'");
      break;
    case '\a':
      klangc_printf(output, "\\a");
      break;
    case '\b':
      klangc_printf(output, "\\b");
      break;
    case '\f':
      klangc_printf(output, "\\f");
      break;
    case '\v':
      klangc_printf(output, "\\v");
      break;
    default:
      if (cstr[i] < 32 || cstr[i] >= 127)
        klangc_printf(output, "\\x%02x", cstr[i]);
      else
        klangc_printf(output, "%c", cstr[i]);
    }
  }
  klangc_printf(output, "\"");
}