`timescale 1ns / 1ps

module frq(clk, rst_n, clk_25m);

input clk;
input rst_n;
output clk_25m;

reg cnt;
reg clk_25m;

always@ (posedge clk or negedge rst_n)
begin
	if(!rst_n)
		begin
			cnt <= 1'b0;
			clk_25m <= 1'b0;
		end
	else if(cnt < 1'd0)
		cnt <= cnt + 1'b1;
	else
		begin
		cnt <= 1'b0;
		clk_25m <= ~clk_25m;
		end
end

endmodule
