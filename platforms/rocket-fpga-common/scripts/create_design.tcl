source $::env(BAO_DEMOS)/platforms/$::env(PLATFORM)/scripts/env.tcl

# Create SoC
create_project $project_name $project_dir -part $part
set_property board_part $board_part [current_project]
set_property ip_repo_paths $ip_repo [current_project]
update_ip_catalog

#Create Design
create_bd_design "rocket_design"
update_compile_order -fileset sources_1

proc creat_design_zynqmp {} {

    # Create Zynq PS and Config
    create_bd_cell -type ip -vlnv xilinx.com:ip:zynq_ultra_ps_e:3.3 zynq_ultra_ps_e_0
    apply_bd_automation -rule xilinx.com:bd_rule:zynq_ultra_ps_e -config {apply_board_preset "1" }  [get_bd_cells zynq_ultra_ps_e_0]
    set_property -dict [list CONFIG.PSU__USE__M_AXI_GP0 {0} CONFIG.PSU__USE__M_AXI_GP1 {0} CONFIG.PSU__USE__S_AXI_GP2 {1} CONFIG.PSU__SAXIGP2__DATA_WIDTH {64} CONFIG.PSU__USE__S_AXI_GP6 {1} CONFIG.PSU__SAXIGP6__DATA_WIDTH {64}] [get_bd_cells zynq_ultra_ps_e_0]
    set_property -dict [list CONFIG.PSU__IRQ_P2F_UART0__INT {1} CONFIG.PSU__IRQ_P2F_UART1__INT {1}] [get_bd_cells zynq_ultra_ps_e_0]

    # Add AXI interconnects for MMIO and MEM
    create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_0
    set_property -dict [list CONFIG.NUM_MI {1}] [get_bd_cells axi_interconnect_0]
    create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_1
    set_property -dict [list CONFIG.NUM_MI {1}] [get_bd_cells axi_interconnect_1]

    # Add rocket ip 
    create_bd_cell -type ip -vlnv user.org:user:rocket_top:1.0 rocket_0

    # Add concat ip for uart interrupt concatenation
    create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0

    # Connect all elements
    connect_bd_intf_net [get_bd_intf_pins rocket_0/mem_axi] -boundary_type upper [get_bd_intf_pins axi_interconnect_0/S00_AXI]
    connect_bd_intf_net [get_bd_intf_pins rocket_0/mmio_axi] -boundary_type upper [get_bd_intf_pins axi_interconnect_1/S00_AXI]
    connect_bd_intf_net -boundary_type upper [get_bd_intf_pins axi_interconnect_0/M00_AXI] [get_bd_intf_pins zynq_ultra_ps_e_0/S_AXI_HP0_FPD]
    connect_bd_intf_net -boundary_type upper [get_bd_intf_pins axi_interconnect_1/M00_AXI] [get_bd_intf_pins zynq_ultra_ps_e_0/S_AXI_LPD]
    connect_bd_net [get_bd_pins xlconcat_0/dout] [get_bd_pins rocket_0/interrupts]
    connect_bd_net [get_bd_pins zynq_ultra_ps_e_0/ps_pl_irq_uart0] [get_bd_pins xlconcat_0/In0]
    connect_bd_net [get_bd_pins zynq_ultra_ps_e_0/ps_pl_irq_uart1] [get_bd_pins xlconcat_0/In1]
    apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config { Clk {/zynq_ultra_ps_e_0/pl_clk0 (100 MHz)} Freq {100} Ref_Clk0 {} Ref_Clk1 {} Ref_Clk2 {}}  [get_bd_pins axi_interconnect_0/ACLK]
    apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config { Clk {/zynq_ultra_ps_e_0/pl_clk0 (100 MHz)} Freq {100} Ref_Clk0 {} Ref_Clk1 {} Ref_Clk2 {}}  [get_bd_pins axi_interconnect_0/M00_ACLK]
    apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config { Clk {/zynq_ultra_ps_e_0/pl_clk0 (100 MHz)} Freq {100} Ref_Clk0 {} Ref_Clk1 {} Ref_Clk2 {}}  [get_bd_pins axi_interconnect_0/S00_ACLK]
    apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config { Clk {/zynq_ultra_ps_e_0/pl_clk0 (100 MHz)} Freq {100} Ref_Clk0 {} Ref_Clk1 {} Ref_Clk2 {}}  [get_bd_pins axi_interconnect_1/ACLK]
    apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config { Clk {/zynq_ultra_ps_e_0/pl_clk0 (100 MHz)} Freq {100} Ref_Clk0 {} Ref_Clk1 {} Ref_Clk2 {}}  [get_bd_pins axi_interconnect_1/M00_ACLK]
    apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config { Clk {/zynq_ultra_ps_e_0/pl_clk0 (100 MHz)} Freq {100} Ref_Clk0 {} Ref_Clk1 {} Ref_Clk2 {}}  [get_bd_pins axi_interconnect_1/S00_ACLK]
    apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config { Clk {/zynq_ultra_ps_e_0/pl_clk0 (100 MHz)} Freq {100} Ref_Clk0 {} Ref_Clk1 {} Ref_Clk2 {}}  [get_bd_pins rocket_0/mem_axi_aclk]
    apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config { Clk {/zynq_ultra_ps_e_0/pl_clk0 (100 MHz)} Freq {100} Ref_Clk0 {} Ref_Clk1 {} Ref_Clk2 {}}  [get_bd_pins rocket_0/mmio_axi_aclk]

    # Create address segments
    assign_bd_address -offset 0x40000000 -range 0x40000000 -target_address_space [get_bd_addr_spaces rocket_0/mem_axi] [get_bd_addr_segs zynq_ultra_ps_e_0/SAXIGP2/HP0_DDR_LOW] -force
    assign_bd_address -offset 0xFF000000 -range 0x01000000 -target_address_space [get_bd_addr_spaces rocket_0/mmio_axi] [get_bd_addr_segs zynq_ultra_ps_e_0/SAXIGP6/LPD_LPS_OCM] -force

}

switch -regexp $board { 
    zcu(.*) {
        creat_design_zynqmp
    }
    default {
        puts "Platform $board not supported!\n"
        exit -1
    }
}

# Make wrapper and generate output products
make_wrapper -files [get_files $project_dir/$project_name.srcs/sources_1/bd/rocket_design/rocket_design.bd] -top
add_files -norecurse $project_dir/$project_name.srcs/sources_1/bd/rocket_design/hdl/rocket_design_wrapper.v
generate_target all [get_files  $project_dir/$project_name.srcs/sources_1/bd/rocket_design/rocket_design.bd]
close_project