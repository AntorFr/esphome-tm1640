from esphome import pins
import esphome.codegen as cg
from esphome.components import display
import esphome.config_validation as cv
from esphome.const import (
    CONF_CLK_PIN,
    CONF_ID,
    CONF_INTENSITY,
    CONF_INVERTED,
    CONF_LAMBDA,
    CONF_LENGTH,
)

CODEOWNERS = ["@AntorFr"]

CONF_DIN_PIN = "din_pin"

tm1640_ns = cg.esphome_ns.namespace("tm1640")
TM1640Display = tm1640_ns.class_("TM1640Display", cg.PollingComponent)
TM1640DisplayRef = TM1640Display.operator("ref")

CONFIG_SCHEMA = display.BASIC_DISPLAY_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(TM1640Display),
        cv.Optional(CONF_INTENSITY, default=7): cv.All(
            cv.uint8_t, cv.Range(min=0, max=7)
        ),
        cv.Optional(CONF_INVERTED, default=False): cv.boolean,
        cv.Optional(CONF_LENGTH, default=16): cv.All(cv.uint8_t, cv.Range(min=1, max=16)),
        cv.Required(CONF_CLK_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_DIN_PIN): pins.gpio_output_pin_schema,
    }
).extend(cv.polling_component_schema("1s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)

    clk = await cg.gpio_pin_expression(config[CONF_CLK_PIN])
    cg.add(var.set_clk_pin(clk))
    din = await cg.gpio_pin_expression(config[CONF_DIN_PIN])
    cg.add(var.set_din_pin(din))

    cg.add(var.set_intensity(config[CONF_INTENSITY]))
    cg.add(var.set_inverted(config[CONF_INVERTED]))
    cg.add(var.set_length(config[CONF_LENGTH]))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(TM1640DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
