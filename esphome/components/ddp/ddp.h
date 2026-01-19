#pragma once

#ifdef USE_ARDUINO

#include "esphome/core/component.h"
#include "esphome/components/udp/udp_component.h"

#include "esphome/components/light/light_effect.h"

namespace esphome::ddp {

class DDPRenderer;

class DDPComponent : public Component, public Parented<esphome::udp::UDPComponent> {
 public:
  void setup() override;
  float get_setup_priority() const override { return this->parent_->get_setup_priority(); }
  void loop() override;

  void add_renderer(DDPRenderer *effect);
  void remove_renderer(DDPRenderer *effect);

 protected:
  std::vector<DDPRenderer *> renderers_;
  void process_(const std::vector<uint8_t> &data);
};

}  // namespace esphome::ddp

#endif  // USE_ARDUINO
