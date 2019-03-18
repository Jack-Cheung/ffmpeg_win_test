#include "decoder.h"
namespace ffmpeg{

CDecoder::CDecoder(const CDemuxer& demuxer, MEDIA_TYPE intrested /*= VIDEO*/)
	:_fmt_ctx(demuxer._fmt_ctx)
{
	int ret = av_find_best_stream(_fmt_ctx, AVMediaType(intrested), -1, -1, NULL, 0);
	FC_ASSERT(ret >= 0, "could not find stream");
	_stream_id = ret;
	AVStream* stream = _fmt_ctx->streams[_stream_id];
	AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
	FC_ASSERT(codec, "fail to find codec");
	_dec_ctx = avcodec_alloc_context3(codec);
	FC_ASSERT(_dec_ctx, "fail to allocate codec context");
	FC_ASSERT(avcodec_parameters_to_context(_dec_ctx, stream->codecpar) >= 0, "fail to copy codec parameters");
	FC_ASSERT(avcodec_open2(_dec_ctx, codec, NULL) == 0, "initialize avcodecContext failed");
	int width = _dec_ctx->width;
	int height = _dec_ctx->height;
	AVPixelFormat pix_fmt = _dec_ctx->pix_fmt;
	_dst_buf_size = av_image_alloc(_dst_data, _dst_linesize, width, height, pix_fmt, 1);
}

CDecoder::~CDecoder()
{
}

bool CDecoder::Decode(CPacket& pkt, CFrame& frame)
{
	AVPacket* pavPacket = &pkt._pkt;
	AVFrame* pavFrame = frame.avframe;
	if (pavPacket->stream_index == _stream_id)
	{
		dlog("pkt.pts = ${pts}", ("pts", pavPacket->pts));
		int ret = avcodec_send_packet(_dec_ctx, pavPacket);
		if (ret == 0)
		{
			ret = avcodec_receive_frame(_dec_ctx, pavFrame);
			if (ret == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

CFrame::CFrame()
{
	avframe = av_frame_alloc();
}

CFrame::~CFrame()
{
	av_frame_free(&avframe);
}

}
