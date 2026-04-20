# Plantilla Universal: dsPIC33FJ (Proteus) & dsPIC33EP (Hardware)

Este código permite trabajar con un solo archivo fuente para ambos entornos.  
Configurado a **40 MIPS** con cristal de 8MHz + PLL.

### 🛠️ Instrucciones:

1. **Simulación:** Selecciona `dsPIC33FJ32MC204` en las propiedades del proyecto de MPLAB X.
2. **Hardware Real:** Selecciona `dsPIC33EP32MC204` en las propiedades del proyecto de MPLAB X.

**Importante:** Configurado para ICSP en **PGD3** (específico para mi tarjeta de desarrollo).

### Estructura
- `dsPIC33FJ32MC204_Proteus/` → Archivos para simulación en Proteus
- `dsPIC33EP32MC204_Hardware/` → Archivos para hardware real
- `Proteus_Simulation.dsproj` o `.pdsprj` → Archivo del proyecto de Proteus
