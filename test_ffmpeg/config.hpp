#pragma once
#include "common.hpp"
using std::vector;
using std::string;
namespace ffcv
{ 
	class CConfig
	{
	public:
		bool need_muxer = false;
		bool need_snapshot = true;
		string input_file;
		string output_file;
		string snapshot_filename_prefix = "snapshot";
		vector<uint32_t> intrested_frame_num;

		bool Validate() const;
		void LoadFile();
		void WriteFile() const;
	private:
	};
}
