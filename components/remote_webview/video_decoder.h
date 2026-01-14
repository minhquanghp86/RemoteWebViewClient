#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/display/display.h"

#ifdef USE_ESP32
#include <esp_http_client.h>
#include "esp_jpg_decode.h"
#endif

namespace esphome {
namespace remote_webview {

static const char *const TAG_VIDEO = "remote_webview.video";

class VideoDecoder {
 public:
  VideoDecoder() = default;
  
  void setup(display::Display *display);
  void set_camera_url(const std::string &url) { this->camera_url_ = url; }
  
  bool decode_and_display_frame(const uint8_t *jpeg_data, size_t jpeg_size);
  
 protected:
  display::Display *display_{nullptr};
  std::string camera_url_;
  
  uint16_t *rgb_buffer_{nullptr};
  size_t rgb_buffer_size_{0};
  
  bool decode_jpeg_to_rgb565_(const uint8_t *jpeg_data, size_t jpeg_size, 
                               uint16_t *&out_buffer, int &out_width, int &out_height);
};

}  // namespace remote_webview
}  // namespace esphome
