#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUCKET_NUM 12
#define SYMBOL_BASE 16

enum LineType {
    COMMENT = '/',
    EMPTY_LINE = '\n',
    LABEL = '(',
    A_INSTRUCTION = '@',
};

enum Length {
    BASENAME_MAX_LENGTH = 256,
    LINE_MAX_LENGTH = 1024,
    ADDR_MAX_LENGTH = 32767,
    BI_INSTRUCTION_LENGTH = 16,
    ASM_COMP_MAX_LENGTH = 3,
    ASM_DEST_MAX_LENGTH = 3,
    ASM_JUMP_MAX_LENGTH = 3,
};

/* HASH FUNCTION */
int adler32(const void *buf, size_t buflength) {
     const char *buffer = (const char*)buf;

     int s1 = 1;
     int s2 = 0;

     for (size_t n = 0; n < buflength; n++) {
        s1 = (s1 + buffer[n]) % 65521;
        s2 = (s2 + s1) % 65521;
     }
     return (s2 << 16) | s1;
}

typedef struct {
    char *key;
    int  value;
} Pair;

typedef struct {
    int count;
    Pair *pairs;
} Bucket;

typedef struct {
    int count;
    int var_offset;
    Bucket *buckets;
} SymbolMap;

SymbolMap *create_map()
{
    SymbolMap *map = malloc(sizeof(*map));
    if (map == NULL) return NULL;

    Bucket *buckets = malloc(sizeof(*buckets) * BUCKET_NUM);
    if (buckets == NULL) {
        free(map);
        return NULL;
    }
    memset(buckets, 0, sizeof(*buckets) * BUCKET_NUM);

    map->count = BUCKET_NUM;
    map->var_offset = 0;
    map->buckets = buckets;

    return map;
}

void destroy_map(SymbolMap *map)
{
    for (int i = 0; i < map->count; i++) {
        Bucket *bucket = &(map->buckets[i]);

        if (bucket->count == 0) continue;
        for (int j = 0; j < bucket->count; j++) {
            Pair *pair = &(bucket->pairs[j]);
            free(pair->key);
        }
        free(bucket->pairs);
    }

    free(map->buckets);
    free(map);
}

int get(SymbolMap *map, char *key)
{
    int hashCode = adler32(key, strlen(key));
    int index = hashCode % BUCKET_NUM;
    Bucket *bucket = &(map->buckets[index]);

    if (bucket->count == 0) return -1;

    for (int i = 0; i < bucket->count; i++) {
        Pair *pair = &(bucket->pairs[i]);
        if (strcmp(pair->key, key) == 0) return pair->value;
    }

    return -1;
}

int put(SymbolMap *map, char *key, int val)
{
    int hashCode = adler32(key, strlen(key));
    int index = hashCode % BUCKET_NUM;
    Bucket *bucket = &(map->buckets[index]);

    for (int i = 0; i < bucket->count; i++) {
        Pair *pair = &(bucket->pairs[i]);
        if (strcmp(pair->key, key) == 0) {
            pair->value = val;
            return 0;
        }
    }

    bucket->count++;

    Pair *new_pairs = realloc(bucket->pairs, sizeof(*new_pairs) * bucket->count);
    if (new_pairs == NULL) return -1;

    char *new_key = malloc(sizeof(*new_key) * (strlen(key)+1));
    if (new_key == NULL) {
        free(new_pairs);
        return -1;
    }

    bucket->pairs = new_pairs;
    Pair *new_pair = &(bucket->pairs[(bucket->count)-1]);
    new_pair->key = new_key;
    strcpy(new_key, key);
    new_pair->value = val;

    return 0;
}

void append_variable(SymbolMap *map, char *key)
{
    int index = map->var_offset + SYMBOL_BASE;

    put(map, key, index);

    map->var_offset++;
}

void seed(SymbolMap *map)
{
    put(map, "R0", 0);
    put(map, "R1", 1);
    put(map, "R2", 2);
    put(map, "R3", 3);
    put(map, "R4", 4);
    put(map, "R5", 5);
    put(map, "R6", 6);
    put(map, "R7", 7);
    put(map, "R8", 8);
    put(map, "R9", 9);
    put(map, "R10", 10);
    put(map, "R11", 11);
    put(map, "R12", 12);
    put(map, "R13", 13);
    put(map, "R14", 14);
    put(map, "R15", 15);

    put(map, "SP", 0);
    put(map, "LCL", 1);
    put(map, "ARG", 2);
    put(map, "THIS", 3);
    put(map, "THAT", 4);

    put(map, "SCREEN", 16384);
    put(map, "KBD", 24576);
}

char *get_dst_path(char *src_path)
{
    char *from = src_path;
    char *to = NULL;

    while ((to = strchr(from, '/')) != NULL) from = to + 1;

    if ((to = strchr(from, '.')) == NULL) return NULL;

    char *buf = malloc(sizeof(*buf) * BASENAME_MAX_LENGTH);
    int length = to - from;

    memcpy(buf, from, length);
    buf[length] = '\0';
    strcat(buf, ".hack");

    return buf;
}

char *trim_preceiding_whitespaces(char *line)
{
    while (line[0] == ' ' || line[0] == '\t') line++;

    return line;
}

char *decimal_to_binary(int addr)
{
    char *buf = malloc(sizeof(*buf) * (BI_INSTRUCTION_LENGTH));
    buf[BI_INSTRUCTION_LENGTH-1] = '\0';
    int j = BI_INSTRUCTION_LENGTH - 2;

    for (int i = 0; i <= (BI_INSTRUCTION_LENGTH - 2); i++) {
        if ((addr >> i) & 1)
            buf[j--] = '1';
        else
            buf[j--] = '0';
    }

    return buf;
}

void get_prefix(char *bi_instr)
{
    strcat(bi_instr, "111");
}

// Only has two kinds of comp in asm
char *get_asm_comp(char *asm_instr)
{
    char *asm_comp = malloc(sizeof(*asm_comp) * (ASM_COMP_MAX_LENGTH + 1));

    int is_jump = (strchr(asm_instr, '=') == NULL);

    if (is_jump) {
        // D;JMP
        char *from = asm_instr;
        char *to = strchr(asm_instr, ';');
        int length = to - from;
        memcpy(asm_comp, from, length);
        asm_comp[length] = '\0';
    } else {
        // M=MD
        char *from = (strchr(asm_instr, '=') + 1);
        char *to = strchr(asm_instr, '\n');
        int length = to - from;
        memcpy(asm_comp, from, length);
        asm_comp[length] = '\0';
    }

    return asm_comp;
}

void translate_COMP(char *bi_instr, char *asm_instr)
{
    char *asm_comp = get_asm_comp(asm_instr);
    int is_M_register = strchr(asm_comp, 'M') != NULL;

    strcat(bi_instr, (is_M_register) ? "1" : "0");

    if (strcmp(asm_comp, "0") == 0) {
        strcat(bi_instr, "101010");
        goto clean;
    }

    if (strcmp(asm_comp, "1") == 0) {
        strcat(bi_instr, "111111");
        goto clean;
    }

    if (strcmp(asm_comp, "-1") == 0) {
        strcat(bi_instr, "111010");
        goto clean;
    }

    if (strcmp(asm_comp, "D") == 0) {
        strcat(bi_instr, "001100");
        goto clean;
    }

    if (strcmp(asm_comp, "A") == 0 || strcmp(asm_comp, "M") == 0) {
        strcat(bi_instr, "110000");
        goto clean;
    }

    if (strcmp(asm_comp, "!D") == 0) {
        strcat(bi_instr, "001101");
        goto clean;
    }

    if (strcmp(asm_comp, "!A") == 0 || strcmp(asm_comp, "!M") == 0) {
        strcat(bi_instr, "110001");
        goto clean;
    }

    if (strcmp(asm_comp, "-D") == 0) {
        strcat(bi_instr, "001111");
        goto clean;
    }

    if (strcmp(asm_comp, "-A") == 0 || strcmp(asm_comp, "-M") == 0) {
        strcat(bi_instr, "110011");
        goto clean;
    }

    if (strcmp(asm_comp, "D+1") == 0) {
        strcat(bi_instr, "011111");
        goto clean;
    }

    if (strcmp(asm_comp, "A+1") == 0 || strcmp(asm_comp, "M+1") == 0) {
        strcat(bi_instr, "110111");
        goto clean;
    }

    if (strcmp(asm_comp, "D-1") == 0) {
        strcat(bi_instr, "001110");
        goto clean;
    }

    if (strcmp(asm_comp, "A-1") == 0 || strcmp(asm_comp, "M-1") == 0) {
        strcat(bi_instr, "110010");
        goto clean;
    }

    if (strcmp(asm_comp, "D+A") == 0 || strcmp(asm_comp, "D+M") == 0) {
        strcat(bi_instr, "000010");
        goto clean;
    }

    if (strcmp(asm_comp, "D-A") == 0 || strcmp(asm_comp, "D-M") == 0) {
        strcat(bi_instr, "010011");
        goto clean;
    }

    if (strcmp(asm_comp, "A-D") == 0 || strcmp(asm_comp, "M-D") == 0) {
        strcat(bi_instr, "000111");
        goto clean;
    }

    if (strcmp(asm_comp, "D&A") == 0 || strcmp(asm_comp, "D&M") == 0) {
        strcat(bi_instr, "000000");
        goto clean;
    }

    if (strcmp(asm_comp, "D|A") == 0 || strcmp(asm_comp, "D|M") == 0) {
        strcat(bi_instr, "010101");
        goto clean;
    }

clean:
    free(asm_comp);
}

void translate_DEST(char *bi_instr, char *asm_instr)
{
    if (strchr(asm_instr, '=') == NULL) {
        strcat(bi_instr, "000");
        goto ret;
    }

    char asm_dest[ASM_DEST_MAX_LENGTH+1] = {0};
    char *from = asm_instr;
    char *to = strchr(asm_instr, '=');
    int length = to - from;

    memcpy(asm_dest, from, length);
    asm_dest[length] = '\0';

    if (strcmp(asm_dest, "M") == 0) {
        strcat(bi_instr, "001");
        goto ret;
    }

    if (strcmp(asm_dest, "D") == 0) {
        strcat(bi_instr, "010");
        goto ret;
    }

    if (strcmp(asm_dest, "MD") == 0) {
        strcat(bi_instr, "011");
        goto ret;
    }

    if (strcmp(asm_dest, "A") == 0) {
        strcat(bi_instr, "100");
        goto ret;
    }

    if (strcmp(asm_dest, "AM") == 0) {
        strcat(bi_instr, "101");
        goto ret;
    }

    if (strcmp(asm_dest, "AD") == 0) {
        strcat(bi_instr, "110");
        goto ret;
    }

    if (strcmp(asm_dest, "AMD") == 0) {
        strcat(bi_instr, "111");
        goto ret;
    }

ret:
    return;
}

void translate_JUMP(char *bi_instr, char *asm_instr)
{
    if (strchr(asm_instr, ';') == NULL) {
        strcat(bi_instr, "000");
        goto ret;
    }

    char asm_jump[ASM_JUMP_MAX_LENGTH+1] = {0};
    char *from = (strchr(asm_instr, ';') + 1);
    char *to = strchr(asm_instr, '\n');
    int length = to - from;

    memcpy(asm_jump, from, length);
    asm_jump[length] = '\0';

    if (strcmp(asm_jump, "JGT") == 0) {
        strcat(bi_instr, "001");
        goto ret;
    }

    if (strcmp(asm_jump, "JEQ") == 0) {
        strcat(bi_instr, "010");
        goto ret;
    }

    if (strcmp(asm_jump, "JGE") == 0) {
        strcat(bi_instr, "011");
        goto ret;
    }

    if (strcmp(asm_jump, "JLT") == 0) {
        strcat(bi_instr, "100");
        goto ret;
    }

    if (strcmp(asm_jump, "JNE") == 0) {
        strcat(bi_instr, "101");
        goto ret;
    }

    if (strcmp(asm_jump, "JLE") == 0) {
        strcat(bi_instr, "110");
        goto ret;
    }

    if (strcmp(asm_jump, "JMP") == 0) {
        strcat(bi_instr, "111");
        goto ret;
    }

ret:
    return;
}

void translate_A_instruction(char *asm_instr, FILE *dst, SymbolMap *map)
{
    printf("asm_instr(%zd)>>>%s", strlen(asm_instr), asm_instr);
    char bi_instr[BI_INSTRUCTION_LENGTH+1] = {0};
    char *from = asm_instr + 1;
    char *to = strchr(asm_instr, '\n');
    int length = to - from;

    char *str_addr = malloc(sizeof(*str_addr) * (length+1));
    memcpy(str_addr, from, length);
    str_addr[length] = '\0';

    char *end_ptr = NULL;
    int decimal_addr = strtol(str_addr, &end_ptr, 10);

    if (end_ptr == str_addr) {
        if (get(map, str_addr) == -1) append_variable(map, str_addr);
        decimal_addr = get(map, str_addr);
    }

    char *bi_addr = decimal_to_binary(decimal_addr);

    strcat(bi_instr, "0");
    strcat(bi_instr, bi_addr);

    fputs(bi_instr, dst);
    fputc('\n', dst);

    free(str_addr);
    free(bi_addr);
}

void translate_C_instruction(char *asm_instr, FILE *dst)
{
    char bi_instr[BI_INSTRUCTION_LENGTH+1] = {0};

    get_prefix(bi_instr);
    translate_COMP(bi_instr, asm_instr);
    translate_DEST(bi_instr, asm_instr);
    translate_JUMP(bi_instr, asm_instr);

    fputs(bi_instr, dst);
    fputc('\n', dst);
}

void translate_src(FILE *src, FILE *dst, SymbolMap *map)
{
    char line[LINE_MAX_LENGTH] = {0};

    while(fgets(line, LINE_MAX_LENGTH, src)) {
        char *trim_line = trim_preceiding_whitespaces(line);

        switch (trim_line[0]) {
        case COMMENT:
        case EMPTY_LINE:
        case LABEL:
            continue;
        case A_INSTRUCTION:
            translate_A_instruction(trim_line, dst, map);
            break;
        default:
            translate_C_instruction(trim_line, dst);
            break;
        }
    }
}

void scan_label(char *trim_line, int line_count, SymbolMap *map)
{
    char *from = strchr(trim_line, '(') + 1;
    char *to = strchr(trim_line, ')');
    int length = to - from;

    char *buf = malloc(sizeof(*buf) * (length+1));
    memcpy(buf, from, length);
    buf[length] = '\0';

    put(map, buf, line_count);

    free(buf);
}

void scan_src(FILE *src, SymbolMap *map)
{
    char line[LINE_MAX_LENGTH] = {0};
    int line_count = 0;

    while(fgets(line, LINE_MAX_LENGTH, src)) {
        char *trim_line = trim_preceiding_whitespaces(line);

        switch (trim_line[0]) {
        case COMMENT:
        case EMPTY_LINE:
            continue;
        case LABEL:
            scan_label(trim_line, line_count, map);
            break;
        default:
            line_count++;
            break;
        }
    }
}

int main(int argc, char **argv)
{
    int status = EXIT_FAILURE;
    SymbolMap *map = NULL;
    char *src_path = NULL;
    char *dst_path = NULL;
    FILE *src = NULL;
    FILE *dst = NULL;

    if (argc != 2) {
        fprintf(stderr, "USAGE: ./assembler <FILEPATH>");
        goto ret;
    }

    src_path = argv[1];

    if ((dst_path = get_dst_path(src_path)) == NULL) {
        fprintf(stderr, "Invalid file path");
        goto ret;
    }

    if ((src = fopen(src_path, "r")) == NULL) {
        fprintf(stderr, "Could not open the source file");
        goto clean;
    }

    if ((dst = fopen(dst_path, "w")) == NULL) {
        fprintf(stderr, "Could not open the destination file");
        goto clean;
    }

    map = create_map();
    seed(map);

    scan_src(src, map);
    rewind(src);
    translate_src(src, dst, map);

    status = EXIT_SUCCESS;
    printf("======> Success assemble %s\n", src_path);
    printf("======> Generating %s\n", dst_path);

clean:
    if (dst_path) free(dst_path);
    if (map)      destroy_map(map);
    if (src)      fclose(src);
    if (dst)      fclose(dst);
ret:
    return status == EXIT_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}
