`timescale 1ns / 1ps

module song(clk_5MHz, select, beep);

input clk_5MHz;
input select;				       
output beep;

reg  [25:0] cnt2;
reg  clk_5Hz;
wire  beep_r;
wire out3;
wire clk;

assign beep = beep_r;

/* 《天空之城》 */
song3 m3(.clk_5MHz(clk_5MHz),.clk_4Hz(clk),.select(select),.beep(out3));

assign beep_r = out3;

assign clk = clk_5Hz;

/* 5hz分频 */
always@(posedge clk_5MHz)			
begin    
//	if(select)
//	begin
		if(cnt2<25'd400000)
			cnt2 <= cnt2+25'b1;
		else
			begin
			cnt2<=25'b0;
			clk_5Hz <= ~clk_5Hz;
			end
//	end
end

endmodule















