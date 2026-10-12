//
// PCILeech FPGA.
//
// Top module for various 75T-484 x1 Artix-7 boards.
//
// (c) Ulf Frisk, 2019-2024
// Author: Ulf Frisk, pcileech@frizk.net
//

//`timescale 1ns / 1ps
//`include "pcileech_header.svh"

module ft601 /*#(
    parameter       PARAM_DEVICE_ID = 9,
    parameter       PARAM_VERSION_NUMBER_MAJOR = 4,
    parameter       PARAM_VERSION_NUMBER_MINOR = 14,
    parameter       PARAM_CUSTOM_VALUE = 32'hffffffff
)*/ 
(
    // SYS
    input           clk,
    input           ft601_clk,
    
    // SYSTEM LEDs and BUTTONs
    output          user_ld1_n,
    output          user_ld2_n,
    input           user_sw1_n,
    input           user_sw2_n,
    
    // PCI-E FABRIC
    /*
	 output  [0:0]   pcie_tx_p,
    output  [0:0]   pcie_tx_n,
    input   [0:0]   pcie_rx_p,
    input   [0:0]   pcie_rx_n,
    input           pcie_clk_p,
    input           pcie_clk_n,
    input           pcie_present,
    input           pcie_perst_n,
    output reg      pcie_wake_n = 1'b1,
    */
    // TO/FROM FT601 PADS
    output          ft601_rst_n,
    
    inout   [31:0]  ft601_data,
    output  [3:0]   ft601_be,
    input           ft601_rxf_n,
    input           ft601_txe_n,
    output          ft601_wr_n,
    output          ft601_siwu_n,
    output          ft601_rd_n,
    output          ft601_oe_n
    );
    
    // SYS
    wire            rst;
    
	 
	 /*
    // FIFO CTL <--> COM CTL
    wire [63:0]     com_dout;
    wire            com_dout_valid;
    wire [255:0]    com_din;
    wire            com_din_wr_en;
    wire            com_din_ready;
    wire            led_com;
    wire            led_pcie;
    
    // FIFO CTL <--> COM CTL
    IfComToFifo     dcom_fifo();
	
    // FIFO CTL <--> PCIe
    IfPCIeFifoCfg   dcfg();
    IfPCIeFifoTlp   dtlp();
    IfPCIeFifoCore  dpcie();
    IfShadow2Fifo   dshadow2fifo();
	 
    // ----------------------------------------------------
    // TickCount64 CLK
    // ----------------------------------------------------
    */
    time tickcount64 = 0;
    //time tickcount64_reload = 0;
    always @ ( posedge clk ) begin
        tickcount64         <= user_sw2_n ? (tickcount64 + 1) : 0;
        //tickcount64_reload  <= user_sw2_n ? 0 : (tickcount64_reload + 1);
    end
	 
    assign rst =  ((tickcount64 < 64) ? 1'b1 : 1'b0);
    assign ft601_rst_n = ~rst;
    wire led_pwronblink;
    
    assign led_pwronblink= (tickcount64[26] == 0);
    (* mark_debug = "true" *) wire        com_rx_valid32;
    //assign user_ld1_n= ~com_rx_valid32;
    OBUF led_ld1_obuf(.O(user_ld1_n), .I(led_pwronblink));
    
    // ----------------------------------------------------
    // BUFFERED COMMUNICATION DEVICE (FT601)
    // ----------------------------------------------------
    (* mark_debug = "true" *) wire [31:0] com_rx_data32;
    
	(* mark_debug = "true" *) wire [31:0] core_din;
    (* mark_debug = "true" *) wire        core_din_wr_en;
    (* mark_debug = "true" *) wire        core_din_ready;
	 reg [31:0] core_din_n;
    reg        core_din_wr_en_n;
	 
	 always @(posedge ft601_clk,posedge rst)
	 begin
		if(rst) begin
		core_din_n<=32'h0;
		core_din_wr_en_n<=1'h0;	
		
		end
		else
		begin
		core_din_n<=core_din;
		core_din_wr_en_n<=core_din_wr_en;
		end
	 end
	 
	
	 
    pcileech_ft601 i_pcileech_ft601(
        // SYS
        .clk                ( ft601_clk             ),
        //.clk_com            ( ft601_clk             ),
        .rst                ( rst                   ),
        //.led_state_txdata   ( led_com               ),  // ->
        //.led_state_invert   ( led_pwronblink        ),  // <-
        // FIFO CTL <--> COM CTL
        //.dfifo              ( dcom_fifo.mp_com      ),
        // TO/FROM FT601 PADS
        /*.ft601_data         ( ft601_data            ),  // <> [31:0]
        .ft601_be           ( ft601_be              ),  // -> [3:0]
        .ft601_txe_n        ( ft601_txe_n           ),  // <-
        .ft601_rxf_n        ( ft601_rxf_n           ),  // <-
        .ft601_siwu_n       ( ft601_siwu_n          ),  // ->
        .ft601_wr_n         ( ft601_wr_n            ),  // ->
        .ft601_rd_n         ( ft601_rd_n            ),  // ->
        .ft601_oe_n         ( ft601_oe_n            )   // ->*/
		  .FT601_DATA         ( ft601_data            ),
        .FT601_BE           ( ft601_be              ),
        .FT601_TXE_N        ( ft601_txe_n           ),
        .FT601_RXF_N        ( ft601_rxf_n           ),
        .FT601_SIWU_N       ( ft601_siwu_n          ),
        .FT601_WR_N         ( ft601_wr_n            ),
        .FT601_RD_N         ( ft601_rd_n            ),
        .FT601_OE_N         ( ft601_oe_n            ),
        // TO/FROM FIFO
        .dout               ( com_rx_data32         ),  // -> [31:0]
        .dout_valid         ( com_rx_valid32        ),  // ->        
        .din                ( core_din_n              ),  // <- [31:0]
        .din_wr_en          ( core_din_wr_en_n        ),  // <-
        .din_req_data       ( core_din_ready        )   // ->  //solicita que le pasen un siguiente dato, la fifo tendra que ddar el siguiente dato al ser solicitado. 
    );
	 
	 
	 ft601_loopback loop (
        .clk        (ft601_clk),
        .reset      (rst),

        .din_data   (com_rx_data32),
        .din_valid  (com_rx_valid32),
        //.din_req_data(din_req_data),

        .dout_data  (core_din),
        .dout_valid (core_din_wr_en),
        .dout_req_data(core_din_ready)
    );
    
    // ----------------------------------------------------
    // FIFO CTL
    // ----------------------------------------------------
    /*
    pcileech_fifo #(
        .PARAM_DEVICE_ID            ( PARAM_DEVICE_ID               ),
        .PARAM_VERSION_NUMBER_MAJOR ( PARAM_VERSION_NUMBER_MAJOR    ),
        .PARAM_VERSION_NUMBER_MINOR ( PARAM_VERSION_NUMBER_MINOR    ),
        .PARAM_CUSTOM_VALUE         ( PARAM_CUSTOM_VALUE            )
    ) i_pcileech_fifo (
        .clk                ( clk                   ),
        .rst                ( rst                   ),
        .rst_cfg_reload     ( (tickcount64_reload > 500000000) ? 1'b1 : 1'b0 ),     // config reload after 5s button press
        .pcie_present       ( pcie_present          ),
        .pcie_perst_n       ( pcie_perst_n          ),
        // FIFO CTL <--> COM CTL
        .dcom               ( dcom_fifo.mp_fifo     ),
        // FIFO CTL <--> PCIe
        .dcfg               ( dcfg.mp_fifo          ),
        .dtlp               ( dtlp.mp_fifo          ),
        .dpcie              ( dpcie.mp_fifo         ),
        .dshadow2fifo       ( dshadow2fifo.fifo     )
    );
    
    // ----------------------------------------------------
    // PCIe
    // ----------------------------------------------------
    
    pcileech_pcie_a7 i_pcileech_pcie_a7(
        .clk_sys            ( clk                   ),
        .rst                ( rst                   ),
        // PCIe fabric
        .pcie_tx_p          ( pcie_tx_p             ),
        .pcie_tx_n          ( pcie_tx_n             ),
        .pcie_rx_p          ( pcie_rx_p             ),
        .pcie_rx_n          ( pcie_rx_n             ),
        .pcie_clk_p         ( pcie_clk_p            ),
        .pcie_clk_n         ( pcie_clk_n            ),
        .pcie_perst_n       ( pcie_perst_n          ),
        // State and Activity LEDs
        .led_state          ( led_pcie              ),
        // FIFO CTL <--> PCIe
        .dfifo_cfg          ( dcfg.mp_pcie          ),
        .dfifo_tlp          ( dtlp.mp_pcie          ),
        .dfifo_pcie         ( dpcie.mp_pcie         ),
        .dshadow2fifo       ( dshadow2fifo.shadow   )
    );
	*/
	reg con0;
	reg con1;
	reg con2;
	reg con0_led;
	reg con1_led;
	reg stated_led;
	reg stated_con_led;
	reg [9:0]mem_status;
	reg [3:0]con_mem_status;
	reg [3:0]con_mem_status_led;
	always @(posedge ft601_clk,posedge rst)
	 begin
		if(rst) begin
            con0<=1'h0;
            con1<=1'h0;
            con2<=1'h0;	
            stated_led<=1'h0;
            stated_con_led<=1'h0;
            con_mem_status<= 4'h0;
            con_mem_status_led<=4'h0;
            con1_led<=1'h1;
            con0_led<=1'h1;
            mem_status<=10'h0;
		end
		else
		begin
            con0<=ft601_rxf_n;
            con1<=con0;
            con2<=con1;
            if ((!con0) && (!con1) &&(con2))begin
              stated_led<=!stated_led;
            end
            
            if (ft601_rd_n)begin
                mem_status[con_mem_status]<=ft601_data[8'h1];
                
                if(con_mem_status==4'd9)
                    con_mem_status<= 4'h0;
                else
                    con_mem_status<=con_mem_status+4'h1;
            end
		  
		    con0_led<=led_pwronblink;
		    con1_led<=con0_led;
		    
		    if (con0_led && !(con1_led))begin
		      stated_con_led<=mem_status[con_mem_status_led];
		      if(con_mem_status_led==4'd9)
                    con_mem_status_led<= 4'h0;
                else
                    con_mem_status_led<=con_mem_status_led+4'h1;
		    end
		      
		  
		  
		  
		  
		  
		  
		  
		end
	 
	end
	
	OBUF led_ld2_obuf(.O(user_ld2_n), .I(stated_led));//stated_led sirve para saber si se manda un dato
	//
	
	
	
endmodule
