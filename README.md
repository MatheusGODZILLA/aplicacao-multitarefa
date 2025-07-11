# Aplicação Multitarefa com Raspberry Pi Pico

Este repositório contém implementações bare-metal em C utilizando a Raspberry Pi Pico (RP2040) para leitura e exibição de dados provenientes de sensores digitais via I²C. Cada sensor está estruturado em seu próprio diretório com código modularizado e documentação correspondente.

## 🔧 Estrutura

- `/BH1750` — Medição de luminosidade ambiente
- `/AHT10` — Leitura de temperatura e umidade
- `/MPU6050` — Medição de inclinação via acelerômetro
- `inc/` — Headers compartilhados
- `lib/ssd1306/` — Biblioteca para OLED I2C

---

## 📦 Requisitos

- Raspberry Pi Pico ou Pico W
- SDK da Raspberry Pi Pico
- Cabos jumper e sensores BH1750, AHT10, MPU6050
- Display OLED I2C (opcional para AHT10 e MPU6050)
- Buzzer (opcional para AHT10)
- Ferramenta de compilação (`cmake`, `ninja`)
- Monitor serial (Thonny, PuTTY, minicom, etc.)

---

## 🧪 Projetos Realizados

### 🌞 BH1750 – Sensor de Luminosidade

Leitura de lux ambiente com medição contínua em alta resolução. Os dados são exibidos via terminal serial USB.

- **I2C usado:** `i2c0` (GPIO 0 e 1)
- **Frequência I2C:** 100 kHz
- **Modo do sensor:** 0x10 (alta resolução contínua)
- **Saída:** `Luminosidade: XXXX lux`

📁 Veja: [`/BH1750`](./BH1750)

---

### 🌡️ AHT10 – Temperatura e Umidade

Sensor de ambiente com exibição simultânea no OLED e envio via serial. Emite alerta sonoro via buzzer se:
- Temperatura < 20 °C
- Umidade > 70 %

- **Sensor I2C:** `i2c0` (GPIO 0 e 1)
- **OLED I2C:** `i2c1` (GPIO 14 e 15)
- **Frequência:** 400 kHz

📁 Veja: [`/AHT10`](./AHT10)

---

### 📐 MPU6050 – Sensor de Inclinação

Leitura dos eixos do acelerômetro e cálculo da inclinação com alerta visual no OLED.

- **Sensor I2C:** `i2c0` (GPIO 0 e 1)
- **OLED I2C:** `i2c1` (GPIO 14 e 15)
- **Alerta:** Se inclinação no eixo X > 30°

📁 Veja: [`/MPU6050`](./MPU6050)

---

## 📝 Observações

- O projeto foi desenvolvido como parte da **Residência Tecnológica EmbarcaTech**
- A execução parcial se deve à limitação na entrega dos kits de sensores
- Todos os códigos foram feitos utilizando **C bare-metal com a SDK oficial da Raspberry Pi**

---

## 👤 Autor

**Matheus da Silva**  
Residência EmbarcaTech – IFPI  

