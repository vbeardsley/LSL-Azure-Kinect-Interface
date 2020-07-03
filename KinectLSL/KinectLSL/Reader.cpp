#include <iostream>
#include "lsl_cpp.h"
#include <k4a\k4a.hpp>

//Code Assumes little endian machine.
int main() {


    // Set up all the lab streaming layer data streams.
    lsl_streaminfo colorInfo = lsl_create_streaminfo("Color", "Image", 3686400, 5, lsl_channel_format_t::cft_int8, "ColorID");
    lsl_outlet colorOutlet = lsl_create_outlet(colorInfo, 0, 360);
    lsl_streaminfo depthInfo = lsl_create_streaminfo("Depth", "Image", 92160, 5, lsl_channel_format_t::cft_int16, "DepthID");
    lsl_outlet depthOutlet = lsl_create_outlet(depthInfo, 0, 360);
    lsl_streaminfo irInfo = lsl_create_streaminfo("IR", "Image", 92160, 5, lsl_channel_format_t::cft_int16, "IRID");
    lsl_outlet irOutlet = lsl_create_outlet(irInfo, 0, 360);


    // Number of images to send
    int reps = 100;

    // Hook up receiver streams to outlets with blank samples.
    char* tempArray = new char[3686400]();
    short* tempShorts = new short[92160]();
    std::cin.ignore();
    lsl_push_sample_c(colorOutlet, tempArray);
    std::cin.ignore();
    lsl_push_sample_s(depthOutlet, tempShorts);
    std::cin.ignore();
    lsl_push_sample_s(irOutlet, tempShorts);
    std::cin.ignore();
    delete[] tempArray;
    delete[] tempShorts;



    // Initialize device.
    char* serial_number = NULL;
    size_t serial_number_length = 0;
    k4a_device_t device = NULL;
    uint32_t device_count = k4a_device_get_installed_count();
    printf("Found %d connected devices:\n", device_count);
    if (device_count != 1)
    {
        printf("Unexpected number of devices found (%d)\n", device_count);
        return -1;
    }
    if (K4A_RESULT_SUCCEEDED != k4a_device_open(K4A_DEVICE_DEFAULT, &device))
    {
        printf("Failed to open device\n");
        return -1;
    }
    if (K4A_BUFFER_RESULT_TOO_SMALL != k4a_device_get_serialnum(device, NULL, &serial_number_length))
    {
        printf("Failed to get serial number length\n");
        k4a_device_close(device);
        device = NULL;
        return -1;
    }
    serial_number = (char*) malloc(serial_number_length);
    if (serial_number == NULL)
    {
        printf("Failed to allocate memory for serial number (%zu bytes)\n", serial_number_length);
        k4a_device_close(device);
        device = NULL;
        return -1;
    }
    if (K4A_BUFFER_RESULT_SUCCEEDED != k4a_device_get_serialnum(device, serial_number, &serial_number_length))
    {
        printf("Failed to get serial number\n");
        free(serial_number);
        serial_number = NULL;
        k4a_device_close(device);
        device = NULL;
        return -1;
    }
    printf("Device \"%s\"\n", serial_number);




    //Device configuration
    k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    config.camera_fps = K4A_FRAMES_PER_SECOND_5;
    config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
    config.color_resolution = K4A_COLOR_RESOLUTION_720P;
    config.depth_mode = K4A_DEPTH_MODE_NFOV_2X2BINNED;
    config.synchronized_images_only = true;
    if (K4A_RESULT_SUCCEEDED != k4a_device_start_cameras(device, &config))
    {
        printf("Failed to start device\n");
        return -1;
    }


    //Send {reps} images.
    for (int i = 0; i < reps; i++) {
        printf("%d\n", i);
        k4a_capture_t capture;
        switch (k4a_device_get_capture(device, &capture, 2000))
        {
        case K4A_WAIT_RESULT_SUCCEEDED:
            break;
        case K4A_WAIT_RESULT_TIMEOUT:
            printf("Timed out waiting for a capture\n");
            break;
        case K4A_WAIT_RESULT_FAILED:
            printf("Failed to read a capture\n");
            return -1;
        }

        k4a_image_t color = k4a_capture_get_color_image(capture);
        k4a_image_t depth = k4a_capture_get_depth_image(capture);
        k4a_image_t ir = k4a_capture_get_ir_image(capture);

        if (color != NULL) {
            uint8_t* colorBuffer = k4a_image_get_buffer(color);
            char* colorArray = (char*)colorBuffer;
            lsl_push_sample_c(colorOutlet, colorArray);
        }
        if (depth != NULL) {
            uint8_t* depthBuffer = k4a_image_get_buffer(depth);
            int16_t* depthArray = (int16_t*)depthBuffer;
            int16_t temp[92160] = { 0 };
            lsl_push_sample_s(depthOutlet, depthArray);
        }
        if (ir != NULL) {
            uint8_t* irBuffer = k4a_image_get_buffer(ir);
            int16_t* irArray = (int16_t*)irBuffer;
            lsl_push_sample_s(irOutlet, irArray);
        }
        k4a_image_release(ir);
        k4a_image_release(depth);
        k4a_image_release(color);
        k4a_capture_release(capture);
    }


    //Shut down device.
    k4a_device_stop_cameras(device);
    k4a_device_close(device);
    free(serial_number);

    //Wait until the receiver is done, then shut down lsl.
    std::cin.ignore();
    
    lsl_destroy_outlet(colorOutlet);
    lsl_destroy_streaminfo(colorInfo);
    lsl_destroy_outlet(depthOutlet);
    lsl_destroy_streaminfo(depthInfo);
    lsl_destroy_outlet(irOutlet);
    lsl_destroy_streaminfo(irInfo);
}