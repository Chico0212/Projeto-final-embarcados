# Introdução

O presente projeto foi desenvolvido no âmbito da disciplina de **Sistemas Embarcados**, com o objetivo de projetar, implementar e documentar um sistema embarcado interativo, baseado na plataforma **ESP32**, que integra sensores inerciais, interface gráfica em display OLED, controle físico via botões e armazenamento externo de dados em cartão SD.

### Objetivos do Projeto

O objetivo geral consiste na construção de um sistema completo, composto por **quatro minigames independentes**, cujo controle se dá por meio da **detecção de movimento** utilizando o sensor **MPU6050**, com visualização em display **SSD1306** e persistência de pontuação em **cartão SD**. O sistema deve apresentar interface de navegação por menu, operada por dois botões físicos, além de prover feedback sonoro por meio de um **buzzer PWM**.

Entre os objetivos específicos, destacam-se:

* Desenvolver quatro jogos funcionais controlados por inclinação.
* Implementar um menu de seleção inicial com navegação responsiva.
* Integrar sensores e atuadores utilizando os protocolos **I2C** e **SPI**.
* Construir uma arquitetura modularizada baseada em **componentes reutilizáveis**.
* Aplicar técnicas de controle multitarefa com uso de **FreeRTOS**.
* Garantir a **persistência de dados** entre sessões por meio de armazenamento em cartão SD.
* Prover uma interface de fácil interação, com elementos gráficos e sonoros.

### Bibliotecas Desenvolvidas

A estrutura do projeto foi segmentada em múltiplos componentes, localizados no diretório `components/`, os quais abstraem o controle de dispositivos e funcionalidades específicas. As bibliotecas desenvolvidas incluem:

* **`mpu6050`** – leitura de dados inerciais (aceleração e giroscópio) via I2C.
* **`ssd1306`** – controle do display OLED monocromático, incluindo funções para texto e gráficos.
* **`sd_card`** – gerenciamento de arquivos e persistência de dados com suporte à FatFS.
* **`buzzer`** – geração de sinais sonoros utilizando modulação por largura de pulso (PWM).
* **`button`** – tratamento de entrada digital com debounce e detecção de eventos.
* **`i2c_commons`** – funções utilitárias para comunicação I2C com múltiplos dispositivos.

Esses módulos foram implementados visando encapsulamento, reaproveitamento de código e desacoplamento entre camadas.

### Objetivo da Documentação

Esta documentação tem como finalidade registrar, de forma sistematizada, o processo de desenvolvimento do projeto, sua arquitetura, funcionamento interno e instruções de uso. Os objetivos da documentação incluem:

* Explicitar o funcionamento dos módulos físicos e lógicos do sistema.
* Fornecer orientação para instalação, execução e testes do sistema.
* Servir como base para futuras manutenções, melhorias ou reuso do código.
* Documentar as decisões de projeto e estratégias adotadas.

### Visão Geral do Sistema Desenvolvido

O sistema embarcado implementado inicia em um menu principal, onde o usuário pode navegar entre os minigames utilizando dois botões físicos. A escolha de um jogo leva à execução do mesmo, cujo controle se dá por **inclinação do dispositivo**, interpretada em tempo real pelo sensor **MPU6050**. A interface gráfica, incluindo menus, sprites e textos, é exibida no display **SSD1306**, operando em modo I2C.

Ao final de cada partida, a pontuação obtida é comparada ao recorde previamente armazenado no **cartão SD**. Em caso de superação, o novo valor é salvo de forma persistente. O controle lógico do sistema é estruturado por meio de uma **máquina de estados finitos**, e a execução das funcionalidades principais ocorre de forma assíncrona por meio de **threads gerenciadas pelo FreeRTOS**.

Confira o repositório oficial do projeto [clicando aqui](https://github.com/Chico0212/Projeto-final-embarcados)

Confira a documentação oficial do projeto [clicando aqui](https://embarcados2025-1.gitbook.io/projeto-embarcados-2025.1/)

### Desenvolvedores

<figure><img src="https://2203337374-files.gitbook.io/~/files/v0/b/gitbook-x-prod.appspot.com/o/spaces%2FRsqZHqRg6FIWtis1N7A8%2Fuploads%2FNbjy2SWgFDC0CyVimVlf%2Ffrancisco.png?alt=media&#x26;token=4f46ed79-940a-4fac-b038-1227c7fdaa56" alt="" width="188"><figcaption><p>Francisco Bezerra</p></figcaption></figure>

<figure><img src="https://2203337374-files.gitbook.io/~/files/v0/b/gitbook-x-prod.appspot.com/o/spaces%2FRsqZHqRg6FIWtis1N7A8%2Fuploads%2FE6o43ffwCSgnar1cdbFB%2Figor.jpeg?alt=media&#x26;token=e3bc06e2-2464-4a3e-a126-5a5c3c14c4cd" alt="" width="188"><figcaption><p>Igor Kádson</p></figcaption></figure>

<figure><img src="https://2203337374-files.gitbook.io/~/files/v0/b/gitbook-x-prod.appspot.com/o/spaces%2FRsqZHqRg6FIWtis1N7A8%2Fuploads%2FDPZoVtiPV5t8sPS14XV0%2Fmarilia.jpeg?alt=media&#x26;token=08c9b31b-af56-461d-95d1-a65058497c79" alt="" width="188"><figcaption><p>Marília Oliveira</p></figcaption></figure>

