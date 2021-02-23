#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt.h"
#include "driver/gpio.h"

#define RMT_RX_CHANNEL   0     /*!< RMT channel for receiver */
#define RMT_RX_GPIO_NUM  23    /*!< GPIO number for receiver */
#define RMT_CLK_DIV      8     /*!< RMT counter clock divider */
#define RMT_TICK_US    (80000000/RMT_CLK_DIV/1000000)   /*!< RMT counter value for 10 us.(Source clock is APB clock) */
#define PPM_IMEOUT_US  3500   /*!< RMT receiver timeout value(us) */


/**
 * @brief PPM receiver task
 *
 */
static void ppm_rx_task(void *pvParameter)
{
    int channel = RMT_RX_CHANNEL;

    RingbufHandle_t rb = NULL;
    //get RMT RX ringbuffer
    rmt_get_ringbuf_handle(channel, &rb);
    rmt_rx_start(channel, 1);
    while(rb) {
        size_t rx_size = 0;
        int channels;
        //try to receive data from ringbuffer.
        //RMT driver will push all the data it receives to its ringbuffer.
        //We just need to parse the value and return the spaces of ringbuffer.
        rmt_item32_t* item = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, 1000);
        if(item) {
                channels = (rx_size / 4) - 1;
                printf("PPM RX %d (%d) channels: ", channels, rx_size);
                for (int i = 0; i < channels; i++)
                        printf("%04d ", ((item+i)->duration1 + (item+i)->duration0) / RMT_TICK_US);

                printf("\n");

            vRingbufferReturnItem(rb, (void*) item);
        }
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    gpio_pulldown_en(RMT_RX_GPIO_NUM);
    gpio_set_direction(RMT_RX_GPIO_NUM, GPIO_MODE_INPUT);

    rmt_config_t rmt_rx;
    rmt_rx.channel = RMT_RX_CHANNEL;
    rmt_rx.gpio_num = RMT_RX_GPIO_NUM;
    rmt_rx.clk_div = RMT_CLK_DIV;
    rmt_rx.mem_block_num = 4;
    rmt_rx.rmt_mode = RMT_MODE_RX;
    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = 100;
    rmt_rx.rx_config.idle_threshold = PPM_IMEOUT_US * (RMT_TICK_US);
    rmt_config(&rmt_rx);

    // from: https://stackoverflow.com/questions/56127996/how-to-receive-long-data-frame-by-esp32-rmt-ringbuffer
//    rmt_set_mem_block_num((rmt_channel_t) 0, 4);

    rmt_driver_install(rmt_rx.channel, 1000, 0);

    xTaskCreate(ppm_rx_task, "ppm_rx_task", 2048, NULL, 10, NULL);
    printf("PPM RX initialized\n");
}


