module selector(input1, input2, select, out);
    parameter N = 8;
    input [N-1:0] input1;
    input [N-1:0] input2;
    input select;
    output reg [N-1:0] out;
    always @(*) begin
       if (select == 0) begin
           out = input1;
       end else begin
           out = input2;
       end 
    end

endmodule // selector