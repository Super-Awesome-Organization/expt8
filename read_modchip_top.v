// Group 2: Raj Patel, Zachary Rouviere, Evan Waxman
// Experiement 7 Part 1
// 10/24/21

// Description:
//	This module takes the SRAM bytes data from the microprocesser
// and stores it into a ram enity, so that the values can be seen 
// in the in system content memory editor window. A total of 64 bytes are 
// transfered to the FPGA

`timescale 1ns/1ns

module  read_modchip_top (
	input 				clk,
	input				rst,
	input		[7:0]	uprocessor_din,
	input				uprocessor_wren,
	output reg	[7:0]	led_out
);

localparam correct_key = 8'h00;
wire [7:0] 	key;

ram	RAM (
	.address ( 5'h00 ),
	.clock ( clk ),
	.data ( uprocessor_din ),
	.wren ( uprocessor_wren ),
	.q ( key )
);


always @(posedge clk) begin
	if (rst) begin
		led_out <= #1 8'h00;
	end else begin
		if (key == correct_key) begin
			led_out <= 8'h0F;
		end else begin
			led_out <= 8'hF0;
		end
	end
end

endmodule