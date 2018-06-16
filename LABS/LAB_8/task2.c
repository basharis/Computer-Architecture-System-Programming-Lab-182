#include "task2.h"

int main(int argc, char** argv) {
    int action = -1;
    int num_of_actions = sizeof(_menu)/sizeof(struct action);
    while (!_quit_f){
        for (int i = 1 ; i < num_of_actions ; i ++)
            printf("\n%d - %s", i, _menu[i].name);
        printf("\n\nPlease choose action:\n");
        scanf("%d", &action); fflush(stdin);
        if (action >= 0 && action < num_of_actions)
            _menu[action].func();
        else
            fprintf(stderr, "Invalid action.\n\n");
    }
    return EXIT_SUCCESS;
}



// Function handlers
void examine_elf_file() {
    open_file();
    Elf64_Ehdr * header = (Elf64_Ehdr *) _map_start;
    printf("\nELF Header:\n");
    printf("%-30s %X %X %X\n", 			"  Magic 1-3:",                     header->e_ident[1], header->e_ident[2], header->e_ident[3]);
    printf("%-30s 2's complement, %s\n", 	"  Data:",                          header->e_ident[EI_DATA] == ELFDATA2LSB ? "little endian" : "big endian");
    printf("%-30s 0x%X\n", 			"  Entry point address:",           (int) header->e_entry);
    printf("%-30s %d (bytes into file)\n", 	"  Start of section headers:",      (int) header->e_shoff);
    printf("%-30s %d\n", 			"  Number of section headers:",     (int) header->e_shnum);
    printf("%-30s %d (bytes)\n", 		"  Size of section headers:",       (int) header->e_shentsize);
    printf("%-30s %d (bytes into file)\n", 	"  Start of program headers:",      (int) header->e_phoff);
    printf("%-30s %d\n", 			"  Number of program headers:",     (int) header->e_phnum);
    printf("%-30s %d (bytes)\n", 		"  Size of program headers:",       (int) header->e_phentsize);
    printf("\n\n");
}

void print_section_names() {
    Elf64_Ehdr * header;
    if (_map_start)
        header = (Elf64_Ehdr *) _map_start;
    else{
        fprintf(stderr, "No file, please try again.\n");
        return;
    }
    int num_sh = header->e_shnum;
    int sh_off = header->e_shoff;
    Elf64_Shdr * sections = (Elf64_Shdr *) (_map_start + sh_off);
    Elf64_Shdr * sh_strtab = &sections[header->e_shstrndx];
    char* sh_strtab_char = _map_start + sh_strtab->sh_offset;
    printf("\nSection Headers:\n\n");
    printf("  %-5s   %-17s %21s \t\t  %s \t %s \t\t %s\n", 
            "Index", 
            "Name", 
            "Address", 
            "Offset", 
            "Size", 
            "Type");
    for (int i = 0 ; i < num_sh ; i ++) {
        printf("  [%2d]   %-17s %-15s%#016x %-7s%#08x \t %-7x \t %-7s\n", 
                i, 
                &sh_strtab_char[sections[i].sh_name],
                "",
                (int)   sections[i].sh_addr,
                "",
                (int)   sections[i].sh_offset, 
                (int)   sections[i].sh_size, 
                type_to_str(sections[i].sh_type));
    }   
}

void print_symbols() {
    Elf64_Ehdr * header;
    if (_map_start)
        header = (Elf64_Ehdr *) _map_start;
    else{
        fprintf(stderr, "No file, please try again.\n");
        return;
    }
    int num_sh = header->e_shnum;
    int sh_off = header->e_shoff;
    Elf64_Shdr * sections = (Elf64_Shdr *) (_map_start + sh_off);
    Elf64_Shdr * sh_strtab = &sections[header->e_shstrndx];
    char* sh_strtab_char = _map_start + sh_strtab->sh_offset;
    Elf64_Sym * symbols;
    for (int i = 0 ; i < num_sh ; i ++) {
        if (sections[i].sh_type == SHT_SYMTAB) {
            symbols = (Elf64_Sym *) (_map_start + sections[i].sh_offset);
            Elf64_Shdr * st_strtab = &sections[sections[i].sh_link];
            char * st_strtab_char = (char*) (_map_start + st_strtab->sh_offset);
            int num_of_symbols = sections[i].sh_size/sizeof(Elf64_Sym);
            printf("\nSymbol Table:\n\n");
            printf("  %-5s   %-17s %21s \t %s \t\t %s\n", 
                    "Index", 
                    "Value", 
                    "Section Index", 
                    "Section Name", 
                    "Symbol Name");
            for (int j = 0 ; j < num_of_symbols ; j ++) {
                printf("  [%2d] \t %#016x \t\t %d \t\t %-7s \t\t %-7s\n", 
                        j,
                        (int)   symbols[j].st_value,
                        symbols[j].st_shndx,
                        symbols[j].st_shndx > 0xff00 ? ndx_to_str(symbols[j].st_shndx) : &sh_strtab_char[sections[symbols[j].st_shndx].sh_name],
                        &st_strtab_char[symbols[j].st_name]);
            }
            return;
        }
    }
}

void quit() {
    printf("Quitting...\n\n");
    if (_map_start)
        munmap(_map_start, _fd_stat.st_size);
    _quit_f = TRUE;
}


// Helper Functions
void open_file() {
    int fd;
    printf("Please choose file name:\n");
    scanf("%s", _filename); fflush(stdin);
    if ((fd = open(_filename, O_RDWR)) < 0) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }
    if (fstat(fd, &_fd_stat) != 0) {
        perror("fstat failed");
        exit(EXIT_FAILURE);
    }
    if ((_map_start = mmap(NULL, _fd_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        perror("mmap failed");
        exit(-4);
    }
    if (close(fd) < 0){
        perror("close failed");
        exit(EXIT_FAILURE);
    }
}
char* type_to_str (int type) {
    switch (type) {
        case SHT_NULL:
            return "SHT_NULL";
        case SHT_PROGBITS:
            return "SHT_PROGBITS";
        case SHT_SYMTAB:
            return "SHT_SYMTAB";
        case SHT_STRTAB:
            return "SHT_STRTAB";
        case SHT_RELA:
            return "SHT_RELA";
        case SHT_HASH:
            return "SHT_HASH";
        case SHT_DYNAMIC:
            return "SHT_DYNAMIC";
        case SHT_NOTE:
            return "SHT_NOTE";
        case SHT_NOBITS:
            return "SHT_NOBITS";
        case SHT_REL:
            return "SHT_REL";
        case SHT_SHLIB:
            return "SHT_SHLIB";
        case SHT_DYNSYM:
            return "SHT_DYNSYM";
        case SHT_INIT_ARRAY:
            return "SHT_INIT_ARRAY";
        case SHT_PREINIT_ARRAY:
            return "SHT_PREINIT_ARRAY";
        case SHT_GROUP:
            return "SHT_GROUP";
        case SHT_SYMTAB_SHNDX:
            return "SHT_SYMTAB_SHNDX";
        case SHT_NUM:
            return "SHT_NUM";
        case SHT_LOOS:
            return "SHT_LOOS";
        case SHT_GNU_ATTRIBUTES:
            return "SHT_GNU_ATTRIBUTES";
        case SHT_GNU_HASH:
            return "SHT_GNU_HASH";
        case SHT_GNU_LIBLIST:
            return "SHT_GNU_LIBLIST";
        case SHT_CHECKSUM:
            return "SHT_CHECKSUM";
        case SHT_SUNW_COMDAT:
            return "SHT_SUNW_COMDAT";
        case SHT_SUNW_syminfo:
            return "SHT_SUNW_syminfo";
        case SHT_GNU_verdef:
            return "SHT_GNU_verdef";
        case SHT_GNU_verneed:
            return "SHT_GNU_verneed";
        case SHT_LOPROC:
            return "SHT_LOPROC";
        case SHT_HIPROC:
            return "SHT_HIPROC";
        case SHT_LOUSER:
            return "SHT_LOUSER";
        case SHT_HIUSER:
            return "SHT_HIUSER";
        default:
            return NULL;
    }
}
char* ndx_to_str(Elf64_Section i) {
    switch (i) {
	case 0: 
            return "UNDEF";
	case 0xff00: 
            return "LORESERVE";
	case 0xff1f: 
            return "HIPROC";
	case 0xfff1: 
            return "ABS";
	case 0xfff2: 
            return "COMMON";
	case 0xffff: 
            return "HIRESERVE";
	default: 
            return "UNKNOWN";
    }
}