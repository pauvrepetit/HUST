module counter(clk, out);
    input clk;
    output reg [2:0] out;

    always @(posedge clk) begin
       if (out == 3'b111) begin
           out = 3'b000;
       end
       else begin
           out = out + 1;
       end
    end

endmodule // counter