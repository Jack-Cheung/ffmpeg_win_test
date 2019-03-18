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
		CFrame frame;
		CDemuxer demuxer("D:\\FFOutput\\20190307.mov");
		CDecoder decoder(demuxer);
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
	return 0;
}