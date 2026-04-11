import pywinusb.hid as hid
import time

VENDOR_ID = 0x1209
PRODUCT_ID = 0x2345

# Filter by Vendor ID / Product ID
device = hid.HidDeviceFilter(vendor_id = VENDOR_ID, product_id = PRODUCT_ID).get_devices()[0]

if device:
    device.open()

    for in_reports in device.find_input_reports():
        print("IN reports: ", in_reports)

    for out_reports in device.find_output_reports():
        print("OUT report: ", out_reports)

    report = device.find_output_reports()[0]
    
    try:
        while True:
            for i in range (0, 255, 16):
                report.set_raw_data([4, i])  # first byte is report ID
                report.send()
                time.sleep(0.1)
    except KeyboardInterrupt:
        pass

    device.close()
else:
    print("Device not found")