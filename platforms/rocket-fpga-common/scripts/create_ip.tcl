source $::env(BAO_DEMOS)/platforms/$::env(PLATFORM)/scripts/env.tcl

set src_dir $::env(BAO_DEMOS)/platforms/$::env(PLATFORM)/srcs/
set rocket_config freechips.rocketchip.system.TestHarness.$::env(BAO_DEMOS_ROCKET_CONFIG)
set generated_srcs $::env(BAO_DEMOS_CHIPYARD)/sims/vcs/generated-src

create_project $ip_name $ip_repo -part $part
set_property board_part $board_part [current_project]
import_files $src_dir/rocket_top.v
import_files $generated_srcs/$rocket_config/$rocket_config.top.v
import_files $generated_srcs/$rocket_config/$rocket_config.top.mems.v
import_files $generated_srcs/$rocket_config/plusarg_reader.v
set_property top rocket_top [get_filesets sim_1]
set_property top_lib xil_defaultlib [get_filesets sim_1]
update_compile_order -fileset sources_1
ipx::package_project -root_dir $ip_repo -vendor user.org -library user -taxonomy /UserIP
set_property core_revision 2 [ipx::current_core]
set_property value 64 [ipx::get_user_parameters C_MMIO_AXI_ADDR_WIDTH -of_objects [ipx::current_core]]
set_property value 64 [ipx::get_hdl_parameters C_MMIO_AXI_ADDR_WIDTH -of_objects [ipx::current_core]]
set_property value 64 [ipx::get_user_parameters C_MEM_AXI_ADDR_WIDTH -of_objects [ipx::current_core]]
set_property value 64 [ipx::get_hdl_parameters C_MEM_AXI_ADDR_WIDTH -of_objects [ipx::current_core]]
ipx::create_xgui_files [ipx::current_core]
ipx::update_checksums [ipx::current_core]
ipx::check_integrity [ipx::current_core]
ipx::save_core [ipx::current_core]
set_property  ip_repo_paths $ip_repo [current_project]
update_ip_catalog
close_project