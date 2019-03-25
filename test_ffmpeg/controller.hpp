#pragma once
#include "common.hpp"
#include "config.hpp"
#include "context.hpp"
#include "demuxer.hpp"
#include "decoder.hpp"
#include "encoder.hpp"
#include "muxer.hpp"

namespace ffcv
{
	class CController
	{
	public:
		CController() = delete;
		CController(const CConfig& conf);
		~CController() = default;

		void Process();
	private:
		CConfig _conf;
	};
}
