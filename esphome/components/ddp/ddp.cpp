#ifdef USE_ARDUINO

#include "ddp.h"
#include "ddp_renderer.h"
#include "esphome/core/log.h"

namespace esphome::ddp {

static const char *const TAG = "ddp";

void DDPComponent::setup() {
  this->parent_->add_listener([this](std::vector<uint8_t> &buf) { this->process_(buf); });
}

void DDPComponent::loop() {
  for (auto &renderer : this->renderers_) {
    renderer->loop();
  }
}

void DDPComponent::add_renderer(DDPRenderer *effect) { this->renderers_.push_back(effect); }

void DDPComponent::remove_renderer(DDPRenderer *effect) {
  auto it = std::find(this->renderers_.begin(), this->renderers_.end(), effect);
  if (it != this->renderers_.end()) {
    this->renderers_.erase(it);
  }
}

void DDPComponent::process_(const std::vector<uint8_t> &data) {
  // DDP (Distributed Display Protocol) UDP packet format
  //
  // Fixed header: 10 bytes total
  //
  // Byte 0 : Flags / Version
  //          bits 7–4 -> Protocol version (usually 0x4)
  //          bit 3    -> Push flag (1 = apply data immediately)
  //          bit 2    -> Query flag
  //          bit 1    -> Reply flag
  //          bit 0    -> Reserved
  //
  // Byte 1 : Sequence number (uint8_t, wraps at 255)
  //
  // Byte 2 : Data type
  //          0x01 = RGB  (3 bytes per pixel)
  //          0x02 = Grayscale / Monochrome
  //          0x03 = RGBW (4 bytes per pixel)
  //
  // Byte 3 : Source ID (typically 0x01)
  //
  // Bytes 4–7 : Data offset (uint32_t, big-endian)
  //             Byte offset into the full pixel buffer
  //
  // Bytes 8–9 : Data length (uint16_t, big-endian)
  //             Number of payload bytes following the header
  //
  // Bytes 10..N : Payload data (pixel data)
  //
  // Notes     : Large frames are split across packets using the offset field
  //             Offset and length are big-endian

  ESP_LOGVV(TAG, "DDP packet received");

  if (this->renderers_.empty()) {
    return;
  }

  // Validate packet
  if (data.size() < 10) {
    ESP_LOGD(TAG, "Invalid DDP packet received, too short (size=%d)", data.size());
    return;
  }

  if (data[4] || data[5] || data[6] || data[7]) {
    ESP_LOGD(TAG, "Ignoring DDP Packet with non-zero data offset.");
    return;
  }

  // Render to all renderers
  for (auto *renderer : this->renderers_) {
    renderer->process_(data);
  }
}

}  // namespace esphome::ddp

#endif  // USE_ARDUINO
