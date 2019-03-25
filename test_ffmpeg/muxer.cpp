#include "muxer.hpp"

namespace ffcv
{
	CMuxer::CMuxer(CContext& ctx)
		:_ctx(ctx)
	{
		dlog("has nofile flag = ${f}",("f", _ctx._ofmt_ctx->flags & AVFMT_NOFILE));
		if (!(_ctx._ofmt_ctx->flags & AVFMT_NOFILE)) {
			int ret = avio_open(&_ctx._ofmt_ctx->pb, _ctx._ofile_path.preferred_string().c_str(), AVIO_FLAG_WRITE);
			if (ret < 0) {
				fprintf(stderr, "Could not open output file '%s'", _ctx._ofile_path.preferred_string().c_str());
			}
		}
		FC_ASSERT(avformat_write_header(_ctx._ofmt_ctx, NULL) >= 0, "Error occurred when opening output file");
	}

	CMuxer::~CMuxer()
	{
		av_write_trailer(_ctx._ofmt_ctx);
	}

	void CMuxer::WritePacket(CPacket& pkt)
	{
		if (av_read_frame(_ctx._ifmt_ctx, &pkt._pkt) < 0)
		{
			wlog("read frame < 0");
			return;
		}
		AVStream* out_stream = _ctx._ofmt_ctx->streams[pkt._pkt.stream_index];
		if (!out_stream)
		{
			wlog("no out stream");
			return;
		}
		/* copy packet */
		pkt._pkt.pts = av_rescale_q_rnd(pkt._pkt.pts, _ctx._istream->time_base, _ctx._ostream->time_base, AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt._pkt.dts = av_rescale_q_rnd(pkt._pkt.dts, _ctx._istream->time_base, _ctx._ostream->time_base, AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt._pkt.duration = av_rescale_q(pkt._pkt.duration, _ctx._istream->time_base, _ctx._ostream->time_base);
		pkt._pkt.pos = -1;
		FC_ASSERT(av_interleaved_write_frame(_ctx._ofmt_ctx, &pkt._pkt) >= 0, "Error muxing packet");
		av_packet_unref(&pkt._pkt);//TODO
	}

}

