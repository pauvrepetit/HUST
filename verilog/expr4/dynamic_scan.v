module dynamic_scan(clk, data, SEG, AN);
    input clk;

    output [7:0] SEG;
    output [7:0] AN;
    output [7:0] data;

    wire clk_N;
    wire [2:0] num;
    wire [3:0] out [7:0];
    
    assign out[1] = data / 0'b1010;
    assign out[0] = data % 0'b1010;
    assign out[2] = 15;
    assign out[3] = 15;
    assign out[4] = 15;
    assign out[5] = 15;
    assign out[6] = 15;
    assign out[7] = 15;
    
    divider #(100_000)(clk, clk_N);
    
    counter(clk_N, num);
    decoder3_8(num, AN);        //选择位置
    pattern(out[num], SEG);         //选择输出内容
endmodule // dynamic_scan