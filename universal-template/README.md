# Plantilla Universal: dsPIC33FJ (Proteus) & dsPIC33EP (Hardware)

Este código permite trabajar con un solo archivo fuente para ambos entornos.  Usando PROTEUS y dspic33fj para simulación y la tarjeta de desarrollo **DAR-CPU** para validar en Hardware.
Configurado a **40 MIPS** con cristal de 8MHz + PLL.

### 🛠️ Instrucciones:

1. **Simulación:** Selecciona `dsPIC33FJ32MC204` en las propiedades del proyecto de MPLAB X.
2. **Hardware Real:** Selecciona `dsPIC33EP32MC204` en las propiedades del proyecto de MPLAB X.

**Importante:** Configurado para ICSP en **PGD3** para dsPIC33ep (específico para mi tarjeta de desarrollo).

### Estructura
- `test_dspic33fj.X` → Proyecto MPLAB X para el **dsPIC33fj valido para simulación en Proteus y en hardware**. 
- `test_dspic33ep.X` → Proyecto MPLAB X para el **dsPIC33ep valido solamente para Hardware**. 
- `test_dspic33fj32mc204.pdsprj` → Archivo Proteus para el dsPIC33fj

  
