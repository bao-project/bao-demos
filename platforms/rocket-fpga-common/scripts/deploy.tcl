source $::env(BAO_DEMOS)/platforms/$::env(PLATFORM)/scripts/env.tcl

connect
targets -set -filter {name =~ "PSU"}

source  $output_psu_init
psu_init
after 1000
psu_ps_pl_isolation_removal
after 1000
psu_ps_pl_reset_config

set file [lindex $argv 0]

switch -regexp $board { 
    zcu(.*) {
        set addr 0x40000000
    }
    default {
        puts "Platform not supported!\n"
        exit -1
    }
}

puts "loading $file to $addr"
dow -data $file $addr

puts "flashing bitstream $output_bit"
fpga -f $output_bit

disconnect