`timescale 1ns / 1ps

module selector(SW, AN);
    input [15:0]SW;
    output reg [7:0]AN;

    always begin
         case (SW[15:13])
               3'b000: AN = ~8'b00000001;
               3'b001: AN = ~8'b00000010;
               3'b010: AN = ~8'b00000100;
               3'b011: AN = ~8'b00001000;
               3'b100: AN = ~8'b00010000;
               3'b101: AN = ~8'b00100000;
               3'b110: AN = ~8'b01000000;
               3'b111: AN = ~8'b10000000;
           endcase
    end
endmodule
