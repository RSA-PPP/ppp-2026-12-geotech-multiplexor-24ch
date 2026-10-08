# ppp-2026-13-geotech-multiplexor-24ch

> **Red Sísmica del Austro (RSA) — Universidad de Cuenca**  
> Prácticas Preprofesionales (PPP) • Sistemas Embebidos, Instrumentación Geotécnica y Control Electromecánico

---

## 📋 Ficha Técnica del Proyecto

| Campo | Detalle |
| :--- | :--- |
| **Código Institucional** | `RSA-PPP-2026-13` |
| **Nombre del Proyecto** | Sistema de Adquisición Electromecánico Multiplexado para Galgas Extensométricas en Presas (ESP32 y PIC16F628A) |
| **Pasante** | Walter Calderón (`walter.calderon@ucuenca.edu.ec`) |
| **Carrera / Institución** | Ingeniería en Telecomunicaciones — Universidad de Cuenca |
| **Tutor Institucional** | Ing. Milton Muñoz (`milton.munozc@ucuenca.edu.ec`) — RSA |
| **Duración / Horas** | 144 horas (10.5 semanas) • Presencial |
| **Fecha de Ejecución** | 2026-10-01 al 2026-12-18 |
| **Estado** | `En Ejecución` |

---

## 🎯 Descripción General y Objetivos

La Red Sísmica del Austro (RSA) gestiona la auscultación estructural continua de presas hidroeléctricas críticas (como Chanlud y Labrado), donde la monitorización de la estabilidad del cuerpo de presa depende de redes de **galgas extensométricas de 4 hilos** embebidas en el concreto y galerías. Cada sensor entrega dos variables físicas acopladas: **deformación mecánica ($\mu\varepsilon$)** y **temperatura interna ($^\circ\text{C}$)** para compensación térmica.

Para optimizar costos de instrumental analógico de ultra alta resolución, el sistema utiliza un mecanismo de multiplexación electromecánica compuesto por **dos selectores rotativos mecánicos de 12 canales** (totalizando 24 sensores analógicos). Cada selector es gobernado por un motor DC acoplado a encoders ópticos de ranura y referencia absoluta (*Home*). La conmutación entre el modo de deformación y el modo de temperatura se realiza mediante un banco de relés (K1..K5) que reconfigura un puente de Wheatstone conectado al convertidor analógico-digital de 24 bits **HX711**.

Este proyecto implementa una arquitectura distribuida de firmware resiliente y de alta precisión:
* **Controlador Esclavo (Microchip PIC16F628A):** Firmware en C (XC8) para posicionamiento angular determinista por interrupciones, frenado dinámico activo en puente H L293D y parser binario UART a 9600 bps.
* **Nodo Maestro de Gestión (Espressif ESP32-WROOM-32):** Firmware en C++ (PlatformIO) para lectura del HX711, secuenciamiento de relés con tiempos muertos de guarda ($\ge 20\text{ ms}$), datalogger autónomo en MicroSD (FAT32/CSV), pantalla LCD alfanumérica y telemetría por comandos ASCII vía RS-232 (115200 bps).

### Objetivos Clave
1. **Hardware y Diagnóstico de Encoders:** Auditar en osciloscopio las señales digitales de los encoders de ranura y muesca Home, y caracterizar las formas de onda de conmutación del puente H L293D.
2. **Firmware de Posicionamiento (PIC16F628A):** Desarrollar en MPLAB X / XC8 las rutinas de búsqueda de origen (*Homing*), conteo de ranuras por interrupciones y frenado dinámico asistido para evitar deslizamientos por inercia.
3. **Protocolo Inter-Microcontroladores:** Diseñar el enlace serie UART binario robusto con delimitadores y suma de comprobación XOR a 9600 bps con máquina de reintentos y tolerancia a fallos.
4. **Adquisición HX711 y Reconfiguración de Puente:** Desarrollar el driver de lectura diferencial de 24 bits para el HX711 y el secuenciador de relés K1..K5 con tiempos de estabilización (*dead-time* $\ge 20\text{ ms}$).
5. **Datalogger MicroSD, Interfaz LCD y Telemetría RS-232:** Implementar la persistencia local en `DATALOG.CSV`, la pantalla informativa LCD y el protocolo de telemetría exterior compatible con sistemas SCADA.
6. **Barrido Secuencial de 24 Canales y Ensayo en Banco:** Orquestar el ciclo completo de auscultación ($\le 90\text{ s}$ para 24 canales) y evaluar la resiliencia en un ensayo de 100 barridos continuos con resistencias patrón.
7. **Guía de Troubleshooting y Documentación Final:** Redactar el manual de solución de averías electromecánicas e Informe Técnico Final.

> 📄 Para consultar el cronograma detallado semana a semana, la carga horaria y los checkpoints verificables, revisa el [Plan de Trabajo Oficial](docs/planificacion.md).

---

## 🏗️ Arquitectura del Sistema y Flujo de Trabajo

```text
+-----------------------------------------------------------------------------------+
|                        DIAGRAMA DE ARQUITECTURA DISTRIBUIDA                       |
+-----------------------------------------------------------------------------------+
|                                                                                   |
|  [24 Galgas Extensométricas en Presa]                                            |
|         |                                                                         |
|         v                                                                         |
|  [Selectores Rotativos Electromecánicos M1 y M2 (12 canales c/u)]                 |
|         ^                                                                         |
|         | (Acoplamiento Mecánico)                                                 |
|  [2x Motores DC con Encoders Ópticos (sens1 / sens2_Home)]                       |
|         ^                                                                         |
|         | (Driver H-Bridge L293D con Frenado Dinámico)                            |
|  [Microchip PIC16F628A - Esclavo de Motores]                                      |
|         ^                                                                         |
|         | (Enlace UART Binario @ 9600 bps / Checksum XOR)                         |
|         v                                                                         |
|  [Espressif ESP32 - Maestro de Adquisición y Gestión]                             |
|  ├── Secuenciador de Relés K1..K5 (Dead-time >= 20 ms) ---> [Puente Wheatstone]   |
|  ├── Driver ADC Diferencial 24-bit (HX711) <----------------+                     |
|  ├── Datalogger Autónomo MicroSD (Bus SPI / FAT32 / DATALOG.CSV)                  |
|  ├── Interfaz de Operación Local (Pantalla LCD Alfanumérica)                      |
|  └── Puerto Serie de Telemetría RS-232 / SCADA ASCII @ 115200 bps                 |
+-----------------------------------------------------------------------------------+
```

---

## 📂 Estructura del Repositorio

> [!IMPORTANT]
> **Estructura Raíz Inmutable:** Para mantener la coherencia con los lineamientos de la RSA, la estructura de carpetas raíz no debe ser alterada ni renombrada sin previa coordinación con el tutor.

```text
ppp-2026-13-geotech-multiplexor-24ch/
├── data/
│   ├── evidence/              # Capturas oscilográficas, evidencias de frenado y curvas
│   └── raw_samples/           # Volcados de prueba de tarjetas MicroSD (DATALOG.CSV)
│
├── docs/
│   ├── hardware/              # Esquemáticos de placas de relés, motores y pinouts
│   ├── troubleshooting/       # Bitácora de fallas mecánicas, atascos y calibración
│   └── planificacion.md       # Documento rector del plan de trabajo (144h) y checkpoints
│
├── firmware/
│   ├── common/                # Definiciones de tramas binarias UART y constantes globales
│   ├── drivers/               # Librerías de bajo nivel: HX711, LCD, L293D, MicroSD
│   ├── pic16f628a/            # Proyecto MPLAB X / XC8 para control de motores
│   ├── esp32/                 # Proyecto PlatformIO / C++ maestro de adquisición
│   └── tests/                 # Firmwares de verificación de reloj, relés y encoders
│
├── python/
│   ├── scripts/               # Scripts de monitoreo RS-232 y parser de DATALOG.CSV
│   ├── utils/                 # Utilitarios para cálculo de estadísticas y verificación
│   └── requirements.txt       # Dependencias de Python para herramientas de soporte PC
│
├── README.md                  # Este documento
└── .gitignore                 # Exclusiones de Git (MPLAB X, PlatformIO, Python, SO)
```

---

## 🛠️ Herramientas y Requisitos de Desarrollo

* **Hardware / Instrumental:**
  - Microcontroladores Microchip **PIC16F628A** y módulo Espressif **ESP32-WROOM-32**.
  - Programador Microchip **PICkit 3 / 4** o **SNAP** con interfaz ICSP.
  - Driver H-Bridge **L293D** y banco de relés conmutadores K1..K5.
  - ADC diferencial de 24 bits **HX711** y módulo MicroSD SPI.
  - Osciloscopio digital, analizador lógico de 8 canales y multímetro de banco.
  - Caja de décadas de resistencias patrón ($350\,\Omega$ y $120\,\Omega \pm 0.05\%$).
* **Entornos de Desarrollo e IDEs:**
  - **MPLAB X IDE v6.xx** con compilador **Microchip XC8** para el PIC16F628A.
  - **VS Code + PlatformIO** (o ESP-IDF / Arduino Core) para el ESP32.
* **Herramientas de Soporte PC:**
  - Python 3.10+ en entorno virtual (`.venv`):
    ```bash
    pip install -r python/requirements.txt
    ```

---

## 🚀 Puesta en Marcha

### 1. Clonar el Repositorio
```bash
git clone git@github.com-rsa:RSA-PPP/ppp-2026-13-geotech-multiplexor-24ch.git
cd ppp-2026-13-geotech-multiplexor-24ch
```

### 2. Compilación del Firmware del Esclavo (PIC16F628A)
1. Abrir **MPLAB X IDE**.
2. Abrir el proyecto ubicado en `firmware/pic16f628a/`.
3. Seleccionar el compilador **XC8** y la herramienta de programación (**PICkit 3/4**).
4. Compilar (`Clean and Build`) y programar la tarjeta de control de motores vía ICSP.

### 3. Compilación del Firmware del Maestro (ESP32)
1. Abrir **VS Code** con la extensión **PlatformIO**.
2. Abrir la carpeta `firmware/esp32/`.
3. Compilar el proyecto (`PlatformIO: Build`) y subir al módulo vía puerto serie (`PlatformIO: Upload`).

### 4. Herramientas de PC y Telemetría RS-232
```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -r python/requirements.txt
python python/scripts/scada_monitor.py
```

---

## 🛡️ Reglas de Trabajo y Control de Versiones

Para asegurar la calidad y trazabilidad del proyecto durante las prácticas:

1. **Rama Principal:** La rama activa de trabajo es **`main`**. Realiza commits frecuentes y atómicos al finalizar cada bloque o jornada de trabajo.
2. **Formato de Commits:** Utiliza la convención estándar en minúsculas:
   - `feat: [nueva funcionalidad, driver o máquina de estados implementada]`
   - `fix: [corrección de bug en firmware, protocolo o circuito]`
   - `docs: [actualización de esquemas, pinouts, troubleshooting o informe]`
   - `test: [pruebas de encoders, oscilogramas o calibración con resistencias patrón]`
   - `refactor: [optimización de temporizaciones sin cambio de comportamiento]`
3. **Integridad de Código:** No subas archivos temporales de compilación (`.o`, `.d`, carpetas `build/`, `.pio/`) ni volcados masivos sin comprimir. Apóyate en el archivo `.gitignore`.

---

## 📞 Contacto y Soporte Institucional

* **Tutor Institucional:** Ing. Milton Muñoz (`milton.munozc@ucuenca.edu.ec`)
* **Institución:** [Red Sísmica del Austro (RSA)](https://redsismicaaustro.github.io/RSA-Metodologias) — Universidad de Cuenca
