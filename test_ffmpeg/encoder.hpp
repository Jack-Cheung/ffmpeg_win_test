#pragma once
#include "context.hpp"
#include "decoder.hpp"
#include "packet.hpp"
namespace ffcv
{
	class CEncoder
	{
	public:
		CEncoder() = delete;
		explicit CEncoder(CContext& ctx);
		~CEncoder();
		void Encode(CFrame& frame, CPacket& pkt);
	private:
		CContext& _ctx;

	};

}
