module memory(clk, read_addr, data);
    parameter Data_Width = 8;
    input clk;
    input [Data_Width - 1 : 0] read_addr;
    output reg [Data_Width-1:0] data;
    reg [Data_Width-1 : 0] ram [15 : 0];

    initial begin
        $readmemh("D:/ram_init.txt", ram);
    end

    always @(read_addr) begin
        data <= ram[read_addr];
    end

endmodule // memory