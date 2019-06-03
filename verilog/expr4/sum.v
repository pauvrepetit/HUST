module sum(add1, add2, sum);
    parameter N = 8;
    input [N-1:0] add1;
    input [N-1:0] add2;
    output reg [N-1:0] sum;

    always @(add1, add2) begin
        sum = add1 + add2;
    end

endmodule // sum