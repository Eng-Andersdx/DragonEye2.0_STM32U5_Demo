# STM32U5 TouchGFX Project

## Overview
This repository contains a reference firmware project for an **STM32U5-based platform** using **TouchGFX** for graphical user interface development.  
It serves as a starting point for developers who want to build custom firmware and GUI applications.

For technical assistance, please contact the engineering team at engineering@andersdx.com.

---

## Repository Purpose
This project is intended for developers who want to:

- Develop firmware for **STM32U5 microcontrollers**
- Build **graphical user interfaces using TouchGFX**
- Use a **reference STM32CubeIDE project structure**

The repository provides a working baseline that can be extended for custom applications.

---

## Getting Started

### Clone the Repository
Clone the repository to your local machine and place it inside your **STM32CubeIDE workspace**.

Example:

### Import into STM32CubeIDE

1. Open **STM32CubeIDE**
2. Select **File → Open Projects from File System**
3. Browse to the cloned project folder and import it

### Build and Run

1. Connect the STM32U5 board using the debugger
2. Build the project
3. Run or debug the firmware from STM32CubeIDE

---

## GUI Development

GUI development is done using **TouchGFX Designer**.

Typical workflow:

1. Open the project in TouchGFX Designer  
2. Modify or create GUI screens  
3. Generate code from TouchGFX  
4. Rebuild the project in STM32CubeIDE  

Existing GUI projects can also be imported if the **display configuration matches the target hardware**.

---

## Custom Development

For custom firmware development:

- Modify the **`.ioc` file** to configure hardware and regenerate code
- Add board-specific drivers inside:
- Implement application logic in:
- Update TouchGFX hardware integration in:

---

## Project Structure

- **Core** – Application source code  
- **Drivers** – HAL drivers and board support  
- **TouchGFX** – GUI framework and generated code  
- **Middlewares** – Additional libraries