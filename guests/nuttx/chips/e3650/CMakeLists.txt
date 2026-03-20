
set(SRCS e3650_boot.c
         e3650_allocateheap.c
         e3650_timestamp.c
         e3650_irq.c
         e3650_uart.c)

if(CONFIG_SMP)
  list(APPEND SRCS e3650_cpustart.c)
endif()

target_sources(arch PRIVATE ${SRCS})

target_include_directories(arch PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
