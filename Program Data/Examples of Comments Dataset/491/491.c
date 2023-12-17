/*
This function is responsible for adding a device and setting its properties based on the provided options.

Here's the detail:
- It first retrieves the driver name from the options and validates its existence.
- Then it looks up the corresponding ObjectClass based on the driver name to ensure it's a valid device model.
- If the device class is abstract or cannot be instantiated with qdev_device_add yet, it reports an error and returns.
- It then finds the bus for the device based on the options, ensuring that the specified bus is valid for the device type.
- After creating the device and setting its properties, it adds the device to the appropriate parent bus and sets its ID.
- It then checks for any errors during the device initialization process and reports them if encountered.

Usage needs to notice:
- Ensure that the options passed to this function contain the necessary parameters such as "driver" and "bus" if applicable.
- Handle any error conditions reported by the function, such as missing parameters, invalid device types, or bus-related errors.
- Understand the structure and format of the options and the expected behavior of the function in creating and initializing devices.
*/
DeviceState *qdev_device_add(QemuOpts *opts)

{

    ObjectClass *oc;

    DeviceClass *dc;

    const char *driver, *path, *id;

    DeviceState *dev;

    BusState *bus = NULL;

    Error *err = NULL;



    driver = qemu_opt_get(opts, "driver");

    if (!driver) {

        qerror_report(QERR_MISSING_PARAMETER, "driver");

        return NULL;

    }



    /* find driver */

    oc = object_class_by_name(driver);

    if (!oc) {

        const char *typename = find_typename_by_alias(driver);



        if (typename) {

            driver = typename;

            oc = object_class_by_name(driver);

        }

    }



    if (!object_class_dynamic_cast(oc, TYPE_DEVICE)) {

        qerror_report(ERROR_CLASS_GENERIC_ERROR,

                      "'%s' is not a valid device model name", driver);

        return NULL;

    }



    if (object_class_is_abstract(oc)) {

        qerror_report(QERR_INVALID_PARAMETER_VALUE, "driver",

                      "non-abstract device type");

        return NULL;

    }



    dc = DEVICE_CLASS(oc);

    if (dc->cannot_instantiate_with_device_add_yet) {

        qerror_report(QERR_INVALID_PARAMETER_VALUE, "driver",

                      "pluggable device type");

        return NULL;

    }



    /* find bus */

    path = qemu_opt_get(opts, "bus");

    if (path != NULL) {

        bus = qbus_find(path);

        if (!bus) {

            return NULL;

        }

        if (!object_dynamic_cast(OBJECT(bus), dc->bus_type)) {

            qerror_report(QERR_BAD_BUS_FOR_DEVICE,

                          driver, object_get_typename(OBJECT(bus)));

            return NULL;

        }

    } else if (dc->bus_type != NULL) {

        bus = qbus_find_recursive(sysbus_get_default(), NULL, dc->bus_type);

        if (!bus) {

            qerror_report(QERR_NO_BUS_FOR_DEVICE,

                          dc->bus_type, driver);

            return NULL;

        }

    }

    if (qdev_hotplug && bus && !bus->allow_hotplug) {

        qerror_report(QERR_BUS_NO_HOTPLUG, bus->name);

        return NULL;

    }



    /* create device, set properties */

    dev = DEVICE(object_new(driver));



    if (bus) {

        qdev_set_parent_bus(dev, bus);

    }



    id = qemu_opts_id(opts);

    if (id) {

        dev->id = id;

    }

    if (qemu_opt_foreach(opts, set_property, dev, 1) != 0) {

        object_unparent(OBJECT(dev));

        object_unref(OBJECT(dev));

        return NULL;

    }

    if (dev->id) {

        object_property_add_child(qdev_get_peripheral(), dev->id,

                                  OBJECT(dev), NULL);

    } else {

        static int anon_count;

        gchar *name = g_strdup_printf("device[%d]", anon_count++);

        object_property_add_child(qdev_get_peripheral_anon(), name,

                                  OBJECT(dev), NULL);

        g_free(name);

    }



    dev->opts = opts;

    object_property_set_bool(OBJECT(dev), true, "realized", &err);

    if (err != NULL) {

        qerror_report_err(err);

        error_free(err);

        dev->opts = NULL;

        object_unparent(OBJECT(dev));

        object_unref(OBJECT(dev));

        qerror_report(QERR_DEVICE_INIT_FAILED, driver);

        return NULL;

    }

    return dev;

}
