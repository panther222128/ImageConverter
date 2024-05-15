//
//  main.cpp
//  ImageConverter
//
//  Created by Horus on 5/15/24.
//

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <libheif/heif.h>

void heic_to_jpg(const std::string& heic_path, const std::string& jpg_path) {
    // Create a heif context
    struct heif_context* ctx = heif_context_alloc();
    
    // Read the HEIC file
    heif_context_read_from_file(ctx, heic_path.c_str(), nullptr);

    // Get a handle to the primary image
    struct heif_image_handle* handle;
    heif_context_get_primary_image_handle(ctx, &handle);

    // Decode the image
    struct heif_image* img;
    heif_decode_image(handle, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGB, nullptr);

    // Get image properties
    const int width = heif_image_get_width(img, heif_channel_interleaved);
    const int height = heif_image_get_height(img, heif_channel_interleaved);
    int stride;
    const uint8_t* data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);

    
    const int resized_width = 2016;
    const int resized_height = static_cast<int>(height * (resized_width / static_cast<double>(width)));
    
    cv::Mat image(height, width, CV_8UC3, const_cast<uint8_t*>(data));

    cv::Mat resized_image;
    cv::resize(image, resized_image, cv::Size(resized_width, resized_height));

    int text_position_x = 0;
    int text_position_y = 0;
    
    // Convert the color from RGB to BGR (OpenCV uses BGR by default)
    cv::cvtColor(resized_image, resized_image, cv::COLOR_RGB2BGR);
    
    if (resized_width > resized_height) {
        text_position_x = 1600;
        text_position_y = 1200;
    } else {
        text_position_x = 1600;
        text_position_y = 2000;
    }
    
    cv::putText(resized_image, "watermark text", cv::Point(text_position_x, text_position_y), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(128, 128, 128), 2);
    
    // Save the image as JPG
    cv::imwrite(jpg_path, resized_image);

    // Clean up
    heif_image_release(img);
    heif_image_handle_release(handle);
    heif_context_free(ctx);
}

int main() {
    std::string heic_path = "input: heic image path";
    std::string jpg_path = "output: jpg image path";

    heic_to_jpg(heic_path, jpg_path);

    std::cout << "Conversion complete!" << std::endl;
    return 0;
}
