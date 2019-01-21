
#include "kxcrt.h"

char* logo = "XVM Hypervisor Loader. [Version: 1.0.0.2011]\n\r"
"Copyright (c) 2011, Kyatupe@3KxLepton. All Rights Reserved!\n\r"
"\n\r------ Welcome ------\n\r";
char* any_key = "\n\r\n\rPress Any Key to continue ... \n\r";
char* STR_EOL = "\n\r";


void __entry_point(void)
{
	__cpuid_retval* pcrv;
	char* cpustr;
	char* tmp;
	int i;
	int j;
	int err;

	cpustr = kxalloc(49);

	printf(logo);
	printf("\n\rVendor: ");
	pcrv = __cpuid(0, 0);
	memcpy(cpustr, &__cpuid_ebx(pcrv), 4);
	memcpy(cpustr+4, &__cpuid_edx(pcrv), 4);
	memcpy(cpustr+8, &__cpuid_ecx(pcrv), 4);
	print_string(cpustr, 12);
	printf("\n\r#Brand: ");
	j=0;
	for(i=2; i<5; i++)
	{
		pcrv = __cpuid(i, 0x8000);
		memcpy(cpustr+j*16, &__cpuid_eax(pcrv), 4);
		memcpy(cpustr+j*16+4, &__cpuid_ebx(pcrv), 4);
		memcpy(cpustr+j*16+8, &__cpuid_ecx(pcrv), 4);
		memcpy(cpustr+j*16+12, &__cpuid_edx(pcrv), 4);
		j++;
	}
	print_string(cpustr, 48);
	print_string(STR_EOL, -1);
	i=0;
	j=0;
label_Next:
	err = __get_memory_map(cpustr, &i, &j);
	//printf("%dq %xq %xb %xb %xb %xb %xb %xb %xb %xb\r\n", cpustr, cpustr,
		//&cpustr[0], &cpustr[1], &cpustr[2], &cpustr[3], &cpustr[4], &cpustr[5], &cpustr[6], &cpustr[7]
	//	cpustr, cpustr, cpustr, cpustr, cpustr, cpustr, cpustr, cpustr);
	//dump_memory((char*)&i, 2, 0);
	//dump_memory((char*)&j, 2, 0);
	//print_string(STR_EOL, -1);
	if(err)
	{
		dump_memory((char*)&err, 2, 1);
		print_string(STR_EOL, -1);
		goto label_Done;
	}
	//dump_memory(cpustr, 20, 1);
	printf("%xw %xq %xq %xw\n\r", &i, &cpustr[0], &cpustr[8], &cpustr[16]);
	//print_string(STR_EOL, -1);
	if((i!=0)||(j!=0))
		goto label_Next;

label_Done:
	__asm mov ax, 0xe801
	__asm int 15h
	__asm mov bx, cpustr
	__asm mov [bx], ax
	__asm mov [bx+2], bx
	__asm mov [bx+4], cx
	__asm mov [bx+6], dx
	dump_memory(cpustr, 8, 1);
	print_string(STR_EOL, -1);

	kxfree(cpustr);
	printf(any_key);
	__asm xor ax, ax
	__asm cwde
	__asm int 0x16
	__asm int 0x18
}

#ifdef _CRT_NO_ALLOC_

#define HEAP_BITMAP_BASE 0x1000
#define HEAP_BITMAP_LIMIT 0x1BFE;
#define HEAP_LOCATE_BASE 0x1C00;
#define HEAP_ZERO_SIZE_PTR 0x7BF8

void* kxalloc(int sz)
{
	char* bmp;
	short* RET;
	int blks;
	int blk_x;
	
	__asm int 0x3D

	if(sz==0)
		return (char*)HEAP_ZERO_SIZE_PTR;

	sz += 2;
	blks = sz / 8;
	if(sz % 8)
		blks++;

	bmp = (char*)HEAP_BITMAP_BASE;
	RET = (short*)HEAP_BITMAP_LIMIT;
	blk_x = 0;
	for( ; ; )
	{
		if(*bmp)
		{
			blk_x = 0;
		}
		else
		{
			blk_x++;
			if(blk_x>=blks)
			{
				bmp++;
				goto label_DoAlloc;
			}
		}
		bmp++;
		if(bmp>=(char*)RET)
			return 0;
	}

label_DoAlloc:
// set bitmap
	bmp -= blks;
	memset(bmp, 0xCC, blks);
// calc location
	blk_x = (int)bmp;
	blk_x -= HEAP_BITMAP_BASE;
	blk_x *= 8;
	blk_x += HEAP_LOCATE_BASE;
	RET = (short*)blk_x;
	*RET = blks;
	
	//__asm int 0xD3

	return (++RET);
}

void kxfree(void* ptr)
{
	short* sptr;
	int bitmap;
	short sz;

	__asm int 0x6F

	sptr = (short*)ptr;
	if((sptr == (short*)HEAP_ZERO_SIZE_PTR)||(sptr==0))
	{
		return;
	}

	sptr--;
	sz = *sptr;
	bitmap = (int)sptr - HEAP_LOCATE_BASE;
	bitmap /= 8;
	bitmap += HEAP_BITMAP_BASE;
	memset((char*)bitmap, 0, sz);

	//__asm int 0xF6
}

#endif
