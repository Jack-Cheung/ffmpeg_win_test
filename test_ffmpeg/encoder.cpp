#include "encoder.hpp"

namespace ffcv
{
	CEncoder::CEncoder(CContext& ctx)
		:_ctx(ctx)
	{
		//avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);

	}

	void CEncoder::Encode(CFrame& frame, CPacket& pkt)
	{
		if (pkt._pkt.stream_index != _ctx._istream_id)
			return;
		/*AVFrame* avFrame = frame.avframe;
		avFrame->format = _ctx._ec_ctx->pix_fmt;
		avFrame->width = _ctx._ec_ctx->width;
		avFrame->height = _ctx._ec_ctx->height;*/
		
		dlog("frame.format=${fmt}, frame.width=${w}, frame.height=${h}", ("fmt", frame.avframe->format)("w", frame.avframe->width)("h", frame.avframe->height));
		int ret = avcodec_send_frame(_ctx._ec_ctx, frame.avframe);
		if (ret < 0) {
			fprintf(stderr, "Error sending a frame for encoding\n");
		}
		//int ret;
		//frame.SetPayload().ConvertColor().MarkFace()/*.CreateSnapShot()*/;
		{
			ret = avcodec_receive_packet(_ctx._ec_ctx, &pkt._pkt);

			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				return;
			else if (ret < 0) {
				fprintf(stderr, "Error during encoding\n");
			}

			printf("Write packet %3lld (size=%5d)\n", pkt._pkt.pts, pkt._pkt.size);
			//fwrite(pkt->data, 1, pkt->size, outfile);
			av_packet_unref(&pkt._pkt);
		}
	}

	CEncoder::~CEncoder()
	{
	}
}

