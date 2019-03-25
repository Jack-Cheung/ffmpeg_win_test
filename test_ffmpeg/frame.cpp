#pragma once
#include "frame.hpp"
namespace ffcv
{ 
	CFrame::CFrame(CContext& ctx)
		:_ctx(ctx)
	{
		avframe = av_frame_alloc();
		_width = _ctx._dec_ctx->width;
		_height = _ctx._dec_ctx->height;
		_pix_fmt = _ctx._dec_ctx->pix_fmt;
		_buf_size = av_image_alloc(_data, _linesize, _width, _height, _pix_fmt, 1);
		dlog("image buffer size = ${size}", ("size", _buf_size));
		//TODO decoded frames are YUV420P data
		_originImg.create(_height * 3 / 2, _width, CV_8UC1);
		_convtImg.create(_height, _width, CV_8UC3);
	}

	CFrame::~CFrame()
	{
		av_frame_free(&avframe);
	}

	CFrame& CFrame::SetPayload()
	{
		av_image_copy(_data, _linesize, (const uint8_t **)avframe->data, avframe->linesize, _pix_fmt, _width, _height);
		memcpy(_originImg.data, _data[0], _buf_size);
		return *this;
	}

	CFrame& CFrame::ConvertColor()
	{
		//TODO
		cv::cvtColor(_originImg, _convtImg, cv::COLOR_YUV2BGR_I420);
		return *this;
	}

	CFrame& CFrame::CreateSnapShot()
	{
		std::string file_name = "test" + std::to_string(avframe->coded_picture_number) + ".jpg";
		cv::imwrite(file_name, _convtImg);
		return *this;
	}

	CFrame& CFrame::MarkFace()
	{
		using namespace cv;
		//TODO efficiency low
		string face_cascade_name = "haarcascade_frontalface_alt.xml";
		string eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
		//-- 1. Load the cascades
		FC_ASSERT(_face_cascade.load(face_cascade_name), "--(!)Error loading face cascade");
		FC_ASSERT(_eyes_cascade.load(eyes_cascade_name), "--(!)Error loading eyes cascade");
		Mat frame_gray;
		cvtColor(_convtImg, frame_gray, COLOR_BGR2GRAY);
		equalizeHist(frame_gray, frame_gray);
		//-- Detect faces
		std::vector<Rect> faces;
		_face_cascade.detectMultiScale(frame_gray, faces);
		for (size_t i = 0; i < faces.size(); i++)
		{
			Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
			ellipse(_convtImg, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4);
			Mat faceROI = frame_gray(faces[i]);
			//-- In each face, detect eyes
			std::vector<Rect> eyes;
			_eyes_cascade.detectMultiScale(faceROI, eyes);
			for (size_t j = 0; j < eyes.size(); j++)
			{
				Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
				int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
				circle(_convtImg, eye_center, radius, Scalar(255, 0, 0), 4);
			}
		}
		/*int av_image_fill_arrays(uint8_t *dst_data[4], int dst_linesize[4],
			const uint8_t *src,
			enum AVPixelFormat pix_fmt, int width, int height, int align);*/
		return *this;
	}
}