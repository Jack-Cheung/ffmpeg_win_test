#include "controller.hpp"

namespace ffcv
{
	CController::CController(const CConfig& conf)
		:_conf(conf)
	{

	}

	void CController::Process()
	{
		//read config
		CContext ctx("D:\\FFOutput\\IMG_1859.mov", "D:\\kyo.mov");
		//CPacket pkt;
		//CDemuxer demuxer(ctx);
		//CDecoder decoder(ctx);
		//CFrame frame(ctx);
		//decoder.SetFilter(800).SetFilter(1000).SetFilter(300);//TODO
	
		//CMuxer muxer(ctx);//write header
		//CEncoder encoder(ctx);
		//while (demuxer.LoopPacket(pkt))
		//{
		//	frame.avframe->width = 358;
		//	frame.avframe->height = 480;
		//	frame.avframe->format = AV_PIX_FMT_YUV420P;
		//	fprintf(stdout, "1frame.data=%p  frame.format=%d \n", frame.avframe->data, frame.avframe->format);
		//	decoder.Decode(pkt, frame);
		//	fprintf(stdout, "2frame.data=%p  frame.format=%d \n", frame.avframe->data, frame.avframe->format);
		////TODO　DoSomething
		//	//frame.SetPayload().ConvertColor().MarkFace();
		//	//wlog("frame.data=${d}", ("d", fc::to_string(avframe->data).));

		//	fprintf(stdout, "3frame.data=%p  frame.format=%d \n", frame.avframe->data, frame.avframe->format);
		//	encoder.Encode(frame, pkt);
		//	muxer.WritePacket(pkt);//TODO 视频画质变差很多
		//}
		//muxer.Close();
	}
}


