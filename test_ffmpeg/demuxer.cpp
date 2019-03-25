#include "demuxer.hpp"

namespace ffcv{

CDemuxer::CDemuxer(CContext& context)
	:_ctx(context)
{

}

CDemuxer::~CDemuxer()
{
	//avformat_close_input(&_fmt_ctx);
}

bool CDemuxer::LoopPacket(CPacket& pkt)
{
	AVPacket* pavPacket = &pkt._pkt;
	if (pavPacket == NULL)
	{
		av_init_packet(pavPacket);
		pavPacket->data = NULL;
		pavPacket->size = 0;
	}
	if (av_read_frame(_ctx._ifmt_ctx, pavPacket) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
}
