#include "demuxer.hpp"

namespace ffmpeg{

CDemuxer::CDemuxer()
{

}

CDemuxer::CDemuxer(const std::string& src_file_path):_src_file_path(src_file_path)
{
	if (!fc::exists(_src_file_path))
	{
		FC_THROW_EXCEPTION(fc::file_not_found_exception,
			"could not open file ${file}", ("file", _src_file_path.generic_string()));
	}
	if (avformat_open_input(&_fmt_ctx, _src_file_path.preferred_string().c_str(), NULL, NULL) < 0)
	{
		FC_THROW("fail to open input file: ${input_file}", ("input_file", _src_file_path.filename()));
	}
	if (avformat_find_stream_info(_fmt_ctx, NULL) < 0)
	{
		FC_THROW("could not find stream info");
	}
	FC_ASSERT(_fmt_ctx);
}

CDemuxer::~CDemuxer()
{
	avformat_close_input(&_fmt_ctx);
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
	if (av_read_frame(_fmt_ctx, pavPacket) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

CPacket::CPacket()
{
	av_init_packet(&_pkt);
	_pkt.data = NULL;
	_pkt.size = 0;
}

CPacket::CPacket(const CPacket& pkt)
{
	av_packet_ref(&_pkt, &(pkt._pkt));
}

CPacket::~CPacket()
{
	av_packet_unref(&_pkt);
}

}

