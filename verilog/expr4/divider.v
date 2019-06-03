`timescale 1ns / 1ps
module divider(clk, clk_N);
    input clk;
    output reg clk_N;
    parameter N = 100_000_000;

    reg [31:0] counter;

    always @(posedge clk) begin
        if (counter == (N >> 1) - 1) begin
            counter <= 32'b0;
            clk_N = ~clk_N;
        end else begin
            counter <= counter + 1;
        end
    end

endmodule