#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

.section .nloader, "a"
.global _start
_start:

    adr r3, _start

    cpsid if

    mrc p15, 0, r1, c1, c0, 0 // sctlr
    mov r1, #0x1007
    bic r0, r0, r1
    mcr p15, 0, r0, c1, c0, 0 // sctlr

    mov r0, #0
    mov r1, #0xffffffff
    ldr r2, =__dtb_start
    add r2, r2, r3

    ldr r4, =__linux_start
    add r4, r4, r3
    bx r4

.section .linux, "a"
    .incbin STRINGIFY(IMAGE)

.section .dtb, "a"
    .incbin STRINGIFY(DTB)
 
