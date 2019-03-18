#pragma once
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}
#include <string>
#include <fc/filesystem.hpp>
#include <fc/exception/exception.hpp>
namespace ffmpeg {
	class CPacket;
	class CDemuxer
	{
	public:
		
		CDemuxer();
		CDemuxer(const std::string& src_file_path);
		~CDemuxer();
		bool LoopPacket(CPacket& pkt);
	private:
		AVFormatContext* _fmt_ctx;
		fc::path _src_file_path;
		friend class CDecoder;
	};

	class CPacket
	{
	public:
		CPacket();
		CPacket(const CPacket& pkt);
		~CPacket();
	private:
		AVPacket _pkt;
		friend class CDecoder;
		friend class CDemuxer;
	};
}