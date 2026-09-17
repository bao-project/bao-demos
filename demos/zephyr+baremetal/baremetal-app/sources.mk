src_c_srcs:= demo.c

# No generic passthrough for this upstream (unlike SHMEM_BASE/SHMEM_SIZE).
ifneq ($(SHMEM_IRQ_ID),)
CPPFLAGS+=-DSHMEM_IRQ_ID=$(SHMEM_IRQ_ID)
endif
