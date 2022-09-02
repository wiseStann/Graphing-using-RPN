#include "c_type.h"

int is_digit(int c) { return (c >= '0' && c <= '9' ? 1 : 0); }

int is_alpha(int c) {
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ? 1 : 0);
}

int is_space(int c) {
  return (c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' ||
            c == ' ' ? 1 : 0);
}
