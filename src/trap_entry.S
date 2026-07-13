# this is what will be loaded into the stvec CSR; the
# trap handler address must be 4-byte aligned:
# https://docs.riscv.org/reference/isa/priv/supervisor.html#12-1-1-2-supervisor-trap-vector-base-address-stvec-register
.globl trap_entry
.align 4
trap_entry:
    addi sp, sp, -256 #aloca espaço na memória

    sd ra, 0(sp) #salva o ra
    sd gp, 8(sp)
    sd tp, 16(sp)
    sd t0, 24(sp)
    sd t1, 32(sp)
    sd t2, 40(sp)
    sd t3, 48(sp)
    sd t4, 56(sp)
    sd t5, 64(sp)
    sd t6, 72(sp)
    sd s0, 80(sp)
    sd s1, 88(sp)
    sd s2, 96(sp)
    sd s3, 104(sp)
    sd s4, 112(sp)
    sd s5, 120(sp)
    sd s6, 128(sp)
    sd s7, 136(sp)
    sd s8, 144(sp)
    sd s9, 152(sp)
    sd s10,160(sp)
    sd s11,168(sp)
    sd a0, 176(sp)
    sd a1, 184(sp)
    sd a2, 192(sp)
    sd a3, 200(sp)
    sd a4, 208(sp)
    sd a5, 216(sp)
    sd a6, 224(sp)
    sd a7, 232(sp)
    

    call handle_trap

    # your code here; restore registers
    ld ra, 0(sp)
    ld gp, 8(sp)
    ld tp, 16(sp)
    ld t0, 24(sp)
    ld t1, 32(sp)
    ld t2, 40(sp)
    ld t3, 48(sp)
    ld t4, 56(sp)
    ld t5, 64(sp)
    ld t6, 72(sp)
    ld s0, 80(sp)
    ld s1, 88(sp)
    ld s2, 96(sp)
    ld s3, 104(sp)
    ld s4, 112(sp)
    ld s5, 120(sp)
    ld s6, 128(sp)
    ld s7, 136(sp)
    ld s8, 144(sp)
    ld s9, 152(sp)
    ld s10,160(sp)
    ld s11,168(sp)
    ld a0, 176(sp)
    ld a1, 184(sp)
    ld a2, 192(sp)
    ld a3, 200(sp)
    ld a4, 208(sp)
    ld a5, 216(sp)
    ld a6, 224(sp)
    ld a7, 232(sp)
    addi sp, sp, 256
    sret
