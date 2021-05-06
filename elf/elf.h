#ifndef ___ELF____H__
#define ___ELF____H__

#include <string>
#include <iostream>
#include <fstream>
#include <fstream>
#include <limits>
#include <vector>
#include <map>
#include <string.h>

//This will fucking read the elf files
typedef uint32_t Elf32_Addr;   //4 4 Unsigned program address
typedef uint16_t Elf32_Half;   //2 2 Unsigned medium integer
typedef uint32_t Elf32_Off;    //4 4 Unsigned file offset
typedef int32_t   Elf32_Sword; //4 4 Signed large integer
typedef uint32_t Elf32_Word;   //4 4 Unsigned large integer
//  unsigned char 1 1 Unsigned small intege

//Elf header type macros
#define ET_NONE         0
#define ET_REL          1
#define ET_EXEC         2
#define ET_DYN          3
#define ET_CORE         4
#define ET_LOPROC       0xff00
#define ET_HIPROC       0xffff

//Elf header version macros
#define EV_NONE     0
#define EV_CURRENT  1

//Macros for section header indexes
#define SHN_UNDEF       0       
#define SHN_LORESERVE   0xff00  
#define SHN_LOPROC      0xff00  
#define SHN_HIPROC      0xff1f  
#define SHN_ABS         0xfff1  
#define SHN_COMMON      0xfff2  
#define SHN_HIRESERVE   0xffff  


//Program header type macros
#define PT_NULL         0
#define PT_LOAD         1
#define PT_DYNAMIC      2
#define PT_INTERP       3
#define PT_NOTE         4
#define PT_SHLIB        5
#define PT_PHDR         6
#define PT_LOPROC       0x70000000
#define PT_HIPROC       0x7fffffff


//Macros for section header type
#define SHT_NULL        0
#define SHT_PROGBITS    1
#define SHT_SYMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_HASH        5
#define SHT_DYNAMIC     6
#define SHT_NOTE        7
#define SHT_NOBITS      8
#define SHT_REL         9
#define SHT_SHLIB       10
#define SHT_DYNSYM      11
#define SHT_LOPROC      0x70000000
#define SHT_HIPROC      0x7fffffff
#define SHT_LOUSER      0x80000000
#define SHT_HIUSER      0xffffffff


typedef struct 
{
    uint8_t ei_mag0;    // 0 File identification
    uint8_t ei_mag1;    // 1 File identification
    uint8_t ei_mag2;    // 2 File identification
    uint8_t ei_mag3;    // 3 File identification
    uint8_t ei_class;   // 4 File class
    uint8_t ei_data;    // 5 Data encoding
    uint8_t ei_version; // 6 File version
    uint8_t ei_pad;     // 7 Start of padding bytes
    uint8_t ei_nident;  // 16 Size of e_ident[]
} e_ident_t;

#define EI_NIDENT 16

//ELF Header Struct
typedef struct {
    union
    {
        unsigned char e_ident_raw[EI_NIDENT];
        e_ident_t e_ident;
    };
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
 } Elf32_Ehdr;


//Section Header Struct
typedef struct {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;


//Program Header Struct
typedef struct {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

typedef struct
{
    uint32_t value;
    std::string name;
    std::string defenition;
} e_machine_entry_t;

typedef struct {
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
} Elf32_Sym;

extern std::vector<e_machine_entry_t> e_machine_table;
extern std::map<uint32_t, std::string> e_type_table;
extern std::map<uint32_t, std::string> e_version_table;

class elf_reader
{
    public:
        elf_reader(char* buffer, int size);
        elf_reader(std::string input_file_name);
        ~elf_reader();
        bool is_machine(uint32_t id); //Check machine ID
        bool is_machine(std::string arch_name); //Check machine name
        bool is_64(); //Chech if machine is 64bits
        bool is_32(); //Chech if machine is 32bits
        char* string_table(Elf32_Word index); //Access header string table
        uint32_t get_entry32(); //Return Entry Address
        
        std::vector<Elf32_Phdr>* get_loadable(); //Return a vector containing loadable program headers
        
        //Returns a pointer to the data specified by the program header and index i
        char* get_ph_buffer(size_t i); 


        Elf32_Ehdr elf32_ehdr;
        Elf32_Shdr* elf32_shdr; //All headers
        Elf32_Sym* elf32_sym; //All symbols
        Elf32_Phdr* elf32_phdr; //All symbols
        
        char* file_data;
    private:
};



/*e_entry This member gives the virtual address to which the system first transfers control, 
thus starting the process. If the file has no associated entry point, this member holds 
zero*/
/*e_phoff This member holds the program header table's file offset in bytes. If the file has no 
program header table, this member holds zero.*/
/*e_shoff This member holds the section header table's file offset in bytes. If the file has no 
section header table, this member holds zero.*/
/*e_flags This member holds processor-specific flags associated with the file. Flag names 
take the form EF_machine_flag.*/
/*e_ehsize This member holds the ELF header's size in bytes.*/



#endif