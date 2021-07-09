source $::env(BAO_DEMOS)/platforms/$::env(PLATFORM)/scripts/env.tcl

open_project $project_dir/$project_name.xpr

# Launch synthesis
reset_run synth_1
launch_runs synth_1 -jobs $num_cpus
wait_on_run synth_1

# Launch implementation and generate bitstream

launch_runs impl_1 -jobs $num_cpus -to_step write_bitstream
wait_on_run impl_1 

# Export bitstream to fpga/hw dir
write_hw_platform -fixed -include_bit -force -file $output_xsa
hsi::open_hw_design $output_xsa

close_project