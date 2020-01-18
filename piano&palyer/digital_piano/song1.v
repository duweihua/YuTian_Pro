`timescale 1ns / 1ps

//song1��ģ�� ����ֻ�ϻ���
module song1(clk_5MHz,clk_4Hz,beep);	

input clk_5MHz,clk_4Hz;				       //ϵͳʱ��,50Mhz
output beep;					   //�����������

reg  [3:0] high, med, low;
reg  [15:0] origin;
reg	beep_r;					   //�Ĵ���
reg  [7:0] state;					   
reg  [15:0] count;			


assign beep = beep_r;	          //�������

always @(posedge clk_5MHz)
begin
	count <= count + 1'b1;					  //��������1
	if(count == origin)
	begin
		count <= 16'h0;					      //����������
		beep_r <= !beep_r;				      //���ȡ��
	end
end

always@(posedge clk_4Hz)
begin
	case({high,med,low})
		12'b000000010000:	origin=11466;//mid1
		12'b000000100000:	origin=10216;//mid2
		12'b000000110000:	origin=9101;//mid3
		12'b000001000000:	origin=8590;//mid4
		12'b000001010000:	origin=7653;//mid5
		12'b000001100000:	origin=6818;//mid6
		12'b000000000101:	origin=14447;//low5
	endcase
end


always @(posedge clk_4Hz)
begin
	if(state ==63) state = 0;//��ʱ����ʵ��ѭ������
	else
		state = state + 1'b1;
	case(state)
		0,1:						      {high,med,low}=12'b000000010000;//mid1
		2,3:							   {high,med,low}=12'b000000100000;//mid2
		4,5:							   {high,med,low}=12'b000000110000;//mid3
		6,7:							   {high,med,low}=12'b000000010000;//mid1
		
		8,9:						      {high,med,low}=12'b000000010000;//mid1
		10,11:							{high,med,low}=12'b000000100000;//mid2
		12,13:							{high,med,low}=12'b000000110000;//mid3
		14,15:							{high,med,low}=12'b000000010000;//mid1

		16,17:							{high,med,low}=12'b000000110000;//mid3
		18,19:		 					{high,med,low}=12'b000001000000;//mid4
		20,21,22,23: 					{high,med,low}=12'b000001010000;//mid5
		
		24,25:							{high,med,low}=12'b000000110000;//mid3
		26,27:		 					{high,med,low}=12'b000001000000;//mid4
		28,29,30,31: 					{high,med,low}=12'b000001010000;//mid5
		
		32:								{high,med,low}=12'b000001010000;//mid5
		33:		                  {high,med,low}=12'b000001100000;//mid6
		34:								{high,med,low}=12'b000001010000;//mid5
		35:								{high,med,low}=12'b000001000000;//mid4
		36,37:							{high,med,low}=12'b000000110000;//mid3
		38,39:							{high,med,low}=12'b000000010000;//mid1

		40:								{high,med,low}=12'b000001010000;//mid5
		41:		                  {high,med,low}=12'b000001100000;//mid6
		42:								{high,med,low}=12'b000001010000;//mid5
		43:								{high,med,low}=12'b000001000000;//mid4
		44,45:							{high,med,low}=12'b000000110000;//mid3
		46,47:							{high,med,low}=12'b000000010000;//mid1
		
		48,49:							{high,med,low}=12'b000000100000;//mid2
		50,51:							{high,med,low}=12'b000000000101;//low5
		52,53,54,55:					{high,med,low}=12'b000000010000;//mid1
		
		56,56:							{high,med,low}=12'b000000100000;//mid2
		57,58:							{high,med,low}=12'b000000000101;//low5
		59,60,61,62,63:		      {high,med,low}=12'b000000010000;//mid1
		default: 						{high,med,low}=12'bx;
	endcase
end


endmodule
