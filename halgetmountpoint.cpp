

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "config.h"

#if HAVE_HAL

#include <libhal.h>

char *HALGetMountPoint(const char *udi)
{
	char *result=NULL;
	DBusError error;
	LibHalContext *hal_ctx;

	dbus_error_init (&error);	
	if((hal_ctx = libhal_ctx_new ()))
	{
		if((libhal_ctx_set_dbus_connection (hal_ctx, dbus_bus_get (DBUS_BUS_SYSTEM, &error))))
		{
			if((libhal_ctx_init (hal_ctx, &error)))
			{
				char *str;
				if(str = libhal_device_get_property_string (hal_ctx, udi, "volume.mount_point", &error))
				{
					result=strdup(str);
					libhal_free_string (str);
				}
			}
		}
	}
	return(result);
}

#endif
