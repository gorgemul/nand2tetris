#include <string.h>
#include <stdio.h>
#include <stdlib.h>

enum LineType {
    COMMENT = '/',
    EMPTY_LINE = '\n',
    A_INSTRUCTION = '@',
};

enum Length {
    BASENAME_LENGTH = 256,
    LINE_MAX_LENGTH = 1024,
    ADDR_MAX_LENGTH = 32767,
    BI_INSTRUCTION_LENGTH = 16,
    ASM_COMP_MAX_LENGTH = 3,
    ASM_DEST_MAX_LENGTH = 3,
    ASM_JUMP_MAX_LENGTH = 3,
};

char *get_dst_path(char *src_path)
{
    char *from = src_path;
    char *to = NULL;

    while ((to = strchr(from, '/')) != NULL) from = to + 1;

    if ((to = strchr(from, '.')) == NULL) return NULL;

    char *buf = malloc(sizeof(*buf) * BASENAME_LENGTH);
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

    memcpy(asm_jump, from ,length);
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

void translate_A_instruction(char *asm_instr, FILE *dst)
{
    char bi_instr[BI_INSTRUCTION_LENGTH+1] = {0};

    char *decimal_addr = asm_instr + 1;
    char *bi_addr = decimal_to_binary(atoi(decimal_addr));

    strcat(bi_instr, "0");
    strcat(bi_instr, bi_addr);

    fputs(bi_instr, dst);
    fputc('\n', dst);

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

void scan_src(FILE *src, FILE *dst)
{
    char line[LINE_MAX_LENGTH] = {0};

    while(fgets(line, LINE_MAX_LENGTH, src)) {
        char *asm_instr = trim_preceiding_whitespaces(line);

        switch (asm_instr[0]) {
        case COMMENT:
        case EMPTY_LINE:
            continue;
        case A_INSTRUCTION:
            translate_A_instruction(asm_instr, dst);
            break;
        default:
            translate_C_instruction(asm_instr, dst);
            break;
        }
    }

}

int main(int argc, char **argv)
{
    int status = EXIT_FAILURE;
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
        fprintf(stderr, "Could not open the files");
        goto clean;
    }

    if ((dst = fopen(dst_path, "w")) == NULL) {
        fprintf(stderr, "Could not open the files");
        goto clean;
    }

    scan_src(src, dst);

    status = EXIT_SUCCESS;
    printf("======> Success assemble %s\n", src_path);
    printf("======> Generating %s\n", dst_path);

clean:
    if (dst_path) free(dst_path);
    if (src)      fclose(src);
    if (dst)      fclose(dst);
ret:
    return status == EXIT_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}
