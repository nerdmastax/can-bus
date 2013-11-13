#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <stdint.h>
#include <string.h>

void processMessage(const uint8_t*);


/*----------------------------------------------------------------------*/
int main(int argc, char*argv[])
{
  int res                      = 0;  /* return codes from libusb functions */
  libusb_device_handle* handle = 0;  /* handle for USB device */
  int kernelDriverDetached     = 0;  /* Set to 1 if kernel driver detached */
  int numBytes                 = 0;  /* Actual bytes transferred. */
  uint8_t buffer[64];                /* 64 byte transfer buffer */
  int repeat                   = 0;

  /* Initialise libusb. */
  res = libusb_init(0);
  if (res != 0)
  {
    fprintf(stderr, "Error initialising libusb.\n");
    return 1;
  }

  /* Get the first device with the matching Vendor ID and Product ID. If
   * intending to allow multiple demo boards to be connected at once, you
   * will need to use libusb_get_device_list() instead. Refer to the libusb
   * documentation for details. */
  handle = libusb_open_device_with_vid_pid(0, 0x04d8, 0x0070);
  if (!handle)
  {
    fprintf(stderr, "Unable to open device.\n");
    return 1;
  }

  /* Check whether a kernel driver is attached to interface #0. If so, we'll 
   * need to detach it.
   */
  if (libusb_kernel_driver_active(handle, 0))
  {
    res = libusb_detach_kernel_driver(handle, 0);
    if (res == 0)
    {
      kernelDriverDetached = 1;
    }
    else
    {
      fprintf(stderr, "Error detaching kernel driver.\n");
      return 1;
    }
  }

  /* Claim interface #0. */
  res = libusb_claim_interface(handle, 0);
  if (res != 0)
  {
    fprintf(stderr, "Error claiming interface.\n");
    return 1;
  }

while (repeat < 1000)
{


  /* We can now send and receive messages. For example, set normal mode. */
  memset(buffer, 0, 64);
  buffer[52] = 0x01;  /* Number of CAN messages */ 
  buffer[0] = 0x00; /* Standard ID = ID Bits 10 through 3 (7 - 0) */
  buffer[1] = 0x00; /* Standard ID = ID Bits 2 through 0 (7, 6, 5), bit 3 is enabled for extended ID*/
  buffer[2] = 0x00; /* Extended ID */
  buffer[3] = 0x00; /* Extended ID */
  buffer[4] = 0x08; /* Data Length */
  buffer[5] = 0x63; /* Data 0 */ 
  buffer[6] = 0x00; /* Data 1 andd so on */
  buffer[7] = 0x00; 
  buffer[8] = 0x00; 
  buffer[9] = 0x00; 
  buffer[10] = 0x00;
  buffer[11] = 0x00;
  buffer[12] = 0x00;


  /* Send the message to endpoint 1 with a 100ms timeout. */
  res = libusb_interrupt_transfer(handle, 1, buffer, 64, &numBytes, 100);
  if (res == 0)
  {
    printf("%d bytes transmitted successfully.\n", numBytes);
  }
  else
  {
    fprintf(stderr, "Error sending message to device.\n");
  }

sleep(1);
++repeat;
}


  /* Release interface #0. */
 res = libusb_release_interface(handle, 0);
  if (0 != res)
  {
    fprintf(stderr, "Error releasing interface.\n");
  }

  /* If we detached a kernel driver from interface #0 earlier, we'll now 
   * need to attach it again.  */
  if (kernelDriverDetached)
  {
    libusb_attach_kernel_driver(handle, 0);
  }

  /* Shutdown libusb. */
  libusb_exit(0);

  return 0;
}


