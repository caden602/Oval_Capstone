import struct

def bytes_to_package(package, buf):
    # Get error bits first
    package['bme_error'] = (buf[28]) & 1
    package['adxl_error'] = (buf[28] >> 1) & 1
    package['lis3mdl_error'] = (buf[28] >> 2) & 1

    if not package['bme_error']:
        # Convert temperature
        temp = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0]
        package['bme_data']['temperature'] = struct.unpack('f', struct.pack('I', temp))[0]

        # Convert pressure
        pressure = (buf[7] << 24) | (buf[6] << 16) | (buf[5] << 8) | buf[4]
        package['bme_data']['pressure'] = pressure

        # Convert humidity
        humidity = (buf[11] << 24) | (buf[10] << 16) | (buf[9] << 8) | buf[8]
        package['bme_data']['humidity'] = struct.unpack('f', struct.pack('I', humidity))[0]

        # Convert gas resistance
        gas_res = (buf[15] << 24) | (buf[14] << 16) | (buf[13] << 8) | buf[12]
        package['bme_data']['gas_resistance'] = gas_res
    else:
        print("BME Data error!")

    if not package['adxl_error']:
        # Convert ADXL data
        x = (buf[17] << 8) | buf[16]
        package['adxl_data']['x'] = x

        y = (buf[19] << 8) | buf[18]
        package['adxl_data']['y'] = y

        z = (buf[21] << 8) | buf[20]
        package['adxl_data']['z'] = z
    else:
        print("ADXL Data error!")

    if not package['lis3mdl_error']:
        # Convert LIS3MDL data
        x_mag = (buf[23] << 8) | buf[22]
        package['lis3mdl_data']['x'] = x_mag

        y_mag = (buf[25] << 8) | buf[24]
        package['lis3mdl_data']['y'] = y_mag

        z_mag = (buf[27] << 8) | buf[26]
        package['lis3mdl_data']['z'] = z_mag
    else:
        print("LIS3MDL Data error!")

    # Convert timestamp
    time_stamp = (buf[32] << 24) | (buf[31] << 16) | (buf[30] << 8) | buf[29]
    package['time_stamp'] = time_stamp
