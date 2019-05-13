module upper(clk, out);
    input clk;
    output [2:0] out;
    wire clk_N;
    
    divider(clk, clk_N);
    counter(clk_N, out);
    
endmodule // clk  