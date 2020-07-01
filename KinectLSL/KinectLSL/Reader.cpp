#include <iostream>
#include "lsl_cpp.h"
#include <k4a\k4a.hpp>

int main() {
	std::cout << "Hello World" << std::endl;
	lsl::stream_info myInfo;
	uint32_t count = k4a_device_get_installed_count();
	std::cout << count << std::endl;
	k4a_device_t device = NULL;
	k4a_device_open(K4A_DEVICE_DEFAULT, &device);
	std::cout << "Check 1" << std::endl;
	size_t serial_size = 0;
	k4a_device_get_serialnum(device, NULL, &serial_size);
	std::cout << serial_size << std::endl;
	char* serial = (char*)(malloc(serial_size));
	k4a_device_get_serialnum(device, serial, &serial_size);
	printf("Opened device: %s\n", serial);
	free(serial);
	std::cout << "Check 2" << std::endl;
	k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
	config.camera_fps = K4A_FRAMES_PER_SECOND_5;
	config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
	config.color_resolution = K4A_COLOR_RESOLUTION_720P;
	k4a_device_close(device);
}