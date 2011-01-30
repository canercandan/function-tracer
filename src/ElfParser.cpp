#include <cstdio>
#include <algorithm>

#include <sys/stat.h>
#include <sys/mman.h>

#include "ElfParser.h"
#include "ParseStabs.h"

ElfParser::ElfParser(std::set<Function>& functions, std::list<Type>& types, MemoryManager & manager) :
_headerElf(0), _headerProgram(0), _headerSection(0), _symbolTable(0), _stringTableSymTab(0), _dynamicSection(0), _stringTableDynamicSection(0), _functions(functions), _types(types), _libs(), _manager(manager)
/*
ElfParser::ElfParser(FunctionSet& functions, FunctionMap& functionMap,
		     std::list<Type>& types, MemoryManager & manager) :
_headerElf(0), _headerProgram(0), _headerSection(0), _symbolTable(0),
_stringTableSymTab(0), _dynamicSection(0), _stringTableDynamicSection(0),
_functions(functions), _functionMap(functionMap), _types(types), _libs(),
_manager(manager), _mainFunctionAddress(0)*/
{
}

ElfParser::~ElfParser() // DEBUUUUUUUUUUUUUUUUUUUUUUUUUUUG le Dtor a supprimer
{
    std::list<std::string>::const_iterator it = this->_libs.begin();
    std::list<std::string>::const_iterator itEnd = this->_libs.end();
    std::cout << "Toutes les libs parser " << std::endl;
    for (; it != itEnd; ++it)
    {
	std::cout << *it << std::endl;
    }
}

void ElfParser::parse(std::string const & mainBinary, bool)
{
    std::cout << "\n\nAnalysing\t\t" << mainBinary << std::endl;
    struct stat fileStatus;

    int fd = this->checkFile(mainBinary, &fileStatus);
    if (fd != -1)
    {
	this->_headerElf = mmap(0, fileStatus.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (this->_headerElf != MAP_FAILED)
	{
	    std::cout << "Taille du fichier\t" << fileStatus.st_size << " octets" << std::endl <<
		    "Debut fichier\t\t" << this->_headerElf << "\nFin\t\t\t0x" << std::hex << (size_t)this->_headerElf + fileStatus.st_size << std::endl << std::endl;
	    this->parseElf();
	    std::list<std::string> dependencies;
	    this->getSharedLibraryDependencies(dependencies);
	    this->readElfInMemory(); // la methode test a besoin que le headerprogram soit sette
	    if (munmap(this->_headerElf, fileStatus.st_size) == -1)
	    {
		perror("munmap : ");
	    }
	    if (close(fd) == -1)
	    {
		perror("close : ");
	    }

	    std::list<std::string>::const_iterator it = dependencies.begin();
	    std::list<std::string>::const_iterator itEnd = dependencies.end();
	    for (; it != itEnd; ++it)
	    {
		this->reset();
		//this->parse(*it, false);
	    }
	}
	else
	{
	    perror("mmap : ");
	}
    }
}

void ElfParser::parseElf()
{
    Elf_Ehdr const * headerElf = static_cast<Elf_Ehdr const *> (this->_headerElf);
    if (true == isElfBinary(headerElf->e_ident))
    {
	if (0 != headerElf->e_shoff)
	{
	    this->readHeaderSection();
	    //this->readDebug();
	}
	else
	{
	    std::cerr << "No section header table." << std::endl;
	}

    }
    else
    {
	std::cerr << "Not a elf file" << std::endl;
    }
}

void ElfParser::readHeaderSection()
{
    Elf_Ehdr const * headerElf = static_cast<Elf_Ehdr const *> (this->_headerElf);
    this->_headerSection = (void *) ((size_t) (this->_headerElf) + headerElf->e_shoff);
    Elf_Shdr const * headerSection = static_cast<Elf_Shdr const *> (this->_headerSection);
    this->getIndexInHeaderSection(""); // DEBUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUG
    int indexSymTab = this->getIndexInHeaderSection(SHT_SYMTAB);
    if (-1 != indexSymTab)
    {
	this->_symbolTable = (void *) ((size_t) (this->_headerElf) + headerSection[indexSymTab].sh_offset);
	int indexStringTab = this->getIndexInHeaderSection(".strtab");
	if (indexStringTab != -1)
	{
	    this->_stringTableSymTab = (char *) ((size_t) (this->_headerElf) + headerSection[indexStringTab].sh_offset);
	}
	std::cout << ".symtab ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl; // DEBUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUG
	this->readSymbolTable(headerSection[indexSymTab].sh_size);
    }
    else
    {
	std::cerr << "No symbol table (.symtab)." << std::endl;
    }

    indexSymTab = this->getIndexInHeaderSection(SHT_DYNSYM);
    if (-1 != indexSymTab)
    {
	this->_symbolTable = (void *) ((size_t) (this->_headerElf) + headerSection[indexSymTab].sh_offset);
	int indexStringTable = this->getIndexInHeaderSection(".dynstr");
	if (-1 != indexStringTable)
	{
	    this->_stringTableDynamicSection = (char *) ((size_t) (this->_headerElf) + headerSection[indexStringTable].sh_offset);
	    this->_stringTableSymTab = this->_stringTableDynamicSection;
	}
	std::cout << ".dynsim ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl; // DEBUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUG
	this->readSymbolTable(headerSection[indexSymTab].sh_size);
    }
    else
    {

	std::cerr << "No dynamic symbol table (.dynsym)." << std::endl;
    }
}

void ElfParser::readSymbolTable(Elf_Xword sizeSection)
{
    unsigned int i = 0;
    Elf_Xword currentSize = 0;
    Elf_Sym const * symbolTable = static_cast<Elf_Sym const *> (this->_symbolTable);
    while (currentSize < sizeSection)
    {
	if (STT_FUNC == ELF_ST_TYPE(symbolTable[i].st_info))
	{
	    if (0 == symbolTable[i].st_value)
		std::cout << RED << "0x" << symbolTable[i].st_value << "\t" << this->_stringTableSymTab + symbolTable[i].st_name << NONE << std::endl; // DEBUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUG
	    else
		std::cout << YELLOW << "0x" << symbolTable[i].st_value << "\t" << this->_stringTableSymTab + symbolTable[i].st_name << NONE << std::endl; // DEBUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUG
	    if (0 != symbolTable[i].st_value && SHN_UNDEF != symbolTable[i].st_shndx)
	    {
		if (0 != this->_stringTableSymTab && symbolTable[i].st_name != 0)
		{
		    std::string name(this->_stringTableSymTab + symbolTable[i].st_name);
		    this->_functions.insert(Function(symbolTable[i].st_value, name));
		}
		else
		{
		    std::cout << "UNDEFINED " << symbolTable[i].st_value << std::endl;
		    this->_functions.insert(Function(symbolTable[i].st_value, ""));
		}
	    }
	}
	++i;
	currentSize += sizeof (Elf_Sym);
    }
}

void ElfParser::getSharedLibraryDependencies(std::list<std::string>& dependencies)
{
    Elf_Ehdr const * headerElf = static_cast<Elf_Ehdr const *> (this->_headerElf);
    if (0 != headerElf->e_phoff)
    {
	this->_headerProgram = (void *) ((size_t) (this->_headerElf) + headerElf->e_phoff);
	Elf_Phdr const * headerProgram = static_cast<Elf_Phdr const *> (this->_headerProgram);
	int indexDynamicSection = this->getIndexInHeaderProgram(PT_DYNAMIC);
	if (-1 != indexDynamicSection)
	{
	    this->_dynamicSection = (void *) ((size_t) (this->_headerElf) + headerProgram[indexDynamicSection].p_offset);
	    if (0 == this->_stringTableDynamicSection)
	    {
		int indexStringTable = this->getIndexInHeaderSection(".dynstr");
		if (-1 != indexStringTable)
		{
		    Elf_Shdr const * headerSection = static_cast<Elf_Shdr const *> (this->_headerSection);
		    this->_stringTableDynamicSection = (char *) ((size_t) (this->_headerElf) + headerSection[indexStringTable].sh_offset);
		}
	    }
	    this->readDynamicSection(dependencies);
	}
	else
	{
	    std::cerr << "No Dynamic Section" << std::endl;
	}
    }
    else
    {
	std::cerr << "No Program header table." << std::endl;
    }
}

void ElfParser::readDynamicSection(std::list<std::string>& dependencies)
{
    Elf_Dyn const * dynamicSection = static_cast<Elf_Dyn const *> (this->_dynamicSection);
    unsigned int i = 0;
    std::cout << std::endl << "Dependances:" << std::endl;
    while (DT_NULL != dynamicSection[i].d_tag)
    {
	if (DT_NEEDED == dynamicSection[i].d_tag)
	{
	    if (0 != this->_stringTableDynamicSection)
	    {
		std::string fullPath(this->getFullPath(this->_stringTableDynamicSection + dynamicSection[i].d_un.d_val));

		std::list<std::string>::iterator itEnd = this->_libs.end();
		if (std::find(this->_libs.begin(), itEnd, fullPath) == itEnd)
		{
		    dependencies.push_front(fullPath);
		    this->_libs.push_front(fullPath);
		}
	    }
	    else
	    {
		std::cout << "DT_NEEDED" << std::endl;
	    }
	}
	++i;
    }
}

void ElfParser::readDebug()
{
    int indexStabSection = this->getIndexInHeaderSection(".stab");
    if (-1 != indexStabSection)
    {
	int indexStringTabStab = this->getIndexInHeaderSection(".stabstr");
	if (-1 != indexStringTabStab)
	{
	    Elf_Shdr const * headerSection = static_cast<Elf_Shdr const *> (this->_headerSection);
	    ParseStabs parserStabs(this->_functions, this->_types, (void *) ((size_t) (this->_headerElf) + headerSection[indexStabSection].sh_offset),
			    (char const *) ((size_t) (this->_headerElf) + headerSection[indexStringTabStab].sh_offset));
	    parserStabs.readStabs();
	}
	else
	{
	    std::cerr << "No string table for stab section." << std::endl;
	}
    }
    else
    {
	std::cerr << "No stab sections" << std::endl;
    }
}

#include <link.h>

void ElfParser::readElfInMemory()
{
    size_t baseAddress = this->getBaseAddress();
    Elf_Ehdr const * headerElf = static_cast<Elf_Ehdr const *> (this->_manager.readMemory((void*) baseAddress, sizeof (Elf_Ehdr)));
    std::cout << std::endl << "~~~~~~~~~~~~~~~~~~\tElfParser::readElfInMemory\t~~~~~~~~~~~~~~~~~~" << std::endl << "Base Address : 0x" << std::hex << baseAddress << std::endl << std::endl; // DEEEEEEEEEBUG
    if (0 != headerElf->e_phoff)
    {
	Elf_Phdr const * headerProgram = static_cast<Elf_Phdr const *> (this->_manager.readMemory((void*) (baseAddress + headerElf->e_phoff), headerElf->e_phentsize * headerElf->e_phnum));
	unsigned int i = 0;
	while (headerProgram[i].p_type != PT_DYNAMIC)
	{
	    ++i;
	}
	size_t addrToRead = headerProgram[i].p_vaddr;
	std::cout << "Adresse of the dynamic Section : 0x" << addrToRead << std::endl << std::endl;
	Elf_Dyn const * dynSection = static_cast<Elf_Dyn const *> (this->_manager.readMemory((void*) addrToRead, sizeof (Elf_Dyn)));
	while (dynSection->d_tag != DT_PLTGOT)
	{
	    addrToRead += sizeof (Elf_Dyn);
	    dynSection = static_cast<Elf_Dyn const *> (this->_manager.readMemory((void*) addrToRead, sizeof (Elf_Dyn)));
	}

	size_t addrGot = dynSection->d_un.d_ptr/* + (4 * sizeof (Elf_Word))*/;
	std::cout << "Address of GOT (Global Offset Table) : 0x" << dynSection->d_un.d_ptr << std::endl;

	Elf_Word const * gotEntry = (Elf_Word const *) this->_manager.readMemory((void*) addrGot, sizeof (Elf_Word));
	unsigned int j = 0;
	while (j < 20)
	{
	    std::cout << "[" << std::dec << j << "] --> [ 0x" << std::hex << addrGot << " ] = 0x" << *gotEntry << std::endl;
	    gotEntry = (Elf_Word const *) this->_manager.readMemory((void*) addrGot, sizeof (Elf_Word));
	    addrGot += sizeof (Elf_Word);
	    ++j;
	}

	/*struct link_map * linkList = (struct link_map *) this->_manager.readMemory((void*) * temp, sizeof (struct link_map));
	while (1)
	{
	    std::cout << std::endl << "ADDR READ : " << addrGot << std::endl;
	    std::cout << "l_addr : " << linkList->l_addr << std::endl <<
		    "l_ld" << linkList->l_ld << std::endl <<
		    "L_name = " << (size_t) linkList->l_name << "<---------" << std::endl;
	    linkList = static_cast<struct link_map *> (this->_manager.readMemory((void*) addrGot, sizeof (struct link_map)));
	    addrGot += sizeof (struct link_map);
	}*/
    }
    else
    {
	std::cerr << "No Program header table." << std::endl;

    }
    std::cout << std::endl << "~~~~~~~~~~~~~~~~~~\tElfParser::readElfInMemory\t~~~~~~~~~~~~~~~~~~" << std::endl << std::endl;
}
