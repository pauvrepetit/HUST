module dynamic_scan(clk, choose, SEG, AN);
    input clk;
    input [2:0] choose;

    output [7:0] SEG;
    output [7:0] AN;

    wire clk_N;
    wire [2:0] num;
    wire [3:0] data;
    
    divider #(50_000_000)(clk, clk_N);
    
    counter(clk_N, choose, num);

    decoder3_8(num, AN);
    rom8x4(num, data);
    pattern(data, SEG);

endmodule // dynamic_scan