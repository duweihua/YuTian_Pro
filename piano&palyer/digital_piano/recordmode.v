`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    15:50:33 06/30/2013 
// Design Name: 
// Module Name:    recordmode 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module recordmode(clk_5MHz, record, replay, ps2_asci, record_asci);
input clk_5MHz;                 	//��������ʱ���ź�
input record;							//¼������
input replay;							//�طſ���
input [7:0] ps2_asci;				//����ļ��̰�����asciii��ֵ
output reg [7:0] record_asci;		//��replayΪ��1��ʱ������Ĳ����ļ�ֵ

reg [25:0] count_for_16Hz;			//��Ƶ������
reg clk_16Hz;

reg [7:0] mem [127:0]; 				//�洢����ֵ���ڴ�����
reg [6:0] count;						//�洢������

//���16Hz�Ĳ���Ƶ��
always@(posedge clk_5MHz)
begin
	if(count_for_16Hz==156250)
		begin
			count_for_16Hz <= 26'd0;
			clk_16Hz <= ~clk_16Hz;
		end
	else
		begin
			count_for_16Hz <= count_for_16Hz + 26'b1;
		end
end

//��recordΪ��1��ʱ������������ļ�ֵ���뵽mem�ڴ�������
always@(posedge clk_16Hz)
begin
	if(record == 1'b1)
	begin
		count <= count + 7'b1;
		mem[count] <= ps2_asci;
	end
end

//���洢�ļ�ֵ���
always@(count)
begin
	record_asci <= mem[count];
end
endmodule
