#include "encoder.hpp"

namespace ffcv
{
	CEncoder::CEncoder(const CDecoder& decoder)
	{
		//avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);

	}

	void CEncoder::Encode(CFrame& frame, CPacket& pkt)
	{

	}

	CEncoder::~CEncoder()
	{
	}
}

