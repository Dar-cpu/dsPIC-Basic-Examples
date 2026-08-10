from __future__ import annotations

import re
from pathlib import Path
import tkinter as tk
from tkinter import filedialog, messagebox, colorchooser, ttk

from PIL import Image, ImageTk


APP_TITLE = "dsPIC Image Converter"
DEFAULT_BG = "#071426"

# Estandar fijo del proyecto
C_IDENTIFIER = "microchip"
TFT_WIDTH = 128
TFT_HEIGHT = 160
IMAGE_Y = 62
MAX_IMAGE_WIDTH = TFT_WIDTH
MAX_IMAGE_HEIGHT = TFT_HEIGHT - IMAGE_Y  # 98 px disponibles


def rgb565(r: int, g: int, b: int) -> int:
    """Convert 8-bit RGB to RGB565."""
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)


def sanitize_identifier(text: str) -> str:
    text = re.sub(r"[^A-Za-z0-9_]+", "_", text.strip())
    text = re.sub(r"_+", "_", text).strip("_")
    if not text:
        text = "image"
    if text[0].isdigit():
        text = f"img_{text}"
    return text.lower()


def parse_hex_color(value: str) -> tuple[int, int, int]:
    value = value.strip()
    if not re.fullmatch(r"#[0-9A-Fa-f]{6}", value):
        raise ValueError("El color debe tener formato #RRGGBB.")
    return tuple(int(value[i:i + 2], 16) for i in (1, 3, 5))


def composite_rgba(image: Image.Image, background: tuple[int, int, int]) -> Image.Image:
    rgba = image.convert("RGBA")
    bg = Image.new("RGBA", rgba.size, (*background, 255))
    return Image.alpha_composite(bg, rgba).convert("RGB")


def fit_size(src_w: int, src_h: int, max_w: int, max_h: int, preserve: bool) -> tuple[int, int]:
    if not preserve:
        return max_w, max_h

    scale = min(max_w / src_w, max_h / src_h)
    width = max(1, int(round(src_w * scale)))
    height = max(1, int(round(src_h * scale)))
    return width, height


def quantize_4_colors(image: Image.Image, dither: bool) -> tuple[Image.Image, list[tuple[int, int, int]], list[int]]:
    """
    Quantize RGB image to four palette slots and return:
      - RGB preview
      - four RGB palette entries
      - palette indices for every pixel (0..3)
    """
    dither_mode = Image.Dither.FLOYDSTEINBERG if dither else Image.Dither.NONE

    q = image.quantize(
        colors=4,
        method=Image.Quantize.MEDIANCUT,
        dither=dither_mode,
    )

    raw_palette = q.getpalette() or []
    pixel_values = list(q.getdata())
    used = sorted(set(pixel_values))

    colors: list[tuple[int, int, int]] = []
    remap: dict[int, int] = {}

    for old_index in used:
        p = old_index * 3
        rgb = tuple(raw_palette[p:p + 3])
        if len(rgb) != 3:
            rgb = (0, 0, 0)
        remap[old_index] = len(colors)
        colors.append((int(rgb[0]), int(rgb[1]), int(rgb[2])))

    while len(colors) < 4:
        colors.append(colors[-1] if colors else (0, 0, 0))

    colors = colors[:4]
    indices = [remap.get(int(v), 0) for v in pixel_values]

    preview = Image.new("RGB", q.size)
    preview.putdata([colors[i] for i in indices])
    return preview, colors, indices


def pack_2bpp(indices: list[int]) -> list[int]:
    """
    Pack 4 pixels per byte:
      pixel 0 -> bits 7:6
      pixel 1 -> bits 5:4
      pixel 2 -> bits 3:2
      pixel 3 -> bits 1:0
    """
    packed: list[int] = []
    for i in range(0, len(indices), 4):
        vals = indices[i:i + 4]
        while len(vals) < 4:
            vals.append(0)
        packed.append(
            ((vals[0] & 0x03) << 6)
            | ((vals[1] & 0x03) << 4)
            | ((vals[2] & 0x03) << 2)
            | (vals[3] & 0x03)
        )
    return packed


def build_c_header(
    identifier: str,
    width: int,
    height: int,
    palette_rgb: list[tuple[int, int, int]],
    packed: list[int],
) -> str:
    # El identificador C queda fijo para que main.c nunca tenga que cambiar.
    ident = C_IDENTIFIER
    macro = C_IDENTIFIER.upper()
    guard = f"{macro}_H"

    palette565 = [rgb565(*rgb) for rgb in palette_rgb]

    lines = [
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        "#include <stdint.h>",
        "",
        f"#define {macro}_WIDTH   {width}U",
        f"#define {macro}_HEIGHT  {height}U",
        f"#define {macro}_BYTES   {len(packed)}U",
        "",
        "/*",
        " * Imagen generada por dsPIC Image Converter.",
        " * Formato: 2 bits/pixel, 4 colores, 4 pixeles por byte.",
        " * Las tablas se ubican en Flash/PSV para no consumir RAM.",
        " */",
        f"static const uint16_t {ident}_palette_rgb565[4]",
        "    __attribute__((space(auto_psv))) =",
        "{",
        "    " + ", ".join(f"0x{value:04X}U" for value in palette565),
        "};",
        "",
        f"static const uint8_t {ident}_2bpp[{macro}_BYTES]",
        "    __attribute__((space(auto_psv))) =",
        "{",
    ]

    for i in range(0, len(packed), 16):
        chunk = packed[i:i + 16]
        suffix = "," if i + 16 < len(packed) else ""
        lines.append(
            "    " + ", ".join(f"0x{value:02X}U" for value in chunk) + suffix
        )

    lines.extend(
        [
            "};",
            "",
            "/* Uso recomendado en main.c:",
            " * st7735_draw_image_2bpp(",
            " *     (uint16_t)((ST7735_WIDTH - MICROCHIP_WIDTH) / 2U),",
            f" *     {IMAGE_Y}U,",
            " *     MICROCHIP_WIDTH, MICROCHIP_HEIGHT,",
            " *     microchip_2bpp, microchip_palette_rgb565",
            " * );",
            " */",
            "",
            f"#endif /* {guard} */",
            "",
        ]
    )
    return "\n".join(lines)


class ConverterApp(tk.Tk):
    def __init__(self) -> None:
        super().__init__()
        self.title(APP_TITLE)
        self.geometry("900x610")
        self.minsize(820, 560)

        self.source_path: Path | None = None
        self.source_image: Image.Image | None = None
        self.converted_image: Image.Image | None = None
        self.generated_header = ""
        self.preview_photo: ImageTk.PhotoImage | None = None

        self.path_var = tk.StringVar()
        self.name_var = tk.StringVar(value=C_IDENTIFIER)
        self.width_var = tk.IntVar(value=MAX_IMAGE_WIDTH)
        self.height_var = tk.IntVar(value=MAX_IMAGE_HEIGHT)
        self.preserve_var = tk.BooleanVar(value=True)
        self.dither_var = tk.BooleanVar(value=True)
        self.bg_var = tk.StringVar(value=DEFAULT_BG)
        self.stats_var = tk.StringVar(value="Selecciona una imagen para comenzar.")
        self.palette_var = tk.StringVar(value="Paleta: —")

        self._build_ui()

    def _build_ui(self) -> None:
        style = ttk.Style(self)
        try:
            style.theme_use("vista")
        except tk.TclError:
            pass

        outer = ttk.Frame(self, padding=12)
        outer.pack(fill="both", expand=True)

        top = ttk.LabelFrame(outer, text="Imagen de entrada", padding=10)
        top.pack(fill="x")

        ttk.Label(top, text="Archivo").grid(row=0, column=0, sticky="w", padx=(0, 8))
        ttk.Entry(top, textvariable=self.path_var).grid(row=0, column=1, sticky="ew")
        ttk.Button(top, text="Seleccionar…", command=self.select_image).grid(
            row=0, column=2, padx=(8, 0)
        )

        ttk.Label(top, text="Identificador C").grid(row=1, column=0, sticky="w", pady=(9, 0))
        name_entry = ttk.Entry(
            top, textvariable=self.name_var, width=24, state="readonly"
        )
        name_entry.grid(row=1, column=1, sticky="w", pady=(9, 0))

        top.columnconfigure(1, weight=1)

        options = ttk.LabelFrame(outer, text="Conversión", padding=10)
        options.pack(fill="x", pady=(10, 0))

        ttk.Label(options, text="Máx. ancho").grid(row=0, column=0, sticky="w")
        width_spin = ttk.Spinbox(
            options, from_=1, to=MAX_IMAGE_WIDTH, textvariable=self.width_var, width=8, command=self.convert
        )
        width_spin.grid(row=0, column=1, sticky="w", padx=(6, 18))
        width_spin.bind("<KeyRelease>", lambda _event: self.convert())

        ttk.Label(options, text="Máx. alto").grid(row=0, column=2, sticky="w")
        height_spin = ttk.Spinbox(
            options, from_=1, to=MAX_IMAGE_HEIGHT, textvariable=self.height_var, width=8, command=self.convert
        )
        height_spin.grid(row=0, column=3, sticky="w", padx=(6, 18))
        height_spin.bind("<KeyRelease>", lambda _event: self.convert())

        ttk.Checkbutton(
            options,
            text="Mantener proporción",
            variable=self.preserve_var,
            command=self.convert,
        ).grid(row=0, column=4, sticky="w", padx=(0, 18))

        ttk.Checkbutton(
            options,
            text="Dithering",
            variable=self.dither_var,
            command=self.convert,
        ).grid(row=0, column=5, sticky="w")

        ttk.Label(options, text="Fondo transparencia").grid(
            row=1, column=0, columnspan=2, sticky="w", pady=(10, 0)
        )
        bg_entry = ttk.Entry(options, textvariable=self.bg_var, width=10)
        bg_entry.grid(row=1, column=2, sticky="w", pady=(10, 0))
        bg_entry.bind("<KeyRelease>", lambda _event: self.convert())

        ttk.Button(options, text="Elegir color…", command=self.choose_background).grid(
            row=1, column=3, sticky="w", pady=(10, 0), padx=(6, 0)
        )

        main = ttk.Frame(outer)
        main.pack(fill="both", expand=True, pady=(10, 0))

        preview_frame = ttk.LabelFrame(main, text="Vista previa convertida", padding=10)
        preview_frame.pack(side="left", fill="both", expand=True)

        self.preview_label = tk.Label(
            preview_frame,
            text="Sin imagen",
            bg="#111111",
            fg="white",
            relief="sunken",
            bd=1,
        )
        self.preview_label.pack(fill="both", expand=True)

        right = ttk.Frame(main)
        right.pack(side="right", fill="y", padx=(10, 0))

        info = ttk.LabelFrame(right, text="Resultado", padding=10)
        info.pack(fill="x")

        ttk.Label(info, textvariable=self.stats_var, justify="left").pack(anchor="w")
        ttk.Label(
            info,
            textvariable=self.palette_var,
            justify="left",
            wraplength=280,
        ).pack(anchor="w", pady=(8, 0))

        actions = ttk.LabelFrame(right, text="Salida", padding=10)
        actions.pack(fill="x", pady=(10, 0))

        ttk.Button(
            actions,
            text="Convertir y copiar microchip.h",
            command=self.copy_to_clipboard,
        ).pack(fill="x")

        ttk.Button(
            actions,
            text="Guardar como microchip.h…",
            command=self.save_header,
        ).pack(fill="x", pady=(8, 0))

        ttk.Button(
            actions,
            text="Copiar llamada para main.c",
            command=self.copy_example_call,
        ).pack(fill="x", pady=(8, 0))

        ttk.Label(
            right,
            text=(
                "Compatible con st7735_draw_image_2bpp().\n"
                "4 colores adaptativos · 2 bits/pixel · RGB565.\n"
                f"Área de logo: {MAX_IMAGE_WIDTH}×{MAX_IMAGE_HEIGHT} desde Y={IMAGE_Y}."
            ),
            justify="left",
            wraplength=280,
        ).pack(anchor="w", pady=(12, 0))

    def select_image(self) -> None:
        path = filedialog.askopenfilename(
            title="Seleccionar imagen",
            filetypes=[
                ("Imágenes", "*.png *.jpg *.jpeg *.bmp *.gif *.webp"),
                ("PNG", "*.png"),
                ("JPEG", "*.jpg *.jpeg"),
                ("Todos los archivos", "*.*"),
            ],
        )
        if not path:
            return

        try:
            image = Image.open(path)
            image.load()
        except Exception as exc:
            messagebox.showerror(APP_TITLE, f"No se pudo abrir la imagen:\n{exc}")
            return

        self.source_path = Path(path)
        self.source_image = image
        self.path_var.set(path)
        # El nombre del archivo NO cambia el identificador C.
        self.name_var.set(C_IDENTIFIER)
        self.convert()

    def choose_background(self) -> None:
        color = colorchooser.askcolor(color=self.bg_var.get(), title="Color de fondo")
        if color and color[1]:
            self.bg_var.set(color[1].upper())
            self.convert()

    def convert(self) -> None:
        if self.source_image is None:
            return

        try:
            max_w = int(self.width_var.get())
            max_h = int(self.height_var.get())
        except Exception:
            return

        if not (1 <= max_w <= MAX_IMAGE_WIDTH and 1 <= max_h <= MAX_IMAGE_HEIGHT):
            self.stats_var.set(
                f"Dimensiones inválidas: máximo {MAX_IMAGE_WIDTH} × {MAX_IMAGE_HEIGHT}."
            )
            return

        try:
            background = parse_hex_color(self.bg_var.get())
        except ValueError:
            self.stats_var.set("Color de fondo inválido. Usa #RRGGBB.")
            return

        source = composite_rgba(self.source_image, background)
        width, height = fit_size(
            source.width,
            source.height,
            max_w,
            max_h,
            self.preserve_var.get(),
        )

        resized = source.resize((width, height), Image.Resampling.LANCZOS)

        preview, palette, indices = quantize_4_colors(
            resized,
            self.dither_var.get(),
        )
        packed = pack_2bpp(indices)

        ident = C_IDENTIFIER
        self.generated_header = build_c_header(
            ident,
            width,
            height,
            palette,
            packed,
        )
        self.converted_image = preview

        bytes_data = len(packed)
        bytes_palette = 8
        total = bytes_data + bytes_palette

        self.stats_var.set(
            f"Tamaño: {width} × {height} px\n"
            f"Datos 2bpp: {bytes_data} bytes\n"
            f"Paleta: {bytes_palette} bytes\n"
            f"Total aprox.: {total} bytes de Flash"
        )

        parts = []
        for i, (r, g, b) in enumerate(palette):
            parts.append(f"{i}: #{r:02X}{g:02X}{b:02X} → 0x{rgb565(r,g,b):04X}")
        self.palette_var.set("Paleta RGB565:\n" + "\n".join(parts))

        self._refresh_preview()

    def _refresh_preview(self) -> None:
        if self.converted_image is None:
            return

        max_preview_w = 460
        max_preview_h = 390
        img = self.converted_image.copy()
        scale = min(max_preview_w / img.width, max_preview_h / img.height)
        scale = max(1.0, scale)

        shown = img.resize(
            (
                max(1, int(round(img.width * scale))),
                max(1, int(round(img.height * scale))),
            ),
            Image.Resampling.NEAREST,
        )

        self.preview_photo = ImageTk.PhotoImage(shown)
        self.preview_label.configure(image=self.preview_photo, text="")

    def copy_to_clipboard(self) -> None:
        self.convert()
        if not self.generated_header:
            messagebox.showwarning(APP_TITLE, "Primero selecciona una imagen.")
            return

        self.clipboard_clear()
        self.clipboard_append(self.generated_header)
        self.update()
        messagebox.showinfo(
            APP_TITLE,
            "Código C copiado al portapapeles.\n\n"
            "Pégalo en un archivo .h dentro de MPLAB X.",
        )

    def save_header(self) -> None:
        self.convert()
        if not self.generated_header:
            messagebox.showwarning(APP_TITLE, "Primero selecciona una imagen.")
            return

        ident = C_IDENTIFIER
        path = filedialog.asksaveasfilename(
            title="Guardar header",
            defaultextension=".h",
            initialfile="microchip.h",
            filetypes=[("C header", "*.h"), ("Todos los archivos", "*.*")],
        )
        if not path:
            return

        try:
            Path(path).write_text(self.generated_header, encoding="utf-8")
        except OSError as exc:
            messagebox.showerror(APP_TITLE, f"No se pudo guardar:\n{exc}")
            return

        messagebox.showinfo(APP_TITLE, f"Archivo guardado:\n{path}")

    def copy_example_call(self) -> None:
        call = (
            "st7735_draw_image_2bpp(\n"
            "    (uint16_t)((ST7735_WIDTH - MICROCHIP_WIDTH) / 2U),\n"
            f"    {IMAGE_Y}U,\n"
            "    MICROCHIP_WIDTH,\n"
            "    MICROCHIP_HEIGHT,\n"
            "    microchip_2bpp,\n"
            "    microchip_palette_rgb565\n"
            ");"
        )
        self.clipboard_clear()
        self.clipboard_append(call)
        self.update()
        messagebox.showinfo(
            APP_TITLE,
            "Llamada corregida para main.c copiada.\n"
            "La imagen se centra automáticamente en X."
        )



if __name__ == "__main__":
    ConverterApp().mainloop()