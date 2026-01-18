#include "ddp_monochromatic_light_effect.h"

namespace esphome::ddp {

void DDPMonochromaticLightEffect::start() {
  LightEffect::start();
  DDPRenderer::start();
}

void DDPMonochromaticLightEffect::stop() {
  LightEffect::stop();
  DDPRenderer::stop();
}

void DDPMonochromaticLightEffect::apply() {}

void DDPMonochromaticLightEffect::process_(const std::vector<uint8_t> &data) {
  if (data.size() - 10 < 3) {
    return;
  }

  uint8_t r = data[10];
  uint8_t g = data[11];
  uint8_t b = data[12];

  uint8_t brightness = std::max({r, g, b});

  auto call = this->state_->turn_on();
  call.set_brightness_if_supported((float) brightness / 255.0f);
  call.set_transition_length_if_supported(0);
  call.set_publish(false);
  call.set_save(false);
  call.perform();
};

}  // namespace esphome::ddp
