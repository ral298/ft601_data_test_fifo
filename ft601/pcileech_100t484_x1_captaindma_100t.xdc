set_property IOSTANDARD LVCMOS33 [get_ports {{ft601_be[*]} {ft601_data[*]}}]
set_property SLEW FAST [get_ports {{ft601_be[*]} {ft601_data[*]}}]
set_property IOB TRUE [get_cells {i_pcileech_ft601/FT601_DATA_OUT_reg[0][*]}]
set_multicycle_path -from [list i_pcileech_ft601/OE_reg/C i_pcileech_ft601/OE_reg_replica/C i_pcileech_ft601/OE_reg_replica_1/C i_pcileech_ft601/OE_reg_replica_2/C i_pcileech_ft601/OE_reg_replica_3/C i_pcileech_ft601/OE_reg_replica_4/C i_pcileech_ft601/OE_reg_replica_5/C i_pcileech_ft601/OE_reg_replica_6/C [get_pins {i_pcileech_ft601/OE_reg_replica_3/C i_pcileech_ft601/OE_reg_replica_1/C i_pcileech_ft601/OE_reg_replica/C i_pcileech_ft601/OE_reg_replica_2/C}]] -to [get_ports {{ft601_data[31]} {ft601_data[30]} {ft601_data[29]} {ft601_data[28]} {ft601_data[27]} {ft601_data[26]} {ft601_data[25]} {ft601_data[24]} {ft601_data[23]} {ft601_data[22]} {ft601_data[21]} {ft601_data[20]} {ft601_data[19]} {ft601_data[18]} {ft601_data[17]} {ft601_data[16]} {ft601_data[15]} {ft601_data[14]} {ft601_data[13]} {ft601_data[12]} {ft601_data[11]} {ft601_data[10]} {ft601_data[9]} {ft601_data[8]} {ft601_data[7]} {ft601_data[6]} {ft601_data[5]} {ft601_data[4]} {ft601_data[3]} {ft601_data[2]} {ft601_data[1]} {ft601_data[0]}}] 2
set_multicycle_path -from [list i_pcileech_ft601/OE_reg/C i_pcileech_ft601/OE_reg_replica/C i_pcileech_ft601/OE_reg_replica_1/C i_pcileech_ft601/OE_reg_replica_2/C i_pcileech_ft601/OE_reg_replica_3/C i_pcileech_ft601/OE_reg_replica_4/C i_pcileech_ft601/OE_reg_replica_5/C i_pcileech_ft601/OE_reg_replica_6/C [get_pins {i_pcileech_ft601/OE_reg_replica_3/C i_pcileech_ft601/OE_reg_replica_1/C i_pcileech_ft601/OE_reg_replica/C i_pcileech_ft601/OE_reg_replica_2/C}]] -to [get_ports {{ft601_data[31]} {ft601_data[30]} {ft601_data[29]} {ft601_data[28]} {ft601_data[27]} {ft601_data[26]} {ft601_data[25]} {ft601_data[24]} {ft601_data[23]} {ft601_data[22]} {ft601_data[21]} {ft601_data[20]} {ft601_data[19]} {ft601_data[18]} {ft601_data[17]} {ft601_data[16]} {ft601_data[15]} {ft601_data[14]} {ft601_data[13]} {ft601_data[12]} {ft601_data[11]} {ft601_data[10]} {ft601_data[9]} {ft601_data[8]} {ft601_data[7]} {ft601_data[6]} {ft601_data[5]} {ft601_data[4]} {ft601_data[3]} {ft601_data[2]} {ft601_data[1]} {ft601_data[0]}}] 2
set_property PACKAGE_PIN V20 [get_ports {ft601_be[0]}]
set_property PACKAGE_PIN V19 [get_ports {ft601_be[1]}]
set_property PACKAGE_PIN W20 [get_ports {ft601_be[2]}]
set_property PACKAGE_PIN AB20 [get_ports {ft601_be[3]}]
set_property PACKAGE_PIN H20 [get_ports {ft601_data[0]}]
set_property PACKAGE_PIN H22 [get_ports {ft601_data[1]}]
set_property PACKAGE_PIN J21 [get_ports {ft601_data[2]}]
set_property PACKAGE_PIN J22 [get_ports {ft601_data[3]}]
set_property PACKAGE_PIN K21 [get_ports {ft601_data[4]}]
set_property PACKAGE_PIN K22 [get_ports {ft601_data[5]}]
set_property PACKAGE_PIN L21 [get_ports {ft601_data[6]}]
set_property PACKAGE_PIN N18 [get_ports {ft601_data[7]}]
set_property PACKAGE_PIN M18 [get_ports {ft601_data[8]}]
set_property PACKAGE_PIN L20 [get_ports {ft601_data[9]}]
set_property PACKAGE_PIN L19 [get_ports {ft601_data[10]}]
set_property PACKAGE_PIN M20 [get_ports {ft601_data[11]}]
set_property PACKAGE_PIN M22 [get_ports {ft601_data[12]}]
set_property PACKAGE_PIN M21 [get_ports {ft601_data[13]}]
set_property PACKAGE_PIN N22 [get_ports {ft601_data[14]}]
set_property PACKAGE_PIN N20 [get_ports {ft601_data[15]}]
set_property PACKAGE_PIN P15 [get_ports {ft601_data[16]}]
set_property PACKAGE_PIN T21 [get_ports {ft601_data[17]}]
set_property PACKAGE_PIN R16 [get_ports {ft601_data[18]}]
# OLD  ft601_data[19]
set_property PACKAGE_PIN T20 [get_ports {ft601_data[19]}]
# NEW  ft601_data[19]
#set_property PACKAGE_PIN T18  [get_ports ft601_data[19]]
set_property PACKAGE_PIN U17 [get_ports {ft601_data[20]}]
set_property PACKAGE_PIN U20 [get_ports {ft601_data[21]}]
set_property PACKAGE_PIN U21 [get_ports {ft601_data[22]}]
set_property PACKAGE_PIN V22 [get_ports {ft601_data[23]}]
set_property PACKAGE_PIN W22 [get_ports {ft601_data[24]}]
set_property PACKAGE_PIN W21 [get_ports {ft601_data[25]}]
set_property PACKAGE_PIN Y22 [get_ports {ft601_data[26]}]
set_property PACKAGE_PIN Y21 [get_ports {ft601_data[27]}]
set_property PACKAGE_PIN AB22 [get_ports {ft601_data[28]}]
set_property PACKAGE_PIN AA21 [get_ports {ft601_data[29]}]
set_property PACKAGE_PIN AB21 [get_ports {ft601_data[30]}]
set_property PACKAGE_PIN AA20 [get_ports {ft601_data[31]}]
set_property PACKAGE_PIN W17 [get_ports ft601_oe_n]
set_property PACKAGE_PIN P16 [get_ports ft601_rd_n]
set_property PACKAGE_PIN AA18 [get_ports ft601_rxf_n]
set_property PACKAGE_PIN AB18 [get_ports ft601_siwu_n]
set_property PACKAGE_PIN AA19 [get_ports ft601_txe_n]
set_property PACKAGE_PIN W19 [get_ports ft601_wr_n]
set_property PACKAGE_PIN N13 [get_ports ft601_rst_n]
set_property IOSTANDARD LVCMOS33 [get_ports ft601_txe_n]
set_property IOSTANDARD LVCMOS33 [get_ports ft601_rxf_n]
set_property IOSTANDARD LVCMOS33 [get_ports ft601_wr_n]
set_property IOSTANDARD LVCMOS33 [get_ports ft601_rd_n]
set_property IOSTANDARD LVCMOS33 [get_ports ft601_oe_n]
set_property IOSTANDARD LVCMOS33 [get_ports ft601_siwu_n]
set_property IOSTANDARD LVCMOS33 [get_ports ft601_rst_n]
set_property SLEW FAST [get_ports ft601_wr_n]
set_property SLEW FAST [get_ports ft601_rd_n]
set_property SLEW FAST [get_ports ft601_oe_n]
set_property SLEW FAST [get_ports ft601_siwu_n]
set_property SLEW FAST [get_ports ft601_rst_n]

set_property PACKAGE_PIN G21 [get_ports user_ld1_n]
set_property PACKAGE_PIN G22 [get_ports user_ld2_n]
set_property PACKAGE_PIN F19 [get_ports user_sw1_n]
set_property PACKAGE_PIN F20 [get_ports user_sw2_n]
set_property IOSTANDARD LVCMOS33 [get_ports user_ld1_n]
set_property IOSTANDARD LVCMOS33 [get_ports user_ld2_n]
set_property IOSTANDARD LVCMOS33 [get_ports user_sw1_n]
set_property IOSTANDARD LVCMOS33 [get_ports user_sw2_n]

# SYSCLK
set_property PACKAGE_PIN J19 [get_ports clk]
set_property IOSTANDARD LVCMOS33 [get_ports clk]
create_clock -period 10.000 -name net_clk -waveform {0.000 5.000} [get_ports clk]

# FT601 CLK
create_clock -period 10.000 -name net_ft601_clk -waveform {0.000 5.000} [get_ports ft601_clk]
set_property IOSTANDARD LVCMOS33 [get_ports ft601_clk]
set_property PACKAGE_PIN K18 [get_ports ft601_clk]

set_input_delay -clock [get_clocks net_ft601_clk] -min 6.500 [get_ports {ft601_data[*]}]
set_input_delay -clock [get_clocks net_ft601_clk] -max 7.000 [get_ports {ft601_data[*]}]
set_input_delay -clock [get_clocks net_ft601_clk] -min 6.500 [get_ports ft601_rxf_n]
set_input_delay -clock [get_clocks net_ft601_clk] -max 7.000 [get_ports ft601_rxf_n]
set_input_delay -clock [get_clocks net_ft601_clk] -min 6.500 [get_ports ft601_txe_n]
set_input_delay -clock [get_clocks net_ft601_clk] -max 7.000 [get_ports ft601_txe_n]

set_output_delay -clock [get_clocks net_ft601_clk] -max 1.000 [get_ports {ft601_wr_n ft601_rd_n ft601_oe_n}]
set_output_delay -clock [get_clocks net_ft601_clk] -min 4.800 [get_ports {ft601_wr_n ft601_rd_n ft601_oe_n}]
set_output_delay -clock [get_clocks net_ft601_clk] -max 1.000 [get_ports {{ft601_be[*]} {ft601_data[*]}}]
set_output_delay -clock [get_clocks net_ft601_clk] -min 4.800 [get_ports {{ft601_be[*]} {ft601_data[*]}}]


set_property IOB TRUE [get_cells i_pcileech_ft601/FT601_OE_N_reg]
set_property IOB TRUE [get_cells i_pcileech_ft601/FT601_WR_N_reg]
set_property IOB TRUE [get_cells i_pcileech_ft601/FT601_RD_N_reg]


#set_false_path -from [get_pins {tickcount64_reg[*]/C}]
#set_false_path -from [get_pins {i_pcileech_fifo/_pcie_core_config_reg[*]/C}]
#set_false_path -from [get_pins i_pcileech_pcie_a7/i_pcie_7x_0/inst/inst/user_lnk_up_int_reg/C] -to [get_pins {i_pcileech_fifo/_cmd_tx_din_reg[16]/D}]
#set_false_path -from [get_pins i_pcileech_pcie_a7/i_pcie_7x_0/inst/inst/user_reset_out_reg/C]

#PCIe signals
#set_property PACKAGE_PIN A13 [get_ports pcie_present]
#set_property PACKAGE_PIN C13 [get_ports pcie_perst_n]
#set_property PACKAGE_PIN B13 [get_ports pcie_wake_n]
#set_property IOSTANDARD LVCMOS33 [get_ports pcie_present]
#set_property IOSTANDARD LVCMOS33 [get_ports pcie_perst_n]
#set_property IOSTANDARD LVCMOS33 [get_ports pcie_wake_n]

#set_property PACKAGE_PIN C9 [get_ports {pcie_rx_n[0]}]
#set_property PACKAGE_PIN D9 [get_ports {pcie_rx_p[0]}]
#set_property PACKAGE_PIN C7 [get_ports {pcie_tx_n[0]}]
#set_property PACKAGE_PIN D7 [get_ports {pcie_tx_p[0]}]

#set_property PACKAGE_PIN E10 [get_ports pcie_clk_n]
#set_property PACKAGE_PIN F10 [get_ports pcie_clk_p]

#create_clock -name pcie_sys_clk_p -period 10.0 [get_nets pcie_clk_p]

#set_property CFGBVS Vcco [current_design]
#set_property CONFIG_VOLTAGE 3.3 [current_design]
#set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [current_design]
#set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
#set_property BITSTREAM.CONFIG.SPI_FALL_EDGE YES [current_design]
#set_property BITSTREAM.CONFIG.CONFIGRATE 66 [current_design]



set_multicycle_path -from [list i_pcileech_ft601/OE_reg/C i_pcileech_ft601/OE_reg_replica/C i_pcileech_ft601/OE_reg_replica_1/C i_pcileech_ft601/OE_reg_replica_2/C i_pcileech_ft601/OE_reg_replica_3/C i_pcileech_ft601/OE_reg_replica/C i_pcileech_ft601/OE_reg_replica_4/C i_pcileech_ft601/OE_reg_replica_5/C i_pcileech_ft601/OE_reg_replica_6/C i_pcileech_ft601/OE_reg_replica_1/C [get_pins {i_pcileech_ft601/OE_reg_replica_2/C i_pcileech_ft601/OE_reg_replica_3/C}]] -to [get_ports {ft601_data[*]}] 2

set_multicycle_path -from [list i_pcileech_ft601/OE_reg/C i_pcileech_ft601/OE_reg_replica/C i_pcileech_ft601/OE_reg_replica_1/C i_pcileech_ft601/OE_reg_replica_2/C i_pcileech_ft601/OE_reg_replica_3/C i_pcileech_ft601/OE_reg_replica_4/C i_pcileech_ft601/OE_reg_replica_5/C i_pcileech_ft601/OE_reg_replica_6/C [get_pins {i_pcileech_ft601/OE_reg_replica_3/C i_pcileech_ft601/OE_reg_replica_1/C i_pcileech_ft601/OE_reg_replica/C i_pcileech_ft601/OE_reg_replica_2/C}]] -to [get_ports {ft601_be[*]}] 2







set_multicycle_path -from [list i_pcileech_ft601/OE_reg/C i_pcileech_ft601/OE_reg_replica/C i_pcileech_ft601/OE_reg_replica_1/C i_pcileech_ft601/OE_reg_replica_2/C i_pcileech_ft601/OE_reg_replica_3/C i_pcileech_ft601/OE_reg_replica_4/C i_pcileech_ft601/OE_reg_replica_5/C i_pcileech_ft601/OE_reg_replica_6/C [get_pins {i_pcileech_ft601/OE_reg_replica_3/C i_pcileech_ft601/OE_reg_replica_1/C i_pcileech_ft601/OE_reg_replica/C i_pcileech_ft601/OE_reg_replica_2/C}]] -to [get_ports {{ft601_data[31]} {ft601_data[30]} {ft601_data[29]} {ft601_data[28]} {ft601_data[27]} {ft601_data[26]} {ft601_data[25]} {ft601_data[24]} {ft601_data[23]} {ft601_data[22]} {ft601_data[21]} {ft601_data[20]} {ft601_data[19]} {ft601_data[18]} {ft601_data[17]} {ft601_data[16]} {ft601_data[15]} {ft601_data[14]} {ft601_data[13]} {ft601_data[12]} {ft601_data[11]} {ft601_data[10]} {ft601_data[9]} {ft601_data[8]} {ft601_data[7]} {ft601_data[6]} {ft601_data[5]} {ft601_data[4]} {ft601_data[3]} {ft601_data[2]} {ft601_data[1]} {ft601_data[0]}}] 2



set_property mark_debug true [get_nets -hier [list {sine[*]}]]



create_debug_core u_ila_0 ila
set_property ALL_PROBE_SAME_MU true [get_debug_cores u_ila_0]
set_property ALL_PROBE_SAME_MU_CNT 1 [get_debug_cores u_ila_0]
set_property C_ADV_TRIGGER false [get_debug_cores u_ila_0]
set_property C_DATA_DEPTH 4096 [get_debug_cores u_ila_0]
set_property C_EN_STRG_QUAL false [get_debug_cores u_ila_0]
set_property C_INPUT_PIPE_STAGES 0 [get_debug_cores u_ila_0]
set_property C_TRIGIN_EN false [get_debug_cores u_ila_0]
set_property C_TRIGOUT_EN false [get_debug_cores u_ila_0]
set_property port_width 1 [get_debug_ports u_ila_0/clk]
connect_debug_port u_ila_0/clk [get_nets [list ft601_clk_IBUF_BUFG]]
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe0]
set_property port_width 32 [get_debug_ports u_ila_0/probe0]
connect_debug_port u_ila_0/probe0 [get_nets [list {com_rx_data32[0]} {com_rx_data32[1]} {com_rx_data32[2]} {com_rx_data32[3]} {com_rx_data32[4]} {com_rx_data32[5]} {com_rx_data32[6]} {com_rx_data32[7]} {com_rx_data32[8]} {com_rx_data32[9]} {com_rx_data32[10]} {com_rx_data32[11]} {com_rx_data32[12]} {com_rx_data32[13]} {com_rx_data32[14]} {com_rx_data32[15]} {com_rx_data32[16]} {com_rx_data32[17]} {com_rx_data32[18]} {com_rx_data32[19]} {com_rx_data32[20]} {com_rx_data32[21]} {com_rx_data32[22]} {com_rx_data32[23]} {com_rx_data32[24]} {com_rx_data32[25]} {com_rx_data32[26]} {com_rx_data32[27]} {com_rx_data32[28]} {com_rx_data32[29]} {com_rx_data32[30]} {com_rx_data32[31]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe1]
set_property port_width 1 [get_debug_ports u_ila_0/probe1]
connect_debug_port u_ila_0/probe1 [get_nets [list com_rx_valid32]]
create_debug_core u_ila_1 ila
set_property ALL_PROBE_SAME_MU true [get_debug_cores u_ila_1]
set_property ALL_PROBE_SAME_MU_CNT 1 [get_debug_cores u_ila_1]
set_property C_ADV_TRIGGER false [get_debug_cores u_ila_1]
set_property C_DATA_DEPTH 4096 [get_debug_cores u_ila_1]
set_property C_EN_STRG_QUAL false [get_debug_cores u_ila_1]
set_property C_INPUT_PIPE_STAGES 0 [get_debug_cores u_ila_1]
set_property C_TRIGIN_EN false [get_debug_cores u_ila_1]
set_property C_TRIGOUT_EN false [get_debug_cores u_ila_1]
set_property port_width 1 [get_debug_ports u_ila_1/clk]
connect_debug_port u_ila_1/clk [get_nets [list clk_IBUF_BUFG]]
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_1/probe0]
set_property port_width 32 [get_debug_ports u_ila_1/probe0]
connect_debug_port u_ila_1/probe0 [get_nets [list {core_din[0]} {core_din[1]} {core_din[2]} {core_din[3]} {core_din[4]} {core_din[5]} {core_din[6]} {core_din[7]} {core_din[8]} {core_din[9]} {core_din[10]} {core_din[11]} {core_din[12]} {core_din[13]} {core_din[14]} {core_din[15]} {core_din[16]} {core_din[17]} {core_din[18]} {core_din[19]} {core_din[20]} {core_din[21]} {core_din[22]} {core_din[23]} {core_din[24]} {core_din[25]} {core_din[26]} {core_din[27]} {core_din[28]} {core_din[29]} {core_din[30]} {core_din[31]}]]
create_debug_port u_ila_1 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_1/probe1]
set_property port_width 1 [get_debug_ports u_ila_1/probe1]
connect_debug_port u_ila_1/probe1 [get_nets [list core_din_ready]]
create_debug_port u_ila_1 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_1/probe2]
set_property port_width 1 [get_debug_ports u_ila_1/probe2]
connect_debug_port u_ila_1/probe2 [get_nets [list core_din_wr_en]]
set_property C_CLK_INPUT_FREQ_HZ 300000000 [get_debug_cores dbg_hub]
set_property C_ENABLE_CLK_DIVIDER false [get_debug_cores dbg_hub]
set_property C_USER_SCAN_CHAIN 1 [get_debug_cores dbg_hub]
connect_debug_port dbg_hub/clk [get_nets clk_IBUF_BUFG]
