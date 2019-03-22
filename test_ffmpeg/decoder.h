#pragma once
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}
#include <fc/exception/exception.hpp>
#include <fc/log/logger.hpp>
#include <fc/filesystem.hpp>

#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>

#include <functional>
#include <vector>
#include <fstream>
#include "demuxer.hpp"

using namespace std;
using namespace cv;

namespace ffmpeg{

struct CFrame;
class CDecoder
{
public:
	enum MEDIA_TYPE
	{
		VIDEO = AVMEDIA_TYPE_VIDEO,
		AUDIO = AVMEDIA_TYPE_AUDIO,
	};
	CDecoder(const CDemuxer& demuxer, MEDIA_TYPE intrested = VIDEO);
	~CDecoder();
	CDecoder& SetFilter(int second);
	void SetProcessFunc(function<void(CFrame&)>& func);
	bool Decode(CPacket& pkt, CFrame& frame);
private:
	AVFormatContext* _fmt_ctx = NULL;
	AVCodecContext* _dec_ctx = NULL;
	AVPacket _pkt;
	vector<int> seconds;
	int _stream_id = -1;
	function<void(CFrame&)> _process_img_func;
	friend struct CFrame;
};

struct CFrame
{
public:
	CFrame() = delete;
	CFrame(CDecoder& decoder);
	~CFrame();
	void GenerateImage(fc::path p);
	void DetectFace();
private:
	AVFrame* avframe = NULL;
	AVCodecContext* avCodecCtx = NULL;
	uint8_t * _data[4]{ 0 };
	int _linesize[4]{ 0 };
	int _buf_size = -1;
	int _width = 0;
	int _height = 0;
	AVPixelFormat _pix_fmt;
	CascadeClassifier _face_cascade;
	CascadeClassifier _eyes_cascade;
	friend class CDecoder;
	friend class CDemuxer;
};

}

