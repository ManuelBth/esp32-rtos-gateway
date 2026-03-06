/**
 * @file config_data.h
 * @brief Estructuras de datos globales del sistema
 * @details Define todas las estructuras de configuración, sensores, estados
 *          y la variable global `sistema` que es el núcleo del estado de la aplicación.
 */

#ifndef CONFIG_DATA_H
#define CONFIG_DATA_H

#include <Arduino.h>

/**
 * @brief Máximo número de cultivos permitidos
 */
#define MAX_CULTIVOS 8

// ============================================
// ESTRUCTURAS DE CONFIGURACIÓN
// ============================================

/**
 * @brief Configuración de credenciales WiFi
 */
struct WiFiConfig {
    char ssid[64];
    char password[64];
};

/**
 * @brief Configuración del broker MQTT
 */
struct MQTTConfig {
    char broker[128];
    uint16_t port;
    char topic[128];
    char user_id[32];
    char username[64];
    char password[64];
};

/**
 * @brief Información del nivel de batería
 */
struct BateriaInfo {
    int nivel;
};

/**
 * @brief Configuración de un cultivo individual
 * @note Cada cultivo puede tener hasta 16 variables monitoreadas
 */
struct Cultivo {
    char nombre[64];
    String variables[16];
};

/**
 * @brief Gestión de múltiples cultivos
 */
struct CultivosConfig {
    Cultivo cultivos[MAX_CULTIVOS];
    uint8_t numCultivos;
};

// ============================================
// ESTRUCTURAS DE SENSORES
// ============================================

/**
 * @brief Datos del sensor DHT22 (temperatura y humedad ambiente)
 */
struct DHT22Data {
    float temperatura;
    float humedad;
    bool valido;
};

/**
 * @brief Datos del sensor DS18B20 (temperatura del suelo)
 */
struct DS18B20Data {
    float temperatura;
    bool valido;
};

/**
 * @brief Datos del sensor RS485 NPK (suelo)
 * @details Incluye nutrientes: Nitrógeno (N), Fósforo (P), Potasio (K)
 */
struct RS485Data {
    float temperatura;
    float humedad;
    float ec;
    float ph;
    float nitrogen;
    float phosphorus;
    float potassium;
    bool valido;
};

/**
 * @brief Datos agregados de todos los sensores
 */
struct SensoresData {
    DHT22Data dht22;
    DS18B20Data ds18b20;
    RS485Data rs485;
};

// ============================================
// ESTRUCTURAS DE DISPOSITIVO Y TIEMPO
// ============================================

/**
 * @brief Información estática del dispositivo
 */
struct DispositivoInfo {
    char mac[18];
    char vFirmware[16];
    char vHardware[16];
    char serie[32];
    char modelo[64];
    int heartbeat;
    boolean sleepMode;
    BateriaInfo bateria;
};

/**
 * @brief Datos de tiempo actual del sistema
 */
struct TiempoData {
    uint16_t anho;
    uint8_t mes;
    uint8_t dia;
    uint8_t hora;
    uint8_t minuto;
    uint8_t segundo;
    unsigned long epoch;
    char formato[32];
};

// ============================================
// ESTRUCTURAS DE CONTROL
// ============================================

/**
 * @brief Estados de conectividad del sistema
 */
struct EstadosConexion {
    bool wifiConectado;
    bool mqttConectado;
    bool internetDisponible;
    unsigned long ultimaVerificacionWifi;
    unsigned long ultimaVerificacionMqtt;
};

/**
 * @brief Control de interacción del usuario con la UI
 */
struct ControlUsuario {
    bool envioTelemetriaHabilitado;
    bool lecturaSensoresManual;
    bool lecturaActivaUI;
    bool rutinaMQTTActiva;
    uint8_t cultivoSeleccionado;
    uint8_t tiempoTelemetriaMin;
    bool esperandoConfig;
};

/**
 * @brief Intervalos de ejecución de tareas (en milisegundos)
 */
struct IntervalosConfig {
    uint32_t telemetria;
    uint32_t heartbeat;
    uint32_t lecturaSensores;
    uint32_t lecturaRTC;
    uint32_t verificacionConexion;
};

/**
 * @brief Timestamps de última ejecución de cada tarea
 */
struct UltimaEjecucion {
    unsigned long telemetria;
    unsigned long heartbeat;
    unsigned long lecturaSensores;
    unsigned long lecturaRTC;
    unsigned long verificacionConexion;
};

/**
 * @brief Metadata del sistema para persistencia
 */
struct SistemaMetadata {
    unsigned long ultima_actualizacion;
    unsigned long ultima_config_servidor;
};

// ============================================
// ESTRUCTURA PRINCIPAL
// ============================================

/**
 * @brief Estructura principal que contiene todo el estado del sistema
 * @details Es la única variable global mutable del sistema. Todos los
 *          módulos leen y escriben datos aquí.
 */
struct SistemaConfig {
    WiFiConfig wifi;
    MQTTConfig mqtt;
    CultivosConfig cultivos;
    SensoresData sensores;
    DispositivoInfo dispositivo;
    TiempoData tiempo;
    EstadosConexion estados;
    ControlUsuario controlUsuario;
    IntervalosConfig intervalos;
    UltimaEjecucion ultimaEjecucion;
    SistemaMetadata sistema;
};

// ============================================
// VARIABLE GLOBAL
// ============================================

/**
 * @brief Variable global con todo el estado del sistema
 * @details Acceso: #include "../core/config_data.h" → usar `sistema`
 */
extern SistemaConfig sistema;

#endif