import serial
import time
import os

PUERTO_COM = 'COM3'   # Tu puerto
ARCHIVO_AUDIO = 'audio_16k.raw'  # Nombre de tu audio en raw

def programar_eeprom():
    try:
        puerto = serial.Serial(PUERTO_COM, 115200, timeout=0.5)
        print(f"Abriendo {PUERTO_COM}...")
        
        puerto.reset_input_buffer()
        puerto.reset_output_buffer()

        print("Buscando placa DAR-CPU...")
        # BUCLE DE SINCRONIZACIÓN 
        while True:
            puerto.write(b'?') # ping
            respuesta = puerto.read(5) # leer "START"
            
            if b'START' in respuesta:
                print("¡Placa sincronizada! Iniciando transferencia...")
                break
        
        # PROCESO DE GRABACIÓN
        puerto.timeout = 2.0 
        
        tamaño_archivo = os.path.getsize(ARCHIVO_AUDIO)
        if tamaño_archivo > 65536:
            tamaño_archivo = 65536
            
        with open(ARCHIVO_AUDIO, 'rb') as file:
            datos = file.read(tamaño_archivo)

        print(f"Grabando {tamaño_archivo} bytes...")
        inicio = time.time()

        for i, byte in enumerate(datos):
            puerto.write(bytes([byte]))
            
            resp_byte = puerto.read(1)
            if resp_byte == b'E':
                print(f"\nError de EEPROM en el byte {i}.")
                break
            elif resp_byte != b'K':
                print(f"\nDesincronizado en el byte {i}. Resp: {resp_byte}")
                break
                
            if i > 0 and i % 2048 == 0:
                print(f"Progreso: {i//1024} KB / 64 KB")

        tiempo_total = time.time() - inicio
        print(f"\n¡Finalizado en {tiempo_total:.1f} segundos!")
        puerto.close()

    except Exception as e:
        print(f"Error del sistema: {e}")

if __name__ == "__main__":
    programar_eeprom()