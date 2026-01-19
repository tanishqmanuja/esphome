#pragma once

#include "ddp.h"

namespace esphome::ddp {

class DDPRenderer : public Parented<DDPComponent> {
 public:
  virtual void start() { this->parent_->add_renderer(this); }
  virtual void stop() { this->parent_->remove_renderer(this); }
  virtual void loop() {}

 protected:
  virtual void process_(const std::vector<uint8_t> &data) = 0;

  friend class DDPComponent;
};

}  // namespace esphome::ddp
