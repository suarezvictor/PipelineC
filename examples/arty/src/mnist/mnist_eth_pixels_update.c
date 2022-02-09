#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// Build like:
// g++ mnist_eth_pixels_update.c -I ~/pipelinec_output -o  mnist_eth_pixels_update

#include "mnist-neural-network-plain-c-master/mnist_file.c"
/**
 * Downloaded from: http://yann.lecun.com/exdb/mnist/
 */
const char * train_images_file = "mnist-neural-network-plain-c-master/data/train-images-idx3-ubyte";
const char * train_labels_file = "mnist-neural-network-plain-c-master/data/train-labels-idx1-ubyte";
const char * test_images_file = "mnist-neural-network-plain-c-master/data/t10k-images-idx3-ubyte";
const char * test_labels_file = "mnist-neural-network-plain-c-master/data/t10k-labels-idx1-ubyte";

// 'Software' side of ethernet
#include "../eth/eth_sw.c"

// How to send inputs and outputs over eth
#include "pixels_update.h"
#include "type_bytes_t.h/pixels_update_t_bytes_t.h/pixels_update_t_bytes.h" // Autogenerated
void write_update(pixels_update_t* input)
{
 // Copy into buffer
 uint8_t buffer[pixels_update_t_SIZE];
 pixels_update_t_to_bytes(input, buffer);
 // Send buffer
 eth_write(buffer, pixels_update_t_SIZE);  
}
/*
void output_read(work_outputs_t* output)
{
  // Read buffer
  uint8_t buffer[work_outputs_t_SIZE];
  size_t rd_size = work_outputs_t_SIZE;
  eth_read(buffer, &rd_size);
  if(rd_size != work_outputs_t_SIZE)
  {
    printf("Did not receive enough bytes! Expected %d, got %ld\n",work_outputs_t_SIZE,rd_size);
  }
  // Copy from buffer
  bytes_to_work_outputs_t(buffer,output);
}
*/

int main(int argc, char *argv[])
{
    mnist_dataset_t * train_dataset, * test_dataset;

    // Read the datasets from the files
    train_dataset = mnist_get_dataset(train_images_file, train_labels_file);
    test_dataset = mnist_get_dataset(test_images_file, test_labels_file);

    mnist_image_t test_image = test_dataset->images[0];
    uint8_t test_label = test_dataset->labels[0];
    printf("# Pixels for test image labeled as: %d\n", test_label);
    /*printf("pixels = [\n");
    for (i = 0; i < MNIST_IMAGE_SIZE; i++) {
        uint8_t p = test_image.pixels[i];
        printf("%d, ", p);
    }
    printf("\n");*/

    printf("Sending pixels...\n");

    // Init msgs to/from FPGA
    init_eth();

    // Write each pixel update
    uint16_t addr;
    for (addr = 0; addr < MNIST_IMAGE_SIZE; addr += N_PIXELS_PER_UPDATE) {
        pixels_update_t update;
        update.addr = addr;
        int i;
        for(i=0;i<N_PIXELS_PER_UPDATE;i++)
        {
            update.pixels[i] = test_image.pixels[addr+i];
        }
        write_update(&update);
    }

    printf("Done.\n");

    // Close eth to/from FPGA
	close_eth();

    // Cleanup
    mnist_free_dataset(train_dataset);
    mnist_free_dataset(test_dataset);

    return 0;
}
