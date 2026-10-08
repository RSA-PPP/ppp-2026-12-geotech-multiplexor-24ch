---
titulo: "Plan de Trabajo de Prácticas Preprofesionales: Sistema de Adquisición Electromecánico Multiplexado de 24 Canales para Auscultación de Presas (ESP32 y PIC16F628A)"
proyecto: "Sistema de Adquisición Electromecánico Multiplexado para Galgas Extensométricas en Presas (ESP32 y PIC16F628A)"
codigo_proyecto: "RSA-PPP-2026-13"
area_tematica: "Sistemas Embebidos, Instrumentación Geotécnica, Control Electromecánico, Firmware Distribuido (UART/SPI) y Metrología de Galgas"
estado: "En Ejecución"
version: "1.0"
fecha_creacion: "2026-09-28"
fecha_actualizacion: "2026-09-28"

practicante:
  nombre: "Walter Calderón"
  cedula: "N/D"
  correo: "walter.calderon@ucuenca.edu.ec"
  carrera: "Ingeniería en Telecomunicaciones"
  institucion: "Universidad de Cuenca"

tutoria:
  tutor_institucional: "Ing. Milton Muñoz (Red Sísmica del Austro - RSA)"
  correo_tutor_institucional: "milton.munozc@ucuenca.edu.ec"

cronograma:
  duracion_total_horas: 144
  dedicacion_semanal_horas: 14
  duracion_semanas: 10.5
  fecha_inicio: "2026-10-01"
  fecha_fin_estimada: "2026-12-18"
  modalidad: "Presencial"

tecnologias:
  - "Microcontroladores y Arquitectura: Microchip PIC16F628A (controlador esclavo de motores) y Espressif ESP32-WROOM-32 (nodo maestro de adquisición y gestión)"
  - "Entornos de Desarrollo e IDEs: Microchip MPLAB X IDE + Compilador XC8 (PIC), VS Code + PlatformIO / ESP-IDF / Arduino Core (ESP32)"
  - "Adquisición y Acondicionamiento Analógico: ADC diferencial de 24 bits HX711 (Canal A, Ganancia 128), Banco de relés K1..K5 para reconfiguración del Puente de Wheatstone"
  - "Etapa de Potencia y Actuación: Driver de potencia H-Bridge L293D con frenado dinámico, 2 motores DC acoplados a selectores rotativos electromecánicos de 12 posiciones"
  - "Sensores de Posicionamiento: Encoders ópticos infrarrojos de ranuras (sens1_M1, sens1_M2) y muesca absoluta de referencia Home (sens2_M1, sens2_M2)"
  - "Protocolos y Comunicaciones: Bus UART serie binario robusto con delimitadores y checksum XOR (ESP32-PIC a 9600 bps), Enlace externo RS-232 / SCADA ASCII (115200 bps)"
  - "Almacenamiento Local y UI: Interfaz de bus SPI para tarjeta MicroSD (sistema de archivos FAT32, archivo DATALOG.CSV) y pantalla LCD alfanumérica"
  - "Instrumental de Laboratorio: Osciloscopio digital de fósforo, analizador lógico de 8 canales, multímetro de banco de alta precisión y simulador/resistencias patrón de galgas (350 Ω / 120 Ω)"

repositorio:
  url: "https://github.com/RSA-PPP/ppp-2026-13-geotech-multiplexor-24ch"
  rama_base: "main"

topics:
  - rsa-ppp
  - ucuenca
  - dam-monitoring
  - strain-gauges
  - esp32
  - pic-microcontroller
  - uart
  - spi
  - microsd
  - c
---

---

## 1. Antecedentes y Justificación Técnica

### 1.1. Contexto del Sistema y Estado Inicial del Artefacto
La Red Sísmica del Austro (RSA) gestiona la auscultación estructural de presas hidroeléctricas críticas (como Chanlud y Labrado), donde la monitorización de la estabilidad estática del cuerpo de presa depende de redes de **galgas extensométricas de 4 hilos** embebidas en el concreto y en galerías de drenaje. Cada transductor entrega dos variables físicas acopladas fundamentales: la **deformación mecánica ($\mu\varepsilon$)** y la **temperatura interna ($^\circ\text{C}$)** requerida para la compensación de deriva térmica del material.

Para economizar infraestructura de acondicionamiento de ultra alta resolución, el sistema de auscultación emplea un mecanismo de multiplexación electromecánica compuesto por dos selectores rotativos mecánicos de 12 canales cada uno (totalizando 24 sensores analógicos). Cada selector es impulsado por un motor de corriente continua acoplado a encoders ópticos de conteo de paso y detección de posición de inicio (*Home*). La conmutación entre el modo de lectura de deformación (lazo activo del transductor) y el modo de temperatura (resistencia compensadora) se realiza mediante un banco de relés miniatura (K1..K5) que reconfigura un puente de Wheatstone conectado a un convertidor analógico-digital (ADC) de 24 bits **HX711**.

En iteraciones preliminares del proyecto, el hardware de la placa base de motores (con microcontrolador Microchip PIC16F628A y puente H L293D) y la tarjeta adaptadora del nodo maestro (ESP32) fueron diseñadas y montadas en laboratorio, pero carecen de un firmware determinista, robusto y sincronizado que garantice mediciones fiables sin desalineaciones mecánicas ni derivas de muestreo.

### 1.2. Limitaciones Críticas o Cuellos de Botella Identificados
* **Desalineación Mecánica y Falta de Frenado Dinámico:** Debido a la inercia de los motores DC, al apagar el puente L293D el rotor continuaba girando por inercia, provocando que los contactos electromecánicos del selector quedaran a medio paso o en falso contacto. Se requiere la implementación estricta de frenado dinámico activo (`IN1=1, IN2=1`) controlado por software.
* **Vulnerabilidad a Ruidos y Desincronización en Encoders:** La lectura directa de los sensores ópticos de ranura presentaba disparos erráticos ante rebotes o vibraciones del motor, provocando que el conteo de posiciones se desfasara del canal real sin un mecanismo periódico de autocorrección o referenciación (*Homing*).
* **Transitorios Eléctricos por Conmutación de Relés:** El accionamiento simultáneo de los relés K1..K5 sin tiempos muertos (*dead-time*) ni retardos de estabilización de contactos (*settling time*) acoplaba transitorios severos a la entrada analógica del HX711, falseando las primeras conversiones de 24 bits.
* **Carencia de un Protocolo Binario Resiliente entre ESP32 y PIC:** La comunicación inter-microcontroladores carecía de sincronismo de trama y validación de integridad (checksum), provocando bloqueos permanentes del sistema maestro si el PIC no alcanzaba la posición en el tiempo previsto.
* **Dispersión de Datos sin Almacenamiento Seguro ni Interfaz de Campo:** Las lecturas se transmitían de forma cruda por consola serial sin respaldo local determinista en tarjeta MicroSD (FAT32/CSV) ni retroalimentación visual inmediata en pantalla LCD para los técnicos de presa.

### 1.3. Desacoplamiento del Alcance y Propuesta de Solución
La presente pasantía de **144 horas** aborda de manera integral el desarrollo, depuración, sincronización e integración del firmware distribuido para los microcontroladores PIC16F628A y ESP32, asegurando la validación en banco de los 24 canales de auscultación:
1. **Firmware del Esclavo PIC16F628A:** Desarrollo en MPLAB X / XC8 de las rutinas de posicionamiento absoluto (Homing), conteo determinista de ranuras por interrupciones, frenado dinámico asistido y parser de comandos binarios a 9600 bps.
2. **Firmware del Maestro ESP32:** Desarrollo en PlatformIO / C++ del controlador de bajo nivel para el ADC HX711, secuenciador con tiempos muertos de los relés K1..K5, datalogger en MicroSD con archivos CSV fechados, visualizador LCD y protocolo de telemetría exterior RS-232 / SCADA.
3. **Validación Experimental en Banco:** Caracterización con resistencias patrón de $350\,\Omega$ y $120\,\Omega$, medición de tiempos de ciclo de barrido ($\le 90\text{ s}$ para 24 canales) y verificación de repetibilidad estadística ($\sigma < 5\text{ LSB}$).

---

## 2. Objetivos del Plan de Trabajo

### 2.1. Objetivo General
Desarrollar, sincronizar y validar experimentalmente la arquitectura distribuida de firmware para el sistema de adquisición electromecánico multiplexado de 24 canales de galgas extensométricas de presas, integrando el control de motores y posicionamiento óptico en un PIC16F628A con la instrumentación diferencial de 24 bits (HX711), secuenciamiento de relés, almacenamiento en MicroSD y telemetría RS-232 en un ESP32, con el propósito de garantizar mediciones repetibles, continuas y con precisión submilimétrica en la auscultación estructural de la RSA.

### 2.2. Objetivos Específicos
1. **Hardware y Diagnóstico de Encoders:** Adecuar el banco de pruebas físico, auditar las señales de los encoders ópticos de ranura y de referencia absoluta (*Home*), y caracterizar las formas de onda de conmutación del puente H L293D con instrumental de laboratorio.
2. **Firmware de Posicionamiento (PIC16F628A):** Desarrollar e implementar en lenguaje C (XC8) las máquinas de estado para búsqueda de cero absoluto (*Homing*), desplazamiento determinista por conteo de ranuras de los 2 motores DC y frenado dinámico de inercia.
3. **Protocolo de Enlace Inter-Microcontroladores:** Diseñar e implementar el protocolo binario estructurado con tramas delimitadas y suma de comprobación XOR a través del canal UART serie entre el ESP32 y el PIC16F628A, garantizando tolerancia a fallos y reporte de atascos mecánicos.
4. **Adquisición de Alta Resolución y Reconfiguración de Puente:** Desarrollar en el ESP32 el controlador de lectura diferencial del HX711 y el secuenciador de relés K1..K5 con tiempos de estabilización (*dead-time* $\ge 20\text{ ms}$) para alternar de forma transparente entre la medición de deformación mecánica y la resistencia térmica interna.
5. **Datalogger, Telemetría y Validación Integral:** Integrar el almacenamiento autónomo en tarjeta MicroSD en formato CSV, la interfaz en pantalla LCD, el protocolo de comandos ASCII por RS-232, y ejecutar un ensayo de validación continua de 24 canales sobre un simulador patrón de galgas.

---

## 3. Plan de Trabajo Detallado y Checkpoints Cuantificables

### Fase 1: Diagnóstico de Hardware, Acondicionamiento de Banco y Encoders del PIC16F628A
* **Duración:** 15 horas (Semana 1)
* **Objetivo:** Verificar la integridad eléctrica de la tarjeta de control de motores, caracterizar los sensores ópticos de ambos ejes y configurar el entorno de compilación e in-circuit debugging.
* **Actividades técnicas:**
  1. Configuración del proyecto base en MPLAB X IDE v6.xx con el compilador Microchip XC8 en modo libre y enlace con programador PICkit 3/4 o SNAP en puerto ICSP.
  2. Verificación de continuidades, tensiones de alimentación (5V regulados para lógica y potencia de motores) y comprobación del registro `CMCON = 0x07` para liberar los pines analógicos del PORTA como entradas digitales.
  3. Caracterización con osciloscopio de las señales digitales de los encoders de ranura (`sens1_M1`, `sens1_M2`) y de referencia Home (`sens2_M1`, `sens2_M2`), evaluando niveles lógicos ($V_{OL}, V_{OH}$) y tiempos de subida/bajada.
  4. Inspección de las salidas de potencia del driver L293D hacia los motores DC M1 y M2, evaluando caídas de tensión y corriente en carga.
* **Checkpoints medibles para revisión:**
  - [ ] **CP 1.1:** Entorno de desarrollo MPLAB X y XC8 plenamente operativo; tarjeta del PIC reconocida y programada con firmware de prueba de parpadeo y verificación de reloj.
  - [ ] **CP 1.2:** Capturas oscilográficas registradas de los 4 sensores ópticos validando que no existen falsos disparos ni rebotes al girar manualmente los discos selectores.
  - [ ] **CP 1.3:** Tabla de tensiones y consumos de corriente en reposo y en movimiento de ambos motores documentada en la bitácora técnica.

---

### Fase 2: Firmware del Esclavo PIC16F628A: Homing, Conteo de Ranuras y Frenado Dinámico
* **Duración:** 30 horas (Semanas 2–3)
* **Objetivo:** Desarrollar el firmware de control de movimiento determinista para posicionar los selectores rotativos en cualquiera de sus 12 posiciones con alineación perfecta y sin deslizamiento por inercia.
* **Actividades técnicas:**
  1. Implementación de la rutina de búsqueda de referencia absoluta (*Homing*): giro a baja velocidad hasta detectar el flanco de subida en el sensor de muesca Home (`sens2`), detención inmediata y asignación de la variable `posicion_actual = 1`.
  2. Implementación de la subrutina de conteo de posiciones mediante interrupción por cambio de estado o sondeo determinista sobre `sens1`, actualizando el registro de paso en cada ranura detectada.
  3. Diseño de la rutina de frenado dinámico activo en el L293D: conmutación de las entradas de dirección a estado alto simultáneo (`IN1=1, IN2=1`) durante una ventana calibrada de 40 a 50 ms antes del apagado total (`ENABLE=0`).
  4. Implementación de temporizadores de guarda (*watchdog / timeout* de 5 segundos) para abortar la marcha y declarar condición de atasco mecánico si no se detectan pulsos de encoder.
  5. Ensayos de repetibilidad de 50 ciclos continuos de posicionamiento aleatorio (posiciones 1 a 12) evaluando desalineaciones angulares con microscopio o marcas visuales.
* **Checkpoints medibles para revisión:**
  - [ ] **CP 2.1:** Secuencia de *Homing* ejecutada con éxito en 10 intentos consecutivos en ambos motores (M1 y M2), restableciendo la posición de origen de forma exacta.
  - [ ] **CP 2.2:** Frenado dinámico operativo; el disco selector se detiene exactamente en el centro de la muesca de contacto sin sobreimpulso (*overshoot*).
  - [ ] **CP 2.3:** Prueba de estrés de 50 ciclos continuos de selección de canales sin un solo error de conteo ni desalineación de los contactos rotativos.

---

### Fase 3: Protocolo Binario Robusto UART y Enlace Maestro-Esclavo (ESP32 $\leftrightarrow$ PIC16F628A)
* **Duración:** 15 horas (Semana 4)
* **Objetivo:** Establecer una capa de comunicación serie determinista, binaria y tolerante a fallos entre el nodo maestro ESP32 y el controlador esclavo PIC16F628A.
* **Actividades técnicas:**
  1. Configuración del periférico USART del PIC16F628A a 9600 bps, 8 bits de datos, sin paridad y 1 bit de parada (8N1), gestionado por interrupción de recepción (RCIF).
  2. Implementación del parser receptor de comandos binarios con formato:
     $$\text{Trama Comandos: } [\text{HEADER: } 0\text{xAA}]\,[\text{MOTOR\_ID: } 1|2]\,[\text{CMD}]\,[\text{PARAM}]\,[\text{CHECKSUM}]\,[\text{TAIL: } 0\text{x55}]$$
     donde $\text{CHECKSUM} = \text{XOR}(\text{MOTOR\_ID}, \text{CMD}, \text{PARAM})$.
  3. Implementación del generador de tramas de respuesta del PIC:
     $$\text{Trama Respuesta: } [\text{HEADER: } 0\text{xBB}]\,[\text{MOTOR\_ID: } 1|2]\,[\text{STATUS}]\,[\text{POS\_ACTUAL}]\,[\text{CHECKSUM}]\,[\text{TAIL: } 0\text{x55}]$$
     con codificación de estados `READY (0x00)`, `MOVING (0x01)`, `HOMING (0x02)` y `ERROR (0xEE)`.
  4. Desarrollo del cliente de comunicación en el ESP32 (utilizando `HardwareSerial` UART2), implementando máquina de estados con reintentos automáticos (máximo 3) y descarte de bytes huérfanos.
  5. Pruebas de inyección de ruido y corte de línea para verificar la recuperación autónoma del enlace sin reinicio de los microcontroladores.
* **Checkpoints medibles para revisión:**
  - [ ] **CP 3.1:** Comunicación bidireccional estable a 9600 bps confirmada con analizador lógico; 100% de tramas válidas reconocidas por el parser.
  - [ ] **CP 3.2:** Detección y descarte comprobado de tramas corruptas mediante la verificación del checksum XOR.
  - [ ] **CP 3.3:** El ESP32 recibe correctamente el estado `READY` tras cada desplazamiento ordenado y detecta el código `ERROR (0xEE)` ante atascos forzados.

---

### Fase 4: Acondicionamiento Analógico en ESP32: Driver HX711 y Secuenciamiento de Relés K1..K5
* **Duración:** 30 horas (Semanas 5–6)
* **Objetivo:** Adquirir los desbalances de puente de Wheatstone con resolución de 24 bits y secuenciar los relés de modo conmutado asegurando aislamiento de transitorios.
* **Actividades técnicas:**
  1. Desarrollo del driver de bajo nivel para el convertidor ADC HX711 (pines `DOUT` y `PD_SCK`), generando 24 pulsos de reloj para captura del valor en complemento a dos, más 1 pulso adicional para programar Ganancia 128 en el Canal A.
  2. Implementación de una rutina de filtrado digital no bloqueante (descarte de las dos primeras muestras post-conmutación y cálculo de mediana móvil sobre 5 muestras consecutivas) para eliminar ruido impulsivo.
  3. Programación del secuenciador de salidas digitales en el ESP32 para el control de los transistores que conmutan el banco de relés K1..K5.
  4. Implementación estricta de retardos de guarda (*dead-time* / *settling time* de 20 ms) entre la apertura/cierre de los relés y el inicio del tren de pulsos del ADC para garantizar estabilidad de contactos mecánicos.
  5. Calibración experimental de escala utilizando una caja de décadas de resistencias patrón de $350\,\Omega$ y $120\,\Omega$ simulando desbalances conocidos de galgas ($\pm 500\,\mu\varepsilon$ a $\pm 3000\,\mu\varepsilon$).
* **Checkpoints medibles para revisión:**
  - [ ] **CP 4.1:** Driver de lectura del HX711 operativo con tiempo de lectura determinista; dispersión de ruido en reposo inferior a $\pm 5$ cuentas LSB con resistencia patrón fija.
  - [ ] **CP 4.2:** Formas de onda del accionamiento de relés y la línea de reloj del ADC capturadas en osciloscopio, evidenciando un tiempo muerto limpio de 20 ms sin solapamiento de contactos.
  - [ ] **CP 4.3:** Curva de calibración estática (cuentas ADC vs desbalance en microvoltios) con coeficiente de correlación $R^2 \ge 0.999$.

---

### Fase 5: Datalogger MicroSD, Interfaz LCD y Protocolo Externo RS-232 / SCADA
* **Duración:** 24 horas (Semanas 7–8)
* **Objetivo:** Implementar la persistencia local de datos en memoria flash, la interfaz visual alfanumérica y el protocolo de telemetría por comandos ASCII para sistemas SCADA.
* **Actividades técnicas:**
  1. Configuración del bus SPI en el ESP32 para el módulo de memoria MicroSD, montando el sistema de archivos FAT32 mediante la librería `SD` o `SdFat`.
  2. Creación estructurada del archivo de datos `DATALOG.CSV` con cabecera estandarizada:
     `TIMESTAMP,FECHA,HORA,CANAL,MODO,RAW_ADC,VALOR_ING,ESTADO`
     con volcado forzado al medio físico (*flush*) tras cada registro para prevenir pérdida ante cortes imprevistos de energía.
  3. Integración de la pantalla LCD alfanumérica (vía bus paralelo o interfaz I2C), mostrando el estado del sistema: versión de firmware, canal en barrido actual, lectura en $\mu\varepsilon$ y temperatura ($^\circ\text{C}$).
  4. Desarrollo del intérprete de comandos ASCII externos por el puerto serie principal (RS-232 a 115200 bps):
     * `READ:CH <1..24>`: Conmuta motor, relés y retorna deformación y temperatura.
     * `SCAN:ALL`: Inicia el barrido secuencial de los 24 canales.
     * `CAL:ZERO`: Registra la tara eléctrica para el canal activo.
     * `LOG:STATUS`: Entrega espacio en SD y número de registros almacenados.
* **Checkpoints medibles para revisión:**
  - [ ] **CP 5.1:** Archivo `DATALOG.CSV` generado y legible de forma nativa en PC tras retirar la tarjeta SD sin corrupciones de sectores ni caracteres extraños.
  - [ ] **CP 5.2:** Pantalla LCD actualizando en tiempo real la información de canal y mediciones con refresco estable sin parpadeos.
  - [ ] **CP 5.3:** Intérprete RS-232 respondiendo con tramas válidas terminadas en `\r\n` a todos los comandos definidos en menos de 100 ms tras completar la acción.

---

### Fase 6: Barrido Secuencial Completo de 24 Canales y Validación Experimental en Banco
* **Duración:** 18 horas (Semanas 9–10)
* **Objetivo:** Orquestar el funcionamiento integrado de los dos microcontroladores en un ciclo completo de auscultación sobre los 24 canales y evaluar la resiliencia del sistema ante fallos.
* **Actividades técnicas:**
  1. Implementación de la máquina de estados de barrido secuencial global:
     * Canales 1 al 12: Comandar movimiento escalonado a M1 mientras M2 permanece desenergizado.
     * Canales 13 al 24: Comandar movimiento escalonado a M2 mientras M1 permanece desenergizado.
  2. Integración de la secuencia de medición por canal: Posicionamiento motor $\rightarrow$ confirmación PIC $\rightarrow$ activación K1..K5 Deformación $\rightarrow$ muestreo HX711 $\rightarrow$ activación K1..K5 Temperatura $\rightarrow$ muestreo HX711 $\rightarrow$ almacenamiento en SD $\rightarrow$ emisión RS-232.
  3. Optimización del tiempo total de barrido para completar los 24 sensores en una ventana temporal inferior a 90 segundos.
  4. Ensayos de tolerancia a fallos: simular atasco forzado en uno de los motores o desconexión del cable UART, verificando que el ESP32 registre el error (`ERROR_M_STALL`) en el archivo CSV y continúe el escaneo con el segundo selector sin colgarse.
  5. Ensayo continuo prolongado de 100 barridos consecutivos en banco de laboratorio con banco de resistencias patrón.
* **Checkpoints medibles para revisión:**
  - [ ] **CP 6.1:** Barrido de los 24 canales completado de forma 100% autónoma en menos de 90 segundos por ciclo.
  - [ ] **CP 6.2:** Dataset continuo de 100 barridos (2400 mediciones) registrado en MicroSD sin pérdida de registros ni incongruencias en los números de canal.
  - [ ] **CP 6.3:** Comprobación de resiliencia: el sistema detecta atascos forzados, escribe el código de falla correspondiente y recupera la operación en el siguiente canal libre.

---

### Fase 7: Guía de Troubleshooting, Documentación Técnica e Informe Final
* **Duración:** 12 horas (Semanas 10–11)
* **Objetivo:** Consolidar la documentación formal de ingeniería, manual de servicio y redacción del Informe Técnico Final institucional.
* **Actividades técnicas:**
  1. Elaboración de la Guía de Solución de Problemas (*Troubleshooting*) detallando calibración de optoacopladores, ajuste de tensión en correas/engranajes de motores, resolución de atascos y fallas de bus UART.
  2. Compilación de evidencias experimentales: oscilogramas de frenado dinámico, curvas de linealidad del HX711, capturas de pantalla de la interfaz LCD y tablas de consumo energético.
  3. Redacción del Informe Técnico Final en formato institucional de la RSA y la Universidad de Cuenca, compilando la arquitectura de hardware/firmware, resultados y recomendaciones operativas para despliegue en presa.
  4. Revisión técnica con el tutor institucional, integración de retroalimentación y entrega del repositorio de código fuente ordenado y comentado.
* **Checkpoints medibles para revisión:**
  - [ ] **CP 7.1:** Borrador completo del Informe Técnico Final y Guía de Troubleshooting entregados para revisión de tutoría.
  - [ ] **CP 7.2:** Repositorio en GitHub estructurado con código fuente modular (carpetas `/pic_firmware` y `/esp32_firmware`), diagramas de cableado y archivo README descriptivo.
  - [ ] **CP 7.3:** Aprobación definitiva del informe y firma de actas de culminación de prácticas laborales.

---

## 4. Resumen de Fases y Cronograma Semanal (144 Horas)

El plan de trabajo comprende **144 horas** distribuidas en bloques semanales de **14 horas** (a lo largo de **10.5 semanas**) para un seguimiento exhaustivo de avances mediante checkpoints cuantitativos:

| Fase | Semanas | Horas | Enfoque Principal |
| :---: | :---: | :---: | :--- |
| **Fase 1** | Semana 1 | **15 h** | Diagnóstico de hardware, verificación eléctrica y caracterización de encoders en osciloscopio. |
| **Fase 2** | Semanas 2–3 | **30 h** | Firmware PIC: Homing absoluto, conteo de ranuras por interrupciones y frenado dinámico L293D. |
| **Fase 3** | Semana 4 | **15 h** | Protocolo serie binario robusto ESP32-PIC a 9600 bps con checksum XOR y máquina de reintentos. |
| **Fase 4** | Semanas 5–6 | **30 h** | Driver HX711 (24 bits), secuenciamiento de relés K1..K5 con tiempos muertos y calibración estática. |
| **Fase 5** | Semanas 7–8 | **24 h** | Datalogger en MicroSD (FAT32/CSV), menú informativo en LCD y comandos ASCII por RS-232 / SCADA. |
| **Fase 6** | Semanas 9–10 | **18 h** | Barrido automático completo de 24 canales ($\le 90\text{ s}$), tolerancia a atascos y prueba de 100 ciclos. |
| **Fase 7** | Semanas 10–11 | **12 h** | Guía de *troubleshooting*, consolidación de anexos, repositorio y redacción del Informe Final. |
| **Total** | **~10.5 Semanas** | **144 h** | **Planificación Global de Prácticas Preprofesionales** |

---

## 5. Entregables y Criterios de Éxito de la Pasantía

1. **Firmware del Controlador Esclavo (PIC16F628A):**
   * Código fuente en C (MPLAB X / XC8) modular y comentado, implementando máquinas de estado de *Homing*, conteo de ranuras por interrupciones y frenado dinámico de motor.
   * Parser receptor binario UART con validación de checksum XOR y reporte determinista de estados y alertas de atasco.
2. **Firmware del Nodo Maestro de Adquisición (ESP32):**
   * Código fuente en C++ (PlatformIO / Arduino Core o ESP-IDF) con arquitectura multihilo/modular.
   * Controlador de bajo nivel para el convertidor analógico-digital HX711 con filtrado de mediana móvil.
   * Secuenciador con tiempos muertos de relés K1..K5 para alternancia entre modo Deformación y Temperatura.
   * Datalogger en MicroSD en formato CSV plano legible sin dependencia de software propietario.
   * Interfaz gráfica local en LCD y controlador serie RS-232 compatible con comandos SCADA ASCII estándar.
3. **Paquete de Evidencia y Validación Experimental:**
   * Reporte fotográfico y capturas de osciloscopio demostrando la ausencia de sobreimpulsos en los discos selectores tras aplicar frenado dinámico.
   * Gráficas de dispersión y linealidad del HX711 demostrando estabilidad de lectura ($\sigma < 5\text{ LSB}$) en resistencias patrón.
   * Dataset de validación de 100 barridos continuos de 24 canales (2400 registros) verificando integridad y coherencia temporal.
4. **Guía de Troubleshooting y Manual de Operación:**
   * Documento técnico paso a paso para diagnóstico de averías mecánicas, desajustes ópticos, sustitución de transistores o relés y configuración de parámetros en campo.
5. **Informe Técnico Final Institucional:**
   * Documento formal compilado en formato institucional RSA / Universidad de Cuenca que consolida antecedentes, diseño circuital, diagramas de flujo de firmware, resultados experimentales y conclusiones.
