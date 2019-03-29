#pragma once
#include "common.hpp"

namespace ffcv
{
	struct  CStreamContext
	{
	public:
		CStreamContext();
		~CStreamContext();
		AVCodecContext *dec_ctx = NULL;
		AVCodecContext *enc_ctx = NULL;
	};
}
