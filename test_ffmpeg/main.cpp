#include "decoder.h"
#include "demuxer.hpp"
extern "C" {
#include <libavformat/avformat.h>
}
#include <fc/exception/exception.hpp>
#include <fc/log/logger.hpp>
using namespace ffmpeg;
using namespace fc;
int main(int argc, char** argv)
{
	try
	{
		CPacket pkt;
		CDemuxer demuxer("D:\\FFOutput\\20190307.mov");
		CDecoder decoder(demuxer);
		CFrame frame(decoder);
		decoder.SetFilter(1).SetFilter(2).SetFilter(62);
		//function<void(CFrame&)> func = [](CFrame& f) { wlog("test"); };
		//decoder.SetProcessFunc(func);
		while (demuxer.LoopPacket(pkt))
		{
			decoder.Decode(pkt, frame);
		}
	}
	catch (fc::exception& e)
	{
		elog("[${name}]: ${what}", ("name", e.name())("what", e.top_message()));
	}
	catch (...)
	{
		elog("other exception");
	}
	system("pause");
	return 0;
}