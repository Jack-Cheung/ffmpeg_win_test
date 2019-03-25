#pragma once
#include "common.hpp"

namespace ffcv
{ 
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
		friend class CEncoder;
		friend class CMuxer;
	};
}
