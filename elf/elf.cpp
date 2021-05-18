#include "elf.h"
#include "elf_tables.h"
#include <string.h>
#include <algorithm>

elf_reader::elf_reader(char* buffer, int size)
{
    elf32_shdr = nullptr;
    elf32_sym = nullptr;
    elf32_phdr = nullptr;
    file_data = nullptr;
    if(sizeof(elf32_ehdr) <= size)
    {
        //Allocate memory for file_data
        file_data = new char[size];
        //buffer into internal buffer to avoid use after free
        memcpy(file_data, buffer, size);


        printf("\n\n\n\n\n\n\n\n");
        int row = 0; 

        #define ANSI_COLOR_RED     "\x1b[31m"
        #define ANSI_COLOR_GREEN   "\x1b[32m"
        #define ANSI_COLOR_YELLOW  "\x1b[33m"
        #define ANSI_COLOR_BLUE    "\x1b[34m"
        #define ANSI_COLOR_MAGENTA "\x1b[35m"
        #define ANSI_COLOR_CYAN    "\x1b[36m"
        #define ANSI_COLOR_RESET   "\x1b[0m"

        int sizes [] {
            sizeof(elf32_ehdr.e_ident_raw), 
            sizeof(elf32_ehdr.e_type),
            sizeof(elf32_ehdr.e_machine),
            sizeof(elf32_ehdr.e_version),
            sizeof(elf32_ehdr.e_entry),
            sizeof(elf32_ehdr.e_phoff),
            sizeof(elf32_ehdr.e_shoff),
            sizeof(elf32_ehdr.e_flags),
            sizeof(elf32_ehdr.e_ehsize),
            sizeof(elf32_ehdr.e_phentsize),
            sizeof(elf32_ehdr.e_phnum),
            sizeof(elf32_ehdr.e_shentsize),
            sizeof(elf32_ehdr.e_shnum),
            sizeof(elf32_ehdr.e_shstrndx)
        };
        std::string colors [] = 
        {
            ANSI_COLOR_RED,
            ANSI_COLOR_GREEN,
            ANSI_COLOR_YELLOW,
            ANSI_COLOR_BLUE,
            ANSI_COLOR_MAGENTA,
            ANSI_COLOR_CYAN,
            ANSI_COLOR_RED,
            ANSI_COLOR_GREEN,
            ANSI_COLOR_YELLOW,
            ANSI_COLOR_BLUE,
            ANSI_COLOR_MAGENTA,
            ANSI_COLOR_CYAN,
            ANSI_COLOR_RED,
            ANSI_COLOR_GREEN,
            ANSI_COLOR_YELLOW,
            ANSI_COLOR_BLUE,
            ANSI_COLOR_MAGENTA,
            ANSI_COLOR_CYAN
        };
        std::cout << ANSI_COLOR_RESET "elf32_ehdr.e_ident_raw: " ANSI_COLOR_RED "ANSI_COLOR_RED\n";
        std::cout << ANSI_COLOR_RESET "elf32_ehdr.e_type: " ANSI_COLOR_GREEN "ANSI_COLOR_GREEN\n";
        std::cout << ANSI_COLOR_RESET "elf32_ehdr.e_machine: " ANSI_COLOR_YELLOW "ANSI_COLOR_YELLOW\n";
        std::cout << ANSI_COLOR_RESET "elf32_ehdr.e_version:" ANSI_COLOR_BLUE "ANSI_COLOR_BLUE\n";
        std::cout << ANSI_COLOR_RESET "elf32_ehdr.e_entry: " ANSI_COLOR_MAGENTA "ANSI_COLOR_MAGENTA\n";
        std::cout << ANSI_COLOR_RESET "elf32_ehdr.e_phoff: " ANSI_COLOR_CYAN "ANSI_COLOR_CYAN\n";
        std::cout << ANSI_COLOR_RESET "elf32_ehdr.e_shoff: " ANSI_COLOR_RED "ANSI_COLOR_RED\n";
        std::cout << ANSI_COLOR_RESET "elf32_ehdr.e_flags: " ANSI_COLOR_GREEN "ANSI_COLOR_GREEN\n";
        std::cout << ANSI_COLOR_RESET "elf32_ehdr.e_ehsize: " ANSI_COLOR_YELLOW "ANSI_COLOR_YELLOW\n";
        std::cout << ANSI_COLOR_RESET "elf32_ehdr.e_phentsize: " ANSI_COLOR_BLUE "ANSI_COLOR_BLUE\n";
        std::cout << ANSI_COLOR_RESET "elf32_ehdr.e_phnum: " ANSI_COLOR_MAGENTA "ANSI_COLOR_MAGENTA\n";
        std::cout << ANSI_COLOR_RESET "elf32_ehdr.e_shentsize: " ANSI_COLOR_CYAN "ANSI_COLOR_CYAN\n";
        std::cout << ANSI_COLOR_RESET "elf32_ehdr.e_shnum: " ANSI_COLOR_RED "ANSI_COLOR_RED\n";
        std::cout << ANSI_COLOR_RESET "elf32_ehdr.e_shstrndx: " ANSI_COLOR_GREEN "ANSI_COLOR_GREEN\n";
        std::cout << ANSI_COLOR_RESET "\n\n\n\n";
        //Print the read data
        memcpy(&elf32_ehdr, file_data, sizeof(elf32_ehdr));
        int x = 0;
        int olds = 0;
        std::cout << colors[x];
        for(int i = 0;i <= sizeof(elf32_ehdr);i++)
        {
            if(row > 16)
            {
                printf("\n");
                row = 0;
            }
            if(i >= (sizes[x]+olds))
            {
                printf(ANSI_COLOR_RESET);
                olds = olds + sizes[x];
                if(x < 14) x++;
                std::cout << colors[x];
            }
            printf("0x%02x ", file_data[i] & 0xff);
            row++;
        }
        printf(ANSI_COLOR_RESET);
        //Get data 

        printf("\n\n\n\nStarting Address of execution 0x%08x\n\n\n\n", elf32_ehdr.e_entry & 0xffffffff);

        std::cout << "e_type: " << elf32_ehdr.e_type << std::endl;

        std::cout << "section table address: " <<  elf32_ehdr.e_shoff << std::endl;
        std::cout << "section table entry size: " <<  elf32_ehdr.e_shentsize << std::endl;
        std::cout << "section table entries: " <<  elf32_ehdr.e_shnum << std::endl;
        std::cout << "string table index: " <<  elf32_ehdr.e_shstrndx << std::endl;
        
        
        //for(int i = 0;i < 342;i++)
        //{
        //    if(row > 16)
        //    {
        //        printf("\n");
        //        row = 0;
        //    }
        //    printf("%c ", ((char*)buffer + elf32_ehdr.e_shstrndx)[i] & 0xff);
        //    row++;
        //}        
        
        
        //Allocate all header sections
        elf32_shdr = new Elf32_Shdr[elf32_ehdr.e_shnum];
        memcpy(elf32_shdr, &file_data[elf32_ehdr.e_shoff], elf32_ehdr.e_shnum * elf32_ehdr.e_shentsize);

        for(int i = 0; i < elf32_ehdr.e_shnum;i++)
        {
            //print word
            //mempcpy(&elf32_sym, (buffer + elf32_ehdr.e_shstrndx)[elf32_shdr[i].sh_name * elf32_shdr].st_name, sizeof(Elf32_Sym));
            printf("name %s\n", &file_data[elf32_shdr[elf32_ehdr.e_shstrndx].sh_offset] + elf32_shdr[i].sh_name);
        }
        
        e_machine_entry_t e_machine_entry {243,   "EM_RISCV",         "RISC-V"};
        if(std::find_if(e_machine_table.begin(), e_machine_table.end(), 
        [&](const e_machine_entry_t& entry)->bool{return (entry.value == e_machine_entry.value)? true : false;}) != e_machine_table.end())
        {
            std::cout << "RISCV machine\n";
        }
        else
        {
            std::cout << "error: Not a RISCV machine\n";
            exit(1);
        }

        //Now extract all program headers

        //Allocate memory for the program headers
        elf32_phdr = new Elf32_Phdr[elf32_ehdr.e_phnum];

        //Now initialize all program headers
        memcpy(elf32_phdr, &file_data[elf32_ehdr.e_phoff], elf32_ehdr.e_phnum * elf32_ehdr.e_phentsize);


        printf("Type\tOffset\t\tVirtAddr\t\tPhysAddr\t\tFileSiz\t\tMemSiz\t\tFlg Align\n");   
        for(int i = 0;i < elf32_ehdr.e_phnum;i++)
        {
            printf("%s\t0x%08x\t0x%08x\t\t0x%08x\t\t0x%08x\t0x%08x\tNONE 0x%08x\n",
            p_type_table.find(elf32_phdr[i].p_type)->second.c_str(),
            elf32_phdr[i].p_offset,
            elf32_phdr[i].p_vaddr,
            elf32_phdr[i].p_paddr,
            elf32_phdr[i].p_filesz,
            elf32_phdr[i].p_memsz,
            elf32_phdr[i].p_flags,
            elf32_phdr[i].p_align
            );
                // /elf32_phdr[i].p_type   
        }

        std::cout << "Searching for string table\n";
        for(int i = 0;i < elf32_ehdr.e_shnum;i++)
        {
            if(elf32_shdr[i].sh_type == SHT_STRTAB)
            {
                printf("Comparing %s %s\n", ".strtab", &buffer[elf32_shdr[elf32_ehdr.e_shstrndx].sh_offset + elf32_shdr[i].sh_name]);
                if(strcmp(".strtab", &buffer[elf32_shdr[elf32_ehdr.e_shstrndx].sh_offset + elf32_shdr[i].sh_name]) == 0)
                {
                    printf("Found string table at index %d\n", i);
                    strtab_index = i;
                }
            }
        }
        //Dumping all symbols from the symbol section
        for(int i = 0;i < elf32_ehdr.e_shnum;i++)
        {
            if(elf32_shdr[i].sh_type == SHT_SYMTAB)
            {
                printf("Entry Size %d\n", elf32_shdr[i].sh_entsize);
                printf("\n\n\nSymbol table found at index %d\n", i);
                printf("symbtab contains %d entries\n", ((elf32_shdr[i].sh_size)/sizeof(Elf32_Sym)));
                elf32_sym = (Elf32_Sym*)malloc(sizeof(Elf32_Sym)*((elf32_shdr[i].sh_size)/sizeof(Elf32_Sym)));
                memcpy(elf32_sym, &buffer[elf32_shdr[i].sh_offset], sizeof(Elf32_Sym)*((elf32_shdr[i].sh_size)/sizeof(Elf32_Sym)));
                for(int x = 0;x < ((elf32_shdr[i].sh_size)/sizeof(Elf32_Sym));x++)
                {
                    printf("Symbol: %08x %08s\n", elf32_sym[x].st_value, &buffer[elf32_shdr[strtab_index].sh_offset + elf32_sym[x].st_name]);
                }
            }
        }

    }
    else
    {
        std::cout << "error elf_reader: buffer size too small\n";
    }
    
}


elf_reader::elf_reader(std::string input_file_name)
{
    elf32_shdr = nullptr;
    elf32_sym = nullptr;
    elf32_phdr = nullptr;
    file_data = nullptr;
    //Open File
    std::ifstream input_file;
    input_file.open(input_file_name, std::ifstream::binary); 

    if(input_file.is_open())
    {
        //Get input file size
        input_file.ignore( std::numeric_limits<std::streamsize>::max());
        std::streamsize size = input_file.gcount();
        input_file.clear();   //  Since ignore will have set eof.
        input_file.seekg( 0, std::ios_base::beg );
        char* file_data = (char*)malloc(size * sizeof(size));
        //Allocate memory and read file into allocated buffer
        memset(file_data, 0x00, size);
        input_file.read(file_data, size);
        input_file.close();
    }
    else
    {
        std::cout << "Couln't open file\n";
    }
}
elf_reader::~elf_reader()
{
    if(elf32_shdr)
    {
        delete[] elf32_shdr;
    }
    if(elf32_sym)
    {
        delete[] elf32_sym;
    }
    if(elf32_phdr)
    {
        delete[] elf32_phdr;
    }
    if(file_data)
    {
        delete file_data;
    }
}

char* elf_reader::string_table(Elf32_Word offset)
{
    return &file_data[elf32_shdr[strtab_index].sh_offset + offset];
}

//Check machine ID
bool elf_reader::is_machine(uint32_t id)
{
    if(std::find_if(e_machine_table.begin(), e_machine_table.end(), 
    [&](const e_machine_entry_t& entry)->bool{return (entry.value == id)? true : false;}) != e_machine_table.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//Check machine name
bool elf_reader::is_machine(std::string arch_name)
{
    if(std::find_if(e_machine_table.begin(), e_machine_table.end(), 
    [&](const e_machine_entry_t& entry)->bool{return (entry.name.compare(arch_name) == 0)? true : false;}) != e_machine_table.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}


//Chech if machine is 64bits
bool elf_reader::is_64()
{
    return (elf32_ehdr.e_ident.ei_class == 2)? true : false;
}
//Chech if machine is 32bits
bool elf_reader::is_32()
{
    return (elf32_ehdr.e_ident.ei_class == 1)? true : false;
}

char* elf_reader::get_ph_buffer(size_t i)
{
    //Check if index is within range
    if(i < elf32_ehdr.e_phnum)
    {
        return &file_data[elf32_phdr[i].p_offset];
    }
    else
    {
        return NULL;
    }
    
}

uint32_t elf_reader::get_entry32() //Return Entry Address
{
    return elf32_ehdr.e_entry;
}

std::vector<symbol32_t>* elf_reader::dump_symbols()
{
    std::vector<symbol32_t>* symbols = new std::vector<symbol32_t>();
    if(elf32_shdr == nullptr)
    {
        return nullptr;
    }
    else
    {
        if(elf32_sym == nullptr)
        {
            return nullptr;
        }
        else
        {
            for(int i = 0;i < elf32_ehdr.e_shnum;i++)
            {
                if(elf32_shdr[i].sh_type == SHT_SYMTAB)
                {
                    for(int x = 0;x < ((elf32_shdr[i].sh_size)/sizeof(Elf32_Sym));x++)
                    {
                        symbols->push_back(
                            {
                                (symbol32_t)
                                {
                                    std::string(&file_data[elf32_shdr[strtab_index].sh_offset + elf32_sym[x].st_name]),
                                    elf32_sym[x].st_value,
                                    elf32_sym[x].st_size,
                                    elf32_sym[x].st_info,
                                    elf32_sym[x].st_other,
                                    elf32_sym[x].st_shndx
                                }
                            });                    
                    }
                }
            }
            return symbols;
        }
    }
}