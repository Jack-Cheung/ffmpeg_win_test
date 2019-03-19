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
	_process_img_func = [&](CFrame& frame) {
		frame.GenerateImage("");
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
				vector<int>::const_iterator  itr;
				if (!seconds.empty() && (itr = find_if(seconds.cbegin(), seconds.cend(), [&](auto elem)->bool {
					return elem * av_q2d(av_inv_q(_fmt_ctx->streams[_stream_id]->time_base)) == pavPacket->pts;
				})) != end(seconds))
				{
					if (_process_img_func)
					{
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


CFrame::CFrame( CDecoder& decoder)
{
	avframe = av_frame_alloc();
	_width = decoder._dec_ctx->width;
	_height = decoder._dec_ctx->height;
	_pix_fmt = decoder._dec_ctx->pix_fmt;
	_buf_size = av_image_alloc(_data, _linesize, _width, _height, _pix_fmt, 1);
}

CFrame::~CFrame()
{
	av_frame_free(&avframe);
}

void CFrame::GenerateImage(fc::path p)
{
	//TODO
	av_image_copy(_data, _linesize, (const uint8_t **)avframe->data, avframe->linesize, _pix_fmt, _width, _height);
	std::fstream  fs;
	std::string file_name = "test" + std::to_string(avframe->coded_picture_number) + ".i420";
	fs.open(file_name, std::iostream::out);
	fs.write((const char*)_data[0], _buf_size);
	fs.close();
}

}
