#ifndef __ZEPHYR_ORB_ORB_LOGGER_H__
#define __ZEPHYR_ORB_ORB_LOGGER_H__

enum EnOrbLoggerId
{
    en_orb_inited = 0x00001000, //¼ÇÂ¼,OrbÕ¼1024¸ö 
    en_obj_not_registered,
    en_service_registered,
    en_obj_registered,
    en_obj_unregistered,
    en_obj_registered_run,
    en_stop_service,
};

#endif
