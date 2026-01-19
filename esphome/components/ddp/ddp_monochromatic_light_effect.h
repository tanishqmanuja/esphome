#pragma once

#include "ddp.h"
#include "ddp_renderer.h"
#include "esphome/core/application.h"
#include "esphome/components/light/light_state.h"

namespace esphome::ddp {

enum DDPBrightnessCalculation : uint8_t {
  DDP_BRIGHTNESS_CALCULATION_AVG,        // R+ G + B / 3
  DDP_BRIGHTNESS_CALCULATION_MAX,        // MAX (R,G,B)
  DDP_BRIGHTNESS_CALCULATION_PERCEPTIVE  // 0.2126 * R + 0.7152 * G + 0.0722 * B, Rec. 709
};

class DDPMonochromaticLightEffect : public DDPRenderer, public light::LightEffect {
 public:
  DDPMonochromaticLightEffect(const char *name) : LightEffect(name) {}

  void start() override;
  void stop() override;
  void apply() override;
  void loop() override;

  void set_timeout(uint32_t timeout) { this->timeout_ = timeout; }
  void set_blank_on_idle(bool blank_on_idle) { this->blank_on_idle_ = blank_on_idle; }
  void set_brightness_calculation(DDPBrightnessCalculation brightness_calculation) {
    this->brightness_calculation_ = brightness_calculation;
  }

 protected:
  bool is_idle_{false};
  uint32_t timeout_{10000};
  uint32_t last_packet_millis_{0};

  bool blank_on_idle_{false};

  DDPBrightnessCalculation brightness_calculation_{DDPBrightnessCalculation::DDP_BRIGHTNESS_CALCULATION_PERCEPTIVE};

  void process_(const std::vector<uint8_t> &data) override;
  bool timeout_expired_() { return App.get_loop_component_start_time() - this->last_packet_millis_ >= this->timeout_; }
};

}  // namespace esphome::ddp
