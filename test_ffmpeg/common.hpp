#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}

#include <fc/exception/exception.hpp>
#include <fc/log/logger.hpp>
#include <fc/filesystem.hpp>
#include <fc/scoped_exit.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/objdetect.hpp>

#include <functional>
#include <vector>
#include <fstream>
#include <string>