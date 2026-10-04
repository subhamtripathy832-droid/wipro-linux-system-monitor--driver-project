# Driver Flow

```text
insmod
  |
  v
monitor_init()
  |
  +--> alloc_chrdev_region()
  |
  +--> cdev_add()
  |
  +--> class_create()
  |
  +--> device_create()
  |
  v
/dev/system_monitor
  |
  +--> open()
  |
  +--> write("status")
  |
  +--> read()
  |
  +--> release()
  |
  v
rmmod
  |
  v
monitor_exit()
  |
  +--> device_destroy()
  +--> class_destroy()
  +--> cdev_del()
  +--> unregister_chrdev_region()
```
