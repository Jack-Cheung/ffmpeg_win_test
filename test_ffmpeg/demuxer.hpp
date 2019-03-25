#pragma once
#include "common.hpp"
#include "context.hpp"
#include "packet.hpp"

namespace ffcv {
	class CDemuxer
	{
	public:
		CDemuxer() = delete;
		CDemuxer(CContext& context);
		~CDemuxer();
		bool LoopPacket(CPacket& pkt);
	private:
		CContext&  _ctx;
	};


}