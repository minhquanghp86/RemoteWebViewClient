#include "video_decoder.h"

#ifdef USE_ESP32
#include <esp_heap_caps.h>
#endif

namespace esphome {
namespace remote_webview {

void VideoDecoder::setup(display::Display *display) {
  this->display_ = display;
  
  // Allocate RGB buffer in PSRAM (320x240 max)
  this->rgb_buffer_size_ = 320 * 240;
  
#ifdef USE_ESP32
  this->rgb_buffer_ = (uint16_t *) heap_caps_malloc(this->rgb_buffer_size_ * sizeof(uint16_t), 
                                                      MALLOC_CAP_SPIRAM);
#else
  this->rgb_buffer_ = (uint16_t *) malloc(this->rgb_buffer_size_ * sizeof(uint16_t));
#endif
  
  if (this->rgb_buffer_ == nullptr) {
    ESP_LOGE(TAG_VIDEO, "Failed to allocate RGB buffer");
  } else {
    ESP_LOGI(TAG_VIDEO, "Video decoder initialized, buffer size: %d pixels", this->rgb_buffer_size_);
  }
}

bool VideoDecoder::decode_and_display_frame(const uint8_t *jpeg_data, size_t jpeg_size) {
  if (this->display_ == nullptr || this->rgb_buffer_ == nullptr) {
    return false;
  }
  
  uint16_t *decoded_buffer = nullptr;
  int width = 0, height = 0;
  
  if (!this->decode_jpeg_to_rgb565_(jpeg_data, jpeg_size, decoded_buffer, width, height)) {
    ESP_LOGW(TAG_VIDEO, "Failed to decode JPEG frame");
    return false;
  }
  
  // Push to display
  if (decoded_buffer != nullptr) {
    // Assuming display has method to push raw RGB565 buffer
    // You may need to adapt this to your display's API
    
    // For now, just log success
    ESP_LOGD(TAG_VIDEO, "Decoded frame: %dx%d", width, height);
    
    // TODO: Push to display
    // this->display_->draw_pixels_at(0, 0, width, height, decoded_buffer, ...);
    
    return true;
  }
  
  return false;
}

bool VideoDecoder::decode_jpeg_to_rgb565_(const uint8_t *jpeg_data, size_t jpeg_size,
                                          uint16_t *&out_buffer, int &out_width, int &out_height) {
#ifdef USE_ESP32
  esp_jpeg_image_cfg_t jpeg_cfg;
  jpeg_cfg.indata = (uint8_t *) jpeg_data;
  jpeg_cfg.indata_size = jpeg_size;
  jpeg_cfg.outbuf = (uint8_t *) this->rgb_buffer_;
  jpeg_cfg.outbuf_size = this->rgb_buffer_size_ * sizeof(uint16_t);
  jpeg_cfg.out_format = JPEG_IMAGE_FORMAT_RGB565;
  jpeg_cfg.out_scale = JPEG_IMAGE_SCALE_0;
  jpeg_cfg.flags.swap_color_bytes = 0;
  
  esp_jpeg_image_output_t outimg;
  
  esp_err_t ret = esp_jpeg_decode(&jpeg_cfg, &outimg);
  
  if (ret == ESP_OK) {
    out_buffer = this->rgb_buffer_;
    out_width = outimg.width;
    out_height = outimg.height;
    return true;
  } else {
    ESP_LOGE(TAG_VIDEO, "JPEG decode error: %d", ret);
    return false;
  }
#else
  ESP_LOGE(TAG_VIDEO, "JPEG decode only supported on ESP32");
  return false;
#endif
}

}  // namespace remote_webview
}  // namespace esphome
