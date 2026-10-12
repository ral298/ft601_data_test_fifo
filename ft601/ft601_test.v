// Copyright (C) 2025  Altera Corporation. All rights reserved.
// Your use of Altera Corporation's design tools, logic functions 
// and other software and tools, and any partner logic 
// functions, and any output files from any of the foregoing 
// (including device programming or simulation files), and any 
// associated documentation or information are expressly subject 
// to the terms and conditions of the Altera Program License 
// Subscription Agreement, the Altera Quartus Prime License Agreement,
// the Altera IP License Agreement, or other applicable license
// agreement, including, without limitation, that your use is for
// the sole purpose of programming logic devices manufactured by
// Altera and sold by Altera or its authorized distributors.  Please
// refer to the Altera Software License Subscription Agreements 
// on the Quartus Prime software download page.

// *****************************************************************************
// This file contains a Verilog test bench template that is freely editable to  
// suit user's needs .Comments are provided in each section to help the user    
// fill out necessary details.                                                  
// *****************************************************************************
// Generated on "01/13/2026 21:32:52"
                                                                                
// Verilog Test Bench template for design : ft601
// 
// Simulation tool : Questa Altera FPGA (Verilog)
// 

`timescale 1 ps/ 1 ps
module ft601_vlg_tst();
// constants                                           
// general purpose registers

// test vector input registers
reg clk;
reg ft601_clk;
reg [31:0] treg_ft601_data;
reg ft601_rxf_n;
reg ft601_txe_n;
reg user_sw1_n;
reg user_sw2_n;
// wires                                               
wire [3:0]  ft601_be;
wire [31:0]  ft601_data;
wire ft601_oe_n;
wire ft601_rd_n;
wire ft601_rst_n;
wire ft601_siwu_n;
wire ft601_wr_n;
wire user_ld1_n;
wire user_ld2_n;
integer i_rag;
// assign statements (if any)                          
assign ft601_data = treg_ft601_data;
ft601 i1 (
// port map - connection between master ports and signals/registers   
	.clk(ft601_clk),
	.ft601_clk(ft601_clk),
	.ft601_be(ft601_be),
	.ft601_data(ft601_data),
	.ft601_oe_n(ft601_oe_n),
	.ft601_rd_n(ft601_rd_n),
	.ft601_rst_n(ft601_rst_n),
	.ft601_rxf_n(ft601_rxf_n),
	.ft601_siwu_n(ft601_siwu_n),
	.ft601_txe_n(ft601_txe_n),
	.ft601_wr_n(ft601_wr_n),
	.user_ld1_n(user_ld1_n),
	.user_ld2_n(user_ld2_n),
	.user_sw1_n(user_sw1_n),
	.user_sw2_n(user_sw2_n)
);
initial                                                
begin                                                  
// code that executes only once                        
// insert code here --> begin                          
                                                       
// --> end                                             
$display("Running testbench");    
ft601_clk=0;    
clk=1;               
end    


integer i;
    reg [31:0] test_values [0:9];

    initial begin
        // Datos que enviará el PC
        /*
        test_values[0] = 32'hDEADBEEF;
        test_values[1] = 32'h12345678;
        test_values[2] = 32'hCAFEBABE;
        test_values[3] = 32'hA5A5A5A5;
        test_values[4] = 32'hFACEB00C;
        test_values[5] = 32'hF00DBABE;
        test_values[6] = 32'hCCDDEEFF;
        test_values[7] = 32'h87654321;
        test_values[8] = 32'h11112222;
        test_values[9] = 32'hCAFED00D;
        */
        
        
        test_values[0] = 32'h0;
        test_values[1] = 32'h1;
        test_values[2] = 32'h2;
        test_values[3] = 32'h3;
        test_values[4] = 32'h4;
        test_values[5] = 32'h5;
        test_values[6] = 32'h6;
        test_values[7] = 32'h7;
        test_values[8] = 32'h8;
        test_values[9] = 32'h9;
    end
	 initial begin
        $display("=== INICIANDO SIMULACIÓN ===");
		ft601_rxf_n = 1;
		ft601_txe_n = 1;
		treg_ft601_data=32'h0;
         user_sw2_n = 0;
		#50 ;
		user_sw2_n = 1;
		#10;
		#700;
        // Simular envío desde PC hacia FPGA
        ft601_rxf_n = 0;  // decir "hay dato disponible"

        
        
		  treg_ft601_data = test_values[0];
        $display("PC -> FPGA  Enviando: %h", ft601_data);
        #50;
        for (i = 1; i < 10; i = i + 1) begin
				treg_ft601_data = test_values[i];
				$display("PC -> FPGA  Enviando: %h", ft601_data);
				#10;
			end
		  
		  
		  
        ft601_rxf_n = 1; // ya no hay más datos
			#200;
			
			treg_ft601_data=32'hz;
			#100;
			//#136000000;
        //-----------------------------------------------------
        //  Simular recepción desde FPGA hacia PC
        //-----------------------------------------------------
        
		  
		  /*
		  ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #575;// para todos los datos son 135, dejare 4 sin leer entonces 95
		  
        ft601_txe_n = 1;  // ya no acepto más datos
			
		  #20;
		  */
		  
		  /*
		  ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #135;// para todos los datos son 135, dejare 4 sin leer entonces 95
		  
        ft601_txe_n = 1;  // ya no acepto más datos
			
		  #20;
		  */
		  
		  
		  
		  /*
		  ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #95;// para todos los datos son 125, dejare 3 sin leer entonces 95
		  
        ft601_txe_n = 1;  // ya no acepto más datos
			
		  #40;
			
			
			
			ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #60;//son 30 para pasar los wait, hasta el 40 ya se lee el dato, 60 para leer los 3 datos faltantes.
		  
        ft601_txe_n = 1;  // ya no acepto más datos
        #40;
        
		  
		  ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #60;//son 30 para pasar los wait, hasta el 40 ya se lee el dato, 60 para leer los 3 datos faltantes.
		  
        ft601_txe_n = 1;  // ya no acepto más datos
        #40;
		  
		*/  
		  
		    ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #95;// para todos los datos son 125, dejare 3 sin leer entonces 95
		  
        ft601_txe_n = 1;  // ya no acepto más datos
			
		  #40;
			
			
			
			ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #60;//son 30 para pasar los wait, hasta el 40 ya se lee el dato, 60 para leer los 3 datos faltantes.
		  
        ft601_txe_n = 1;  // ya no acepto más datos
        #100;
        
		  
		  ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #60;//son 30 para pasar los wait, hasta el 40 ya se lee el dato, 60 para leer los 3 datos faltantes.
		  
        ft601_txe_n = 1;  // ya no acepto más datos
        #40;
		  
		  
		  
		  for (i_rag = 1; i_rag < 2; i_rag = i_rag + 1) begin
		  test_values[0] = $urandom;
        test_values[1] = $urandom;
        test_values[2] = 32'h2;
        test_values[3] = 32'h3;
        test_values[4] = 32'h4;
        test_values[5] = 32'h5;
        test_values[6] = 32'h6;
        test_values[7] = 32'h7;
        test_values[8] = $urandom;
        test_values[9] = $urandom;
        #200;
		  // Simular envío desde PC hacia FPGA
        ft601_rxf_n = 0;  // decir "hay dato disponible"

        
        
		  treg_ft601_data = test_values[0];
        $display("PC -> FPGA  Enviando: %h", ft601_data);
        #50;
        for (i = 1; i < 10; i = i + 1) begin
				treg_ft601_data = test_values[i];
				$display("PC -> FPGA  Enviando: %h", ft601_data);
				#10;
			end
		  
		  
		  
        ft601_rxf_n = 1; // ya no hay más datos
			#200;
			
			treg_ft601_data=32'hz;
			#200;
		      ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #95;// para todos los datos son 125, dejare 3 sin leer entonces 95
		  
        ft601_txe_n = 1;  // ya no acepto más datos
			
		  #40;
			
			
			
			ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #60;//son 30 para pasar los wait, hasta el 40 ya se lee el dato, 60 para leer los 3 datos faltantes.
		  
        ft601_txe_n = 1;  // ya no acepto más datos
        #100;
        
		  
		  ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #60;//son 30 para pasar los wait, hasta el 40 ya se lee el dato, 60 para leer los 3 datos faltantes.
		  
        ft601_txe_n = 1;  // ya no acepto más datos
        #200;
        
        end
        
        
        
        
        
        
        
        
        
        
        test_values[0] = $urandom;
        test_values[1] = $urandom;
        test_values[2] = 32'h2;
        test_values[3] = 32'h3;
        test_values[4] = 32'h4;
        test_values[5] = 32'h5;
        test_values[6] = 32'h6;
        test_values[7] = 32'h7;
        test_values[8] = $urandom;
        test_values[9] = $urandom;
        #200;
		  // Simular envío desde PC hacia FPGA
        ft601_rxf_n = 0;  // decir "hay dato disponible"

        
        
		  treg_ft601_data = test_values[0];
        $display("PC -> FPGA  Enviando: %h", ft601_data);
        #50;
        for (i = 1; i < 4; i = i + 1) begin
				treg_ft601_data = test_values[i];
				$display("PC -> FPGA  Enviando: %h", ft601_data);
				#10;
			end
		  
		  
		  
        ft601_rxf_n = 1; // ya no hay más datos
			#200;
			
			treg_ft601_data=32'hz;
			#200;
		      ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #95;// para todos los datos son 125, dejare 3 sin leer entonces 95
		  
        ft601_txe_n = 1;  // ya no acepto más datos
			
		  #40;
			
			
			
			ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #60;//son 30 para pasar los wait, hasta el 40 ya se lee el dato, 60 para leer los 3 datos faltantes.
		  
        ft601_txe_n = 1;  // ya no acepto más datos
        #100;
        
		  
		  ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #60;//son 30 para pasar los wait, hasta el 40 ya se lee el dato, 60 para leer los 3 datos faltantes.
		  
        ft601_txe_n = 1;  // ya no acepto más datos
        #200;
		  /*
		  ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #95;// para todos los datos son 125, dejare 3 sin leer entonces 95
		  
        ft601_txe_n = 1;  // ya no acepto más datos
			
		  #200;
			
			
			
			ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #60;//son 30 para pasar los wait, hasta el 40 ya se lee el dato, 60 para leer los 3 datos faltantes.
		  
        ft601_txe_n = 1;  // ya no acepto más datos
        #40;
        
		  
		  ft601_txe_n = 0;  // permitir al FPGA mandar datos

        #60;//son 30 para pasar los wait, hasta el 40 ya se lee el dato, 60 para leer los 3 datos faltantes.
		  
        ft601_txe_n = 1;  // ya no acepto más datos
        #40;
		  */
		  
		  $stop();
    end


    //---------------------------------------------------------
    //  Monitor de datos que la FPGA envía de regreso al PC
    //---------------------------------------------------------
    always 
	 begin
    #5 ft601_clk=!ft601_clk;     
	 clk=!clk;
    end


                                                
endmodule

