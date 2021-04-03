ENTRY(_start)

SECTIONS
{   
    .nloader : {
        KEEP(*(.nloader))
    }
    
    .linux : ALIGN(0x200000) { 
        __linux_start = .;
        KEEP(*(.linux)) 
        __linux_end = .;
    }  

    .dtb : ALIGN(0x200000) { 
        __dtb_start = ABSOLUTE(.);
        KEEP(*(.dtb)) 
        __dtb_end = .;
    }  
}
