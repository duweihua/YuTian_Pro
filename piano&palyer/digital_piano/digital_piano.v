`timescale 1ns / 1ps

module digital_piano(inclk, out_r, out_l, select, key_in);

input  inclk; 			//ѡ��50M����
input  select;			//������ͣ
input [3:0] key_in;	//�������ټ��� 

output out_r, out_l;	//�������������ֲ��� and ������
 
reg  clk_5MHz;			//ʱ��Ƶ��5MHz
reg  [2:0] cnt;  

/* ������ */
piano m1(.inclk(inclk), .clk_5MHz(clk_5MHz), .key_in(key_in), .beep(out_r)); 							

/* ���ֲ��� */
song m2(.clk_5MHz(clk_5MHz), .select(select), .beep(out_l));

/* 10��Ƶ��50MHz��5Mhz��Ƶ */
always@(posedge inclk)				
begin    
	if(cnt<3'd5)
    	cnt <= cnt + 3'b1;
    else
		begin
		cnt <= 3'b0;
		clk_5MHz <= ~clk_5MHz;
		end
end

endmodule












