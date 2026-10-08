# Mapeo de Pines y Señales de Referencia (Hardware Fabricado)

> **Red Sísmica del Austro (RSA) — Universidad de Cuenca**  
> Proyecto: `RSA-PPP-2026-13` • Multiplexor Geotécnico de 24 Canales  
> Documento de referencia técnica derivado de los esquemáticos KiCad y prototipos funcionales.

---

## 1. Nodo Maestro: Espressif ESP32-WROOM-32

Mapeo de señales implementado en la tarjeta adaptadora [`acople_esp32`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/kicad/acople_esp32/) y validado en el prototipo [`legacy_micropython`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/esp32/legacy_micropython/):

### 1.1. Instrumentación Analógica (ADC 24-bit HX711)
| Pin ESP32 | Señal HX711 | Dirección | Descripción |
| :---: | :---: | :---: | :--- |
| **GPIO 4** | `DOUT` (DT) | Entrada | Línea serial de datos del HX711 (pulso bajo = dato listo) |
| **GPIO 33** | `PD_SCK` (SCK) | Salida | Reloj de lectura generado por el ESP32 (24 pulsos + 1 para Ganancia 128) |

> [!IMPORTANT]
> Mantener `PD_SCK` en nivel bajo durante reposo. Si permanece en nivel alto más de $60\,\mu\text{s}$, el HX711 entra en modo de bajo consumo (*Power Down*).

### 1.2. Banco de Conmutación de Relés (K1..K5)
Controla los transistores que conmutan la topología del puente de Wheatstone y la selección de modo:

| Pin ESP32 | Relé / Función | Estado Lógico | Acción Eléctrica |
| :---: | :---: | :---: | :--- |
| **GPIO 13** | Habilitación Medición | `0` = Deshabilitado<br>`1` = Habilitado | Conecta la salida del puente al convertidor HX711 |
| **GPIO 14** | Alimentación Puente | `0` = Desenergizado<br>`1` = Energizado (3.3V) | Aplica tensión de excitación al lazo del sensor |
| **GPIO 16** | Relé Selección Modo (Bit 0) | Conmuta rama | Junto con GPIO 17 define modo Deformación vs Temperatura |
| **GPIO 17** | Relé Selección Modo (Bit 1) | Conmuta rama | Ver tabla de verdad de modos |
| **GPIO 25** | Selección Tipo Sensor | `0` = Sensor Tipo 1<br>`1` = Sensor Tipo 2 | Adapta la resistencia puente según tecnología de galga |

#### Tabla de Verdad de Modos de Medición (GPIO 16 / 17):
| GPIO 17 | GPIO 16 | Modo Seleccionado | Estado |
| :---: | :---: | :--- | :--- |
| **0** | **0** | **Deformación Mecánica ($\mu\varepsilon$)** | Modo Nominal |
| `0` | `1` | Configuración Inválida 1 | Prohibido en software |
| `1` | `0` | Configuración Inválida 2 | Prohibido en software |
| **1** | **1** | **Temperatura Interna ($^\circ\text{C}$)** | Modo Nominal |

### 1.3. Bus SPI — Datalogger Tarjeta MicroSD
| Pin ESP32 | Señal MicroSD | Descripción |
| :---: | :---: | :--- |
| **GPIO 5** | `CS` (Chip Select) | Selección de esclavo SPI (activo en bajo) |
| **GPIO 23** | `MOSI` | Master Out Slave In |
| **GPIO 19** | `MISO` | Master In Slave Out |
| **GPIO 18** | `SCK` | Reloj de bus SPI |

### 1.4. Bus I2C — Pantalla LCD y RTC
| Pin ESP32 | Señal I2C | Dirección I2C | Dispositivo |
| :---: | :---: | :---: | :--- |
| **GPIO 21** | `SDA` | — | Línea de datos bidireccional |
| **GPIO 22** | `SCL` | — | Línea de reloj (100 kHz estándar) |
| *Bus I2C* | — | `0x27` o `0x3F` | Módulo PCF8574 (Pantalla LCD Alfanumérica 16x2 / 20x4) |
| *Bus I2C* | — | `0x68` | Módulo RTC de alta precisión DS3231 |

### 1.5. Interfaz Local y Actuadores Auxiliares
| Pin ESP32 | Función | Tipo | Descripción |
| :---: | :---: | :---: | :--- |
| **GPIO 15** | Touch SD | Entrada Capacitiva | Sensor táctil para desmontar/sincronizar MicroSD |
| **GPIO 2** | LED Neopixel | Salida Digital | LED direccionable WS2812B para diagnóstico de estado visual |

### 1.6. Enlace UART Inter-Microcontroladores (ESP32 $\leftrightarrow$ PIC16F628A)
* **Baudrate:** 9600 bps (8N1).
* **Puerto ESP32:** `UART2` (o configurado vía `HardwareSerial`).
* **Conexión cruzada:** TX ESP32 $\rightarrow$ RX PIC (RB1), RX ESP32 $\leftarrow$ TX PIC (RB2), GND común.

---

## 2. Controlador Esclavo: Microchip PIC16F628A

Mapeo extraído del esquemático [`control_motores`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/kicad/control_motores/) y código base [`Control_motor.X`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/pic16f628a/Control_motor.X/):

### 2.1. Pines del Puerto B (PORTB)
| Pin PIC | Nombre Pin | Dirección | Función Asociada |
| :---: | :---: | :---: | :--- |
| **RB0** | `RB0/INT` | Entrada | Reservado / Interrupción externa de conteo |
| **RB1** | `RB1/RX` | Entrada | Receptor serie USART (9600 bps desde ESP32) |
| **RB2** | `RB2/TX` | Salida | Transmisor serie USART (9600 bps hacia ESP32) |
| **RB3** | `M2_EN` | Salida | Habilitación (*Enable*) Driver Puente H L293D para Motor 2 |
| **RB4** | `M1_EN` | Salida | Habilitación (*Enable*) Driver Puente H L293D para Motor 1 |
| **RB5** | `RB5` | I/O | Línea auxiliar / libre |
| **RB6** | `PGC` | Entrada | Reloj de programación ICSP (PICkit) |
| **RB7** | `PGD` | I/O | Datos de programación ICSP (PICkit) |

### 2.2. Pines del Puerto A (PORTA) — Encoders y Dirección de Motores
> [!NOTE]
> Para usar PORTA como I/O digitales, es mandatorio configurar `CMCON = 0x07` al iniciar el microcontrolador para apagar los comparadores analógicos internos.

* **Dirección de Motores en Puente H (L293D):**
  * `M1_1` y `M1_2`: Entradas de dirección para Motor 1.
  * `M2_1` y `M2_2`: Entradas de dirección para Motor 2.
  * **Frenado Dinámico Activo:** Conducir simultáneamente ambas entradas a nivel alto (`M1_1 = 1` y `M1_2 = 1`) durante 40 a 50 ms antes de apagar `M1_EN = 0`.
* **Sensores Ópticos de Posicionamiento:**
  * `SENS1_M1_1`: Encoder óptico de ranuras de paso para Motor 1 (conteo de canales 1..12).
  * `SENS2_M1_1`: Sensor óptico de muesca absoluta Home para Motor 1 (referencia canal 1).
  * `SENS1_M2_1`: Encoder óptico de ranuras de paso para Motor 2 (conteo de canales 13..24).
  * `SENS2_M2_1`: Sensor óptico de muesca absoluta Home para Motor 2 (referencia canal 13).
