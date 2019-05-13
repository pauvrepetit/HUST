module counter(clk, choose, out);
    input clk;
    input [2:0] choose;
    output reg [2:0] out;

    always @(posedge clk) begin
       if (out == 3'b111 && choose == 3'b001) begin
           out = 3'b000;
       end
       else if(out == 3'b000 && choose == 3'b100) begin
           out = 3'b111;
       end
       else if(choose == 3'b001) begin
            out <= out + 1;
       end
       else if(choose == 3'b100) begin
           out <= out - 1;
       end
       else begin
           ;
       end
    end

endmodule // counter