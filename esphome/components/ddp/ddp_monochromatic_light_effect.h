#pragma once

#include "ddp.h"
#include "ddp_renderer.h"
#include "esphome/components/light/light_state.h"

namespace esphome::ddp {

class DDPMonochromaticLightEffect : public DDPRenderer, public light::LightEffect {
 public:
  DDPMonochromaticLightEffect(const char *name) : LightEffect(name) {}

  void start() override;
  void stop() override;
  void apply() override;

 protected:
  void process_(const std::vector<uint8_t> &data) override;
};

}  // namespace esphome::ddp
