#include "ddp_monochromatic_light_effect.h"
#include "esphome/core/log.h"

namespace esphome::ddp {

static const char *const TAG = "ddp";

void DDPMonochromaticLightEffect::start() {
  this->is_idle_ = true;

  LightEffect::start();
  DDPRenderer::start();
}

void DDPMonochromaticLightEffect::stop() {
  this->is_idle_ = true;

  LightEffect::stop();
  DDPRenderer::stop();
}

void DDPMonochromaticLightEffect::apply() {}

void DDPMonochromaticLightEffect::loop() {
  if (!this->is_idle_ && timeout_expired_()) {
    ESP_LOGD(TAG, "Stream timed out %s", this->get_name());

    this->is_idle_ = true;

    auto call = this->state_->turn_on();
    if (this->blank_on_idle_) {
      call.set_brightness_if_supported(0.0f);
    } else {
      call.set_brightness_if_supported(this->state_->remote_values.get_brightness());
    }
    call.set_publish(false);
    call.set_save(false);
    call.perform();
  }
}

void DDPMonochromaticLightEffect::process_(const std::vector<uint8_t> &data) {
  if (data.size() < 13) {
    // HEADER(10) + RGB(1+1+1)
    return;
  }

  this->is_idle_ = false;
  this->last_packet_millis_ = App.get_loop_component_start_time();

  uint8_t r = data[10];
  uint8_t g = data[11];
  uint8_t b = data[12];

  uint8_t brightness = 0;

  switch (this->brightness_calculation_) {
    case DDPBrightnessCalculation::DDP_BRIGHTNESS_CALCULATION_AVG:
      brightness = (uint16_t(r) + g + b) / 3;
      break;

    case DDPBrightnessCalculation::DDP_BRIGHTNESS_CALCULATION_MAX:
      brightness = std::max(r, std::max(g, b));
      break;

    case DDPBrightnessCalculation::DDP_BRIGHTNESS_CALCULATION_PERCEPTIVE:
      brightness = (uint8_t) ((54 * r + 183 * g + 19 * b) >> 8);
  }

  auto call = this->state_->turn_on();
  call.set_brightness_if_supported(brightness * (1.0f / 255.0f));
  call.set_transition_length_if_supported(0);
  call.set_publish(false);
  call.set_save(false);
  call.perform();
};

}  // namespace esphome::ddp
