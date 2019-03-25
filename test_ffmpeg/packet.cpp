#pragma once
#include "packet.hpp"
namespace ffcv
{ 
	CPacket::CPacket()
	{
		av_init_packet(&_pkt);
		_pkt.data = NULL;
		_pkt.size = 0;
	}

	CPacket::CPacket(const CPacket& pkt)
	{
		av_packet_ref(&_pkt, &(pkt._pkt));
	}

	CPacket::~CPacket()
	{
		av_packet_unref(&_pkt);
	}
}