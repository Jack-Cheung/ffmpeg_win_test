extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}
#include <iostream>
#include <string>
#include <fstream>
using std::string;
using std::cout;
using std::endl;
#define ASSERT(expression, log) \
	do{\
		if (!(expression))\
		{\
			throw log;\
		}\
	}while(false)
	
int main(int argc, char** argv)
{
	ASSERT(argc == 2, "usage: executable_file source_video_file");
	const char* src_file_name = argv[1];
	AVFormatContext* fmt_ctx = NULL;
	ASSERT(avformat_open_input(&fmt_ctx, src_file_name, NULL, NULL)>=0, "fail to open input file");
	ASSERT(avformat_find_stream_info(fmt_ctx, NULL) >=0, "could not find stream info");
	int ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	ASSERT(ret >= 0, "could not find stream");
	int stream_idx = ret;
	AVStream* video_stream = fmt_ctx->streams[stream_idx];
	AVCodec* codec = avcodec_find_decoder(video_stream->codecpar->codec_id);
	ASSERT(codec, "fail to find codec");
	AVCodecContext* dec_ctx = avcodec_alloc_context3(codec);
	ASSERT(dec_ctx, "fail to allocate codec context");
	ASSERT(avcodec_parameters_to_context(dec_ctx, video_stream->codecpar) >= 0, "fail to copy codec parameters");
	avcodec_open2(dec_ctx, codec, NULL);
	int width = dec_ctx->width;
	int height = dec_ctx->height;
	AVPixelFormat pix_fmt = dec_ctx->pix_fmt;
	uint8_t * video_dst_data[4];
	int video_dst_linesize[4];
	int video_dst_buf_size = av_image_alloc(video_dst_data, video_dst_linesize, width, height, pix_fmt, 1);


	AVFrame * frame = av_frame_alloc();
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;
	pkt.size = 0;
	int got_frame;
	int video_frame = 0;
	while (av_read_frame(fmt_ctx, &pkt) >= 0)
	{
		if (pkt.buf == NULL)
		{
			continue;
		}
		AVPacket origin_pkt = pkt;

		if (pkt.stream_index == stream_idx)
		{
			ret = avcodec_send_packet(dec_ctx, &pkt);
			while (ret >= 0)
			{
				ret = avcodec_receive_frame(dec_ctx, frame);
				cout << "ret = " << ret << endl;
				if (AVERROR(EAGAIN) == ret || AVERROR_EOF == ret)
				{
					break;
				}
				av_image_copy(video_dst_data, video_dst_linesize, (const uint8_t **)frame->data, frame->linesize, pix_fmt, width, height);
				std::fstream  fs;
				fs.open("test1", std::iostream::out);
				fs.write((const char*)video_dst_data[0], video_dst_buf_size);
				fs.close();
			}
		}
		pkt.data = NULL;
		pkt.size = 0;
	}

	cout << "error eagain" << AVERROR(EAGAIN) << endl;
	cout << "error eof" << AVERROR_EOF << endl;
	cout << "error einval" << AVERROR(EINVAL) << endl;



	av_freep(&video_dst_data[0]);
	/*for (int i = 0; i < 4 ; i++)
	{
		cout << video_dst_linesize[]
	}*/

}