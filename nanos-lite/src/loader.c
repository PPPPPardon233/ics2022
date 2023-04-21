#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

//#define __ONLY_DUMMY__

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();

static uintptr_t loader(PCB *pcb, const char *filename) {
#ifdef __ONLY_DUMMY__
  /*
  * In the case of only dummy program in Ramdisk,there is
  * no need to process the pcb as well as filename parameter
  * we only need to call the funcs which are provided in the 
  * ramdisk.c file 
  */

  Elf_Ehdr ehdr;
  ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
  assert((*(uint32_t *)ehdr.e_ident == 0x464c457f));

  Elf_Phdr phdr[ehdr.e_phnum];
  ramdisk_read(phdr, ehdr.e_phoff, sizeof(Elf_Phdr)*ehdr.e_phnum);
  for (int i = 0; i < ehdr.e_phnum; i++) {
    if (phdr[i].p_type == PT_LOAD) {
      ramdisk_read((void*)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
      memset((void*)(phdr[i].p_vaddr+phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);
    }
  }
  return ehdr.e_entry;
#else
  Elf32_Ehdr header;
	int fd = fs_open(filename, 0, 0);
  /*
  * Check whether the elf-file meets the specification 
  */
	fs_read(fd, &header, sizeof(Elf32_Ehdr));
	if (*(uint32_t *)header.e_ident == (uint32_t)0x464c457f){
		Log("header=%x",*(uint32_t *)header.e_ident );
    panic("header.e_ident != 0x464c457f");
	}
  
	Elf32_Phdr pro_header[8];

	fs_lseek(fd, header.e_phoff, SEEK_SET);

	fs_read(fd, pro_header, sizeof(Elf32_Phdr) * header.e_phnum);
	for (int i = 0; i < header.e_phnum; i++){
		if (pro_header[i].p_type == PT_LOAD){
			fs_lseek(fd, pro_header[i].p_offset, SEEK_SET);
			fs_read(fd, (void *)(pro_header[i].p_vaddr), pro_header[i].p_filesz);
			memset((void *)(pro_header[i].p_vaddr + pro_header[i].p_filesz), 0, pro_header[i].p_memsz - pro_header[i].p_filesz);
		}
	}
	fs_close(fd);
	return header.e_entry;
#endif
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

