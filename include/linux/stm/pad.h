#ifndef __LINUX_STM_PAD_H
#define __LINUX_STM_PAD_H

#include <linux/stm/pad.h>
#include <linux/stm/sysconf.h>

#define STM_PAD_LABEL_LEN 32

/* Label is defined as "<pads_group>[.<subgroup>[.<subgroup>]...].<pad_id>"
 *
 * Internally it is represented as a prefix string and optional suffix,
 * given either as a string or integer. Labels are always compared
 * as strings of the mentioned format, so it is possible to combine
 * different definitions.
 *
 * Maximum total length of label part (including dots and NULL
 * termination character) is STM_PAD_LABEL_LEN.
 *
 * The following label definitions of PIO3.7 pad are identical:
 *
 * - STM_PAD_LABEL("PIO3.7")
 * - STM_PAD_LABEL_NUMBER("PIO3", 7)
 * - STM_PAD_LABEL_STRINGS("PIO3", "7")
 *
 * These definitions are identical and describe set of PIO3.5, PIO3.6
 * and PIO3.7 pads:
 *
 * - STM_PAD_LABEL_RANGE("PIO3", 5, 7)
 * - STM_PAD_LABEL_LIST("PIO3", 5, 6, 7)
 * - STM_PAD_LABEL_STRINGS("PIO3", "5", "6", "7")
 *
 * Other examples:
 *
 * - STM_PAD_LABEL("MII.RXCLK")
 * - STM_PAD_LABEL_STRINGS("MII", "RXCLK")
 *
 * - STM_PAD_LABEL("VIDDIGOUT.YC.0")
 * - STM_PAD_LABEL_NUMBER("VIDDIGOUT.YC", 0)
 *
 * - STM_PAD_LABEL("TS0.OUT.DATA.0")
 * - STM_PAD_LABEL_NUMBER("TS0.OUT.DATA", 0)
 * - STM_PAD_LABEL_STRINGS("TS0.OUT.DATA", "0")
 */
struct stm_pad_label {
	const char *prefix;
	enum {
		stm_pad_label_suffix_none = 0,
		stm_pad_label_suffix_number,
		stm_pad_label_suffix_range,
		stm_pad_label_suffix_list,
		stm_pad_label_suffix_strings,
	} suffix_type;
	union {
		int number;
		struct {
			int from, to;
		} range;
		int *list; /* STM_PAD_LABEL_LIST_LAST terminated */
		char **strings; /* STM_PAD_LABEL_STRINGS_LAST terminated */
	} suffix;
};

#define STM_PAD_LABEL(_label) \
	{ \
		.prefix = _label, \
	}

#define STM_PAD_LABEL_NUMBER(_prefix, _number) \
	{ \
		.prefix = _prefix, \
		.suffix_type = stm_pad_label_suffix_number, \
		.suffix.number = _number, \
	}

#define STM_PAD_LABEL_RANGE(_prefix, _from, _to) \
	{ \
		.prefix = _prefix, \
		.suffix_type = stm_pad_label_suffix_range, \
		.suffix.range.from = _from, \
		.suffix.range.to = _to, \
	}

#define STM_PAD_LABEL_LIST_LAST -1

#define STM_PAD_LABEL_LIST(_prefix, _suffix...) \
	{ \
		.prefix = _prefix, \
		.suffix_type = stm_pad_label_suffix_list, \
		.suffix.list = (int []) { \
			_suffix, \
			STM_PAD_LABEL_LIST_LAST, \
		}, \
	}

#define STM_PAD_LABEL_STRINGS_LAST NULL

#define STM_PAD_LABEL_STRINGS(_prefix, _strings...) \
	{ \
		.prefix = _prefix, \
		.suffix_type = stm_pad_label_suffix_strings, \
		.suffix.strings = (char *[]) { \
			_strings, \
			STM_PAD_LABEL_STRINGS_LAST, \
		}, \
	}

struct stm_pad_sysconf_value {
	int regtype;
	int regnum;
	int lsb;
	int msb;
	int value;
};

#define STM_PAD_SYS_CFG(_regnum, _lsb, _msb, _value) \
	{ \
		.regtype = SYS_CFG, \
		.regnum = _regnum, \
		.lsb = _lsb, \
		.msb = _msb, \
		.value = _value, \
	}

/*
 * We have to do this indirection to allow the first argument to
 * STM_PAD_CFG to be a macro, as used by 5197 for ewxample.
 */
#define STM_PAD_XXX(_regtype, _regnum, _lsb, _msb, _value) \
        { \
                .regtype = _regtype, \
                .regnum = _regnum, \
                .lsb = _lsb, \
                .msb = _msb, \
                .value = _value, \
        }


#define STM_PAD_CFG(_regtypenum, _lsb, _msb, _value) \
	STM_PAD_XXX(_regtypenum, _lsb, _msb, _value)

struct stm_pad_gpio_value {
	unsigned gpio;
	int direction;
};

#define STM_PAD_PIO_BIDIR(_port, _pin) \
	{ \
		.gpio = stm_gpio(_port, _pin), \
		.direction = STM_GPIO_DIRECTION_BIDIR, \
	}

#define STM_PAD_PIO_OUT(_port, _pin) \
	{ \
		.gpio = stm_gpio(_port, _pin), \
		.direction = STM_GPIO_DIRECTION_OUT, \
	}

#define STM_PAD_PIO_IN(_port, _pin) \
	{ \
		.gpio = stm_gpio(_port, _pin), \
		.direction = STM_GPIO_DIRECTION_IN, \
	}

#define STM_PAD_PIO_ALT_OUT(_port, _pin) \
	{ \
		.gpio = stm_gpio(_port, _pin), \
		.direction = STM_GPIO_DIRECTION_ALT_OUT, \
	}

#define STM_PAD_PIO_ALT_BIDIR(_port, _pin) \
	{ \
		.gpio = stm_gpio(_port, _pin), \
		.direction = STM_GPIO_DIRECTION_ALT_BIDIR, \
	}

#define STM_PAD_PIO_UNKNOWN(_port, _pin) \
	{ \
		.gpio = stm_gpio(_port, _pin), \
		.direction = -1, \
	}

struct stm_pad_config {
	int labels_num;
	struct stm_pad_label *labels;
	int sysconf_values_num;
	struct stm_pad_sysconf_value *sysconf_values;
	int gpio_values_num;
	struct stm_pad_gpio_value *gpio_values;
	int (*custom_claim)(void *priv);
	int (*custom_release)(void *priv);
	void *custom_priv;
};

int stm_pad_claim(struct stm_pad_config *config, const char *dev_name);
int stm_pad_switch(struct stm_pad_config *old_config,
		struct stm_pad_config *new_config, const char *new_dev_name);
void stm_pad_release(struct stm_pad_config *config);

const char *stm_pad_owner(const char *label);



struct stm_pad_config * __init stm_pad_config_alloc(int min_labels_num,
		int min_sysconf_values_num, int min_gpio_values_num);

int __init stm_pad_config_add_label(struct stm_pad_config *config,
		const char *label);
int __init stm_pad_config_add_label_number(struct stm_pad_config *config,
		const char *prefix, int number);
int __init stm_pad_config_add_label_range(struct stm_pad_config *config,
		const char *prefix, int from, int to);

int __init stm_pad_config_add_sysconf(struct stm_pad_config *config,
		int regtype, int regnum, int lsb, int msb, int value);
#ifdef SYS_CFG
static inline int stm_pad_config_add_sys_cfg(struct stm_pad_config *config,
		int regnum, int lsb, int msb, int value)
{
	return stm_pad_config_add_sysconf(config, SYS_CFG, regnum, lsb, msb,
		value);
}
#endif

int __init stm_pad_config_add_pio(struct stm_pad_config *config,
		int port, int pin, int direction);



#endif