import esphome.codegen as cg
from esphome.components.light.effects import register_monochromatic_effect
from esphome.components.light.types import LightEffect
from esphome.components.udp import UDP_SCHEMA, register_udp_client
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_NAME, CONF_PORT

AUTO_LOAD = ["socket"]
DEPENDENCIES = ["network"]
CODEOWNERS = ["@tanishqmanuja"]

ddp_ns = cg.esphome_ns.namespace("ddp")
DDPComponent = ddp_ns.class_("DDPComponent", cg.Component)
DDPMonochromaticLightEffect = ddp_ns.class_("DDPMonochromaticLightEffect", LightEffect)

CONF_DDP_ID = "ddp_id"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DDPComponent),
            cv.Optional(CONF_PORT, default=4048): cv.port,
        }
    ).extend(UDP_SCHEMA),
    cv.only_with_arduino,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    udp_var = await register_udp_client(var, config)
    cg.add(udp_var.set_should_listen())
    cg.add(udp_var.set_listen_port(config[CONF_PORT]))


# Light Effects
@register_monochromatic_effect(
    "ddp",
    DDPMonochromaticLightEffect,
    "DDP",
    {
        cv.GenerateID(CONF_DDP_ID): cv.use_id(DDPComponent),
    },
)
async def ddp_light_effect_to_code(config, effect_id):
    var = cg.new_Pvariable(effect_id, config[CONF_NAME])
    await link_to_ddp(var, config)
    return var


# Helpers
async def link_to_ddp(var, config):
    ddp_var = await cg.get_variable(config[CONF_DDP_ID])
    cg.add(var.set_parent(ddp_var))
