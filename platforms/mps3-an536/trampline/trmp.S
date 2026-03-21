#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

.text
.global _start
_start:
    ldr r0, =BAO_BASE_ADDR
    bx r0
    b  .
