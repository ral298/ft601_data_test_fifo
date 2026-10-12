/*module ft601_loopback (
    input  wire        clk,
    input  wire        reset,

    // Desde pcileech_ft601 (PC → FPGA)
    input  wire [31:0] din_data,
    input  wire        din_valid,

    // Hacia pcileech_ft601 (FPGA → PC)
    output reg  [31:0] dout_data,
    output reg         dout_valid,
    input  wire        dout_req_data
);
    parameter fin=256;
	wire [7:0]byte3to0 = din_data[7:0];
	wire [7:0]byte2to1 = din_data[15:8];
	wire [7:0]byte1to2 = din_data[23:16];
	wire [7:0]byte0to3 = din_data[31:24];
    // -------------------------------------------------
    // FIFO 10 x 32
    // -------------------------------------------------
    reg [31:0] fifo_mem [0:fin-1];
    reg [8:0]  wr_ptr;
    reg [8:0]  rd_ptr;
    reg [9:0]  fifo_count;  // 0..10

    wire fifo_full  = (fifo_count == fin);
    wire fifo_empty = (fifo_count == 0);
    integer i_mem;
    always @(posedge clk or posedge reset) begin
        if (reset) begin
            wr_ptr     <= 9'h0;
            rd_ptr     <= 9'h0;
            fifo_count <= 10'h0;
            dout_data  <= 32'd0;
            dout_valid <= 1'b0;
            for (i_mem = 1; i_mem < fin; i_mem = i_mem + 1) begin
            fifo_mem [i_mem]<=32'd0;
            
            end
        end else begin

            // -----------------------------------------
            // WRITE: entra 1 dato por ciclo
            // -----------------------------------------
            if (din_valid && !fifo_full) begin
                fifo_mem[wr_ptr] <= din_data;
                wr_ptr <= wr_ptr + 1;//(fifo_full) ? 0 : wr_ptr + 1;
                fifo_count <=fifo_count + 1; //(fifo_full) ? 0 :fifo_count + 1;
                rd_ptr <=0;
            end

            // -----------------------------------------
            // READ: sale 1 dato por ciclo
            // -----------------------------------------
            if (dout_req_data && !fifo_empty) begin
                dout_data  <= fifo_mem[rd_ptr];
                dout_valid <= 1'b1;
                rd_ptr <= rd_ptr + 1;//(rd_ptr == 9) ? 0 : rd_ptr + 1;
                wr_ptr <=(wr_ptr==0)  ? 0: ((rd_ptr == fin) ? 0 :wr_ptr - 1);
                fifo_count <= fifo_count - 1;
            end else begin
                dout_valid <= 1'b0;
            end

        end
    end

endmodule
*/


module ft601_loopback (
    input  wire        clk,
    input  wire        reset,

    // Desde pcileech_ft601 (PC -> FPGA)
    input  wire [31:0] din_data,
    input  wire        din_valid,

    // Hacia pcileech_ft601 (FPGA -> PC)
    output reg  [31:0] dout_data,
    output reg         dout_valid,
    input  wire        dout_req_data
);

    // 2048 palabras x 32 bits = 8192 bytes = 8 KiB
    parameter integer fin = 2048;

    localparam integer PTR_WIDTH =
        (fin <= 2) ? 1 : 12;

    localparam integer COUNT_WIDTH = PTR_WIDTH +1 ;

    reg [31:0] fifo_mem [0:fin-1];

    reg [PTR_WIDTH-1:0] wr_ptr;
    reg [PTR_WIDTH-1:0] rd_ptr;
    reg [COUNT_WIDTH-1:0] fifo_count;

    wire fifo_full  = (fifo_count == fin);
    wire fifo_empty = (fifo_count == 0);

    // Acepta lectura solo si hay datos.
    wire read_fire = dout_req_data && !fifo_empty;

    // Si está llena pero se lee en este ciclo,
    // también se puede aceptar una escritura.
    wire write_fire = din_valid && (!fifo_full || read_fire);

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            wr_ptr     <= 0;
            rd_ptr     <= 0;
            fifo_count <= 0;
            dout_data  <= 0;
            dout_valid <= 0;
        end else begin

            dout_valid <= 1'b0;

            // ESCRITURA
            if (write_fire) begin
                fifo_mem[wr_ptr] <= din_data;

                if (wr_ptr == fin - 1)
                    wr_ptr <= 0;
                else
                    wr_ptr <= wr_ptr + 1'b1;
            end

            // LECTURA
            if (read_fire) begin
                dout_data  <= fifo_mem[rd_ptr];
                dout_valid <= 1'b1;

                if (rd_ptr == fin - 1)
                    rd_ptr <= 0;
                else
                    rd_ptr <= rd_ptr + 1'b1;
            end

            // Actualización del número de palabras almacenadas
            case ({write_fire, read_fire})
                2'b10: fifo_count <= fifo_count + 1'b1;
                2'b01: fifo_count <= fifo_count - 1'b1;
                default: fifo_count <= fifo_count;
            endcase

        end
    end

endmodule
