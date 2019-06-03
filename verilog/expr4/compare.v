module compare(in, next_zero);
    parameter N = 8;
    input [N-1:0] in;
    output reg next_zero;

    always @(in) begin
        if (in == 0) begin
            next_zero = 1;
        end else begin
            next_zero = 0;
        end
    end

endmodule // compare