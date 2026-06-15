/*
 * RTOS — Multi-Task Sensor & Alert System using FreeRTOS
 * Intern: Beulah Jenifer P | URK23EC1047
 * CodSoft Embedded Systems Internship
 * Project: Real-Time Operating System (RTOS)
 * Board: ESP32 | Simulation: Wokwi
 */

#include <DHT.h>

#define DHT_PIN     4
#define DHT_TYPE    DHT22
#define ALERT_LED   2
#define HEARTBEAT_LED 15
#define TEMP_ALERT  35.0
#define HUM_ALERT   80.0

DHT dht(DHT_PIN, DHT_TYPE);

// ── Struct for sensor data ─────────────────────────────────
typedef struct {
  float temperature;
  float humidity;
  unsigned long timestamp;
} SensorData_t;

// ── RTOS Objects ────────────────────────────────────────────
QueueHandle_t sensorQueue;
SemaphoreHandle_t alertSemaphore;
SemaphoreHandle_t serialMutex;

// ── Task 1: Sensor Reader (Core 0, High Priority) ──────────
void TaskSensorReader(void *pvParameters) {
  for (;;) {
    SensorData_t data;
    data.temperature = dht.readTemperature();
    data.humidity    = dht.readHumidity();
    data.timestamp   = millis();

    if (!isnan(data.temperature) && !isnan(data.humidity)) {
      xQueueSend(sensorQueue, &data, portMAX_DELAY);

      if (data.temperature > TEMP_ALERT || data.humidity > HUM_ALERT) {
        xSemaphoreGive(alertSemaphore);
      }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// ── Task 2: Display Handler (Core 1, Medium Priority) ──────
void TaskDisplayHandler(void *pvParameters) {
  SensorData_t received;
  for (;;) {
    if (xQueueReceive(sensorQueue, &received, portMAX_DELAY) == pdTRUE) {
      if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
        Serial.println("┌────────────────────────────────────┐");
        Serial.print("│ [Core ");
        Serial.print(xPortGetCoreID());
        Serial.print("] Task: DisplayHandler           │\n");
        Serial.print("│ Time: "); Serial.print(received.timestamp);
        Serial.println(" ms                       │");
        Serial.print("│ Temp: "); Serial.print(received.temperature, 1);
        Serial.println(" C                          │");
        Serial.print("│ Hum:  "); Serial.print(received.humidity, 1);
        Serial.println(" %                          │");
        Serial.println("└────────────────────────────────────┘");
        xSemaphoreGive(serialMutex);
      }
    }
  }
}

// ── Task 3: Alert Monitor (Core 0, High Priority) ──────────
void TaskAlertMonitor(void *pvParameters) {
  for (;;) {
    if (xSemaphoreTake(alertSemaphore, portMAX_DELAY) == pdTRUE) {
      if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
        Serial.println("🚨🚨🚨 [Core 0] ALERT TASK TRIGGERED 🚨🚨🚨");
        xSemaphoreGive(serialMutex);
      }
      digitalWrite(ALERT_LED, HIGH);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      digitalWrite(ALERT_LED, LOW);
    }
  }
}

// ── Task 4: Heartbeat (Core 1, Low Priority) ───────────────
void TaskHeartbeat(void *pvParameters) {
  for (;;) {
    digitalWrite(HEARTBEAT_LED, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(HEARTBEAT_LED, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);

    if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
      Serial.print("💓 [Core ");
      Serial.print(xPortGetCoreID());
      Serial.print("] Heartbeat | Free Heap: ");
      Serial.print(esp_get_free_heap_size());
      Serial.println(" bytes");
      xSemaphoreGive(serialMutex);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ALERT_LED, OUTPUT);
  pinMode(HEARTBEAT_LED, OUTPUT);
  dht.begin();

  Serial.println("========================================");
  Serial.println("  RTOS Multi-Task Sensor & Alert System");
  Serial.println("  Beulah Jenifer P | CodSoft 2025");
  Serial.println("  ESP32 Dual-Core FreeRTOS Demo");
  Serial.println("========================================");
  Serial.println();

  // Create RTOS objects
  sensorQueue    = xQueueCreate(5, sizeof(SensorData_t));
  alertSemaphore = xSemaphoreCreateBinary();
  serialMutex    = xSemaphoreCreateMutex();

  // Create Tasks pinned to specific cores
  xTaskCreatePinnedToCore(TaskSensorReader,  "SensorReader",  4096, NULL, 3, NULL, 0);
  xTaskCreatePinnedToCore(TaskAlertMonitor,  "AlertMonitor",  2048, NULL, 3, NULL, 0);
  xTaskCreatePinnedToCore(TaskDisplayHandler,"DisplayHandler",4096, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskHeartbeat,     "Heartbeat",     2048, NULL, 1, NULL, 1);

  Serial.println("✅ All RTOS tasks created successfully!");
  Serial.println("   - SensorReader   (Core 0, Priority 3)");
  Serial.println("   - AlertMonitor   (Core 0, Priority 3)");
  Serial.println("   - DisplayHandler (Core 1, Priority 2)");
  Serial.println("   - Heartbeat      (Core 1, Priority 1)");
  Serial.println();
}

void loop() {
  // Empty - all work done in FreeRTOS tasks
  vTaskDelay(1000);
}
