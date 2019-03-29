#pragma once
#include "context.hpp"

namespace ffcv
{
	CContext::CContext(const string& ifile_path, const string& ofile_path)
		:_ifile_path(ifile_path),
		_ofile_path(ofile_path)
	{
		OpenInputFile();
		OpenOutputFile();
		//FC_ASSERT(fc::exists(_ifile_path) , "input file does not exist");
		//if (avformat_open_input(&_ifmt_ctx, _ifile_path.preferred_string().c_str(), NULL, NULL) < 0)
		//{
		//	FC_THROW("fail to open input file: ${input_file}", ("input_file", _ifile_path.filename()));
		//}
		//if (avformat_find_stream_info(_ifmt_ctx, NULL) < 0)
		//{
		//	FC_THROW("could not find stream info");
		//}
		//FC_ASSERT(_ifmt_ctx);
		//int ret = av_find_best_stream(_ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);//TODO
		//FC_ASSERT(ret >= 0, "could not find stream");
		//_istream_id = ret;
		//_istream = _ifmt_ctx->streams[_istream_id];
		//_decodec = avcodec_find_decoder(_istream->codecpar->codec_id);
		//FC_ASSERT(_decodec, "fail to find codec");
		//_dec_ctx = avcodec_alloc_context3(_decodec);
		//FC_ASSERT(_dec_ctx, "fail to allocate codec context");
		//FC_ASSERT(avcodec_parameters_to_context(_dec_ctx, _istream->codecpar) >= 0, "fail to copy codec parameters");
		//FC_ASSERT(avcodec_open2(_dec_ctx, _decodec, NULL) == 0, "initialize avcodecContext failed");
		//dlog("the pix_fmt = { ${fmt}  }", ("fmt", av_get_pix_fmt_name(_dec_ctx->pix_fmt)));

		//avformat_alloc_output_context2(&_ofmt_ctx, NULL, NULL, _ofile_path.preferred_string().c_str());
		//FC_ASSERT(_ofmt_ctx, "Could not create output context");
		//_ostream = avformat_new_stream(_ofmt_ctx, NULL);
		//FC_ASSERT(_ostream, "Failed allocating output stream");
		//dlog("ostream id=${idx}", ("idx", _ostream->index));
		//FC_ASSERT(avcodec_parameters_copy(_ostream->codecpar, _istream->codecpar) >= 0, "Failed to copy codec parameters");
		//av_dump_format(_ofmt_ctx, 0, _ofile_path.preferred_string().c_str(), 1);
		//av_dump_format(_ifmt_ctx, 0, _ifile_path.preferred_string().c_str(), 0);

		//_encodec = avcodec_find_encoder(_ostream->codecpar->codec_id);
		//if (!_encodec) {
		//	fprintf(stderr, "Codec not found\n");
		//}
		//_ostream_id = _istream_id;//TODO
		//_ec_ctx = avcodec_alloc_context3(_encodec);
		//if (!_ec_ctx) {
		//	fprintf(stderr, "Could not allocate video codec context\n");
		//}
		////_ec_ctx->time_base = _dec_ctx->time_base;
		///* put sample parameters */
		//_ec_ctx->bit_rate = 400000;
		///* resolution must be a multiple of two */
		//_ec_ctx->width = _dec_ctx->width;
		//_ec_ctx->height = _dec_ctx->height;
		///* frames per second */
		//_ec_ctx->time_base = AVRational{ 1, 25 };
		//_ec_ctx->framerate = AVRational{ 25, 1 };

		///* emit one intra frame every ten frames
		// * check frame pict_type before passing frame
		// * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
		// * then gop_size is ignored and the output of encoder
		// * will always be I frame irrespective to gop_size
		// */
		//_ec_ctx->gop_size = 10;
		//_ec_ctx->max_b_frames = 1;
		//_ec_ctx->pix_fmt = _dec_ctx->pix_fmt;


		///*_ec_ctx->time_base = AVRational{ 1, 25 };
		//_ec_ctx->pix_fmt = _dec_ctx->pix_fmt;
		//_ec_ctx->width = _dec_ctx->width;
		//_ec_ctx->height = _dec_ctx->height;*/
		//dlog("timebase=${n}/${d}", ("n", _ec_ctx->time_base.num)("d", _ec_ctx->time_base.den));
		//ret = avcodec_open2(_ec_ctx, _encodec, NULL);
		//if (ret < 0) {
		//	fprintf(stderr, "Could not open codec"/*, av_err2str(ret)*/);
		//}
	}

	void CContext::OpenInputFile()
	{
		int ret;
		unsigned int i;

		_ifmt_ctx = NULL;
		if ((ret = avformat_open_input(&_ifmt_ctx, _ifile_path.preferred_string().c_str(), NULL, NULL)) < 0) {
			av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
			return;
		}

		if ((ret = avformat_find_stream_info(_ifmt_ctx, NULL)) < 0) {
			av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
			return;
		}

		for (int i = 0; i < _ifmt_ctx->nb_streams; ++i)
		{
			CStreamContext ctx;
			_stream_ctxes.push_back(ctx);
		}
		

		for (i = 0; i < _ifmt_ctx->nb_streams; i++) {
			AVStream *stream = _ifmt_ctx->streams[i];
			AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
			AVCodecContext *codec_ctx;
			if (!dec) {
				av_log(NULL, AV_LOG_ERROR, "Failed to find decoder for stream #%u\n", i);
				return;
			}
			codec_ctx = avcodec_alloc_context3(dec);
			if (!codec_ctx) {
				av_log(NULL, AV_LOG_ERROR, "Failed to allocate the decoder context for stream #%u\n", i);
				return;
			}
			ret = avcodec_parameters_to_context(codec_ctx, stream->codecpar);
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Failed to copy decoder parameters to input decoder context "
					"for stream #%u\n", i);
				return;
			}
			/* Reencode video & audio and remux subtitles etc. */
			if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
				|| codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
				if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
					codec_ctx->framerate = av_guess_frame_rate(_ifmt_ctx, stream, NULL);
				/* Open decoder */
				ret = avcodec_open2(codec_ctx, dec, NULL);
				if (ret < 0) {
					av_log(NULL, AV_LOG_ERROR, "Failed to open decoder for stream #%u\n", i);
					return;
				}
			}
			_stream_ctxes[i].dec_ctx = codec_ctx;
		}
		av_dump_format(_ifmt_ctx, 0, _ifile_path.preferred_string().c_str(), 0);
		return;
	}

	void CContext::OpenOutputFile()
	{
		AVStream *out_stream;
		AVStream *in_stream;
		AVCodecContext *dec_ctx, *enc_ctx;
		AVCodec *encoder;
		int ret;
		unsigned int i;

		_ofmt_ctx = NULL;
		avformat_alloc_output_context2(&_ofmt_ctx, NULL, NULL, _ofile_path.preferred_string().c_str());
		if (!_ofmt_ctx) {
			av_log(NULL, AV_LOG_ERROR, "Could not create output context\n");
			return;
		}


		for (i = 0; i < _ifmt_ctx->nb_streams; i++) {
			out_stream = avformat_new_stream(_ofmt_ctx, NULL);
			if (!out_stream) {
				av_log(NULL, AV_LOG_ERROR, "Failed allocating output stream\n");
				return;
			}

			in_stream = _ifmt_ctx->streams[i];
			dec_ctx = _stream_ctxes[i].dec_ctx;

			if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
				|| dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
				/* in this example, we choose transcoding to same codec */
				encoder = avcodec_find_encoder(dec_ctx->codec_id);
				if (!encoder) {
					av_log(NULL, AV_LOG_FATAL, "Necessary encoder not found\n");
					return;
				}
				enc_ctx = avcodec_alloc_context3(encoder);
				if (!enc_ctx) {
					av_log(NULL, AV_LOG_FATAL, "Failed to allocate the encoder context\n");
					return;
				}

				/* In this example, we transcode to same properties (picture size,
				 * sample rate etc.). These properties can be changed for output
				 * streams easily using filters */
				if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
					enc_ctx->height = dec_ctx->height;
					enc_ctx->width = dec_ctx->width;
					enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
					/* take first format from list of supported formats */
					if (encoder->pix_fmts)
						enc_ctx->pix_fmt = encoder->pix_fmts[0];
					else
						enc_ctx->pix_fmt = dec_ctx->pix_fmt;
					/* video time_base can be set to whatever is handy and supported by encoder */
					enc_ctx->time_base = av_inv_q(dec_ctx->framerate);
				}
				else {
					enc_ctx->sample_rate = dec_ctx->sample_rate;
					enc_ctx->channel_layout = dec_ctx->channel_layout;
					enc_ctx->channels = av_get_channel_layout_nb_channels(enc_ctx->channel_layout);
					/* take first format from list of supported formats */
					enc_ctx->sample_fmt = encoder->sample_fmts[0];
					enc_ctx->time_base = AVRational { 1, enc_ctx->sample_rate };
				}

				if (_ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
					enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

				/* Third parameter can be used to pass settings to encoder */
				ret = avcodec_open2(enc_ctx, encoder, NULL);
				if (ret < 0) {
					av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
					return;
				}
				ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
				if (ret < 0) {
					av_log(NULL, AV_LOG_ERROR, "Failed to copy encoder parameters to output stream #%u\n", i);
					return;
				}

				out_stream->time_base = enc_ctx->time_base;
				_stream_ctxes[i].enc_ctx = enc_ctx;
			}
			else if (dec_ctx->codec_type == AVMEDIA_TYPE_UNKNOWN) {
				av_log(NULL, AV_LOG_FATAL, "Elementary stream #%d is of unknown type, cannot proceed\n", i);
				return;
			}
			else {
				/* if this stream must be remuxed */
				ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
				if (ret < 0) {
					av_log(NULL, AV_LOG_ERROR, "Copying parameters for stream #%u failed\n", i);
					return;
				}
				out_stream->time_base = in_stream->time_base;
			}

		}
		av_dump_format(_ofmt_ctx, 0, _ofile_path.preferred_string().c_str(), 1);

		if (!(_ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
			ret = avio_open(&_ofmt_ctx->pb, _ofile_path.preferred_string().c_str(), AVIO_FLAG_WRITE);
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Could not open output file '%s'", _ofile_path.preferred_string().c_str());
				return;
			}
		}

		/* init muxer, write output file header */
		ret = avformat_write_header(_ofmt_ctx, NULL);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Error occurred when opening output file\n");
			return;
		}
	}

}

