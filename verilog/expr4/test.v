`timescale 1ns/1ps

module test1();
//    reg clk, rst, start, next_zero;
//    wire LD_SUM, LD_NEXT, SUM_SEL, NEXT_SEL, A_SEL, done;

//    control dut(clk, rst, start, next_zero, LD_SUM, LD_NEXT, SUM_SEL, NEXT_SEL, A_SEL, done);
    reg clk, rst, start;
    wire done;
    wire [7:0] sum;

    compute #(8) dut(clk, rst, start, done, sum);

    always begin
        clk <= 0;
        #10;
        clk <= 1;
        #10;
    end // Ê±ÖÓ

    initial begin
        rst <= 1;
        start <= 0;
        @(posedge clk);
        @(posedge clk);
        #5 rst <= 0;
        @(posedge clk);
        #5 start <= 1;
    end

endmodule // test