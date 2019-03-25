#pragma once
#include "common.hpp"
#include "context.hpp"
namespace ffcv
{ 

	struct CFrame
	{
	public:
		CFrame() = delete;
		CFrame(CContext& ctx);
		~CFrame();
		CFrame& SetPayload();
		CFrame& ConvertColor();
		CFrame& CreateSnapShot();
		CFrame& MarkFace();
	private:
		AVFrame* avframe = NULL;
		CContext& _ctx;
		uint8_t * _data[4]{ 0 };
		int _linesize[4]{ 0 };
		int _buf_size = -1;
		int _width = 0;
		int _height = 0;
		AVPixelFormat _pix_fmt;
		cv::CascadeClassifier _face_cascade;
		cv::CascadeClassifier _eyes_cascade;
		cv::Mat _originImg;
		cv::Mat _convtImg;
		friend class CDecoder;
		friend class CDemuxer;
		//friend class CMuxer;
		friend class CEncoder;
	};
}
