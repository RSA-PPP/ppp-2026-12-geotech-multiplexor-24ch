# Guía de Referencia Técnica y Material Heredado
## Proyecto: Sistema de Adquisición Electromecánico Multiplexado para Galgas Extensométricas en Presas (ESP32 y PIC16F628A)

Este documento establece el puente técnico y metodológico entre los prototipos previos de hardware/firmware y la ejecución del plan de trabajo oficial (**`RSA-PPP-2026-13`**), desarrollado por **Walter Calderón** bajo la tutoría del **Ing. Milton Muñoz**.

---

## 📌 Datos de Identificación del Proyecto

| Parámetro | Detalle |
| :--- | :--- |
| **Código Institucional** | `RSA-PPP-2026-13` |
| **Proyecto** | Sistema de Adquisición Electromecánico Multiplexado para Galgas Extensométricas en Presas (ESP32 y PIC16F628A) |
| **Pasante** | Walter Calderón (`walter.calderon@ucuenca.edu.ec`) |
| **Tutor Institucional** | Ing. Milton Muñoz (`milton.munozc@ucuenca.edu.ec`) — Red Sísmica del Austro (RSA) |
| **Repositorio** | [RSA-PPP/ppp-2026-13-geotech-multiplexor-24ch](https://github.com/RSA-PPP/ppp-2026-13-geotech-multiplexor-24ch) |
| **Plan Rector** | [docs/planificacion.md](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/planificacion.md) (144 horas) |

---

## 🔍 Resumen del Material Heredado (Qué está disponible y probado)

Se ha transferido al repositorio todo el desarrollo previo clasificado y depurado para servir de base en las distintas fases del proyecto:

### 1. Diseños Circuitales y Archivos KiCad (`docs/hardware/kicad/`)
* **Placa de Acople ESP32 ([`docs/hardware/kicad/acople_esp32/`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/kicad/acople_esp32/)):**  
  Diseño esquemático (`.kicad_sch`), ruteo de circuito impreso (`.kicad_pcb`), paquete de fabricación Gerber/BOM para JLCPCB y diagrama vectorial (`acople_sp32.svg`). Define la interfaz física entre el módulo ESP32-WROOM-32, el ADC HX711, la tarjeta MicroSD y los conectores a la placa de relés y motores.
* **Control de Motores PIC16F628A ([`docs/hardware/kicad/control_motores/`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/kicad/control_motores/)):**  
  Esquemático del circuito que integra el microcontrolador PIC16F628A, el driver puente H L293D para los dos motores DC (M1 y M2) y las entradas de acondicionamiento para los sensores ópticos de ranura y *Home*.
* **Puente de Relés ([`docs/hardware/kicad/puente_reles/`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/kicad/puente_reles/)):**  
  Diseño circuital del banco de relés (K1..K5) que conmuta la topología del puente de Wheatstone para alternar entre el canal de deformación mecánica ($\mu\varepsilon$) y el sensor de temperatura ($^\circ\text{C}$).
* **Acondicionamiento Legacy ([`docs/hardware/kicad/acondicionamiento_legacy/`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/kicad/acondicionamiento_legacy/)):**  
  Esquemático histórico con amplificador de instrumentación INA114AP para consulta y referencia comparativa.

### 2. Planos CAD de Envolvente y Caja (`docs/hardware/cad/`)
* **Archivo Vectorworks ([`docs/hardware/cad/esquemas.vwx`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/cad/esquemas.vwx)):**  
  Planos mecánicos y vistas de disposición física de la caja de medición donde van instalados los selectores rotativos electromecánicos y los soportes de motor.

### 3. Firmware Base del Controlador Esclavo (`firmware/pic16f628a/`)
* **Proyecto MPLAB X IDE ([`firmware/pic16f628a/Control_motor.X/`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/pic16f628a/Control_motor.X/)):**  
  Proyecto base compilable en XC8 con:
  * Configuración de *fuses* (`#pragma config`) y oscilador interno a 4 MHz (`INTOSCIO`).
  * Desactivación de comparadores analógicos (`CMCON = 0x07`) para liberar PORTA.
  * Inicialización de USART a 9600 bps (`TXSTA`, `RCSTA`, `SPBRG = 25`).
  * Archivo fuente [`newmain.c`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/pic16f628a/Control_motor.X/newmain.c) con parser básico de prueba (`PING`, `A1`, `R1`, etc.).

### 4. Firmware Prototipo del ESP32 en MicroPython (`firmware/esp32/legacy_micropython/`)
* **Golden Reference Funcional:**  
  Conjunto completo de scripts en MicroPython que sirvió para validar el hardware en laboratorio:
  * [`drivers/control_reles.py`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/esp32/legacy_micropython/drivers/control_reles.py): Abstracción orientada a objetos que contiene la **tabla de verdad exacta de los relés K1..K5** y los pines GPIO asignados.
  * [`drivers/hx711.py`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/esp32/legacy_micropython/drivers/hx711.py): Driver de lectura bit-banging de 24 bits para el HX711, tara y promedio.
  * [`drivers/sd_spi.py`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/esp32/legacy_micropython/drivers/sd_spi.py): Inicialización del bus SPI para la tarjeta MicroSD.
  * [`drivers/lcd_i2c.py`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/esp32/legacy_micropython/drivers/lcd_i2c.py) e [`i2c_bus.py`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/esp32/legacy_micropython/drivers/i2c_bus.py): Manejo del bus I2C y display LCD 16x2.
  * [`drivers/touch_sd.py`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/esp32/legacy_micropython/drivers/touch_sd.py), [`ds3231.py`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/esp32/legacy_micropython/drivers/ds3231.py) y [`rgb_led.py`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/esp32/legacy_micropython/drivers/rgb_led.py): Controladores para periféricos auxiliares.
  * [`main.py`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/esp32/legacy_micropython/main.py): Lógica de adquisición secuencial, descarte de primeras lecturas y cálculo de milivoltios.

### 5. Scripts de Soporte en PC (`python/scripts/legacy_pc/`)
* Scripts auxiliares ([`enviar_esp32.py`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/python/scripts/legacy_pc/enviar_esp32.py) y [`config.py`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/python/scripts/legacy_pc/config.py)) empleados para comunicación por puerto serie y carga de archivos hacia el microcontrolador.

---

## ⚠️ Cuellos de Botella y Limitaciones Heredadas (¡Lectura Obligatoria!)

El material previo representa una base experimental de gran valor, pero **no constituye la solución de ingeniería final**. Presenta limitaciones técnicas críticas que debes resolver a lo largo de tu pasantía:

### 1. En el Controlador de Motores (PIC16F628A)
* **Falta de Frenado Dinámico Activo:**  
  El código anterior (`newmain.c`) solo apagaba las salidas al detener la marcha. Debido a la inercia del rotor de los motores DC acoplados a la reducción mecánica, el disco selector se pasaba de largo (*overshoot*), dejando los contactos del selector desalineados.  
  👉 **Tu reto (Fase 2):** Implementar frenado dinámico activo en el puente H L293D conmutando las dos entradas a nivel alto simultáneamente (`IN1=1, IN2=1`) durante una ventana calibrada (40–50 ms) antes de cortar `ENABLE=0`.
* **Vulnerabilidad a Rebotes y Pérdida de Pasos:**  
  La lectura previa de los encoders ópticos se hacía por sondeo simple en bucle, vulnerable a vibraciones mecánicas y ruidos.  
  👉 **Tu reto (Fase 2):** Implementar la búsqueda de origen absoluta (*Homing* en `sens2`) y conteo determinista de ranuras (`sens1`) asistido por interrupciones y temporizadores de guarda (*timeout* de 5 segundos para detectar atascos).
* **Protocolo Serie Informal (Texto Plano):**  
  El PIC procesaba comandos de texto ASCII sin delimitadores estrictos ni verificación de integridad (e.g. `"A1"` o `"R1"`).  
  👉 **Tu reto (Fase 3):** Sustituir este esquema por el **protocolo binario estructurado con delimitadores `0xAA` / `0x55` y suma de comprobación XOR** estipulado en la planificación.

### 2. En el Nodo Maestro (ESP32)
* **Migración Obligatoria de MicroPython a C++ (PlatformIO):**  
  El código heredado está escrito en MicroPython. Si bien sirvió como prototipo rápido, MicroPython introduce latencias variables (*jitter* del recolector de basura) que impiden garantizar el estricto sincronismo en microsegundos que exige el tren de pulsos del HX711 y la orquestación en tiempo real.  
  👉 **Tu reto (Fase 4):** Portar y refactorizar la lógica hacia **C++ nativo en PlatformIO**, beneficiándote de la velocidad, concurrencia de tareas FreeRTOS y tipado estricto.
* **Transitorios por Falta de Tiempos Muertos (*Dead-Time*):**  
  En las pruebas previas, el accionamiento de relés no contemplaba retardos de guarda de estabilización de contactos mecánicos (*settling time*), inyectando ruido eléctrico severo a las primeras conversiones de 24 bits del ADC.  
  👉 **Tu reto (Fase 4):** Programar un secuenciador con tiempos muertos de guarda $\ge 20\text{ ms}$ y descarte de las dos primeras muestras post-conmutación.
* **Persistencia y Telemetría Industriales Incompletas:**  
  El prototipo anterior solo imprimía valores por la consola serial de depuración.  
  👉 **Tu reto (Fase 5 y 6):** Desarrollar el datalogger autónomo en MicroSD con archivos CSV estructurados (`DATALOG.CSV`), la interfaz LCD interactiva y el protocolo exterior RS-232 / SCADA a 115200 bps.

---

## 🗺️ Mapa de Navegación del Material Proporcionado

Todo el material útil ha sido ubicado en el árbol del repositorio:

| Ruta en el Repositorio | Contenido | Propósito y Uso Recomendado |
| :--- | :--- | :--- |
| [`docs/hardware/pinout_referencia.md`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/pinout_referencia.md) | Tabla consolidada de señales y pines | **Consulta inmediata:** Consulta rápida de GPIOs de ESP32, pines de PIC y tabla de verdad de relés. |
| [`docs/hardware/kicad/acople_esp32/`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/kicad/acople_esp32/) | Proyecto KiCad de placa ESP32 y Gerbers | **Auditoría física (Fase 1 y 4):** Verificar continuidades, niveles de tensión y ruteo hacia el HX711 y MicroSD. |
| [`docs/hardware/kicad/control_motores/`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/kicad/control_motores/) | Proyecto KiCad del controlador PIC16F628A | **Hardware de potencia (Fase 1 y 2):** Conexiones del L293D, pines de encoders y puerto serie. |
| [`docs/hardware/kicad/puente_reles/`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/kicad/puente_reles/) | Proyecto KiCad del banco K1..K5 | **Topología de galgas (Fase 1 y 4):** Rastreo de las ramas del puente de Wheatstone y contactos de relés. |
| [`docs/hardware/cad/esquemas.vwx`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/cad/esquemas.vwx) | Planos mecánicos en Vectorworks | **Ensamble mecánico:** Inspeccionar dimensiones mecánicas y sujeción de motores y selectores. |
| [`firmware/pic16f628a/Control_motor.X/`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/pic16f628a/Control_motor.X/) | Proyecto MPLAB X / XC8 (`newmain.c`) | **Código base de partida (Fase 1, 2 y 3):** Base para implementar Homing, frenado dinámico y protocolo binario UART. |
| [`firmware/esp32/legacy_micropython/`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/esp32/legacy_micropython/) | Scripts funcionales en MicroPython | **Golden Reference algorítmica (Fase 4):** Consulta de lógica de relés, timing de HX711 y conversión a milivoltios para portar a C++. |
| [`python/scripts/legacy_pc/`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/python/scripts/legacy_pc/) | Herramientas seriales para PC | **Pruebas de enlace:** Referencia para estructurar el cliente SCADA en Python. |

---

## 🚀 Guía de Aprovechamiento Paso a Paso según el Plan de Trabajo (144h)

### Semana 1: Fase 1 — Diagnóstico de Hardware y Encoders del PIC16F628A
1. Abre los esquemáticos KiCad en [`docs/hardware/kicad/control_motores/`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/kicad/control_motores/) y [`acople_esp32/`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/kicad/acople_esp32/).
2. Revisa la tabla rápida en [`docs/hardware/pinout_referencia.md`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/docs/hardware/pinout_referencia.md) para cotejar alimentaciones de 5V y 3.3V.
3. Abre el proyecto [`firmware/pic16f628a/Control_motor.X/`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/pic16f628a/Control_motor.X/) en MPLAB X, compila con XC8 y programa el PIC mediante PICkit / ICSP.
4. Conecta el osciloscopio en las señales `SENS1` y `SENS2` de ambos ejes, gira manualmente los discos y verifica niveles TTL limpios sin ruido ni rebotes (**Checkpoint 1.2**).

### Semanas 2–3: Fase 2 — Firmware PIC: Homing, Ranuras y Frenado Dinámico
1. Toma el bucle principal de [`newmain.c`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/pic16f628a/Control_motor.X/newmain.c) y reemplázalo por una máquina de estados formal (`ESTADO_HOMING`, `ESTADO_MOVIENDO`, `ESTADO_FRENADO`, `ESTADO_LISTO`, `ESTADO_ERROR`).
2. Implementa la rutina de búsqueda de *Home*: giro controlado hasta flanco en `sens2` y reinicio del contador a `pos = 1` (**Checkpoint 2.1**).
3. Implementa el **frenado dinámico**: al detectar la ranura de destino en `sens1`, activa `IN1=1, IN2=1` durante 45 ms antes de apagar `ENABLE=0`. Captura con osciloscopio para verificar la ausencia de sobreimpulso (**Checkpoint 2.2**).
4. Ejecuta 50 ciclos continuos de selección aleatoria de canal validando alineación mecánica (**Checkpoint 2.3**).

### Semana 4: Fase 3 — Protocolo Binario UART (ESP32 $\leftrightarrow$ PIC16F628A)
1. Sustituye las cadenas de texto (`A1`, `R1`) de `newmain.c` por el parser de tramas binarias:
   $$[\text{HEADER: } 0\text{xAA}]\,[\text{MOTOR\_ID}]\,[\text{CMD}]\,[\text{PARAM}]\,[\text{CHECKSUM}]\,[\text{TAIL: } 0\text{x55}]$$
2. Programa la respuesta determinista del PIC con estados (`READY`, `MOVING`, `HOMING`, `ERROR`).
3. Diseña el cliente UART en C++ para el ESP32 con reintentos automáticos y recuperación ante desconexión (**Checkpoints 3.1, 3.2, 3.3**).

### Semanas 5–6: Fase 4 — Driver HX711 en C++ y Secuenciador de Relés K1..K5
1. Estudia [`legacy_micropython/drivers/control_reles.py`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/esp32/legacy_micropython/drivers/control_reles.py) para entender la conmutación de GPIO 13, 14, 16, 17 y 25.
2. Crea el proyecto C++ en PlatformIO bajo `firmware/esp32/`.
3. Desarrolla el driver C++ del HX711 basado en [`legacy_micropython/drivers/hx711.py`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/esp32/legacy_micropython/drivers/hx711.py), implementando el filtrado por mediana móvil y los tiempos de estabilización (*dead-time* $\ge 20\text{ ms}$).
4. Calibra con la caja de décadas de resistencias patrón ($350\,\Omega$ y $120\,\Omega$) verificando $R^2 \ge 0.999$ (**Checkpoints 4.1, 4.2, 4.3**).

### Semanas 7–8: Fase 5 — Datalogger MicroSD, Interfaz LCD y Telemetría RS-232
1. Configura el bus SPI en el ESP32 (GPIO 5, 18, 19, 23) usando `SD.h` o `SdFat`.
2. Implementa la persistencia en `DATALOG.CSV` con volcado forzado (`flush()`) tras cada lectura (**Checkpoint 5.1**).
3. Integra la pantalla LCD por I2C (GPIO 21 y 22) tomando como referencia [`legacy_micropython/drivers/lcd_i2c.py`](file:///C:/Users/miltonrsa/Documents/git/ppp/ppp-2026-13-geotech-multiplexor-24ch/firmware/esp32/legacy_micropython/drivers/lcd_i2c.py) (**Checkpoint 5.2**).
4. Implementa el parser de comandos ASCII por el puerto RS-232 (`READ:CH`, `SCAN:ALL`, `CAL:ZERO`, `LOG:STATUS`) (**Checkpoint 5.3**).

### Semanas 9–10: Fase 6 — Barrido Secuencial de 24 Canales y Tolerancia a Fallos
1. Integra la máquina de estados global: M1 (canales 1..12) y M2 (canales 13..24).
2. Optimiza los tiempos de ciclo para completar los 24 canales en menos de 90 segundos (**Checkpoint 6.1**).
3. Simula desconexión y atascos mecánicos forzados, validando que el sistema reporte `ERROR_M_STALL` en el CSV y prosiga con los siguientes canales sin colgarse (**Checkpoint 6.3**).
4. Corre el ensayo de estrés continuo de 100 barridos (2400 mediciones) (**Checkpoint 6.2**).

### Semanas 10–11: Fase 7 — Troubleshooting e Informe Final
1. Documenta las holguras mecánicas y el procedimiento de calibración de optoacopladores en `docs/troubleshooting/`.
2. Redacta el Informe Técnico Final institucional y prepara el repositorio para cierre y firma de actas (**Checkpoints 7.1, 7.2, 7.3**).
