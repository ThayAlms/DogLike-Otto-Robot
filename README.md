# 🐶 DogLike Otto Robot + GC9A01 Display

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Arduino-green)
![Language](https://img.shields.io/badge/language-C%2B%2B-orange)

Um projeto divertido e educacional para montar um **robô quadrúpede inspirado no Otto DIY**, mas com personalidade de cachorro (**DogLike Otto Robot**), usando a **RoboCore Vespa**, alimentado por **duas baterias 3.7 V**, e equipado com um **display circular GC9A01** para mostrar emoções enquanto o robô interage.  

Ele combina **controle de servo motores com PID**, **gaits de caminhada**, **expressões faciais no display** e **movimentos simples** para criar uma experiência muito mais envolvente.

---

## 🐾 Funcionalidades  

- 🚶 **Andar para frente** com movimento suave usando controle PID nas patas  
- 🪑 **Sentar** (movimento estável das 4 pernas)  
- ✋ **Dar a patinha** (levanta a pata dianteira direita e volta à posição)  
- 🛏 **Deitar** (movimento gradual de relaxamento das patas)  
- 😀 **Mostrar emoções** no display GC9A01 (olhos piscando e expressões felizes)

> 💡 **Diferencial:** o uso de PID garante que as patas se movimentem suavemente e cheguem ao ângulo certo sem trancos, mesmo com variações de carga ou atrito.

---

## 📷 Visão Geral  

<p align="center">
<img src="Media/Capa.jpeg" alt="DogLike Otto Robot" width="400">
</p>

O resultado é um robô que parece ter **vida própria**, reagindo com expressões enquanto executa suas ações.

---

## 🔧 Componentes Necessários  

| Componente | Qtde | Observações |
|-----------|------|-------------|
| [RoboCore Vespa](https://www.robocore.net/) | 1 | Placa controladora |
| Micro servo 9 g | 4 | Para as pernas |
| Display GC9A01 1.28" | 1 | Interface SPI |
| Bateria 18650 3.7 V | 2 | Alimentação |
| Suporte para 2× 18650 | 1 | Com fios ou conector JST |
| Parafusos M2 + porcas | 2 de cada | Para fixar o display |
| Patinhas de TPU | 4 | Para melhor aderência no chão |
| Parafusos dos servos | — | Normalmente vêm com o servo |
| Cabo USB | 1 | Para programação |

---

## 🖥 Ligações  

### 📺 Display GC9A01 → Vespa  

| Vespa | GC9A01 |
|------|--------|
| 5 (SPI CS) | CS |
| 17 (DC) | DC |
| 16 (RST) | RST |
| 22 (SCK) | SCL |
| 21 (MOSI) | SDA |
| 3.3 V | VCC |
| GND | GND |

### ⚙️ Servos → Vespa  

| Vespa | Servo |
|------|-------|
| D0 | Servo 1 (Frente Esquerda) |
| D1 | Servo 2 (Frente Direita) |
| D2 | Servo 3 (Trás Esquerda) |
| D3 | Servo 4 (Trás Direita) |
| 5 V | VCC comum |
| GND | GND comum |

> 💡 **Dica:** use uma linha de alimentação separada para os servos se eles puxarem muita corrente — isso evita que a Vespa reinicie durante o movimento.

---

## ⚙️ Controle PID das Patas  

Cada servo tem seu ângulo corrigido usando **PID simples**:
- **Kp** ajusta a força para chegar no ângulo desejado  
- **Kd** suaviza o movimento, evitando overshoot  
- Isso garante **caminhada fluida**, sem trancos e com resposta estável

---

## 🛠 Montagem  

- 🔩 **Servos:** prenda cada um com os parafusos que vêm no kit.  
- 🐾 **Patinhas:** encaixe ou parafuse nas hastes dos servos para maior tração.  
- 📺 **Display:** fixe o GC9A01 com **parafusos M2 + porcas** em um suporte 3D.  
- 🔋 **Baterias:** use duas 18650 em série (≈ 7,4 V). A Vespa regula a tensão para si e para o display.  

---

## 📂 Arquivos 3D  

| Peça | Arquivo STL |
|-----|-------------|
| Patinhas de TPU | [`stl/pata_tpu.stl`](stl/pata_tpu.stl) |
| Suporte do Display | [`stl/suporte_display.stl`](stl/suporte_display.stl) |

> 📌 Basta clicar no link para baixar os arquivos e imprimir em TPU ou PLA.  

---

## 🚀 Próximos Passos  

- [ ] Adicionar diagrama visual de fiação (`media/wiring.png`)  
- [ ] Publicar código com gaits básicos  
- [ ] Criar conjunto de faces para o display  
- [ ] Documentar ajustes finos de PID para cada tipo de servo  

---

## 📜 Licença  

Este projeto está licenciado sob a [MIT License](LICENSE).  
Sinta-se livre para usar, modificar e compartilhar!  

---

<p align="center">
💡 **Transforme este projeto no seu DogLike Otto e compartilhe no LinkedIn e GitHub!**  
</p>
