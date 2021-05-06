#ifndef __ELF_TABLES__H__
#define __ELF_TABLES__H__

#include <vector>
#include <map>
#include "elf.h"

//These tables contain the strings that define each ID or type

extern std::vector<e_machine_entry_t> e_machine_table;
extern std::map<uint32_t, std::string> e_type_table;
extern std::map<uint32_t, std::string> e_version_table;
extern std::map<uint32_t, std::string> sh_type_table;
extern std::map<uint32_t, std::string> p_type_table;
extern std::map<uint32_t, std::string> sh_index_table;
extern std::map<uint32_t, std::string> sh_type_table;
#endif