# Guía de Cotización y Fabricación JLCPCB: Acople Cajas de Revisión (`acople_sp32`)

Este directorio contiene el paquete integral para la cotización y fabricación rápida en **JLCPCB** (PCB + Ensamble SMT).

---

## 📦 Contenido del Paquete

1. **`acople_sp32_gerbers.zip`**: Archivo comprimido con todos los planos Gerbers RS-274X y archivos de taladro Excellon (PTH y NPTH). Listo para arrastrar y soltar en [jlcpcb.com/quote](https://jlcpcb.com/quote).
2. **`BOM_acople_sp32_JLCPCB_SMT.csv`**: Lista de materiales SMD con códigos de catálogo LCSC verificados para cotización y compra automática de piezas.
3. **`CPL_acople_sp32_JLCPCB.csv`**: Archivo de centroides Pick & Place con coordenadas milimétricas y capas para el robot SMT.
4. **`BOM_acople_sp32_Montaje_Manual.csv`**: Lista de conectores THT y tiras de pines para soldadura manual en laboratorio o taller.
5. **`acople_sp32_ensamble_3D.step`**: Modelo 3D electromecánico completo (PCB + componentes) para validación de carcasas y gabinetes.

---

## 🛠️ Parámetros Técnicos para el Cotizador de JLCPCB

Al cargar `acople_sp32_gerbers.zip`, el sistema detectará automáticamente los parámetros básicos. Verifica los siguientes datos:

| Parámetro | Valor Recomendado | Notas |
| :--- | :--- | :--- |
| **Dimensiones** | **72.31 mm × 62.01 mm** | Detectado automáticamente por Gerber |
| **Capas (Layers)** | **2 Layers** | Capas Top y Bottom |
| **Material** | **FR-4 Standard** | TG 130-140 o TG 150 |
| **Espesor de Placa** | **1.6 mm** | Estándar recomendado |
| **Color de Máscara (Solder Mask)** | **Green** (o a elección) | Verde es el más económico y rápido |
| **Color de Serigrafía (Silkscreen)** | **White** | Contraste óptimo con logos RSA |
| **Acabado Superficial** | **HASL lead-free** o **ENIG** | HASL sin plomo (económico) o ENIG (dorado plano de alta durabilidad) |
| **Peso de Cobre** | **1 oz** (35 µm) | Estándar para señales y alimentación |
| **Confirmación de Borde** | No panelizar (Single PCB) | Placa individual con corte perimetral limpio |

---

## 🤖 Parámetros para Ensamble SMT (PCB Assembly)

Activa la opción **"PCB Assembly"** en la parte inferior del cotizador:

- **Assembly Side**: **Top Side Only** (¡Todos los 27 componentes SMD están en la cara superior, ahorrando costo de doble cara!).
- **PCBA Quantity**: Selecciona cuántas unidades deseas ensambladas (ej. 2, 5 o 10).
- **Tooling Holes**: "Added by JLCPCB" (ellos añaden borde de descarte para los rieles de montaje).
- **Confirmación de Partes**: Al cargar `BOM_acople_sp32_JLCPCB_SMT.csv` y `CPL_acople_sp32_JLCPCB.csv`, la herramienta de JLCPCB previsualizará en 3D la posición de cada integrado, resistencia y condensador. Si la orientación de algún transistor o integrado requiere ajuste de 90° o 180°, la interfaz web de JLCPCB dispone de un botón de rotación gráfica de un solo clic antes de confirmar el pedido.
