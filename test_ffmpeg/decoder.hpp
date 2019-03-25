#pragma once
#include "common.hpp"
//#include "demuxer.hpp"
#include "context.hpp"
#include "frame.hpp"
#include "packet.hpp"
using namespace std;
using namespace cv;

namespace ffcv{

class CDecoder
{
public:
	enum MEDIA_TYPE
	{
		VIDEO = AVMEDIA_TYPE_VIDEO,
		AUDIO = AVMEDIA_TYPE_AUDIO,
	};
	CDecoder(CContext& ctx);
	~CDecoder();
	CDecoder& SetFilter(int second);//TODO
	void SetProcessFunc(function<void(CFrame&)>& func);//TODO
	bool Decode(CPacket& pkt, CFrame& frame);
private:
	CContext& _ctx;
	AVPacket _pkt;
	vector<int> seconds;
	function<void(CFrame&)> _process_img_func;
	friend struct CFrame;
	friend class CEncoder;
};

}