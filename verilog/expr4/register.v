module register(clk, rst, load, D, Q);
    parameter N = 8;
    input clk, rst, load;
    input [N-1:0] D;
    output reg [N-1:0] Q;

    always @(posedge clk) begin
        if (rst == 1) begin
            Q <= 0;
        end else if(load == 1) begin
            Q <= D;
        end
    end

endmodule // register