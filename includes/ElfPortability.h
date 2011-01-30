#ifndef ELFPORTABILITY_H_
#define ELFPORTABILITY_H_

#include <elf.h>

#include "portability.h"

#if ARCHI == 64
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Shdr Elf64_Shdr
#define Elf_Sym Elf64_Sym
#define Elf_Dyn  Elf64_Dyn
#define Elf_Word Elf64_Word
#define Elf_Sxword Elf64_Sxword
#define Elf_Xword Elf64_Xword
#if SYSTEM != 1
#define ELF_ST_TYPE ELF64_ST_TYPE
#endif
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Elf_Shdr Elf32_Shdr
#define Elf_Sym Elf32_Sym
#define Elf_Dyn  Elf32_Dyn
#define Elf_Word Elf32_Word
#define Elf_Sxword Elf32_Sxword
#define Elf_Xword Elf32_Xword
#if SYSTEM != 1
#define ELF_ST_TYPE ELF32_ST_TYPE
#endif
#endif


#define RED     "\033[31m" // DEBUUUUUUUUUUUUUUUUUUUUG A RETIRER
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define NONE    "\033[39m"

#endif // ELFPORTABILITY_H_