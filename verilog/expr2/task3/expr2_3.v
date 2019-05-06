`timescale 1ns / 1ps

module _7Seg_Driver_Choice(SW, SEG, AN, LED);
    input [15:0]SW;
    output [7:0]SEG;
    output [7:0]AN;
    output [15:0]LED;

    assign LED[15:0] = SW[15:0];
    
    decoder(SW, SEG);
    selector(SW, AN);
    

endmodule // _7Seg_Driver_Choice