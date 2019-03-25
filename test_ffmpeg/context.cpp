#pragma once
#include "context.hpp"

namespace ffcv
{
	CContext::CContext(const string& ifile_path, const string& ofile_path)
		:_ifile_path(ifile_path),
		_ofile_path(ofile_path)
	{
		FC_ASSERT(fc::exists(_ifile_path) , "input file does not exist");
		if (avformat_open_input(&_ifmt_ctx, _ifile_path.preferred_string().c_str(), NULL, NULL) < 0)
		{
			FC_THROW("fail to open input file: ${input_file}", ("input_file", _ifile_path.filename()));
		}
		if (avformat_find_stream_info(_ifmt_ctx, NULL) < 0)
		{
			FC_THROW("could not find stream info");
		}
		FC_ASSERT(_ifmt_ctx);
		int ret = av_find_best_stream(_ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);//TODO
		FC_ASSERT(ret >= 0, "could not find stream");
		_istream_id = ret;
		_istream = _ifmt_ctx->streams[_istream_id];
		_decodec = avcodec_find_decoder(_istream->codecpar->codec_id);
		FC_ASSERT(_decodec, "fail to find codec");
		_dec_ctx = avcodec_alloc_context3(_decodec);
		FC_ASSERT(_dec_ctx, "fail to allocate codec context");
		FC_ASSERT(avcodec_parameters_to_context(_dec_ctx, _istream->codecpar) >= 0, "fail to copy codec parameters");
		FC_ASSERT(avcodec_open2(_dec_ctx, _decodec, NULL) == 0, "initialize avcodecContext failed");
		dlog("the pix_fmt = { ${fmt}  }", ("fmt", av_get_pix_fmt_name(_dec_ctx->pix_fmt)));

		avformat_alloc_output_context2(&_ofmt_ctx, NULL, NULL, _ofile_path.preferred_string().c_str());
		FC_ASSERT(_ofmt_ctx, "Could not create output context");
		_ostream = avformat_new_stream(_ofmt_ctx, NULL);
		FC_ASSERT(_ostream, "Failed allocating output stream");
		dlog("ostream id=${idx}", ("idx", _ostream->index));
		FC_ASSERT(avcodec_parameters_copy(_ostream->codecpar, _istream->codecpar) >= 0, "Failed to copy codec parameters");
		av_dump_format(_ofmt_ctx, 0, _ofile_path.preferred_string().c_str(), 1);
		av_dump_format(_ifmt_ctx, 0, _ifile_path.preferred_string().c_str(), 0);

		//_encodec = avcodec_find_encoder(_ostream->codecpar->codec_id);
		//if (!_encodec) {
		//	fprintf(stderr, "Codec not found\n");
		//}

		//_ec_ctx = avcodec_alloc_context3(_encodec);
		//if (!_ec_ctx) {
		//	fprintf(stderr, "Could not allocate video codec context\n");
		//}
		////_ec_ctx->time_base = _dec_ctx->time_base;
		//_ec_ctx->time_base = AVRational{ 1, 25 };
		//_ec_ctx->pix_fmt = _dec_ctx->pix_fmt;
		//_ec_ctx->width = _dec_ctx->width;
		//_ec_ctx->height = _dec_ctx->height;
		//dlog("timebase=${n}/${d}", ("n", _ec_ctx->time_base.num)("d", _ec_ctx->time_base.den));
		//ret = avcodec_open2(_ec_ctx, _encodec, NULL);
		//if (ret < 0) {
		//	fprintf(stderr, "Could not open codec"/*, av_err2str(ret)*/);
		//}
	}

}

