#include <cstdio>
#include <climits>

#include <fcntl.h>
#include <string.h>

#include "ElfParser.h"
#include "ElfPortability.h"

void ElfParser::reset()
{
    this->_headerElf = 0;
    this->_headerProgram = 0;
    this->_headerSection = 0;
    this->_symbolTable = 0;
    this->_stringTableSymTab = 0;
    this->_dynamicSection = 0;
    this->_stringTableDynamicSection = 0;
}

bool ElfParser::isElfBinary(unsigned char const * magicCode) const
{
    return (magicCode[EI_MAG0] == ELFMAG0 &&
	    magicCode[EI_MAG1] == ELFMAG1 &&
	    magicCode[EI_MAG2] == ELFMAG2 &&
	    magicCode[EI_MAG3] == ELFMAG3);
}

int ElfParser::checkFile(std::string const & path, struct stat* fileStatus) const
{
    int fd = open(path.c_str(), O_RDONLY);
    if (fd != -1)
    {
	if (fstat(fd, fileStatus) != -1)
	{
	    if (false == S_ISREG(fileStatus->st_mode))
	    {
		if (close(fd) == -1)
		{
		    perror("close : ");
		}
		fd = -1;
	    }
	}
	else
	{
	    perror("fstat : ");
	}
    }
    else
    {
	perror("open : ");
    }
    return (fd);
}

int ElfParser::getIndexInHeaderSection(Elf_Word typeSearch) const
{
    Elf_Shdr* headerSection = static_cast<Elf_Shdr*> (this->_headerSection);
    for (uint16_t i = 0; i < (static_cast<Elf_Ehdr*> (this->_headerElf))->e_shnum; ++i)
    {
	if (headerSection[i].sh_type == typeSearch)
	{
	    return (i);
	}
    }
    return (-1);
}

int ElfParser::getIndexInHeaderProgram(Elf_Word typeSearch) const
{
    Elf_Phdr* headerProgram = static_cast<Elf_Phdr*> (this->_headerProgram);
    for (int i = 0; i < (static_cast<Elf_Ehdr*> (this->_headerElf))->e_phnum; ++i)
    {
	if (headerProgram[i].p_type == typeSearch)
	{
	    return (i);
	}
    }
    return (-1);
}

int ElfParser::getIndexInHeaderSection(char const * typeSearch) const
{
    Elf_Shdr const * headerSection = static_cast<Elf_Shdr const *> (this->_headerSection);
    char const * stringTabSection = (char const *) ((size_t) (this->_headerElf) + headerSection[(static_cast<Elf_Ehdr const *> (this->_headerElf))->e_shstrndx].sh_offset);

    for (int i = 0; i < (static_cast<Elf_Ehdr const *> (this->_headerElf))->e_shnum; ++i)
    {
	//std::cout << "[" << std::dec << i << "] = " << stringTabSection + headerSection[i].sh_name << std::endl; // DEBUUUUUUUUUUUUUUUUUUUUUUUUUUUG
	if (strcmp(stringTabSection + headerSection[i].sh_name, typeSearch) == 0)
	{
	    return (i);
	}
    }
    return (-1);
}

int ElfParser::getIndexInDynamicSection(Elf_Sxword searchType) const
{
    Elf_Dyn const * dynamicSection = static_cast<Elf_Dyn const *> (this->_dynamicSection);

    unsigned int i = 0;
    while (DT_NULL != dynamicSection[i].d_tag)
    {
	if (searchType == dynamicSection[i].d_tag)
	{
	    return (i);
	}
	++i;
    }
    return (-1);
}

std::string ElfParser::getFullPath(std::string const & dependence)
{
    static const char * defaultLibPath[] = {"/lib/", "/usr/lib/", 0};
    for (unsigned int i = 0; defaultLibPath[i] != 0; ++i)
    {
	std::string temp(defaultLibPath[i] + dependence);
	struct stat fileStatus;
	if (-1 != lstat(temp.c_str(), &fileStatus))
	{
	    if (S_ISLNK(fileStatus.st_mode))
	    {
		char buffer[PATH_MAX];
		ssize_t size = readlink(temp.c_str(), buffer, PATH_MAX);
		if (size == -1)
		{
		    perror("readlink : ");
		}
		else
		{
		    buffer[size] = '\0';
		    return (this->getFullPath(buffer)); // retourne le vrai lien
		}
	    }
	    else
	    {
		return (temp);
	    }
	}
    }
    return (dependence);
}

size_t ElfParser::getBaseAddress()
{
    size_t baseAddress = -1;
    Elf_Phdr* headerProgram = static_cast<Elf_Phdr*> (this->_headerProgram);
    for (int i = 0; i < (static_cast<Elf_Ehdr*> (this->_headerElf))->e_phnum; ++i)
    {
	if (PT_LOAD == headerProgram[i].p_type && baseAddress > headerProgram[i].p_vaddr)
	{
	    baseAddress = headerProgram[i].p_vaddr;
	}
    }
    return (baseAddress);
}