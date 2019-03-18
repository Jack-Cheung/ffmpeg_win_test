#pragma once
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}
#include <fc/exception/exception.hpp>
#include <fc/log/logger.hpp>
#include "demuxer.hpp"
namespace ffmpeg{

struct CFrame;
class CDecoder
{
public:
	enum MEDIA_TYPE
	{
		VIDEO = AVMEDIA_TYPE_VIDEO,
		AUDIO = AVMEDIA_TYPE_AUDIO,
	};
	CDecoder(const CDemuxer& demuxer, MEDIA_TYPE intrested = VIDEO);
	~CDecoder();
	//bool Decode(AVPacket* pkt, AVFrame* frame);
	bool Decode(CPacket& pkt, CFrame& frame);
private:
	AVFormatContext* _fmt_ctx = NULL;
	AVCodecContext* _dec_ctx = NULL;
	AVPacket _pkt;
	int _stream_id = -1;
	uint8_t * _dst_data[4]{0};
	int _dst_linesize[4]{0};
	int _dst_buf_size = -1;
};

struct CFrame
{
public:
	CFrame();
	~CFrame();

	AVFrame* avframe = NULL;
	friend class CDecoder;
	friend class CDemuxer;
};


}

