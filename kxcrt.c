#include "kxcrt.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void kxmemcpy(void* dst, void* src, int sz)
{
	memcpy(dst, src, sz);
}

void kxmemset(void* dst, int val, int sz)
{
	memset(dst, val, sz);
}

void endia_transpose(char* pval, unsigned int sz)
{
	char tmp;
	unsigned int k;
	unsigned int m;
	sz--;
	k = sz / 2;//(sz>>1);
	m = k;
label:
	tmp = pval[sz-k];
	pval[sz-k] = pval[k];
	pval[k] = tmp;
	k--;
	if((k>=0)&&(k<=m))
		goto label;
}

void toupper(char* c)
{
	if(*c >= 'a')
	{
		if(*c <= 'z')
		{
			(*c) -= 'a' - 'A';
		}
	}
}

void tolower(char* c)
{
	if(*c >= 'A')
	{
		if(*c <= 'Z')
		{
			(*c) += 'a' - 'A';
		}
	}
}

int isspace(char c)
{
	if((c==' ')||(c=='\t')||(c=='\v'))
		return 1;
	return 0;
}

int isdigit(char c)
{
	if((c>='0')&&(c<='9'))
		return 1;
	return 0;
}

int isalpha(char c)
{
	char C = c;
	toupper(&C);
	if((C>='A')&&(C<='Z'))
		return 1;
	return 0;
}

char digit2alpha(char c)
{
	if(c < 0) return ' ';
	if(c > 9)
	{
		if(c < 10) return ' ';
		if(c > 36) return ' ';
		else
			return c + 'A' - 10;
	}
	else
	{
		return c + '0';
	}
}

char alpha2digit(char c)
{
	char C;
	if(isdigit(c))
		return c - '0';
	else if(isalpha(c))
		return C - 'A' + 10;
	else
		return 0;
}

unsigned int itoa_t(char* dst, __int64_t* psrc, int radix)
{
	unsigned int len;
	__int64_t org;
	__int8_t* EOB;
	__int8_t* calcptr;
	__int8_t* orgptr;
	unsigned short tmp;
	unsigned short quot;
	int is_1st;
	int i;

	len = 0;
	memcpy(&org, psrc, 8);
	EOB = &org.i8[0];
	calcptr = &org.i8[7];
	
	tmp = 1;
	i = 0;
	if(radix == 10)
	{
		if(psrc->i8[7] & 0x80)
		{
			*dst = '-';
			dst++;
			for(i; i<8; i++)
			{
				org.i8[i] = ~org.i8[i];
				tmp += org.i8[i];
				org.i8[i] = tmp;
				tmp >>= 8;
				tmp &= 0xFF;
			}
		}
	}

	i=7;
	for(i; i>0; i--)
	{
		if(org.i8[i] == 0)
			calcptr--;
		else
			break;
	}
	if(i>0)
		calcptr--;

	orgptr = calcptr;
	is_1st = 1;

label_next:
	tmp = *(unsigned short*)calcptr;
label_calc:
	quot = tmp / radix;
	if(is_1st){
		*(unsigned short*)calcptr = quot;
		is_1st = 0;
	}else
		*calcptr = (__int8_t)quot;

	tmp %= radix;
	if(calcptr>EOB)
	{
		calcptr--;
		tmp <<= 8;
		tmp |= *calcptr;
		goto label_calc;
	}
	if(tmp<0) tmp = ' ';
	if(tmp>9)
	{
		if(tmp<10) tmp = ' ';
		if(tmp>36) tmp = ' ';
		else tmp += 0x37;
	}
	else tmp += 0x30;
	*dst = (char)tmp;
	dst++;
	len++;

	tmp = *(unsigned short*)orgptr;
	if(quot == 0)
	{
		if(tmp == 0)
			goto label_reverse;
	}

	if((tmp & 0xFF00) == 0)
	{
		if(orgptr>EOB)
			orgptr--;
	}
	calcptr = orgptr;
	
	is_1st = 1;
	goto label_next;

label_reverse:
	*dst = 0;
	dst -= len;
	endia_transpose(dst, len);
	return len;
}

void print_string(char* cstr, unsigned int len)
{
	while((*cstr != 0)&&(len != 0))
	{
		__asm push ax
		__asm push bx
		__asm mov ah, 0xe
		__asm mov bx, cstr
		__asm mov al, byte ptr [bx]
		__asm mov bx, 0
		__asm int 10h
		__asm pop bx
		__asm pop ax
		cstr++;
		len--;
	}
}

// 0x1a -> '1a'
int byte2str(char val)
{
	int ret;

	ret = 0;
	ret = mapdigit2alpha(val & 0x0F);
	ret <<=  8;
	ret |= mapdigit2alpha((val>>4) & 0x0F);
	return ret;
}

void dump_memory(char* val, unsigned int sz, int sep)
{
	unsigned int count;
	int strval;
	char seek;

	for(count=0; count<sz; count++)
	{
		strval = byte2str(*val);
		val++;
		print_string((char*)&strval, 2);
		if(sep)
			print_string(" ", 1);
	}
}


/* supported format list
%[specific]<format>
specific:( MUST IN THIS ORDER)
0 - padding with 0
hex-digitdal - width

format:
s - null terminated string
c - char(lower)
C - char(upper)
<d|x><b|w|d|q> - 8(b), 16(w), 32(d), 64(q) bits
	- dec.(d) or hex.(x) digit
q - pointer value
% - %
*/
void printf(char* format, ...)
{
	char* seekptr;
	char* str_param;
	char char_param;
	short short_param;
	__int64_t* i64_param;
	__int64_t i64_tmp;
	char strbuf[96];
	unsigned int strbufsz;
	unsigned int _out_chars;
	unsigned int seek_chars;
	va_list argptr;
	
	_out_chars = 0;
	va_start(argptr, format);
	seekptr = format;
label_Next:
	while((*seekptr != 0)&&(*seekptr != '%'))
	{
		_out_chars++;
		seekptr++;
		if(_out_chars == -1)
			return;
	}
	print_string(format, _out_chars);
	_out_chars = 0;
	if(*seekptr == '%')
	{
		seekptr++;
		if(*seekptr == 's')
		{
			str_param = va_arg(argptr, char*);
			print_string(str_param, -1);
		}
		else if(*seekptr == 'c')
		{
			char_param = va_arg(argptr, char);
			tolower(&char_param);
			print_string(&char_param, 1);
		}
		else if(*seekptr == 'C')
		{
			char_param = va_arg(argptr, char);
			toupper(&char_param);
			print_string(&char_param, 1);
		}
		else if(*seekptr == 'd')
		{
			seekptr++;
			strbuf[0]=0;
			i64_param = va_arg(argptr, __int64_t*);
			kxmemcpy(&i64_tmp, i64_param, 8);
			if(*seekptr == 'b')
			{
				kxmemset(&i64_tmp.i8[1], 0, 7);
			}
			else if(*seekptr == 'd')
			{
				kxmemset(&i64_tmp.i8[2], 0, 6);
			}
			else if(*seekptr == 'w')
			{
				kxmemset(&i64_tmp.i8[4], 0, 4);
			}
			else if(*seekptr == 'q')
			{
			}
			else
			{
				goto label_SKIP;
			}
			strbufsz = itoa_t(strbuf, &i64_tmp, 10);
			print_string(strbuf, -1);
		}
		else if(*seekptr == 'x')
		{
			seekptr++;
			strbuf[0]=0;
			i64_param = va_arg(argptr, __int64_t*);
			kxmemcpy(&i64_tmp, i64_param, 8);
			if(*seekptr == 'b')
			{
				kxmemset(&i64_tmp.i8[1], 0, 7);
			}
			else if(*seekptr == 'd')
			{
				kxmemset(&i64_tmp.i8[2], 0, 6);
			}
			else if(*seekptr == 'w')
			{
				kxmemset(&i64_tmp.i8[4], 0, 4);
			}
			else if(*seekptr == 'q')
			{
			}
			else
			{
				goto label_SKIP;
			}
			strbufsz = itoa_t(strbuf, &i64_tmp, 16);
			print_string(strbuf, -1);
		}
		else if(*seekptr == 'p')
		{
			seekptr++;
			strbuf[0]=0;
			i64_param = va_arg(argptr, __int64_t*);
			kxmemcpy(&i64_tmp, i64_param, 8);
			kxmemset(&i64_tmp.i8[sizeof(int)], 0, 8-sizeof(int));
			strbufsz = itoa_t(strbuf, &i64_tmp, 16);
			print_string(strbuf, -1);
		}
		else if(*seekptr == '%')
		{
			print_string("%", 1);
		}
		else
		{
			print_string("%", 1);
			print_string(seekptr, 1);
		}
label_SKIP:
		seekptr++;
		format = seekptr;
		goto label_Next;
	}
	va_end(argptr);
}

#ifdef __cplusplus
}
#endif
