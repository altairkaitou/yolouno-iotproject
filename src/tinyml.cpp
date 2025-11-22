#include "tinyml.h"

// Globals, for the convenience of one-shot setup.
namespace
{
    tflite::ErrorReporter *error_reporter = nullptr;
    const tflite::Model *model = nullptr;
    tflite::MicroInterpreter *interpreter = nullptr;
    TfLiteTensor *input = nullptr;
    TfLiteTensor *output = nullptr;
    constexpr int kTensorArenaSize = 8 * 1024; // Adjust size based on your model
    uint8_t tensor_arena[kTensorArenaSize];
} // namespace

void setupTinyML()
{
    Serial.println("TensorFlow Lite Init....");
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    model = tflite::GetModel(dht_anomaly_model_tflite); // g_model_data is from model_data.h
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        error_reporter->Report("Model provided is schema version %d, not equal to supported version %d.",
                               model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;

    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        error_reporter->Report("AllocateTensors() failed");
        return;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);

    Serial.println("TensorFlow Lite Micro initialized on ESP32.");
}

void tiny_ml_task(void *pvParameters)
{
    setupTinyML();

    SensorPacket pkt;   // nhận dữ liệu từ queue

    while (1)
    {
        // Nhận dữ liệu sensor từ queue
        if (xQueueReceive(sensorQueue, &pkt, portMAX_DELAY) == pdTRUE)
        {
            float t = pkt.temperature;
            float h = pkt.humidity;

            // Gán input vào model
            input->data.f[0] = t;
            input->data.f[1] = h;

            // Chạy inference
            if (interpreter->Invoke() != kTfLiteOk)
            {
                error_reporter->Report("Invoke failed");
                continue;
            }

            float result = output->data.f[0];

            Serial.print("[TinyML] Inference: ");
            Serial.println(result);
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}