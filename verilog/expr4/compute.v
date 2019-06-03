`timescale 1ns / 1ps
module compute(clk, rst, start, done, sum, SEG, AN);
    parameter N = 8;
    input clk, rst, start;
    output done;
    output [N-1:0] sum;
    output [7:0] SEG;
    output [7:0] AN;
    
    wire next_zero, LD_SUM, LD_NEXT, SUM_SEL, NEXT_SEL, A_SEL;
    wire clk_N, clk_M;
    
    divider divider_clk(clk, clk_N);
    
    control control_mod(clk_N, rst, start, next_zero, LD_SUM, LD_NEXT, SUM_SEL, NEXT_SEL, A_SEL, done);
    data_bus #(N) databus_mod(clk_N, rst, LD_SUM, LD_NEXT, SUM_SEL, NEXT_SEL, A_SEL, next_zero, sum);
    
    dynamic_scan(clk, sum, SEG, AN);
endmodule
