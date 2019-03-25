#pragma once
#include "common.hpp"
#include "demuxer.hpp"
namespace ffcv {
	class CMuxer
	{
	public:
		CMuxer() = delete;
		explicit CMuxer(CContext& ctx);
		//TODO  CMuxer(const string& file_name)
		~CMuxer();

		void WritePacket(CPacket& pkt);
	private:
		CContext& _ctx;
	};

}



