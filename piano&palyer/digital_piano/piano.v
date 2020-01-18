`timescale 1ns / 1ps

//piano��ģ��
module piano(inclk, clk_5MHz, key_in, beep);

input  inclk;
input  clk_5MHz;
input [3:0] key_in;
output beep;  

wire carry;
reg beep_r;
reg [13:0] count;
reg [13:0] origin;	
	
assign beep = beep_r;	//�������

/* ����ת��������� */
always @ (posedge clk_5MHz)
	begin
		case (key_in[3:0])
			/* ��������ʱ������ */
			4'b0000: origin<=0000;
			
			4'b0001: origin<=6826;   // Hdo
			4'b0010: origin<=7871;   // Hre
			4'b0100: origin<=8798;   // Hmi
			4'b1000: origin<=9224;   // Hfa
			
			default: origin<=0000;
			
		endcase
	end

assign carry=(count == 16383);

/* ��ȡ�����ź� */
always @(posedge clk_5MHz)
begin  
	if(key_in[3:0] == 4'b0000)
		begin
		end
	else
		if(carry)
			count = origin;
		else 
			count = count + 1;
end 

/* ���������� */
always @(posedge carry)
begin 
	beep_r<=~beep_r;
end 

endmodule














