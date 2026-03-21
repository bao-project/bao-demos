ENTRY(_start)

SECTIONS
{   
    .text 0x0 : {
        KEEP(*(.text))
    }

    .bao : {
        KEEP(*(.bao))
    }  
}
