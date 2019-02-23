/* Auto-generated config file usbd_cdc_acm_config.h */
#ifndef USBD_CDC_ACM_CONFIG_H
#define USBD_CDC_ACM_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> CDC ACM Device Descriptor

// <o> bcdUSB
// <0x0200=> USB 2.0 version
// <0x0210=> USB 2.1 version
// <id> usb_cdcd_acm_bcdusb
#ifndef CONF_USB_CDCD_ACM_BCDUSB
#define CONF_USB_CDCD_ACM_BCDUSB 0x200
#endif

// <o> bMaxPackeSize0
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_cdcd_acm_bmaxpksz0
#ifndef CONF_USB_CDCD_ACM_BMAXPKSZ0
#define CONF_USB_CDCD_ACM_BMAXPKSZ0 0x40
#endif

// <o> idVender <0x0000-0xFFFF>
// <id> usb_cdcd_acm_idvender
#ifndef CONF_USB_CDCD_ACM_IDVENDER
#define CONF_USB_CDCD_ACM_IDVENDER 0x3eb
#endif

// <o> idProduct <0x0000-0xFFFF>
// <id> usb_cdcd_acm_idproduct
#ifndef CONF_USB_CDCD_ACM_IDPRODUCT
#define CONF_USB_CDCD_ACM_IDPRODUCT 0x2404
#endif

// <o> bcdDevice <0x0000-0xFFFF>
// <id> usb_cdcd_acm_bcddevice
#ifndef CONF_USB_CDCD_ACM_BCDDEVICE
#define CONF_USB_CDCD_ACM_BCDDEVICE 0x100
#endif

// <o> iManufacturer <0x00-0xFF>
// <id> usb_cdcd_acm_imanufact
#ifndef CONF_USB_CDCD_ACM_IMANUFACT
#define CONF_USB_CDCD_ACM_IMANUFACT 0x0
#endif

// <o> iProduct <0x00-0xFF>
// <id> usb_cdcd_acm_iproduct
#ifndef CONF_USB_CDCD_ACM_IPRODUCT
#define CONF_USB_CDCD_ACM_IPRODUCT 0x0
#endif

// <o> iSerialNumber <0x00-0xFF>
// <id> usb_cdcd_acm_iserialnum
#ifndef CONF_USB_CDCD_ACM_ISERIALNUM
#define CONF_USB_CDCD_ACM_ISERIALNUM 0x0
#endif

// <o> bNumConfigurations <0x01-0xFF>
// <id> usb_cdcd_acm_bnumconfig
#ifndef CONF_USB_CDCD_ACM_BNUMCONFIG
#define CONF_USB_CDCD_ACM_BNUMCONFIG 0x1
#endif
// </h>

// <h> CDC ACM Configuration Descriptor

// <o> bConfigurationValue <0x01-0xFF>
// <id> usb_cdcd_acm_bconfigval
#ifndef CONF_USB_CDCD_ACM_BCONFIGVAL
#define CONF_USB_CDCD_ACM_BCONFIGVAL 0x1
#endif

// <o> iConfiguration <0x00-0xFF>
// <id> usb_cdcd_acm_iconfig
#ifndef CONF_USB_CDCD_ACM_ICONFIG
#define CONF_USB_CDCD_ACM_ICONFIG 0x0
#endif

// <o> bmAttributes
// <0x80=> Bus power supply, not support for remote wakeup
// <0xA0=> Bus power supply, support for remote wakeup
// <0xC0=> Self powered, not support for remote wakeup
// <0xE0=> Self powered, support for remote wakeup
// <id> usb_cdcd_acm_bmattri
#ifndef CONF_USB_CDCD_ACM_BMATTRI
#define CONF_USB_CDCD_ACM_BMATTRI 0x80
#endif

// <o> bMaxPower <0x00-0xFF>
// <id> usb_cdcd_acm_bmaxpower
#ifndef CONF_USB_CDCD_ACM_BMAXPOWER
#define CONF_USB_CDCD_ACM_BMAXPOWER 0x32
#endif
// </h>

// <h> CDC ACM Communication Interface Descriptor

// <o> bInterfaceNumber <0x00-0xFF>
// <id> usb_cdcd_acm_comm_bifcnum
#ifndef CONF_USB_CDCD_ACM_COMM_BIFCNUM
#define CONF_USB_CDCD_ACM_COMM_BIFCNUM 0x0
#endif

// <o> bAlternateSetting <0x00-0xFF>
// <id> usb_cdcd_acm_comm_baltset
#ifndef CONF_USB_CDCD_ACM_COMM_BALTSET
#define CONF_USB_CDCD_ACM_COMM_BALTSET 0x0
#endif

// <o> iInterface <0x00-0xFF>
// <id> usb_cdcd_acm_comm_iifc
#ifndef CONF_USB_CDCD_ACM_COMM_IIFC
#define CONF_USB_CDCD_ACM_COMM_IIFC 0x0
#endif

// <o> Interrupt IN Endpoint Address
// <0x81=> EndpointAddress = 0x81
// <0x82=> EndpointAddress = 0x82
// <0x83=> EndpointAddress = 0x83
// <0x84=> EndpointAddress = 0x84
// <0x85=> EndpointAddress = 0x85
// <0x86=> EndpointAddress = 0x86
// <0x87=> EndpointAddress = 0x87
// <id> usb_cdcd_acm_epaddr
#ifndef CONF_USB_CDCD_ACM_COMM_INT_EPADDR
#define CONF_USB_CDCD_ACM_COMM_INT_EPADDR 0x82
#endif

// <o> Interrupt IN Endpoint wMaxPacketSize
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_cdcd_acm_comm_int_maxpksz
#ifndef CONF_USB_CDCD_ACM_COMM_INT_MAXPKSZ
#define CONF_USB_CDCD_ACM_COMM_INT_MAXPKSZ 0x40
#endif

// <o> Interrupt IN Endpoint Interval <0x00-0xFF>
// <id> usb_cdcd_acm_comm_int_interval
#ifndef CONF_USB_CDCD_ACM_COMM_INT_INTERVAL
#define CONF_USB_CDCD_ACM_COMM_INT_INTERVAL 0xa
#endif
// </h>

// <h> CDC ACM Data Interface Descriptor

// <o> bInterfaceNumber <0x00-0xFF>
// <id> usb_cdcd_acm_data_bifcnum
#ifndef CONF_USB_CDCD_ACM_DATA_BIFCNUM
#define CONF_USB_CDCD_ACM_DATA_BIFCNUM 0x1
#endif

// <o> bAlternateSetting <0x00-0xFF>
// <id> usb_cdcd_acm_data_baltset
#ifndef CONF_USB_CDCD_ACM_DATA_BALTSET
#define CONF_USB_CDCD_ACM_DATA_BALTSET 0x0
#endif

// <o> iInterface <0x00-0xFF>
// <id> usb_cdcd_acm_data_iifc
#ifndef CONF_USB_CDCD_ACM_DATA_IIFC
#define CONF_USB_CDCD_ACM_DATA_IIFC 0x0
#endif

// <o> BULK IN Endpoint Address
// <0x81=> EndpointAddress = 0x81
// <0x82=> EndpointAddress = 0x82
// <0x83=> EndpointAddress = 0x83
// <0x84=> EndpointAddress = 0x84
// <0x85=> EndpointAddress = 0x85
// <0x86=> EndpointAddress = 0x86
// <0x87=> EndpointAddress = 0x87
// <id> usb_cdcd_acm_data_bulkin_epaddr
#ifndef CONF_USB_CDCD_ACM_DATA_BULKIN_EPADDR
#define CONF_USB_CDCD_ACM_DATA_BULKIN_EPADDR 0x81
#endif

// <o> BULK IN Endpoint wMaxPacketSize
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_cdcd_acm_data_builin_maxpksz
#ifndef CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ
#define CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ 0x40
#endif

// <o> BULK OUT Endpoint Address
// <0x01=> EndpointAddress = 0x01
// <0x02=> EndpointAddress = 0x02
// <0x03=> EndpointAddress = 0x03
// <0x04=> EndpointAddress = 0x04
// <0x05=> EndpointAddress = 0x05
// <0x06=> EndpointAddress = 0x06
// <0x07=> EndpointAddress = 0x07
// <id> usb_cdcd_acm_data_bulkout_epaddr
#ifndef CONF_USB_CDCD_ACM_DATA_BULKOUT_EPADDR
#define CONF_USB_CDCD_ACM_DATA_BULKOUT_EPADDR 0x1
#endif

// <o> BULK OUT Endpoint wMaxPacketSize
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_cdcd_acm_data_buckout_maxpksz
#ifndef CONF_USB_CDCD_ACM_DATA_BULKOUT_MAXPKSZ
#define CONF_USB_CDCD_ACM_DATA_BULKOUT_MAXPKSZ 0x40
#endif
// </h>

// <<< end of configuration section >>>

#endif // USBD_CDC_ACM_CONFIG_H
