ENTRY(_start)

SECTIONS
{   
    .nloader : {
        KEEP(*(.nloader))
    }

    .dtb : ALIGN(8) { 
        __dtb_start = ABSOLUTE(.);
        KEEP(*(.dtb)) 
        __dtb_end = .;
    }  
    
    .linux 0x80000 : { 
        __linux_start = .;
        KEEP(*(.linux)) 
        __linux_end = .;
    }  
}
