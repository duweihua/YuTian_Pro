`timescale 1ns / 1ps

//song2��ģ�顶������衷
module song2(clk_5MHz,clk_4Hz,beep);

input	 clk_5MHz,clk_4Hz;				       //ϵͳʱ��,50Mhz
output beep;					   //�����������

reg    [3:0] high, med, low;
reg    [15:0] origin;
reg	 beep_r;					   //�Ĵ���
reg    [7:0] state;					   
reg    [15:0] count;			 

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
		'b000000000001:origin=22900; //��1
		'b000000000010:origin=20408; //��2
		'b000000000011:origin=18181; //��3
		'b000000000101:origin=15267; //��5
		'b000000000110:origin=13605; //��6
		'b000000010000:origin=11472; //��1
		'b000000100000:origin=10216; //��2
		'b000000110000:origin=9101;  //��3
		'b000001010000:origin=7653;  //��5
		'b000001100000:origin=6818;  //��6
		'b000100000000:origin=5733;  //��1
		'b001000000000:origin=5108;  //��2
		'b001100000000:origin=4551;  //��3
	endcase
end

always @(posedge clk_4Hz)	
begin
	if(state ==103) 
		state = 0;
	else
		state = state + 1'b1;                   //��������衷
	case(state)
			0,1:			  		{high,med,low}='b000000110000;//��3
			2,3:			  		{high,med,low}='b000001010000;//��5
			4,5:			  		{high,med,low}='b000001100000;//��6
			6:				  		{high,med,low}='b000001100000;//��6
			7:                {high,med,low}='b000001010000;//��5
			8,9,10:			  	{high,med,low}='b000001100000;//��6
			11:				  	{high,med,low}='b000000110000;//��3
			12,13,14,15:      {high,med,low}='b000000100000;//��2	
			16,17:			  	{high,med,low}='b000000110000;//��3
			18,19:		 	  	{high,med,low}='b000001010000;//��5
			20,21:            {high,med,low}='b000001100000;//��6
			22:				  	{high,med,low}='b000001100000;//��6
			23:			      {high,med,low}='b000001010000;//��5
			24,25:		      {high,med,low}='b000001100000;//��6
			26,27,28,29,30,31:{high,med,low}='b000000110000;//��3
			32,33:			  	{high,med,low}='b000000110000;//��3
			34,35:			  	{high,med,low}='b000001010000;//��5
			36,37:			  	{high,med,low}='b000001100000;//��6
			38:				  	{high,med,low}='b000001100000;//��6
			39:               {high,med,low}='b000001010000;//��5
			40,41,42:		  	{high,med,low}='b000001100000;//��6
			43:				  	{high,med,low}='b000000110000;//��3
			44,45,46,47:      {high,med,low}='b000000100000;//��2
			48,49:			  	{high,med,low}='b000000000101;//��5
			50,51:			  	{high,med,low}='b000000110000;//��3
			52:				  	{high,med,low}='b000000100000;//��2
			53:				  	{high,med,low}='b000000110000;//��3
			54:				  	{high,med,low}='b000000100000;//��2
			55:			      {high,med,low}='b000000000111;//1
			56,57:			  	{high,med,low}='b000000100000;//��2
			58,59,60,61,62,63:{high,med,low}='b000000000110;//��6
			64,65:			  	{high,med,low}='b000001100000;//��6
         66,67,68,69,70,71:{high,med,low}='b000000100000;//��2
			72,73:			  	{high,med,low}='b000000000101;//��5
			74,75,76,77,78,79:{high,med,low}='b000000110000;//��3
			80:               {high,med,low}='b000000100000;//��2
			81:				  	{high,med,low}='b000000000111;//1
			82,83,84,85,86,87:{high,med,low}='b000001100000;//��6
			88,89:			  	{high,med,low}='b000000000101;//��5
			90,91:			  	{high,med,low}='b000000110000;//��3
			92:	            {high,med,low}='b000000100000;//��2
			93:			      {high,med,low}='b000000110000;//��3
			94:				  	{high,med,low}='b000000100000;//��2
			95:				  	{high,med,low}='b000000000111;//1
			96,97:	         {high,med,low}='b000000100000;//��2
			98,99,100,101,102,103:{high,med,low}='b000001100000;//��6
	endcase
end


endmodule
