// #include <am.h>
// #include <klib.h>
// #include <klib-macros.h>
// #include <stdarg.h>

// #if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// static char* get_int(char *p, va_list *ap) {
// 	int d = va_arg(*ap, int);
// 	char str[32];
// 	int len = 0;
// 	if (d == 0) {
// 		*p++ = '0';
// 		return p;
// 	}
// 	if (d < 0) {
// 		*p++ = '-';
// 		d *= -1;
// 	}
// 	while (d) {
// 		str[len++] = d % 10 + '0';
// 		d /= 10;
// 	}
// 	for (int i = len-1; i >= 0; i--) {
// 		*p++ = str[i];
// 	}
// 	return p;
// }

// static char* get_string(char* p, va_list *ap) {
// 	char *str = va_arg(*ap, char*);
// 	while(*str) {
// 		*p++ = *str++;
// 	}
// 	return p;
// }

// static char* get_char(char* p, va_list *ap) {
// 	char ch = (char)va_arg(*ap, int);
// 	*p++ = ch;
// 	return p;
// }

// int printf(const char *fmt, ...) {
//   panic("Not implemented");
// }

// int vsprintf(char *out, const char *fmt, va_list ap) {
//   panic("Not implemented");
// }

// int sprintf(char *out, const char *fmt, ...) {
//   	//panic("Not implemented");
//   	va_list ap;
// 	va_start(ap, fmt);
//   	char* p = (char*)out;
// 	while (*fmt) {
// 		if (*fmt == '%') {
// 			fmt++;
// 			switch (*fmt) {
// 				case 'd': p = get_int(p, &ap);break;
// 				case 's': p = get_string(p, &ap);break;
// 				case 'c': p = get_char(p, &ap);break;
// 			}
// 			fmt++;
// 		}
// 		else {
// 			*p++ = *fmt++;
// 		}
// 	}
// 	*p++ = '\0';
// 	va_end(ap);
// 	return 0;
// }

// int snprintf(char *out, size_t n, const char *fmt, ...) {
//   panic("Not implemented");
// }

// int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
//   panic("Not implemented");
// }

// #endif

#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int vsprintf(char *out, const char *fmt, va_list ap);
int vsnprintf(char *out, size_t n, const char *fmt, va_list ap);

int printf(const char *fmt, ...) {
  char buffer[2048];
  va_list arg;
  va_start (arg, fmt);
  int done = vsprintf(buffer, fmt, arg);
  putstr(buffer);
  va_end(arg);
  return done;
}

static char HEX_CHARACTERS[] = "0123456789ABCDEF";
#define BIT_WIDE_HEX 8

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, -1, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list valist;
  va_start(valist, fmt);
  int res = vsprintf(out ,fmt, valist);
  va_end(valist);
  return res;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list arg;
  va_start (arg, fmt);
  int done = vsnprintf(out, n, fmt, arg);
  va_end(arg);
  return done;
}

#define append(x) {out[j++]=x; if (j >= n) {break;}}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  char buffer[128];
  char *txt, cha;
  int num, len;
  unsigned int unum;
  uint32_t pointer;
  
  
  int state = 0, i, j;
  for (i = 0, j = 0; fmt[i] != '\0'; ++i){
    switch (state){
    case 0:
      if (fmt[i] != '%'){
        append(fmt[i]);
      } 
	  else
        state = 1;
      break;
    
    case 1:
      switch (fmt[i]){
      case 's':
        txt = va_arg(ap, char*);
        for (int k = 0; txt[k] !='\0'; ++k)
          	append(txt[k]);
        break;
      
      case 'd':
        num = va_arg(ap, int);
        if(num == 0){
			append('0');
			break;
        }
        if (num < 0){
			append('-');
			num = 0 - num;
        }
        for (len = 0; num ; num /= 10, ++len)
          	buffer[len] = HEX_CHARACTERS[num % 10];
        for (int k = len - 1; k >= 0; --k)
          	append(buffer[k]);
        break;
      
      case 'c':
        cha = (char)va_arg(ap, int);
        append(cha);
        break;

      case 'p':
        pointer = va_arg(ap, uint32_t);
        for (len = 0; pointer ; pointer /= 16, ++len)
          	buffer[len] = HEX_CHARACTERS[pointer % 16];
        for (int k = 0; k < BIT_WIDE_HEX - len; ++k)
          	append('0');
        for (int k = len - 1; k >= 0; --k)
          	append(buffer[k]);
        break;
      case 'x':
        unum = va_arg(ap, unsigned int);
        if(unum == 0){
			append('0');
			break;
        }
        for (len = 0; unum ; unum >>= 4, ++len)
          	buffer[len] = HEX_CHARACTERS[unum & 0xF];
        for (int k = len - 1; k >= 0; --k)
          	append(buffer[k]);
        break;  
      default:
        assert(0);
      }
      state = 0;
      break;
    }
  }
  out[j] = '\0';
  return j;
}

#endif
