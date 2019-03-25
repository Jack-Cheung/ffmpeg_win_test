#include "decoder.hpp"

namespace ffcv{

//CDecoder::CDecoder(const CDemuxer& demuxer, MEDIA_TYPE intrested /*= VIDEO*/)
//	:_fmt_ctx(demuxer._fmt_ctx)
//{
//	int ret = av_find_best_stream(_fmt_ctx, AVMediaType(intrested), -1, -1, NULL, 0);
//	FC_ASSERT(ret >= 0, "could not find stream");
//	_stream_id = ret;
//	AVStream* stream = _fmt_ctx->streams[_stream_id];
//	AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
//	FC_ASSERT(codec, "fail to find codec");
//	_dec_ctx = avcodec_alloc_context3(codec);
//	FC_ASSERT(_dec_ctx, "fail to allocate codec context");
//	FC_ASSERT(avcodec_parameters_to_context(_dec_ctx, stream->codecpar) >= 0, "fail to copy codec parameters");
//	FC_ASSERT(avcodec_open2(_dec_ctx, codec, NULL) == 0, "initialize avcodecContext failed");
//	dlog("the pix_fmt = { ${fmt}  }", ("fmt", av_get_pix_fmt_name(_dec_ctx->pix_fmt)));
//	_process_img_func = [&](CFrame& frame) {
//		frame.SetPayload().ConvertColor().MarkFace().CreateSnapShot();
//	};
//}

CDecoder::CDecoder(CContext& ctx)
	:_ctx(ctx)
{
	_process_img_func = [&](CFrame& frame) {
			frame.SetPayload().ConvertColor().MarkFace()/*.CreateSnapShot()*/;
	};
}

CDecoder::~CDecoder()
{
}

CDecoder& CDecoder::SetFilter(int second)
{
	seconds.push_back(second);
	return *this;
}

void CDecoder::SetProcessFunc(function<void(CFrame&)>& func)
{
	_process_img_func = func;
}

bool CDecoder::Decode(CPacket& pkt, CFrame& frame)
{
	static uint32_t  frame_counter = 0;
	AVPacket* pavPacket = &pkt._pkt;
	AVFrame* pavFrame = frame.avframe;
	if (pavPacket->stream_index == _ctx._istream_id)
	{
		auto on_exit = fc::make_scoped_exit([&pavFrame]() {
			av_frame_unref(pavFrame);
		});//todo
		wlog("pkt.pts = ${pts} duration=${dur} time_base=${base}", ("pts", pavPacket->pts)("dur",pavPacket->duration)("base",av_q2d(_ctx._ifmt_ctx->streams[_ctx._istream_id]->time_base)));
		int ret = avcodec_send_packet(_ctx._dec_ctx, pavPacket);
		if (ret == 0)
		{
			ret = avcodec_receive_frame(_ctx._dec_ctx, pavFrame);
			if (ret == 0)
			{
				frame_counter++;
				//vector<int>::const_iterator  itr;
				//if (!seconds.empty() && (itr = find_if(seconds.cbegin(), seconds.cend(), [&](auto elem)->bool {
				//	/*return elem * av_q2d(av_inv_q(_fmt_ctx->streams[_stream_id]->time_base)) == pavPacket->pts;*/
				//	return elem == frame_counter;
				//})) != end(seconds))
				{
					if (_process_img_func)
					{
						wlog("hello!");
						_process_img_func(frame);
					}
					return true;
				}
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



}