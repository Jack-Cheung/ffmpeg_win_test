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
		CPacket pkt;
		CDemuxer demuxer(ctx);
		CDecoder decoder(ctx);
		CFrame frame(ctx);
		decoder.SetFilter(800).SetFilter(1000).SetFilter(300);//TODO
	
		CMuxer muxer(ctx);//write header
		CEncoder encoder(ctx);
		while (demuxer.LoopPacket(pkt))
		{
			decoder.Decode(pkt, frame);
		//TODO　DoSomething
			encoder.Encode(frame, pkt);
			muxer.WritePacket(pkt);//TODO 视频画质变差很多
		}
		//muxer.Close();
	}
}


