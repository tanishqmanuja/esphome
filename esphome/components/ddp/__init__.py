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
DDPBrightnessCalculation = ddp_ns.enum("DDPBrightnessCalculation")
BRIGHTNESS_CALCULATIONS = {
    "AVG": DDPBrightnessCalculation.DDP_BRIGHTNESS_CALCULATION_AVG,
    "MAX": DDPBrightnessCalculation.DDP_BRIGHTNESS_CALCULATION_MAX,
    "PERCEPTIVE": DDPBrightnessCalculation.DDP_BRIGHTNESS_CALCULATION_PERCEPTIVE,
}

CONF_DDP_ID = "ddp_id"
CONF_BRIGHTNESS_CALCULATION = "brightness_calculation"
CONF_BLANK_ON_IDLE = "blank_on_idle"

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
        cv.Optional(CONF_BRIGHTNESS_CALCULATION): cv.enum(
            BRIGHTNESS_CALCULATIONS, upper=True
        ),
        cv.Optional(CONF_BLANK_ON_IDLE): cv.boolean,
    },
)
async def ddp_light_effect_to_code(config, effect_id):
    var = cg.new_Pvariable(effect_id, config[CONF_NAME])
    await cg.register_parented(var, config[CONF_DDP_ID])

    if CONF_BRIGHTNESS_CALCULATION in config:
        cg.add(
            var.set_brightness_calculation(
                BRIGHTNESS_CALCULATIONS[config[CONF_BRIGHTNESS_CALCULATION]]
            )
        )

    if CONF_BLANK_ON_IDLE in config:
        cg.add(var.set_blank_on_idle(config[CONF_BLANK_ON_IDLE]))

    return var
