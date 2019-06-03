`timescale 1ns/1ps

module control(clk, rst, start, next_zero, LD_SUM, LD_NEXT, SUM_SEL, NEXT_SEL, A_SEL, done);
    input clk, rst, start, next_zero;
    output reg LD_SUM, LD_NEXT, SUM_SEL, NEXT_SEL, A_SEL, done;

    parameter Start = 0;
    parameter Compute_Sum = 1;
    parameter Get_Next = 2;
    parameter Done = 3;

    reg [1:0] state, nextState;

    always @(start, state, next_zero) begin
        case (state)
            Start: begin
                LD_SUM <= 0;
                LD_NEXT <= 0;
                SUM_SEL <= 0;
                NEXT_SEL <= 0;
                A_SEL <= 0;
                done <= 0;
                if (start == 0) begin
                    nextState <= Start;
                end else begin
                    nextState <= Compute_Sum;
                end
            end
            Compute_Sum: begin
                LD_SUM <= 1;
                LD_NEXT <= 0;
                SUM_SEL <= 1;
                NEXT_SEL <= 1;
                A_SEL <= 1;
                done <= 0;
                nextState <= Get_Next;
            end
            Get_Next: begin
                LD_SUM <= 0;
                LD_NEXT <= 1;
                SUM_SEL <= 1;
                NEXT_SEL <= 1;
                A_SEL <= 0;
                done <= 0;
                if (next_zero == 0) begin
                    nextState <= Compute_Sum;
                end else begin
                    nextState <= Done;
                end
            end
            Done: begin
                LD_SUM <= 0;
                LD_NEXT <= 0;
                SUM_SEL <= 0;
                NEXT_SEL <= 0;
                A_SEL <= 0;
                done <= 1;
                if (start == 0) begin
                    nextState <= Start;
                end else begin
                    nextState <= Done;
                end
            end
        endcase
    end

    always @(posedge clk) begin
        if (rst == 1) begin
            state <= Start;
        end else begin
            state <= nextState;
        end
    end

endmodule // control