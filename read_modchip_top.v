// Group 2: Raj Patel, Zachary Rouviere, Evan Waxman
// Experiement 8 Part 1
// 11/1/21

// Description:
//	This module will turn on the Green LEDs or RED LEDs depending on what key is recived from
// the microprocessor. The FPGA will take 1 byte of EEPROM data from the microprocessor and determines
// if it is the full-functional key or not. If it is the full-functional key then all of the Green
// LEDs on the HaHa board will light up. If a limited key is detected then all of the Red LEDs on the 
// HaHa board will light up. Additionally regardless if the key is full-functional or limited the value 
// recieved by the mircoprocessor will be stored into a RAM enitiy to be viewed in quartus.   

`timescale 1ns/1ns

module  read_modchip_top (
	input 				clk,
	input				rst,
	input		[7:0]	uprocessor_din,
	input				uprocessor_wren,
	output reg	[7:0]	led_out
);

localparam correct_key = 8'h00; // Change correct key here
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
		led_out <= #1 8'h00; // leds off
	end else begin
		if (key == correct_key) begin
			led_out <= 8'h0F; // Green LEDS ON
		end else begin
			led_out <= 8'hF0; // Red LEDs ON
		end
	end
end

endmodule