set(SRCS e3650_boardinit.c e3650_bringup.c)

target_sources(board PRIVATE ${SRCS})

set_property(GLOBAL PROPERTY LD_SCRIPT "${NUTTX_BOARD_DIR}/scripts/linkscript_bao.ld")
