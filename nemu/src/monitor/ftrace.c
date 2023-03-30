#include <elf.h>
#include <stdio.h>
#include <string.h>
#include "ftrace.h"

typedef struct __FUNC_INFO{
    char func_name[64];
    paddr_t start;
    size_t size;
}FUNC_INFO;

typedef struct __STACK_ENTRY_{
    FUNC_INFO* cur_info;
    FUNC_INFO* des_info;
    paddr_t addr;
    int type;
    struct __STACK_ENTRY_ *next;
}STACK_ENTRY;

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
} Finfo;

static FUNC_INFO elf_funcs[1024];
static STACK_ENTRY header;
static STACK_ENTRY *tail = &header;
static char *action_name[] = {"Call", "Ret"};

static void read_from_file(FILE *elf, size_t offset, size_t size, void* dest){
    fseek(elf, offset, SEEK_SET);
    int flag = fread(dest, size, 1, elf);
    assert(flag == 1);
}

static void get_str_from_file(FILE *elf, size_t offset, size_t n, void* dest){
    fseek(elf, offset, SEEK_SET);
    char* flag = fgets(dest, n, elf);
    assert(flag != NULL);
}

static int end;
static void append_(char* func_name, paddr_t start, size_t size){
    strncpy(elf_funcs[end].func_name, func_name, sizeof(elf_funcs[0].func_name));
    elf_funcs[end].start = start;
    elf_funcs[end].size = size;
    end++;
}

FUNC_INFO* check_func_(paddr_t addr){
    for (int i = 0; i < end; ++i){
        FUNC_INFO *info = &elf_funcs[i];
        if (addr >= info->start && addr < info->start + info->size){
            return info;
        }
    }
    return NULL;
}

static void append(paddr_t cur, paddr_t des, int type){
    STACK_ENTRY *node = malloc(sizeof(STACK_ENTRY));
    tail->next = node;
    node->next = NULL;
    node->addr = cur;
    node->cur_info = check_func_(cur);
    node->des_info = check_func_(des);
    node->type = type;
    tail = node;
}

#define FT_CALL 0
#define FT_RET 1

void stack_call(paddr_t cur, paddr_t des){
    append(cur, des, FT_CALL);
}

void stack_return(paddr_t cur, paddr_t des){
    append(cur, des, FT_RET);
}

void init_elf(const char* elf_file, size_t global_offset){
    FILE *elf = fopen(elf_file, "rb");
    assert(elf != NULL);
    Elf32_Ehdr elf_header;
    read_from_file(elf, global_offset + 0, sizeof elf_header, &elf_header);
    /*
    * e_shoff:     table file offset
    * e_shentsize: table entry size
    * e_shnum:     table entry count
    */
    Elf32_Off section_header_offset = elf_header.e_shoff;
    size_t headers_entry_size = elf_header.e_shentsize;
    int headers_entry_num = elf_header.e_shnum;

    assert(sizeof(Elf32_Shdr) == headers_entry_size);

    Elf32_Off symbol_table_offset = 0;
    Elf32_Off string_table_offset = 0;
    size_t symbol_table_total_size = 0;
    size_t symbol_table_entry_size = 0;

    for (int i = 0; i < headers_entry_num; ++i){
        Elf32_Shdr section_entry;
        read_from_file(
            elf, 
            global_offset + i * headers_entry_size + section_header_offset,
            headers_entry_size, 
            &section_entry);
        switch(section_entry.sh_type){
            case SHT_SYMTAB:
                symbol_table_offset = section_entry.sh_offset;
                symbol_table_total_size = section_entry.sh_size;
                symbol_table_entry_size = section_entry.sh_entsize;
                break;

            case SHT_STRTAB:
                if (i != elf_header.e_shstrndx)
                    string_table_offset = section_entry.sh_offset;
                break;
        }
    }

    printf("String Table Offset: %#x\n", string_table_offset);
    printf("Symbol Table Offset: %#x\n", symbol_table_offset);
    
    char function_name[64];
    assert(symbol_table_entry_size == sizeof(Elf32_Sym));
    for (int i = 0; i < symbol_table_total_size / symbol_table_entry_size; ++i){
        Elf32_Sym symbol_section_entry;
        read_from_file(
            elf, 
            global_offset + i * symbol_table_entry_size + symbol_table_offset, 
            symbol_table_entry_size, 
            &symbol_section_entry);
        switch(ELF32_ST_TYPE(symbol_section_entry.st_info)){
            case STT_FUNC:
                get_str_from_file(
                    elf, 
                    global_offset + string_table_offset + symbol_section_entry.st_name, 
                    sizeof(function_name), 
                    function_name);
                append_(function_name, symbol_section_entry.st_value, symbol_section_entry.st_size);  
            break;
        }
    }
    for (int i = 0; i < end; ++i){
        FUNC_INFO *info = &elf_funcs[i];
        printf("Func: %12s | Start: %#x | Size: %ld\n", info->func_name, 
            info->start, info->size);
    }
    header.des_info = NULL;
    header.cur_info = NULL;
    header.next = NULL;
    header.addr = 0;
}

void print_stack_trace(){
    for (STACK_ENTRY* cur = &header; cur != tail; cur = cur->next){
        STACK_ENTRY* r = cur->next;
        // printf("<%#x>" ASNI_FMT(" %-12s ", ASNI_FG_BLUE) ASNI_FMT("%s", ASNI_FG_WHITE)  
        //     ASNI_FMT("\t<%#x> ", ASNI_FG_YELLOW) ASNI_FMT("%-12s \n", ASNI_FG_BLUE),  
        //     r->addr, r->cur_info ? r->cur_info->func_name : "", action_name[r->type], 
        //     r->des_info ? r->des_info->start : 0, r->des_info ? r->des_info->func_name : "");
        
        printf("%d",r->addr);
        if(r->cur_info) printf("%s\n",r->cur_info->func_name);
        else printf(" \n");
        printf("%s\n",action_name[r->type]);
        if(r->des_info) printf("%d\n",r->des_info->start);
        else printf("0\n");
        if(r->des_info) printf("%s\n",r->des_info->func_name);
        else printf(" \n");
    }
    printf("##############");
}