module data_bus(clk, rst, LD_SUM, LD_NEXT, SUM_SEL, NEXT_SEL, A_SEL, NEXT_ZERO, sumout);
    parameter N = 8;
    input clk, rst, LD_SUM, LD_NEXT, SUM_SEL, NEXT_SEL, A_SEL;
    wire [N-1:0] add1, add2, add3, a_sel2, add_sum, next_sel_out, sum_sel_out, addr;
    output NEXT_ZERO;
    output [N-1:0] sumout;
    
    assign sumout = add1;

    sum #(N) sum_1(add1, add2, add_sum);
    selector #(N) selector_1(8'b00000000, add_sum, SUM_SEL, sum_sel_out);
    register #(N) register_1(clk, rst, LD_SUM, sum_sel_out, add1);
    selector #(N) selector_2(8'b00000000, add2, NEXT_SEL, next_sel_out);
    compare #(N) compare_mod(next_sel_out, NEXT_ZERO);
    register #(N) register_2(clk, rst, LD_NEXT, next_sel_out, add3);
    sum #(N) sum_2(8'b00000001, add3, a_sel2);
    selector #(N) selector_3(add3, a_sel2, A_SEL, addr);
    memory #(N) memory_mod(clk, addr, add2);

endmodule // data_bus