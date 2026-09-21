#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"

#include "i2cdev.h" // dependencia do sensor
#include "bmp180.h"

#define SDA_GPIO GPIO_NUM_9
#define SCL_GPIO GPIO_NUM_8

static const char *TAG = "BMP180"; // para os logs


void setup_sensor(bmp180_dev_t *dev)
{
    // Inicializa a dependencia I2C do sensor primeiro
    ESP_ERROR_CHECK(i2cdev_init());

    // Configura e checa os parametros de comunicacao do sensor
    ESP_ERROR_CHECK(
        bmp180_init_desc(
            dev,
            I2C_NUM_0, // controlador
            SDA_GPIO,
            SCL_GPIO
        )
    );

    // Inicializa o sensor
    ESP_ERROR_CHECK(bmp180_init(dev));

    ESP_LOGI(TAG, "BMP180 inicializado com sucesso!");
    ESP_LOGI(TAG, "SDA: GPIO %d | SCL: GPIO %d",
             SDA_GPIO, SCL_GPIO);
}


void app_main(void)
{
    bmp180_dev_t dev = {0}; // representacao do sensor, iniciado em 0 pra evitar valores aleatorios

    float temperature;
    uint32_t pressure;

    // Faz o setup do sensor
    setup_sensor(&dev);

    // Check constante de leitura
    while (1)
    {
        esp_err_t result = bmp180_measure(
            &dev,
            &temperature,
            &pressure,
            BMP180_MODE_STANDARD
        );

        if (result == ESP_OK)
        {
            printf("Temperatura: %.2f C\n", temperature);
            printf("Pressao: %lu Pa\n", (unsigned long) pressure);
            printf("Pressao: %.2f hPa\n\n", pressure / 100.0);
        }
        else
        {
            ESP_LOGE(
                TAG,
                "Erro ao ler BMP180: %s",
                esp_err_to_name(result)
            );
        }

        // Espera 2 secs pra proxima leitura
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}