/*
*	this file is supply an simple C Runtime Library("kxcrt") for build 16-bit application.
*	all Program use kxcrt MUST Less than 24K in entry.
*/

#ifndef __KX_CRT_H_INCLUDE__
#define __KX_CRT_H_INCLUDE__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct KX_ENTRY_PARAM
{
	unsigned char major;
	unsigned char minor;
	unsigned short build;
	char brand[8];
} entry_param;

// the entry point of program use kxcrt.
// the kxcrt call this function with ax = [entry_param]
void __entry_point(void);

/*
// *****************************************************************************
*/

typedef unsigned int size_t;

// memory alloc
// alloc zero size mem is always Ok
// but should better don't do this
void* kxalloc(size_t sz);
// memory free
void kxfree(void* p);
void kxmemcpy(void* dst, void* src, int sz);
void kxmemset(void* dst, int val, int sz);

#define nullptr 0

typedef unsigned char __int8_t;

typedef struct KXINT16
{
	__int8_t i8[2];
} __int16_t;

typedef struct KXINT32
{
	__int8_t i8[4];
} __int32_t;

typedef struct KXINT64
{
	__int8_t i8[8];
} __int64_t;

void __near print_string(char* cstr, unsigned int len);

char __near hexdigit2alpha(char c);
int __near hextoa_byte(char val);
void __near dump_memory(char* val, unsigned int sz, int sep);

void __near endia_transpose(char* pval, unsigned int sz);

void __near toupper(char* c);
void __near tolower(char* c);
int __near isspace(char c);
int __near isdigit(char c);
int __near isalpha(char c);

unsigned int __near itoa_t(char* dst, __int64_t* psrc, int radix);
int __near atoi_t(__int64_t* dst, char* psrc, char** EOB, int radix);

/*
// *****************************************************************************
*/
// from intrin function
// available from intrin.obj

typedef struct __cpuid_retval
{
	__int32_t val[4]; //eax, ebx, ecx, edx
} __cpuid_retval;
#define __cpuid_eax(__cpuid_retval_ptr) \
	__cpuid_retval_ptr->val[0]
#define __cpuid_ebx(__cpuid_retval_ptr) \
	__cpuid_retval_ptr->val[1]
#define __cpuid_ecx(__cpuid_retval_ptr) \
	__cpuid_retval_ptr->val[2]
#define __cpuid_edx(__cpuid_retval_ptr) \
	__cpuid_retval_ptr->val[3]

void __near __cpuid(__cpuid_retval* pret, __int32_t* pid);

int __near __get_memory_map(char* buf, __int32_t* pnum);

typedef struct __get_mem_size_retval
{
	__int64_t extend;
	__int64_t configured;
} __get_mem_size_retval;
void __near __get_memory_size(__get_mem_size_retval* pret);

#ifdef __cplusplus
}
#endif

#endif
