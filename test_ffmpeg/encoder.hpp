#pragma once
#include "decoder.hpp"
#include "packet.hpp"
namespace ffcv
{
	class CEncoder
	{
	public:
		CEncoder() = delete;
		explicit CEncoder(const CDecoder& decoder);
		~CEncoder();
		void Encode(CFrame& frame, CPacket& pkt);
	private:

	};

}
