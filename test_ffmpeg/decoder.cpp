#include "decoder.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <fc/scoped_exit.hpp>
namespace ffmpeg{

CDecoder::CDecoder(const CDemuxer& demuxer, MEDIA_TYPE intrested /*= VIDEO*/)
	:_fmt_ctx(demuxer._fmt_ctx)
{
	int ret = av_find_best_stream(_fmt_ctx, AVMediaType(intrested), -1, -1, NULL, 0);
	FC_ASSERT(ret >= 0, "could not find stream");
	_stream_id = ret;
	AVStream* stream = _fmt_ctx->streams[_stream_id];
	AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
	FC_ASSERT(codec, "fail to find codec");
	_dec_ctx = avcodec_alloc_context3(codec);
	FC_ASSERT(_dec_ctx, "fail to allocate codec context");
	FC_ASSERT(avcodec_parameters_to_context(_dec_ctx, stream->codecpar) >= 0, "fail to copy codec parameters");
	FC_ASSERT(avcodec_open2(_dec_ctx, codec, NULL) == 0, "initialize avcodecContext failed");
	dlog("the pix_fmt = { ${fmt}  }", ("fmt", av_get_pix_fmt_name(_dec_ctx->pix_fmt)));
	_process_img_func = [&](CFrame& frame) {
		//frame.GenerateImage("");
		frame.DetectFace();
	};
}

CDecoder::~CDecoder()
{
}

CDecoder& CDecoder::SetFilter(int second)
{
	seconds.push_back(second);
	return *this;
}

void CDecoder::SetProcessFunc(function<void(CFrame&)>& func)
{
	_process_img_func = func;
}

bool CDecoder::Decode(CPacket& pkt, CFrame& frame)
{
	AVPacket* pavPacket = &pkt._pkt;
	AVFrame* pavFrame = frame.avframe;
	if (pavPacket->stream_index == _stream_id)
	{
		auto on_exit = fc::make_scoped_exit([&pavFrame]() {
			av_frame_unref(pavFrame);
		});
		//dlog("pkt.pts = ${pts}", ("pts", pavPacket->pts));
		int ret = avcodec_send_packet(_dec_ctx, pavPacket);
		if (ret == 0)
		{
			ret = avcodec_receive_frame(_dec_ctx, pavFrame);
			if (ret == 0)
			{
				vector<int>::const_iterator  itr;
				if (!seconds.empty() && (itr = find_if(seconds.cbegin(), seconds.cend(), [&](auto elem)->bool {
					return elem * av_q2d(av_inv_q(_fmt_ctx->streams[_stream_id]->time_base)) == pavPacket->pts;
				})) != end(seconds))
				{
					if (_process_img_func)
					{
						_process_img_func(frame);
					}
					return true;
				}
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}



CFrame::CFrame( CDecoder& decoder)
{
	avframe = av_frame_alloc();
	_width = decoder._dec_ctx->width;
	_height = decoder._dec_ctx->height;
	_pix_fmt = decoder._dec_ctx->pix_fmt;
	_buf_size = av_image_alloc(_data, _linesize, _width, _height, _pix_fmt, 1);
	dlog("image buffer size = ${size}", ("size", _buf_size));
}

CFrame::~CFrame()
{
	av_frame_free(&avframe);
}

void CFrame::GenerateImage(fc::path p)
{
	av_image_copy(_data, _linesize, (const uint8_t **)avframe->data, avframe->linesize, _pix_fmt, _width, _height);
	std::string file_name = "test" + std::to_string(avframe->coded_picture_number) + ".jpg";
	cv::Mat yuvImg;
	cv::Mat rgbImg(_height, _width,  CV_8UC3);
	yuvImg.create(_height * 3 / 2, _width, CV_8UC1);
	memcpy(yuvImg.data, _data[0], _buf_size);
	cv::cvtColor(yuvImg, rgbImg, cv::COLOR_YUV2BGR_I420);

	cv::imwrite(file_name, rgbImg);
}

void CFrame::DetectFace()
{
	av_image_copy(_data, _linesize, (const uint8_t **)avframe->data, avframe->linesize, _pix_fmt, _width, _height);
	std::string file_name = "test" + std::to_string(avframe->coded_picture_number) + ".jpg";
	cv::Mat yuvImg;
	cv::Mat rgbImg(_height, _width, CV_8UC3);
	yuvImg.create(_height * 3 / 2, _width, CV_8UC1);
	memcpy(yuvImg.data, _data[0], _buf_size);
	cv::cvtColor(yuvImg, rgbImg, cv::COLOR_YUV2BGR_I420);
	String face_cascade_name = "haarcascade_frontalface_alt.xml";
	String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
	//-- 1. Load the cascades
	if (!_face_cascade.load(face_cascade_name))
	{
		cout << "--(!)Error loading face cascade\n";
		return;
	};
	if (!_eyes_cascade.load(eyes_cascade_name))
	{
		cout << "--(!)Error loading eyes cascade\n";
		return;
	};
	Mat frame_gray;
	cvtColor(rgbImg, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	std::vector<Rect> faces;
	_face_cascade.detectMultiScale(frame_gray, faces);
	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		ellipse(rgbImg, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4);
		Mat faceROI = frame_gray(faces[i]);
		//-- In each face, detect eyes
		std::vector<Rect> eyes;
		_eyes_cascade.detectMultiScale(faceROI, eyes);
		for (size_t j = 0; j < eyes.size(); j++)
		{
			Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
			circle(rgbImg, eye_center, radius, Scalar(255, 0, 0), 4);
		}
	}
	//-- Show what you got
	cv::imwrite(file_name, rgbImg);
}

}
