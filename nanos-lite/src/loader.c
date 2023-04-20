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

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf32_Ehdr header;
	int fd = fs_open(filename, 0, 0);
	if (fd >= 3)
	{
		panic("fd shoud less then 3");
	}
  /*
  * Check whether the elf-file meets the specification 
  */
	fs_read(fd, &header, sizeof(Elf32_Ehdr));
	if (*(uint32_t *)header.e_ident == 0x464c457f){
		panic("header.e_ident == 0x464c457f");
	}
	assert(header.e_phnum < 8);
	if (header.e_phnum < 8){
		panic("header.e_phnum < 8");
	}

	Elf32_Phdr pro_header[8];

	fs_lseek(fd, header.e_phoff, SEEK_SET);

	fs_read(fd, pro_header, sizeof(Elf32_Phdr) * header.e_phnum);
	for (int i = 0; i < header.e_phnum; i++)
	{

		if (pro_header[i].p_type == PT_LOAD)
		{
			fs_lseek(fd, pro_header[i].p_offset, SEEK_SET);
			fs_read(fd, (void *)(pro_header[i].p_vaddr), pro_header[i].p_filesz);
			memset((void *)(pro_header[i].p_vaddr + pro_header[i].p_filesz), 0, pro_header[i].p_memsz - pro_header[i].p_filesz);
		}
	}
	fs_close(fd);
	return header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

