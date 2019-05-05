#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <byteswap.h>

int PASS_LEN;
#define NB_REGS 9

int32_t* tab;

char vm_opcode[60] = {
 0x01,0x02,0x00,0x00
,0x01,0x03,0x00,0x00
,0x01,0x07,0x04,0x00
,0x01,0x08,0x01,0x00
,0x02,0x03,0x06,0x00
,0x03,0x09,0x00,0x00
,0x04,0x04,0x00,0x00
,0x04,0x05,0x01,0x00
,0x08,0x04,0x05,0x00
,0x05,0x02,0x04,0x00
,0x05,0x00,0x07,0x00
,0x05,0x01,0x08,0x00
,0x05,0x03,0x08,0x00
,0x06,0xF7,0x00,0x00
,0x07,0x02,0x00, 0x00
};

typedef struct {
    int32_t pc;
    int64_t regs[NB_REGS];
    int64_t result;
    int8_t is_zero;
} vm_t;
void dispatcher(vm_t* vm);

void move(vm_t* vm, int32_t instr) {
    int8_t op2 = (instr >> 8) & 0x00FF;
    int8_t op1 = (instr >> 16) & 0x0F;
    vm->regs[op1] = op2;
    // printf("MOV: %d %d\n", op1, op2);

    vm->pc += 4;
    dispatcher(vm);
}

void cmp(vm_t* vm, int32_t instr) {
    int8_t op2 = (instr >> 8) & 0x00F;
    int8_t op1 = (instr >> 16) & 0x0F;

    // printf("CMP: %d %d %d\n", op1, op2, vm->regs[3]);
    // printf("%d %d \n", vm->regs[op1], vm->regs[op2]);
    if ((vm->regs[op1] - vm->regs[op2]) == 0) {
        vm->is_zero = 1;
    } else {
        vm->is_zero = 0;
    }

    if (vm->regs[3] > 45) {
        exit(0);
    }

    vm->pc += 4;
    dispatcher(vm);
}

void jz(vm_t* vm, int32_t instr) {
    // printf("JZ\n");
    if (vm->is_zero == 1) {
        int8_t offset = (instr >> 16) & 0x0F;
        vm->pc += offset*4;
    } else {    
        vm->pc += 4;
    }
    dispatcher(vm);
}

void load(vm_t* vm, int32_t instr) {
    int8_t op2 = (instr >> 8) & 0x00F;
    int8_t op1 = (instr >> 16) & 0x0F;
    
    vm->regs[op1] = 0;
    if (op1 == 5) {
        // printf("%c", vm->regs[op1]);
        vm->regs[op1] = *((int8_t*) vm->regs[op2]);
    } else {
        vm->regs[op1] = *((int32_t*) vm->regs[op2]);
    }

    vm->pc += 4;
    dispatcher(vm);
}

void add(vm_t* vm, int32_t instr) {
    int8_t op2 = (instr >> 8) & 0x00F;
    int8_t op1 = (instr >> 16) & 0x0F;
    // printf("ADD: %d %d\n", op1, op2);
    vm->regs[op1] += vm->regs[op2];

    vm->pc += 4;
    dispatcher(vm);
}

void jmp(vm_t* vm, int32_t instr) {
    int8_t offset = (instr >> 16) & 0xFF;
    // printf("JMP: %d\n", offset);
    vm->pc += offset*4;

    dispatcher(vm);
}

void ret(vm_t* vm, int32_t instr) {
    vm->result = vm->regs[(instr >> 16) & 0x0F];
    vm->pc += 4;
}

void mul(vm_t* vm, int32_t instr) {
    int8_t op2 = (instr >> 8) & 0x00F;
    int8_t op1 = (instr >> 16) & 0x0F;
    // printf("ADD: %d %d\n", op1, op2);
    vm->regs[op1] *= vm->regs[op2];

    vm->pc += 4;
    dispatcher(vm);
}

void dispatcher(vm_t* vm) {
    int32_t instr = __bswap_32 (*((int32_t*) (vm_opcode + vm->pc)));
    int8_t op_code = instr >> 24;

    switch (op_code)
    {
        case 1:
            move(vm, instr);
            break;
        case 2:
            cmp(vm, instr);
            break;
        case 3:
            jz(vm, instr);
            break;
        case 4:
            load(vm, instr);
            break;
        case 5:
            add(vm, instr);
            break;
        case 6:
            jmp(vm, instr);
            break;
        case 7:
            ret(vm, instr);
            break;
        case 8:
            mul(vm, instr);
            break;
        default:
            printf("INVALID INSTR: %d %x %d\n", vm->pc, op_code, instr);
            exit(1);
            break;
    }
}

vm_t* init_vm() {
    vm_t* vm = (vm_t*) malloc(sizeof(vm_t));
    vm->pc = 0;
    for (int i = 0; i < NB_REGS; i++) {
        vm->regs[i] = 0;
    }
    vm->result = 0;
    vm->is_zero = 0;
}

void build_tab() {
    tab[0] = 31;
    for (int i = 1; i < PASS_LEN; i++) {
        tab[i] = tab[i-1] * 31;
    }
}

int64_t hash_vm(char* pass) {
    vm_t* vm = init_vm();
    vm->regs[0] = (int64_t) tab;
    vm->regs[1] = (int64_t) pass;
    vm->regs[6] = strlen(pass);
    dispatcher(vm);
    int64_t hash = vm->result;
    free(vm);

    return hash;
}

void launch_vm() {
    char user_input[255];
    memset(user_input, 255, 0x0);
    printf("Pass: ");
    fgets(user_input, 244, stdin);

    
    PASS_LEN = strlen(user_input);
    tab = malloc(PASS_LEN*sizeof(int32_t));
    build_tab();
    int64_t hash_t = hash_vm(user_input);

    if (hash_t == -237549964763) {
        printf("Finally, you got it ! Validate with this flag.\n");
        exit(8);
    } else {
        printf("I am so sorry...\n");
        exit(1);
    }
}