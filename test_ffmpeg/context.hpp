#pragma once
#include "common.hpp"
using std::string;
namespace ffcv
{
	class CContext
	{
	public:
		CContext(const string& ifile_path, const string& ofile_path);

	private:
		AVFormatContext* _ifmt_ctx = NULL;
		AVFormatContext* _ofmt_ctx = NULL;
		AVCodecContext* _dec_ctx = NULL;
		AVCodecContext* _ec_ctx = NULL;
		AVCodec* _decodec = NULL;
		AVCodec* _encodec = NULL;
		fc::path _ifile_path;
		fc::path _ofile_path;
		//TODO
		int _istream_id = -1;
		int _ostream_id = -1;
		AVStream* _istream = NULL;
		AVStream* _ostream = NULL;

		friend class CDemuxer;
		friend class CDecoder;
		friend class CEncoder;
		friend class CMuxer;
		friend class CFrame;
	};
}
