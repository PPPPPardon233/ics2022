#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static char* get_int(char *p, va_list *ap) {
	int d = va_arg(*ap, int);
	char str[32];
	int len = 0;
	if (d == 0) {
		*p++ = '0';
		return p;
	}
	if (d < 0) {
		*p++ = '-';
		d *= -1;
	}
	while (d) {
		str[len++] = d % 10 + '0';
		d /= 10;
	}
	for (int i = len-1; i >= 0; i--) {
		*p++ = str[i];
	}
	return p;
}

static char* get_string(char* p, va_list *ap) {
	char *str = va_arg(*ap, char*);
	while(*str) {
		*p++ = *str++;
	}
	return p;
}

static char* get_char(char* p, va_list *ap) {
	char ch = (char)va_arg(*ap, int);
	*p++ = ch;
	return p;
}

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  //panic("Not implemented");
  va_list ap;
	va_start(ap, fmt);
  char* p = (char*)out;
	while (*fmt) {
		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
				case 'd': p = get_int(p, &ap);break;
				case 's': p = get_string(p, &ap);break;
				case 'c': p = get_char(p, &ap);break;
			}
			fmt++;
		}
		else {
			*p++ = *fmt++;
		}
	}
	*p++ = '\0';
	va_end(ap);
	return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
