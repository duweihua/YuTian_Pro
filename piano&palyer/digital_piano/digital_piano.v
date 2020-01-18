`timescale 1ns / 1ps

module digital_piano(inclk, out_r, out_l, select, key_in);

input  inclk; 			//选用50M晶振
input  select;			//音乐暂停
input [3:0] key_in;	//按键（琴键） 

output out_r, out_l;	//左右声道：音乐播放 and 电子琴
 
reg  clk_5MHz;			//时钟频率5MHz
reg  [2:0] cnt;  

/* 电子琴 */
piano m1(.inclk(inclk), .clk_5MHz(clk_5MHz), .key_in(key_in), .beep(out_r)); 							

/* 音乐播放 */
song m2(.clk_5MHz(clk_5MHz), .select(select), .beep(out_l));

/* 10分频：50MHz到5Mhz分频 */
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












