/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include "msm_sensor.h"
#include "msm_sensor_driver.h"
#include "msm_cci.h"
#include "msm_camera_io_util.h"

#define s5k5e_SENSOR_NAME "s5k5e"
#define PLATFORM_DRIVER_NAME "msm_camera_s5k5e"
#define s5k5e_obj s5k5e_##obj

DEFINE_MSM_MUTEX(s5k5e_mut);

static struct msm_sensor_ctrl_t s5k5e_s_ctrl;

#define CONFIG_MSMB_CAMERA_DEBUG
#undef CDBG
#ifdef CONFIG_MSMB_CAMERA_DEBUG
#define CDBG(fmt, args...) pr_err("[CAM]"fmt, ##args)
#else
#define CDBG(fmt, args...) do { } while (0)
#endif

struct msm_sensor_power_setting s5k5e_power_setting[] = {
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VDIG,
		.config_val = 1,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VANA,
		.config_val = 1,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_VREG,
		.seq_val = CAM_VIO,
		.config_val = 1,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_RESET,
		.config_val = GPIO_OUT_HIGH,//GPIO_OUT_LOW,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_CLK,
		.seq_val = SENSOR_CAM_MCLK,
		.config_val = 23880000,
		.delay = 1,
	},

	{
		.seq_type = SENSOR_I2C_MUX,
		.seq_val = 0,
		.config_val = 0,
		.delay = 0,
	},
};

struct msm_sensor_power_setting s5k5e_power_down_setting[] = {
	{
		.seq_type = SENSOR_I2C_MUX,
		.seq_val = 0,
		.config_val = 0,
		.delay = 0,
	},
	{
		.seq_type = SENSOR_CLK,
		.seq_val = SENSOR_CAM_MCLK,
		.config_val = 23880000,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_RESET,
		.config_val = GPIO_OUT_LOW,//GPIO_OUT_LOW,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_VREG,
		.seq_val = CAM_VIO,
		.config_val = 0,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VANA,
		.config_val = 0,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VDIG,
		.config_val = 0,
		.delay = 1,
	},
};

static struct v4l2_subdev_info s5k5e_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_SBGGR10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,
	},
};

static const struct i2c_device_id s5k5e_i2c_id[] = {
	{s5k5e_SENSOR_NAME, (kernel_ulong_t)&s5k5e_s_ctrl},
	{ }
};

static int32_t msm_s5k5e_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	return msm_sensor_i2c_probe(client, id, &s5k5e_s_ctrl);
}

static struct msm_camera_i2c_client s5k5e_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static const struct of_device_id s5k5e_dt_match[] = {
	{.compatible = "htc,s5k5e", .data = &s5k5e_s_ctrl},
	{}
};


static struct i2c_driver s5k5e_i2c_driver = {
	.id_table = s5k5e_i2c_id,
	.probe  = msm_s5k5e_i2c_probe,
	.driver = {
		.name = s5k5e_SENSOR_NAME,
	},
};

MODULE_DEVICE_TABLE(of, s5k5e_dt_match);

static struct platform_driver s5k5e_platform_driver = {
	.driver = {
		.name = "htc,s5k5e",
		.owner = THIS_MODULE,
		.of_match_table = s5k5e_dt_match,
	},
};

static const char *s5k5eVendor = "Samsung";
static const char *s5k5eNAME = "s5k5e";
static const char *s5k5eSize = "5.0M";

static ssize_t sensor_vendor_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;

	sprintf(buf, "%s %s %s\n", s5k5eVendor, s5k5eNAME, s5k5eSize);
	ret = strlen(buf) + 1;

	return ret;
}

static DEVICE_ATTR(sensor, 0444, sensor_vendor_show, NULL);

static struct kobject *android_s5k5e;

static int s5k5e_sysfs_init(void)
{
	int ret ;
	pr_info("s5k5e:kobject creat and add\n");
	android_s5k5e = kobject_create_and_add("android_camera", NULL);
	if (android_s5k5e == NULL) {
		pr_info("s5k5e_sysfs_init: subsystem_register " \
		"failed\n");
		ret = -ENOMEM;
		return ret ;
	}
	pr_info("s5k5e:sysfs_create_file\n");
	ret = sysfs_create_file(android_s5k5e, &dev_attr_sensor.attr);
	if (ret) {
		pr_info("s5k5e_sysfs_init: sysfs_create_file " \
		"failed\n");
		kobject_del(android_s5k5e);
	}

	return 0 ;
}

static int32_t s5k5e_platform_probe(struct platform_device *pdev)
{
	int32_t rc = 0;
	const struct of_device_id *match;
	match = of_match_device(s5k5e_dt_match, &pdev->dev);

	/* HTC_START , add to fix Klocwork issue */
	if (match == NULL) {
		pr_err("%s: match is NULL\n", __func__);
		return -ENODEV;
	}
	/* HTC_END */

	rc = msm_sensor_platform_probe(pdev, match->data);
	return rc;
}

static int __init s5k5e_init_module(void)
{
	int32_t rc = 0;
	pr_info("%s:%d\n", __func__, __LINE__);
	rc = platform_driver_probe(&s5k5e_platform_driver,
		s5k5e_platform_probe);
	if (!rc) {
		s5k5e_sysfs_init();
		return rc;
	}
	pr_err("%s:%d rc %d\n", __func__, __LINE__, rc);
	return i2c_add_driver(&s5k5e_i2c_driver);
}

static void __exit s5k5e_exit_module(void)
{
	pr_info("%s:%d\n", __func__, __LINE__);
	if (s5k5e_s_ctrl.pdev) {
		msm_sensor_free_sensor_data(&s5k5e_s_ctrl);
		platform_driver_unregister(&s5k5e_platform_driver);
	} else
		i2c_del_driver(&s5k5e_i2c_driver);
	return;
}


static struct msm_sensor_fn_t s5k5e_sensor_func_tbl = {
	.sensor_power_up = msm_sensor_power_up,
	.sensor_power_down = msm_sensor_power_down,
	.sensor_match_id = msm_sensor_match_id,
	.sensor_config = msm_sensor_config,
};

static struct msm_sensor_ctrl_t s5k5e_s_ctrl = {
	.sensor_i2c_client = &s5k5e_sensor_i2c_client,
	.power_setting_array.power_setting = s5k5e_power_setting,
	.power_setting_array.size = ARRAY_SIZE(s5k5e_power_setting),
	.power_setting_array.power_down_setting = s5k5e_power_down_setting,
	.power_setting_array.size_down = ARRAY_SIZE(s5k5e_power_down_setting),
	.msm_sensor_mutex = &s5k5e_mut,
	.sensor_v4l2_subdev_info = s5k5e_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(s5k5e_subdev_info),
	.func_tbl = &s5k5e_sensor_func_tbl
};

MODULE_DEVICE_TABLE(of, s5k5e_dt_match);

module_init(s5k5e_init_module);
module_exit(s5k5e_exit_module);
MODULE_DESCRIPTION("s5k5e");
MODULE_LICENSE("GPL v2");
